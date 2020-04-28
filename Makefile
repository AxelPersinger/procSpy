obj-m += lkm_procSpy.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
install:
	insmod lkm_procSpy.ko
uninstall:
	rmmod lkm_procSpy.ko
print:
	dmesg
