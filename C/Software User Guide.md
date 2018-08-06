# Software User Guide

## 1. Download the project
- Enter the following command in the terminal to down the project:
```Bash
sudo git clone https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi.git
```
## 2. Install the necessary libraries
- Enter the following command in the terminal.
```Bash
sudo apt-get update
sudo apt-get install subversion
sudo apt-get install libjpeg8-dev
sudo apt-get install imagemagick
sudo apt-get install libv4l-dev
sudo apt-get install cmake
```
## 3. Install Mjpeg Streamer
- Enter the following command in the terminal.
```Bash
cd UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer-experimental
sudo make all
sudo make install
```
## 4. Add bcm2835-v4l2 module
- Enter the following command in the terminal.
```Bash
sudo nano /etc/modules
```
- Go to the end of the file and edit it so that it looks like the following:
```Bash
 bcm2835-v4l2
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/1.png)

- Save and restart the raspberry pie once configured.The camera is then tested to see if it works properly
```Bash
sudo reboot
ls /dev/video0
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/2.png)

## 5. Compile the source code 
- Enter the following command in the terminal
```Bash
cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
sudo make
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/3.png)

## 6. Start and run the demo
- Enter the following command in the terminal to start the camera.
```Bash
sudo chmod +x ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh
sudo ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh start
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/4.png)

- After the camera start successfully, open your browser and test it by inputting
 http://[your raspberry pi ip address]:8080/stream.html 
[Notice: you can check your raspberry pi ip address by running 'sudo ifconfig' ]
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/5.png)

- If anything is normal, you will see the video streaming on your browser like the following:
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/6.png)

- Enter the following command in the terminal to start the UCTRONICS Smart Robot Car
```Bash
cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
sudo ./UCTRONICS_Smart_Robot_Car_RaspberryPi
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/7.png)
- Now, open the “RobotCarKit” app and connect the robot with a control port: 2001 and video display port: 8080. After connect successfully, you can control your robot car by the app
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/8.png)

- If you want to exit the UCTRONICS Smart Robot Car project,just click ‘Ctrl+C’, then click ‘y’ or ‘Y’.
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/9.png)

- If you want to stop the camera from working, just enter the following command in the terminal
```Bash
sudo ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh stop
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/10.png)

## 7. Add automatic start support 
- Enter the following command in the terminal
```Bash
sudo nano /etc/rc.local
```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/11.png)

- Add the following command between “fi” and “exit 0” 
```Bash
if [ -c "/dev/video0" ]; then
# Start the camera
sudo chmod +x /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh
sudo /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/mjpg-streamer.sh start
fi

# Compile and start the robot car
cd /home/pi/UCTRONICS_Smart_Robot_Car_RaspberryPi/C/
sudo make clean
sudo make 
sudo ./UCTRONICS_Smart_Robot_Car_RaspberryPi

```
![Alt text](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/12.png)

- Then click ‘Ctrl’ + ‘X’ ,and then ‘Y’, Click ‘Enter’ to save the file. Now restart your raspberry pi
```Bash
sudo reboot 
```
## 8. Configure the wifi to AP /STA mode.
- We advise users use the AP mode to control the robot car. Our firmware’s default mode is AP.
- After powered on, users just need to search the ‘UCTRONICS’ access point and connect it with ‘12345678’password. 
- Of course, changing the wifi mode is very easy.

- For STA mode ,enter the following command in the terminal
```Bash
cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/
sudo chmod +x sta_Config.sh
sudo ./sta_Config.sh
sudo reboot
```
- For AP mode ,enter the following command in the terminal
```Bash
cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/
sudo chmod +x ap_Config.sh
sudo ./ap_Config.sh
sudo reboot
```














