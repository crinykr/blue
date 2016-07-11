# in fedora, use kpartx additionally

if [ $1 == "mount" ]; then
	modprobe nbd

	qemu-nbd -c /dev/nbd0 /works/oss/kiwi/vbox/kiwi/kiwi.vdi
	#kpartx -a /dev/nbd0
	mount /dev/nbd0p1 /mnt/tmp
elif [ $1 == "umount" ]; then
	umount /mnt/tmp
	#kpartx -d /dev/nbd0
	qemu-nbd -d /dev/nbd0
fi
