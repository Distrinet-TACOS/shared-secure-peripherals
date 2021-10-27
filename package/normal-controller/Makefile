ifneq ($(KERNELRELEASE),)

obj-m := normal-controller.o
ccflags-y := -I$(OPTEE_OS_SRC_DIR)/core/include/drivers

else

PWD := $(shell pwd)
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) CROSS_COMPLIE="ccache arm-linux-gnueabihf-" ARCH=arm LOCALVERSION= modules
	
endif

clean:
	rm -rf .*.cmd *.ko *.mod* *.o modules* Module*
