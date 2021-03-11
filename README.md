PS/2 library for stm32duino boards.
Ultimate goal is to interface with Elantech trackpads.

Hardware:
stm32f103c8t6 blue pill
elan 33200v-3600
elan 33059v-3000

Credits:
queue.h/.cpp is a modified copy from https://github.com/adzierzanowski/ps2-to-usb
PS2.h/.cpp is inspired by the same repository

PS/2 mouse and elantech routines are being adapted from linux kernel
https://git.kernel.org/pub/scm/linux/kernel/git/next/linux-next.git/tree/drivers/input/mouse