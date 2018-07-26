#!/bin/sh
sudo cp wifiConfig/sta_dhcpcd.conf /etc/dhcpcd.conf
sudo cp wifiConfig/sta_dnsmasq.conf /etc/dnsmasq.conf
echo "STA mode configure OK, please run 'sudo ifconfig' to check the ip address"
echo "Please 'run sudo reboot' to reboot"
sudo reboot
