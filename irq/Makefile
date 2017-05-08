obj-m += myinterrupt.o simple_tasklet.o irq_tasklet.o irq_delayed.o

export KROOT=/lib/modules/`uname -r`/build

allofit:   modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions .*.d .*.tmp Module.symvers modules.order
