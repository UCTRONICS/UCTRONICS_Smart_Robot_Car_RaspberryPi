

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "motordriver.h"
#include "remoteserver.h"
#include <pthread.h>   
#include <sys/time.h>

#include <signal.h>
#include <stdarg.h>
#include <getopt.h>


#include "clk.h"
#include "gpio.h"
#include "dma.h"
#include "pwm.h"

#include "ws2811.h"

int baseSpeed, addLeftSpeed, addRightSpeed; 
unsigned long getColour = 0xFF0000;
unsigned int getBrightness = 100;
static int speedVal_1 = 5000;
static int speedVal_2 = 5000;
static int speedVal_3 = 5000;
static int speedVal_4 = 5000;
    
struct motionstate carstate = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned char disWarning  = 0;
unsigned char buf[IR_LIMITS];	// bytes buffer
unsigned int bits;	// 32768 bits capable
unsigned char done;
int sockfd, newsockfd, portno, clilen;
unsigned char client_Connected = 0;
unsigned char count;

#define BLOCK_SIZE (4*1024)

#define ARRAY_SIZE(stuff)       (sizeof(stuff) / sizeof(stuff[0]))

// defaults for cmdline options
#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)
#define WIDTH                 4
#define HEIGHT                 4
#define LED_COUNT               (WIDTH * HEIGHT)

void  INThandler(int sig);

int  mem_fd;
void *gpio_map;
// I/O access
volatile unsigned *gpio;
int width = WIDTH;
int height = HEIGHT;
int led_count = LED_COUNT;
int clear_on_exit = 0;
ws2811_t ledstring =
{
    .freq = TARGET_FREQ,
    .dmanum = DMA,
    .channel =
    {
        [0] =
        {
            .gpionum = GPIO_PIN,
            .count = LED_COUNT,
            .invert = 0,
            .brightness = 255,
            .strip_type = STRIP_TYPE,
        },
        [1] =
        {
            .gpionum = 0,
            .count = 0,
            .invert = 0,
            .brightness = 0,
        },
    },
};

ws2811_led_t *matrix;

unsigned long  grb_colour_table[] =
{
	0xFF0000,  // green
    0x00FF00,  // red
    0x0000FF,  // blue
	0xFFFF00,  //yellow
	0xFF00FF,
	0x00FFFF,  //pink
	0xFFFFFF,  // white
	0x000000,
	
};

