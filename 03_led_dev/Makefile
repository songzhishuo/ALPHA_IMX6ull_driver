#KERNELDIR := /home/zuozhongkai/linux/IMX6ULL/linux/temp/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek
# armv6
# KERNELDIR :=  /home/samba/share/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

# arm v7
KERNELDIR := /home/argon/Desktop/imx6u/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-
export  ARCH  CROSS_COMPILE

CURRENT_PATH := $(shell pwd)
obj-m := char_led.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH)  modules
	rm -rf /home/samba/share/module/*.ko
	cp -rf *.ko /home/samba/share/nfs/module
	
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
