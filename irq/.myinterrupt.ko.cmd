cmd_/root/Study/kernel_examples/irq/myinterrupt.ko := ld -r -m elf_x86_64 -T ./scripts/module-common.lds --build-id  -o /root/Study/kernel_examples/irq/myinterrupt.ko /root/Study/kernel_examples/irq/myinterrupt.o /root/Study/kernel_examples/irq/myinterrupt.mod.o