/* Creates a server socket and listens for a command from the remote.
*/
int main(int argc, char *argv[])
{
	char buffer[BUFFER_SIZE]; ;
    struct sockaddr_in serv_addr, cli_addr;
    int  n, pulsenum, count ;
	signal(SIGINT, INThandler);
	/* Initialise GPIO */
		if (ControllerInit() < 0) return -1;
			setup_io();
			ultraInit();   
			servoInit();
			trackModeInit();
			beepInit();
			irInit();
			myPWMInit();
			GRBInit();
			pthread_t t1,t2;  
			//creat two thread
			pthread_create(&t1,NULL,fun1,NULL); 
			pthread_create(&t2,NULL,fun2,NULL); 
			for(pulsenum = 0; pulsenum <10; pulsenum++){
				servoCtrl(servo_1,1300);
				servoCtrl(servo_2,1300);
			}
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
                perror("ERROR opening socket");
                exit(1);
        }
        /* Initialize socket structure */
        bzero((char *) &serv_addr, sizeof(serv_addr));
        portno = 2001;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
 
        /* Now bind the host address using bind() call.*/
        if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                perror("ERROR on binding");
                exit(1);
        }
        /* Now start listening for the clients, here process will
        * go in sleep mode and will wait for the incoming connection
        */
		while(1){
			listen(sockfd,5);
			clilen = sizeof(cli_addr);
			printf("Waitting connection...\r\n");
			client_Connected = 0;
			BEEP_OPEN();
			/* Accept actual connection from the client */
			newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (void *) &clilen);
			if (newsockfd < 0) {
                perror("ERROR on accept");
                //exit(1);
			}
			/* If connection is established then start communicating */
			printf("connect successfully\r\n");
			sleep(0.5);
			GRB_work(1, getColour,getBrightness);
			n = write(newsockfd,"{\"version\":2}",13);
			client_Connected = 1;
			sleep(0.001);
			bzero(&buffer, BUFFER_SIZE);
			while ((n = read(newsockfd, &buffer, BUFFER_SIZE)) > 0) 
			{	
					//for(count = 0; count < n; count ++)
					//printf("buffer %d = %x\n",count, buffer[count]);
			   if(buffer[0] == 's'){  //0x73
				   baseSpeed = buffer[1];
				   addLeftSpeed = buffer[2];
				   addRightSpeed = buffer[3];
				   speedVal_1 = 10000*((buffer[1]+buffer[2])/255.0);
				   speedVal_3 = 10000*((buffer[1]+buffer[2])/255.0);
				
		           speedVal_2 = 10000*((buffer[1]+buffer[3])/255.0);
				   speedVal_4 = 10000*((buffer[1]+buffer[3])/255.0);
				   
				  // printf("speedVal_1 = %d\n", speedVal_1);
			   }else if(buffer[0] == 'c'){  //0x63
				  getColour = (buffer[1]<<16)|(buffer[2]<<8)|buffer[3]; 
				  getBrightness = buffer[4];
				  //printf("getColour = %x\n", getColour);
				  GRB_work(1, getColour, getBrightness);
			   }
			   else{
					for(count = 0; count < n; count ++){
						updateCarState(buffer[count]);
						updateCarMotion();
						}	
					}
			   bzero(&buffer, BUFFER_SIZE);	
			}
			  stop();
			  clearFlag();
			if (n < 0) {
                //printf("Connection exception!\n");
				break;      
			}		
		}
		client_Connected = 0;
		close(sockfd);
		printf("ERROR\r\n");
        return 0;
}

void *fun1(void *arg){ 
  unsigned char cnt = 0;
  float dis = 0, temp_min = 0, temp_max = 0, temp_value = 0, temp = 0;
   while(1){
	temp_min  = temp_max;
	temp_value= temp_max;
	for(cnt = 0; cnt < 3; cnt ++ ){
		temp = disMeasure();//sleep(0.01);  
		if(temp_max<temp)	temp_max = temp;
		if(temp_min>temp)	temp_min = temp;
		temp_value += temp;
	}
	dis = (temp_value-temp_max-temp_min);
	temp_value = 0;temp_min = 0; temp_max = 0; temp_value = 0; temp = 0;
	if(dis >0 && dis <= 50){
		disWarning = 1; 
		if(carstate.forward) {
		stop();
		}
	}else disWarning = 0;   
	beepWarning();
  }
}

void *fun2(void *arg){ 
int IRVal = 0;
  while(1){
	if(carstate.trackenable){	
		printf("Come in track mode \n");
	   trackModeWork();
	}
	if(carstate.autoAvoid){		   
		avoidance();
	}
    if(done==1){
	    done = 0;  
		IRVal = buf[2]; IRVal=IRVal <<8;
		IRVal = IRVal|buf[3];
		//printf("IRVal= %x",IRVal);
		IR_updateCarState(IRVal);
		IR_updateCarMotion();
		IRVal = 0;
	}
	mySoftPwmWrite1(speedVal_1);
	mySoftPwmWrite2(speedVal_2);
	mySoftPwmWrite3(speedVal_3);
	mySoftPwmWrite4(speedVal_4);
	getLedSta();
	}
		
}



