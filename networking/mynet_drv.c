/* mynet_drv.c */

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/etherdevice.h>

static struct net_device *dev;
static struct net_device_stats *stats;

static void my_rx(struct sk_buff *skb, struct net_device *dev)
{
	/* just a loopback, already has the skb */
	printk("I'm receiving a packet\n");
	++stats->rx_packets;
	netif_rx(skb);
}

static int my_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int i;

	printk("my_hard_start_xmit(%s)\n", dev->name);
	dev->trans_start = jiffies;
	printk("Sending packet :\n");
	/* print out 16 bytes per line */
	for (i = 0; i < skb->len; ++i) {
		if ((i & 0xf) == 0)
			printk("\n ");
		printk("%02x ", skb->data[i]);
	}
	printk("\n");
	++stats->tx_packets;

	skb->protocol = eth_type_trans(skb, dev);

	/* loopback it */
	/* In the real network device, it should send it through
	 * Network hardware such as ethernet card.
	 * Here we are just send it back to kernel by calling netif_rx().
	 */
	my_rx(skb, dev);
	return NETDEV_TX_OK;
}

static int my_do_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	printk("my_do_ioctl(%s)\n", dev->name);
	/* Nothing to control here */
	/* You may want to check 'cmd' to do the actual operations */
	return -1;
}

static struct net_device_stats *my_get_stats(struct net_device *dev)
{
	printk("my_get_stats(%s)\n", dev->name);
	return stats;
}

/*
* This is where ifconfig comes down and tells us who we are, etc. * We can just ignore this.
*/
static int my_config(struct net_device *dev, struct ifmap *map)
{
	printk("my_config(%s)\n", dev->name);
	if (dev->flags & IFF_UP) {
		return -EBUSY;
	}
	return 0;
}

static int my_change_mtu(struct net_device *dev, int new_mtu)
{
	printk("my_change_mtu(%s)\n", dev->name);
	/* MTU changing is not allowed in this device */
	return -1;
}

static int my_open(struct net_device *dev)
{
	printk("my_open(%s)\n", dev->name);
	/* start up the transmission queue */
	/* Until this call, the device won't be available */
	netif_start_queue(dev);
	return 0;
}

static int my_close(struct net_device *dev)
{
	printk("my_close(%s)\n", dev->name);
	/* shutdown the transmission queue */
	netif_stop_queue(dev);
	return 0;
}

static const struct net_device_ops mynet_netdev_ops = {
	.ndo_open = my_open,
	.ndo_stop = my_close,
	.ndo_start_xmit = my_hard_start_xmit,
	.ndo_get_stats = my_get_stats,
	.ndo_do_ioctl = my_do_ioctl,
	.ndo_set_config = my_config,
	.ndo_change_mtu = my_change_mtu,
};

static void my_setup(struct net_device *dev)
{
	int j;

	printk("my_setup(%s)\n", dev->name);
	/* Fill in the MAC address with '00:01:02:03:04:05' */
	for (j = 0; j < ETH_ALEN; ++j) {
		dev->dev_addr[j] = (char)j;
	}
	/* Fill the data with ethernet specific values/operations */
	ether_setup(dev);

	dev->netdev_ops = &mynet_netdev_ops;
	/* We are not setting dev->ethtool_ops as it's not an actual 
	 * ethernet device and ethool operations are not required */

	/* Not going to use ARP just like loopback device */
	dev->flags |= IFF_NOARP;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
	stats = (struct net_device_stats *)
	    kmalloc(sizeof(struct net_device_stats), GFP_KERNEL);
#else
	stats = &dev->stats;
#endif
}

static int __init my_init(void)
{
	printk("Loading transmitting network module:....");
	dev = alloc_netdev(0, "mynet%d", my_setup);
	if (register_netdev(dev)) {
		printk(" Failed to register\n");
		free_netdev(dev);
		return -1;
	}
	printk("Succeeded!\n\n");
	return 0;
}

static void __exit my_exit(void)
{
	printk("Unloading transmitting network module\n\n");
	unregister_netdev(dev);
	free_netdev(dev);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
	kfree(stats);
#endif
}

module_init(my_init);
module_exit(my_exit);
