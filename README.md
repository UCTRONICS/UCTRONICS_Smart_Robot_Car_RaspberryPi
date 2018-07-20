# UCTRONICS_Smart_Robot_Car_RaspberryPi

## Quickly Start:

  - Insert the 16G SD card we send to your raspberryPi and power on.
  - Connect your RaspberryPi to your local router and get the IP address by inputting the sudo ifconfig in the terminal.
  - Search our RobotCarkit app in the google store and install it 
  - Open the RobotCarKit app and choose K0073. Then connect your Robot car with the ip address.
  
## Detail operations
   if you want to know the detail operations, please see the following user guide:

### Note: 
* Before you start this user guide, make sure your network connection is working.
* If you use our SD card with our UCTRONICS Smart Robot Car firmware, you can skip some steps and directly go to step 5. 
### Step1: Download the project
- Enter the following command in the terminal to down the project:
```Bash
 sudo git clone https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi.git
```
### Step2: Install the necessary libraries
- Enter the following command in the terminal.
```Bash
 sudo apt-get update
 sudo apt-get install subversion
 sudo apt-get install libjpeg8-dev
 sudo apt-get install imagemagick
 sudo apt-get install libv4l-dev
 sudo apt-get install cmake
```
### Step3: Install Mjpeg Streamer
- Enter the following command in the terminal.
```Bash
 cd UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer-experimental
 sudo make all
 sudo make install
```

### Step4: Add bcm2835-v4l2 module
- Enter the following command in the terminal.
 ```Bash
 sudo nano /etc/modules
 ```
- Go to the end of the file and edit it so that it looks like the following:
```Bash
bcm2835-v4l2
```
- Save and restart the raspberry pie once configured.The camera is then tested to see if it works properly
```Bash
sudo reboot
```
```Bash
ls /dev/video0
```
- If you can see the video0 device, congratulations, you have prepared successfully.

### Step5: Compile the source code 
- Enter the following command in the terminal.
 ```Bash
 cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
 sudo make
 ```
### Step6: Start and run the demo
- Enter the following command in the terminal to start the camera.
```Bash
 sudo chmod +x ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh
 sudo ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh start
```
- After the camera start successfully, open your browser and test it by inputting
  http://[your raspberry pi ip address]:8080/stream.html 
 [Notice: you can check your raspberry pi ip address by sudo ifconfig ]
- If anything is normal, you will see the video streaming on your browser like the following:

- Enter the following command in the terminal to start the UCTRONICS Smart Robot Car
```Bash
 cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
 sudo ./UCTRONICS_Smart_Robot_Car_RaspberryPi
```
- Now, open the “RobotCarKit” app and connect the robot with a control port: 2001 and video display port: 8080. After connect successfully, you can control your robot car by the app
- If you want to exit the UCTRONICS Smart Robot Car project,just click ‘Ctrl+C’, then click ‘y’ or ‘Y’.
- If you want to stop the camera from working, just enter the following command in the terminal
```Bash
sudo ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh stop
```


### Step7: Add automatic start support 
- Enter the following command in the terminal
```Bash
 sudo nano /etc/rc.local
```
- Add the following command between “fi” and “exit 0” 
```Bash
 # Start the camera
 sudo chmod +x /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh
 sudo /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh start
 # Compile and start the robot car
 cd /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
 sudo make
 sudo ./UCTRONICS_Smart_Robot_Car_RaspberryPi
```
- Then click ‘Ctrl’ + ‘X’ ,and then ‘Y’, Click ‘Enter’ to save the file. Now restart your raspberry pi 
```Bash
 sudo reboot 
```

## Video demo


 