int updateCarMotion(void) {
	static int angleA = 1300;
	static int angleB = 1300;
	
	char direction[16];
	if (carstate.forward && !carstate.back) {
		if ((!carstate.left && !carstate.right) || 
				(carstate.left && carstate.right)) {
			strcpy(direction, "forward");
			go_forward();	
		} else if (carstate.left && !carstate.right) {
			strcpy(direction, "forward left");
			go_forward_left();
		} else if (carstate.right && !carstate.left) {
			strcpy(direction, "forward right");
			go_forward_right();
		} else {
			strcpy(direction, "stop");
			stop();
		}
	} else if (carstate.back && !carstate.forward) {
		if ((!carstate.left && !carstate.right) || 
				(carstate.left && carstate.right)) {
			strcpy(direction, "back");
			go_back();
			
		} else if (carstate.left && !carstate.right) {
			strcpy(direction, "back left");
			go_back_left();
		} else if (carstate.right && !carstate.left) {
			strcpy(direction, "back right");
			go_back_right();
		} else {
			strcpy(direction, "stop");
			stop();
		}
	} else if (carstate.left && !carstate.right) {
		strcpy(direction, "left");
		go_left();
	} else if (carstate.right && !carstate.left) {
		strcpy(direction, "right");
		go_right();
	} else if (carstate.servoLeft) {
		carstate.servoLeft = 0;
		strcpy(direction, "servo_left");
		if(angleA<2300)
		angleA = angleA +10;
		else 
			angleA = 2300;
		servoCtrl(servo_1,  angleA);
		printf("angleA = %d\r\n",angleA);
	} else if (carstate.servoRight) {
		carstate.servoRight = 0;
		strcpy(direction, "servo_right");
		if(angleA> 300)
		angleA = angleA - 10;
		else
			angleA = 300;
		servoCtrl(servo_1,  angleA);
		printf("angleA = %d\r\n",angleA);
	} else if (carstate.servoUp) {
		carstate.servoUp = 0;
		strcpy(direction, "servo_UP");
		if(angleB> 300)
		angleB = angleB - 10;
		else
			angleB = 300;
		 servoCtrl(servo_2,  angleB);
		printf("angleB = %d\r\n",angleB);
	} else if (carstate.servoDown) {
		carstate.servoDown = 0;
		strcpy(direction, "servo_down");
		if(angleB<2300)
		angleB = angleB +10;
		else 
		angleB = 2300;
	    servoCtrl(servo_2,  angleB);

		printf("angleB = %d\r\n",angleB);
	}else {
		strcpy(direction, "stop");
		stop();
	}
	printf("Motion: %s \n", direction);
return 0;
}
void  clearFlag(void){
	 carstate.left=0;
     carstate.forward=0;
     carstate.right=0;
     carstate. back=0;
	 carstate.servoLeft=0;
	 carstate. servoRight=0;
	 carstate. servoUp=0;
	 carstate. servoDown=0;
	 carstate. speedUp=0;
	 carstate. speedDown=0;
	 carstate. autoAvoid=0;
	
}

  int IR_updateCarMotion(void) {
	static int angleA = 1300;
	static int angleB = 1300;
	char direction[16];
	if (carstate.forward) {
			strcpy(direction, "forward");
			go_forward();	printf("Motion: %s \n", direction);
	} 
	if (carstate.back) {
		
			strcpy(direction, "back");
			go_back();printf("Motion: %s \n", direction);
			
	} 
	if (carstate.left) {
		strcpy(direction, "left");
		go_left();printf("Motion: %s \n", direction);
	} 
	
	if (carstate.right) {
		strcpy(direction, "right");
		go_right();printf("Motion: %s \n", direction);
	} 
	if (carstate.servoLeft) {
		carstate.servoLeft = 0;
		strcpy(direction, "servo_left");
		if(angleA<2300)
		angleA = angleA +20;
		else 
			angleA = 2300;
		servoCtrl(servo_1,  angleA);
		printf("angleA = %d\r\n",angleA);
	} 
	if (carstate.servoRight) {
		carstate.servoRight = 0;
		strcpy(direction, "servo_right");
		if(angleA> 300)
		angleA = angleA - 20;
		else
			angleA = 300;
		servoCtrl(servo_1,  angleA);
		printf("angleA = %d\r\n",angleA);
	} 
	if (carstate.servoUp) {
		carstate.servoUp = 0;
		strcpy(direction, "servo_UP");
		if(angleB> 300)
		angleB = angleB - 20;
		else
			angleB = 300;
		 servoCtrl(servo_2,  angleB);
		printf("angleB = %d\r\n",angleB);
	} 
	if (carstate.servoDown) {
		carstate.servoDown = 0;
		strcpy(direction, "servo_down");
		if(angleB<2300)
		angleB = angleB +20;
		else 
		angleB = 2300;
	    servoCtrl(servo_2,  angleB);

		printf("angleB = %d\r\n",angleB);
	}
return 0;
}

