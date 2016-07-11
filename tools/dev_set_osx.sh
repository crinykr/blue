if [ $1 == "mount" ]; then
	vdmutil attach -nomount "/Users/criny/VirtualBox VMs/blue/blue.vdi"
	diskutil mount -mountPoint /tmp/blue /dev/disk1s1
elif [ $1 == "umount" ]; then
	diskutil unmount /tmp/blue
fi
