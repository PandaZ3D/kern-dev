KDIR=/boot/kern-dev/linux-4.14.53/
MOD=hello_param

obj-m += $(MOD).o

ARG = "Safeway" # default

# parameter format:
# sudo insmod hello_param.ko msg="Banana" list="one,two,three,four"

all:
	sudo make -C $(KDIR) M=$(PWD)
clean:
	sudo make -C $(KDIR) M=$(PWD) clean
install: 
	sudo insmod $(MOD).ko msg=${ARG}
remove:
	sudo rmmod $(MOD)

re: clean all install remove
	dmesg

wd:
	echo $(PWD)