#modprobe nbd
#qemu-nbd -c /dev/nbd0 /works/kiwi/vbox/kiwi/kiwi.vdi
#kpartx -a /dev/nbd0
#mount /dev/nbd0p1 /mnt/tmp

umount /mnt/tmp
#kpartx -d /dev/nbd0
qemu-nbd -d /dev/nbd0
