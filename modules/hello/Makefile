KDIR=/boot/kern-dev/linux-4.14.53/
MOD=hello-lkd

obj-m += $(MOD).o


all:
	sudo make -C $(KDIR) M=$(PWD)
clean:
	sudo make -C $(KDIR) M=$(PWD) clean
install: 
	sudo insmod $(MOD)
remove:
	sudo rmmod $(MOD)

wd:
	echo $(PWD)
