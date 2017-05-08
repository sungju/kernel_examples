#!/bin/bash

make
insmod sbd.ko
fdisk /dev/sbd0
mkfs /dev/sbd0p1
mount /dev/sbd0p1 /mnt
echo "Test" > /mnt/file1
cat /mnt/file1
umount /mnt
rmmod sbd
make clean
