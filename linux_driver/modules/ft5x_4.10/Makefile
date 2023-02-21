ifneq ($(KERNELRELEASE),)

	obj-m := ft5316.o

else

KERNELDIR := /home/wangzl/workSpace/sun8iw5p1_Pro/sunxi-linux-kernel-4.10/sunxi_linux4.10

CURRENT_PATH := ${shell pwd}

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
	cp *.ko /home/wangzl/workSpace/sun8iw5p1_Pro/sunxi_nfs/obj_src/ -f
	
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean
	
endif
