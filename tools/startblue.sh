cp ../vmlinuz/vmlinuz-0-blue /tmp/blue/boot
sync
VirtualBox --startvm blue --debug-command-line --start-running
