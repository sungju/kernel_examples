obj-m += sema_mod1.o sema_mod2.o spindead.o

export KROOT=/lib/modules/`uname -r`/build

allofit:   modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	rm -rf   *.o *.ko .*cmd *.mod *.mod.c .tmp_versions .*.d .*.tmp Module.symvers modules.order