int IR_updateCarState(int command) {
	
		if( command == IR_up){
		if(!disWarning)
			carstate.forward = 1;
		else	
			carstate.forward = 0;
		}
	  if( command == IR_down){
		carstate.back = 1;	
	  }
	if( command == IR_Left){
		carstate.left = 1;
	}
	if( command ==IR_right){
		carstate.right = 1;
	}
	if( command ==IR_stop){
	  carstate.forward = 0;
	  carstate.back = 0;
	  carstate.right = 0;
	  carstate.left = 0;
	  stop();printf("Motion: stop \n" );
	}
   if( command ==IR_speed_up){
	  carstate.speedUp = 1;
   }
   if( command ==IR_speed_down){
	  carstate.speedDown = 1;
   } 
	if( command ==IR_servo_up){	
	  carstate.servoUp = 1;
	}
	if( command ==IR_servo_down){
	  carstate.servoDown = 1;
	}	
	  if( command ==IR_servo_left){
	  carstate.servoLeft = 1;
	  }
	 if( command ==IR_servo_right){
	 carstate.servoRight = 1;
	 }
	 if( command ==IR_track){
		carstate.trackenable = 1;
	 }
	 if( command ==IR_track_stop){
		carstate.trackenable = 0;
		stop();
		 printf("Motion: stop \n" );
	 }	
return 0;	 
}
/* Updates the struct MotionState of the car.
*/
int updateCarState(char command) {
        switch (command) {
                case 0: /* left */
                	carstate.left = 1;
                        break;
                case 1: /* up */
					if(!disWarning)
						carstate.forward = 1;
					else	
						carstate.forward = 0;
				carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
                case 2: /* right */
                	carstate.right = 1;
					carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
                case 3: /* down */
                	carstate.back = 1;	
					carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
                case 4: /* stop left */
                	carstate.left = 0;
					carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
                case 5: /* stop*/
                	carstate.forward = 0;
					carstate.back = 0;
					carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
                case 6: /* stop right */
                	carstate.right = 0;
					carstate.trackenable = 0;
				carstate.autoAvoid = 0;
                        break;
				case 7: /* servo left */
                	carstate.servoLeft = 1;
                        break;
                case 8: /*servo right */
                	carstate.servoRight = 1;
                        break;
                case 9: /* servo up */
                	carstate.servoUp = 1;
                        break;
                case 10: /* servo down */
                	carstate.servoDown = 1;
                        break;
				case 11: /* enable track */
                	carstate.trackenable = 1;
                        break;
				case 12: /* disable track */
                	carstate.trackenable = 0;
                        break;
				case 13: /* enable track */
                	carstate.speedUp = 1;
                        break;
				case 14: /* disable track */
                	carstate.speedDown = 1;
                        break;
				case 15: /* disable track */
                //	carstate.beepenable = 1;
					digitalWrite(BEEP, HIGH);
					break;
				case 16: /* disable track */
                //	carstate.beepenable = 0;
					digitalWrite(BEEP, LOW);
                    break;
				case 17: /*automatic avoidance*/
					carstate.autoAvoid = 1;
					break; 
				case 18: /*stop automatic avoidance*/
					carstate.autoAvoid = 0;
					break; 
				case 19: /*turn off the robot car*/
					exit_UCTRONICS_Robot_Car();
					printf("power off\n");
					system("sudo poweroff");
					
					break; 
         }
return 0;
}
void ultraInit(void)    
{    
    pinMode(Echo, INPUT);   
    pinMode(Trig, OUTPUT);  
}    
    
