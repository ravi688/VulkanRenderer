#!/bin/bash
WINDOWS_ENTRY=`grep menuentry /boot/grub/grub.cfg  | grep --line-number Windows`
MENU_NUMBER=$(( `echo $WINDOWS_ENTRY | sed -e "s/:.*//"` - 1 ))
sudo grub-reboot 'Windows Boot Manager (on /dev/nvme0n1p1)'
sleep 15; sudo reboot