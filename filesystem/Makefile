obj-m += my_proc.o my_new_proc.o sig_proc.o my_sysfs.o

export KROOT=/lib/modules/`uname -r`/build

allofit:   modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	rm -rf   *.o *.ko .*cmd *.mod.c *.c~ .tmp_versions .*.d .*.tmp Module.symvers modules.order