float disMeasure(void)    
{    
    struct timeval tv1;    
    struct timeval tv2;    
    long start, stop;    
    float dis; 
	long waitCount = 0;
    
    digitalWrite(Trig, LOW);    
    delayMicroseconds(2);    
    
    digitalWrite(Trig, HIGH);    
    delayMicroseconds(10);        
    digitalWrite(Trig, LOW);    
     waitCount = 0;   
    while(!(digitalRead(Echo) == 1)){
		if(++waitCount>=5000)
			break;
	    else sleep(0.001);}    
    gettimeofday(&tv1, NULL);            
     waitCount = 0;
    while(!(digitalRead(Echo) == 0)){
		if(++waitCount>=5000)
			break;
	    else sleep(0.001);}      
    gettimeofday(&tv2, NULL);             
    /*  
    int gettimeofday(struct timeval *tv, struct timezone *tz);  
    The functions gettimeofday() and settimeofday() can get and set the time as well as a timezone.   
    The use of the timezone structure is obsolete; the tz argument should normally be specified as NULL.  
    */  
    start = tv1.tv_sec * 1000000 + tv1.tv_usec;       
    stop  = tv2.tv_sec * 1000000 + tv2.tv_usec;    
    
    dis = (float)(stop - start) / 1000000 * 34000 / 2;   
    
    return dis;    
}  

void trackModeInit(){
	INP_GPIO(leftSensor);
	GPIO_PULL = 1 <<leftSensor;
	INP_GPIO(middleSensor);
	GPIO_PULL = 1 <<middleSensor;
	INP_GPIO(rightSensor);
	GPIO_PULL = 1 <<rightSensor;
	
	
}
void beepInit(){
	 pinMode(BEEP, OUTPUT);	
	 digitalWrite(BEEP, LOW);
}
void getLedSta(){

	static unsigned long previous_time = 0;
	static unsigned long now_time = 0;
	static unsigned long time_stamp = 0;
	static unsigned char flag = 0;
	static unsigned char colour_index = 0;
	if(!disWarning){
		
		if(!client_Connected){
			if(!flag){
				flag = 1;
				previous_time = get_pwm_timestamp();
			}
		now_time = get_pwm_timestamp();
		time_stamp = now_time - previous_time;
		if(time_stamp > 500000){
			time_stamp = 0;
			flag = 0;
			colour_index = (1+colour_index)%8;
			GRB_work(1, grb_colour_table[colour_index], 50);
			}
		}
	}

	
}
void beepWarning(){
	static unsigned long previous_time = 0;
	static unsigned long now_time = 0;
	static unsigned long time_stamp = 0;
	static unsigned char flag = 0;
	static unsigned char canStopBeep = 0;
	if(disWarning){
		canStopBeep = 1;
	if(!flag){
		flag = 1;
		previous_time = get_pwm_timestamp();
	}
	now_time = get_pwm_timestamp();
	time_stamp = now_time - previous_time;
	if(time_stamp > 0 && time_stamp <= 50000 ){   //1/2T
		digitalWrite(BEEP, HIGH);
		GRB_work(1, 0,0);
	}
	if(time_stamp >50000 && time_stamp <= 2*50000 ){   //1T
		digitalWrite(BEEP, LOW);
		GRB_work(1, getColour,getBrightness);
	}
	if(time_stamp > 2*50000){
		flag = 0;
	}
	}else if(canStopBeep){
		canStopBeep = 0;
		digitalWrite(BEEP, LOW);
		GRB_work(1, getColour,getBrightness);
	}
}

