# in fedora, use kpartx additionally

if [ $1 == "mount" ]; then
	modprobe nbd

	qemu-nbd -c /dev/nbd0 /home/criny/VirtualBox\ VMs/blue/blue.vdi
	#kpartx -a /dev/nbd0
	mount /dev/nbd0p1 /mnt/blue
elif [ $1 == "umount" ]; then
	umount /mnt/blue
	#kpartx -d /dev/nbd0
	qemu-nbd -d /dev/nbd0
fi
