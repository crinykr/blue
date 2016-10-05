cp ../src/vmlinuz/vmlinuz-0-blue /mnt/blue/boot
sync
VirtualBox --startvm blue --debug-command-line --start-running
