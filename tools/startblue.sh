rm /mnt/tmp/boot/vmlinuz-0-blue
cp /works/blue/vmlinuz/vmlinuz-0-blue /mnt/tmp/boot
sync
VirtualBox --startvm kiwi --debug-command-line --start-running

# kiwi is ubuntu 10.04.4 32bit VirtualBox Image
