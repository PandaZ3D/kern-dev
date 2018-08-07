KDIR=/boot/kern-dev/linux-4.14.53/
MOD=block_read

obj-m += $(MOD).o

ARG = "/dev/sdb1" # default

# parameter format:
# sudo insmod $(MOD).ko path="/dev/path"	

all:
	sudo make -C $(KDIR) M=$(PWD)
clean:
	sudo make -C $(KDIR) M=$(PWD) clean
install: 
	sudo insmod $(MOD).ko path=${ARG}
remove:
	sudo rmmod $(MOD)
check:
	lsmod | grep $(MOD)

re: clean all install remove
	dmesg