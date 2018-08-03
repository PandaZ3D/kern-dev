KDIR=/boot/kern-dev/linux-4.14.53/
MOD=block_info

obj-m += $(MOD).o

ARG = "/dev/sdb1" # default

# parameter format:
# sudo insmod $(MOD).ko str="Banana" list="one,two,three,four"	

all:
	sudo make -C $(KDIR) M=$(PWD)
clean:
	sudo make -C $(KDIR) M=$(PWD) clean
install: 
	sudo insmod $(MOD).ko dev=${ARG}
remove:
	sudo rmmod $(MOD)

re: clean all install remove
	dmesg

wd:
	echo $(PWD)