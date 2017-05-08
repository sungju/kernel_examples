/* pci_probe_test.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/init.h>

struct pci_device_id ids[] = {
	{PCI_DEVICE(PCI_ANY_ID, PCI_ANY_ID),},
	{0,}
};

MODULE_DEVICE_TABLE(pci, ids);

int my_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	u8 bval;
	u16 wval;

	/* pci_enabled_device(dev); */
	pci_read_config_word(dev, PCI_VENDOR_ID, &wval);
	pci_read_config_byte(dev, PCI_REVISION_ID, &bval);

	printk(" Vendor ID = 0x%x, revision = 0x%x\n", wval, bval);
	return 0;
}

void remove(struct pci_dev *dev)
{
	printk("device removed\n");
}

struct pci_driver my_driver = {
	.name = "my_pci",
	.id_table = ids,
	.probe = my_probe,
	.remove = remove,
};

int __init my_init(void)
{
	return pci_register_driver(&my_driver);
}

void __exit my_exit(void)
{
	pci_unregister_driver(&my_driver);
}

module_init(my_init);
module_exit(my_exit);
