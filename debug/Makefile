obj-m += kprobe_zap.o task_proc.o zap_jprobe.o

export KROOT=/lib/modules/`uname -r`/build

allofit:   modules
modules:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules
modules_install:
	@$(MAKE) -C $(KROOT) M=$(PWD) modules_install
clean:
	rm -rf   *.o *.ko .*cmd *.mod.c .tmp_versions .*.d .*.tmp Module.symvers modules.order
