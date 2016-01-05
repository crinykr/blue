sudo rm /mnt/tmp/boot/vmlinuz-2.6.32.63-kiwi
sudo cp /home/criny/kiwi/eclipse_workspace/kiwi/linux-2.6.32.63-kiwi/arch/x86/boot/bzImage /mnt/tmp/boot/vmlinuz-2.6.32.63-kiwi
sync
VirtualBox --startvm kiwi --debug-command-line --start-running