void trackModeWork(){
	int num1 = 0, num2 = 0, num3 = 0;
	while(1){
	if(!(carstate.trackenable))break;
	
	mySoftPwmWrite1(speedVal_1);
	mySoftPwmWrite2(speedVal_2);
	mySoftPwmWrite3(speedVal_3);
	mySoftPwmWrite4(speedVal_4);
	
	
    num1 = GET_GPIO(leftSensor);
    num2 = GET_GPIO(middleSensor);
    num3 = GET_GPIO(rightSensor);
    if ((num2 == 0) && (num1 == 0) && (num3 == 0)) {
		stop(); continue;
    } else if ( (num1 == 0) && num3) { //go to right
		go_forward_left();
		while (1) {
			num2 = GET_GPIO(middleSensor);
			mySoftPwmWrite1(speedVal_1);
			mySoftPwmWrite2(speedVal_2);
			mySoftPwmWrite3(speedVal_3);
			mySoftPwmWrite4(speedVal_4);
			if (num2) {
				if(!(carstate.trackenable))break;
				if(disWarning){stop();}
				else{
					go_forward_left();
					}
				 continue;
			} else
				break;
      }
    } else if ((num3 == 0) && num1) { // go to left
		go_forward_right();
		while (1) {
			num2 = GET_GPIO(middleSensor);
			mySoftPwmWrite1(speedVal_1);
			mySoftPwmWrite2(speedVal_2);
			mySoftPwmWrite3(speedVal_3);
			mySoftPwmWrite4(speedVal_4);
			if (num2) {
				if(!(carstate.trackenable))break;
				if(disWarning){stop();}
				else{
					go_forward_right();
				}
				continue;
			} else
				break;
      }
    } else if(disWarning){
		stop();	
	}
	else{
			  go_forward();	
		}
    
	}
	
}  

unsigned char countLow(void){
		unsigned char i=0;

	while( digitalRead(IRIN)==0 );	// wait

	while( digitalRead(IRIN)==1 ){
		++i;
		delayMicroseconds(26);
		if(i==0) return 0;	// timeout
	}
	return i;
	
}

void getIR(){
	unsigned char i;
	unsigned short j;   // capable 32768 bits = 4096 bytes
	unsigned char k;
	bits=0;
	i = countLow();
	for(j=0;j<IR_LIMITS;j++){  // buffer bytes LIMITS
	for(i=0;i<8;i++){   // 8 bits
	  k = countLow();
	  if(k==0){
	   buf[j]>>=(8-i);
	   done=1;printf("  \n");
	   return;
	  }
	  buf[j]>>=1;
	  buf[j]+=((k>30) ? 0x80: 0);
	  ++bits;
	}	
    }
  done=1;
}
void turn() {
  go_back();
  delay(turnTime);
  stop();
    go_left();
  delay(turnTime);
  stop();
}

void avoidance(void)
{
  unsigned char cnt = 0;
  float dis = 0, temp_min = 0, temp_max = 0, temp_value = 0, temp = 0;	  
  while (1) {
	if(!(carstate.autoAvoid))break;
	mySoftPwmWrite1(speedVal_1);
	mySoftPwmWrite2(speedVal_2);
	mySoftPwmWrite3(speedVal_3);
	mySoftPwmWrite4(speedVal_4);
	temp_min  = temp_max;
	temp_value= temp_max;
	for(cnt = 0; cnt < 3; cnt ++ ){
	   temp = disMeasure();sleep(0.01);  
	   if(temp_max<temp)	temp_max = temp;
	   if(temp_min>temp)	temp_min = temp;
	   temp_value += temp;
         }
       dis = (temp_value-temp_max-temp_min);
       temp_value = 0;temp_min = 0; temp_max = 0; temp_value = 0; temp = 0;
    if (dis <= 30 && dis > 0 ) {
      BEEP_INT();
      turn();
    } else
      go_forward();
  }
}
void BEEP_INT (void) {
  digitalWrite(BEEP, HIGH);
  delay(beepFrequence);
  digitalWrite(BEEP, LOW);
  delay(beepFrequence);
}

