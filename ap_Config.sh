#!/bin/sh
sudo apt-get install dnsmasq hostapd
sudo systemctl stop dnsmasq
sudo systemctl stop hostapd
sudo cp wifiConfig/ap_dhcpcd.conf /etc/dhcpcd.conf
sudo cp wifiConfig/ap_dnsmasq.conf /etc/dnsmasq.conf
sudo cp wifiConfig/hostapd.conf  /etc/hostapd/hostapd.conf
sudo cp wifiConfig/hostapd /etc/default/hostapd
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl start hostapd
sudo systemctl start dnsmasq
echo "AP mode configure OK,please run 'sudo reboot' to reboot"
sudo reboot

