
#define Trig   25 //5// 25    
#define Echo    4    
#define BEEP    26


#define servo_1 4 //7  //wiringPi 
#define servo_2 25 //6

#define IRIN  5 //25//5


#define leftSensor   17  // BCM 17  //define the pin of track module
#define middleSensor  27 // BCM 27
#define rightSensor   22 // BCM 22

#define IR_LIMITS  64  // bytes buffer = IR_LIMITS x8 bits

/*******************For old IR controller*****************/
#define IR_up    0x46b9
#define IR_down  0x15ea
#define IR_Left  0x44bb
#define IR_right 0x43bc
#define IR_stop  0x40bf

/******************For new IR controller****************************/
#define IR_up_v2   	 0x18E7        //up
#define IR_down_v2  	 0x52AD        //down
#define IR_Left_v2  	 0x08F7        //left
#define IR_right_v2 	 0x5AA5        //right
#define IR_stop_v2  	 0x1CE3        //stop


#if 1
	#define IR_speed_up      0x16e9
	#define IR_speed_down    0x19e6

	#define IR_servo_up      0x0cf3
	#define IR_servo_down    0x18e7

	#define IR_servo_left    0x08f7
	#define IR_servo_right   0x1ce3

	#define IR_track         0x42bd
	#define IR_track_stop    0x4ab5

#else
	#define IR_speed_up      0x45ba		//1
	#define IR_speed_down    0x46b9		//2

	#define IR_servo_up      0x47B8		//3
	#define IR_servo_down    0x44bb		//4

	#define IR_servo_left    0x40bf		//5
	#define IR_servo_right   0x43bc		//6

	#define IR_track         0x07f8		//7
	#define IR_track_stop    0x15ea		//8

#endif


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
 
#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
 
#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH
 
#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

#define CODE1	{GPIO_SET = 1<<4;delayns(800); GPIO_CLR = 1<<4;delayns(400); }
#define CODE0	{GPIO_SET = 1<<4;delayns(400); GPIO_CLR = 1<<4;delayns(800); }
#define RES	{GPIO_CLR = 1<<4;delayns(800);delayns(50000);delayns(50000);}



 		
#define BUFFER_SIZE 10

#define halfPWMPeriod   5000  //5ms

#define  turnTime        500000    //500ms

struct motionstate {
        unsigned int left:1;
        unsigned int forward:1;
        unsigned int right:1;
        unsigned int back:1;
		unsigned int servoLeft:1;
		unsigned int servoRight:1;
		unsigned int servoUp:1;
		unsigned int servoDown:1;
		unsigned int speedUp:1;
		unsigned int speedDown:1;
		unsigned int trackenable:1;
		unsigned int trackdisable:1;
		unsigned int beepenable:1;
		unsigned int autoAvoid:1;
		unsigned int stop :1;
        }; 
		

void trackModeInit();
void trackModeWork();
void ultraInit(void);
float disMeasure(void);
int updateCarMotion(void);
int updateCarState(char command);
void  clearFlag(void);
void beepWarning(void);
void beepInit(void);
void ledInit(void);
void servoInit(void);
void servoCtrl(int servoNum, int dutyCycle);
void servoAControl( int value);

void getLedSta();
unsigned char countLow(void);
void getIR(void );
int IR_updateCarState(int command) ;
int IR_updateCarMotion (); 
void turn();
void avoidance(void);
void BEEP_INT (void);
void BEEP_OPEN (void);
void delayns(int n);
void GRB_Set(unsigned char gdat,unsigned char rdat,unsigned char bdat);
void GRB_MultiColour_work(unsigned int ledNum,  int brightness );
void setup_io();

void *fun1(void *arg);
void *fun2(void *arg);

uint64_t get_pwm_timestamp();
int updateCarMotion(void);

int updateCarState(char command);

void mySoftPwmWrite1( int value);
void mySoftPwmWrite2( int value);
void mySoftPwmWrite3( int value);
void mySoftPwmWrite4( int value);

void GRBInit();
void GRB_work(unsigned int ledNum, unsigned long colour, int brightness );
void irInit();
void myPWMInit();