void BEEP_OPEN (void) {
  digitalWrite(BEEP, HIGH);
  delay(beepFrequence * 5);
  digitalWrite(BEEP, LOW);
}

// Set up a memory regions to access GPIO
void setup_io()
{
   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }
   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );
 
   close(mem_fd); //No need to keep mem_fd open after mmap
 
   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }
   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;
} // setup_io


uint64_t get_pwm_timestamp()
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC_RAW, &t) != 0) {
        return 0;
    }
    return (uint64_t) t.tv_sec * 1000000 + t.tv_nsec / 1000;
}

void mySoftPwmWrite1( int value)
{
	static int previous_time = 0;
	static int now_time = 0;
	static int time_stamp = 0;
	static  unsigned char flag1 = 0;
	
	if(!flag1){
		flag1 = 1;
		previous_time = get_pwm_timestamp();
	}
	now_time = get_pwm_timestamp();
	time_stamp = now_time - previous_time;
	if(time_stamp > 0 && time_stamp <= 5000 ){   //1/2T
		if(time_stamp <= value ){
			GPIO_SET = 1<<MOTOR_1_PWM;
		}
		 else{	 
			GPIO_CLR = 1<<MOTOR_1_PWM;
		 }
	}
	if(time_stamp >5000 && time_stamp <= 2*5000 ){   //1T
		if(time_stamp <= value ){
			GPIO_SET = 1<<MOTOR_1_PWM;
			}
		 else{	 
			GPIO_CLR = 1<<MOTOR_1_PWM;
		 }
	}
	if(time_stamp > 2*5000){
		flag1 = 0;
	}
}
void mySoftPwmWrite2( int value)
{
	static int previous_time = 0;
	static int now_time = 0;
	static int time_stamp = 0;
	static  unsigned char flag2 = 0;
	if(!flag2){
		flag2 = 1;
		previous_time = get_pwm_timestamp();
	}
	now_time = get_pwm_timestamp();
	time_stamp = now_time - previous_time;
	if(time_stamp > 0 && time_stamp <= 5000 ){   //1/2T
		if(time_stamp <= value ){
			GPIO_SET = 1 << MOTOR_2_PWM;
		}
		 else{	 
			GPIO_CLR = 1<< MOTOR_2_PWM;
		 }
	}
	if(time_stamp >5000 && time_stamp <= 2*5000 ){   //1T
		if(time_stamp <= value ){
			GPIO_SET = 1 << MOTOR_2_PWM;
		}
		 else{	 
			GPIO_CLR = 1<< MOTOR_2_PWM;
		 }	
	}
	if(time_stamp > 2*5000){
		flag2 = 0;
	}
}
void mySoftPwmWrite3( int value)
{
	static int previous_time = 0;
	static int now_time = 0;
	static int time_stamp = 0;
	static  unsigned char flag3 = 0;
	if(!flag3){
		flag3 = 1;
		previous_time = get_pwm_timestamp();
	}
	now_time = get_pwm_timestamp();
	time_stamp = now_time - previous_time;
	if(time_stamp > 0 && time_stamp <= 5000 ){   //1/2T
	if(time_stamp <= value ){
	  GPIO_SET = 1 << MOTOR_3_PWM;
	}
	else{	 
	    GPIO_CLR = 1 << MOTOR_3_PWM;
	}
	}
	if(time_stamp >5000 && time_stamp <= 2*5000 ){   //1T
	if(time_stamp <= value ){
	GPIO_SET = 1 << MOTOR_3_PWM;
	}
        else{	 
	  GPIO_CLR = 1 << MOTOR_3_PWM;
	}
	}
	if(time_stamp > 2*5000){
	   flag3 = 0;
	}
}
void mySoftPwmWrite4( int value)
{
	static int previous_time = 0;
	static int now_time = 0;
	static int time_stamp = 0;
	static  unsigned char flag4 = 0;
	
	if(!flag4){
	flag4 = 1;
	previous_time = get_pwm_timestamp();
	}
	now_time = get_pwm_timestamp();
	time_stamp = now_time - previous_time;
	if(time_stamp > 0 && time_stamp <= 5000 ){   //1/2T
	if(time_stamp <= value ){
	GPIO_SET = 1 << MOTOR_4_PWM;
	}
	else{	 
	GPIO_CLR = 1 << MOTOR_4_PWM;
	}
	}
	if(time_stamp >5000 && time_stamp <= 2*5000 ){   //1T
	if(time_stamp <= value ){
	  GPIO_SET = 1 << MOTOR_4_PWM;
	}
	else{	 
	GPIO_CLR = 1 << MOTOR_4_PWM;
	 }
	}
	if(time_stamp > 2*5000){
	   flag4 = 0;
	}
}

