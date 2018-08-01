KDIR=/boot/kern-dev/linux-4.14.53/
MOD=fs_free

obj-m 	  := $(MOD).o

# ccflags-y := -Ifs/sysfs

all:
	sudo make -C $(KDIR) M=$(PWD)
clean:
	sudo make -C $(KDIR) M=$(PWD) clean
install: 
	sudo insmod $(MOD).ko
remove:
	sudo rmmod $(MOD)
check:
	lsmod | grep $(MOD)
