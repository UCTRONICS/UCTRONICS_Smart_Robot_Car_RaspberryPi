# UCTRONICS_Smart_Robot_Car_RaspberryPi

## Quick start
- Step 1: Insert the SD card with Smart_RobotCar_K0073 firmware to the RPI.
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

## Installation video demo 

## Operation video demo