void servoInit(void)
{
   pinMode(servo_1, OUTPUT);
   digitalWrite(servo_1, 0);
   pinMode(servo_2, OUTPUT);
   digitalWrite(servo_2, 0);
}

void servoCtrl(int servoNum, int dutyCycle)
{
   digitalWrite(servoNum, 1);
   delayMicroseconds(dutyCycle);
   digitalWrite(servoNum, 0);
   delayMicroseconds(25000 - dutyCycle);
}

	
void GRB_work(unsigned int ledNum, unsigned long colour, int brightness ){
  int i;
  ledstring.channel[0].brightness = brightness; 
  for(i = 0; i<ledNum; i++){
	ledstring.channel[0].leds[i] = colour; 
   }  	
   ws2811_render(&ledstring) ;

}
void irInit(){
  pinMode(IRIN,INPUT);
  pullUpDnControl(IRIN,PUD_UP);
   // to detect falling edge
   wiringPiISR(IRIN,INT_EDGE_FALLING,(void*)getIR);
   done=0;
}
void myPWMInit(){	
  // Switch GPIO 7..11 to output mode
    INP_GPIO(MOTOR_1_PWM); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(MOTOR_1_PWM);
    INP_GPIO(MOTOR_2_PWM); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(MOTOR_2_PWM);
    INP_GPIO(MOTOR_3_PWM); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(MOTOR_3_PWM);
    INP_GPIO(MOTOR_4_PWM); // must use INP_GPIO before we can use OUT_GPIO
    OUT_GPIO(MOTOR_4_PWM);
}
void GRBInit(){
  ws2811_return_t ret;
  matrix = malloc(sizeof(ws2811_led_t) * width * height);
  if ((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
   {
       fprintf(stderr, "ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
   }
	
}

void exit_UCTRONICS_Robot_Car(void)
{
	int pulsenum;
	client_Connected = 0;
	close(sockfd); 
	for(pulsenum = 0; pulsenum <10; pulsenum++){
		servoCtrl(servo_1,1300);
		servoCtrl(servo_2,1300);
	}
	 digitalWrite(BEEP, LOW);
	 GRB_work(1, 0,0 );
	
}

void  INThandler(int sig)
{
     char  c;
     signal(sig, SIG_IGN);
     printf("You hit Ctrl-C\n"
            "Do you really want to quit? [y/n] ");
     c = getchar();
     if (c == 'y' || c == 'Y'){
	exit_UCTRONICS_Robot_Car();
      printf("Bye.\n");
		exit(0);
	 }
          
     else
          signal(SIGINT, INThandler);
     getchar(); // Get new line character
}
