#!/bin/bash 
#VM_IP=$(cat /cygdrive/c/programdata/vmware/vmnetdhcp.leases | python find_ip.py)
#echo "Found VM at $VM_IP"
#echo "Rsyncing data"
cd /home/pvnick/
#rsync --delay-updates --delete -ave "ssh -i '/home/pvnick/.ssh/id_rsa'" dev_zone/* "root@$VM_IP:dev_zone"