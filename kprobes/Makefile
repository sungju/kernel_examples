obj-m += kprobe_test.o jprobe_test.o efi_reboot_monitor.o

export KROOT=/lib/modules/`uname -r`/build

allofit:   modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	rm -rf   *.o *.ko .*cmd *.mod.c *.c~ .tmp_versions .*.d .*.tmp Module.symvers modules.order
