#!/bin/bash

make

sys_call_table_addr_hex=$(grep ' sys_call_table' /boot/System.map-$(uname -r) | awk '{ print "0x" $1}')
sys_call_table_addr_dec=$(printf "%u\n" $sys_call_table_addr_hex)

insmod ./mysyscalls.ko syscall_addr=$sys_call_table_addr_dec
