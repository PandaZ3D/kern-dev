KDIR=/boot/kern-dev/linux-4.14.53/
MOD=fs_free
SRC=free_blocks

obj-m 	  := $(MOD).o
$(MOD)-y  := $(SRC).o

ARG = "/dev/sdb1"

# ccflags-y := -Ifs/sysfs

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
