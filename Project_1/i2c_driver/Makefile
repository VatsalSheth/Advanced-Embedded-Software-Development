LINUXDIR := $(HOME)/buildroot/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220
COMPILEDIR := $(HOME)/buildroot/buildroot/output/host/bin/

obj-m += i2c_driver.o

all:
	make ARCH=arm CROSS_COMPILE=$(COMPILEDIR)arm-buildroot-linux-uclibcgnueabihf- -C $(LINUXDIR) M=$(shell pwd) modules

clean:
	make -C $(LINUXDIR) M=$(shell pwd) clean
