# UCTRONICS_Smart_Robot_Car_RaspberryPi
## Add support for Pi4 B
- Just a quick post to let you know that you’ll need a new wiringPi for the Raspberry Pi 4B.
- To upgrade:
```bash
cd /tmp
```
```bash
wget https://project-downloads.drogon.net/wiringpi-latest.deb
```
```bash
sudo dpkg -i wiringpi-latest.deb
```
## Quick start
- Step 1: Insert the SD card with Smart_Robot_Car_K0073 firmware to the RPI.
- Step 2: Power on and wait for the system to start normally. The colored lights on the board will flash alternately.
- Step 3: Open the RobotCarKit app and search the 'UCTRONICS' access point and connect it with password '12345678'.
- Step 4: Choose the K0073 mode and it will connect the robot car with the default IP address and control port automatically.

## Configure the AP/STA mode 
We advise you use the AP mode to control the robot car. Our firmware’s default mode is AP.
- For STA mode ,enter the following command in the terminal
```Bash
$ cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/
$ sudo chmod +x sta_Config.sh
$ sudo ./sta_Config.sh
$ sudo reboot
```
- For AP mode ,enter the following command in the terminal
```Bash
$ cd ~/UCTRONICS_Smart_Robot_Car_RaspberryPi/
$ sudo chmod +x ap_Config.sh
$ sudo ./ap_Config.sh
$ sudo reboot
```
## Detail configuration
If you use the official raspbian system instead of our Smart_Robot_Car_K0073.img firmware, You should refer to the detail configuration steps
  https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/C/Software%20User%20Guide.md

## Graphical programming
- You can download the UCBlock here
  ```bash
  https://drive.google.com/open?id=1raHnyCYRtVoVzZm49_MugTi5a_LqiRIg
  ```
- Connect the Smart Robot Car's wifi whose name is ‘UCTRONICS’ and password is '12345678'. 
 ![EasyBehavior](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/Connect_wifi.gif) 
- Open the UCBlock and connect the Robor Car with IP address: 192.168.1.1 and port 2001.
![EasyBehavior](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/connect_to_car.gif) 
- You can program for the Robot Car through the block easily.
![EasyBehavior](https://github.com/UCTRONICS/UCTRONICS_Smart_Robot_Car_RaspberryPi/blob/master/image/enjoy.gif) 


