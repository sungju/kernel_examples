#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
#define class class_simple
#define class_create class_simple_create
#define device_create class_simple_device_add
#define device_destroy(class,first) \
    class_simple_device_remove(first)
#define class_destroy class_simple_destroy
#endif

struct class *my_class;

#define MYDEV_NAME  "mydrv"
#define KBUF_SIZE (int)(10 * PAGE_SIZE)

static char *kbuf;
static dev_t first;
static unsigned int count = 5;
static struct cdev *my_cdev;

int my_open(struct inode *inode, struct file *file) {
  static int counter = 0;
  printk(" my_open(%s)\n\n", MYDEV_NAME);
  printk(" major = %d, minor = %d\n",
    imajor(inode), iminor(inode));
  counter++;
  printk("counter = %d\n", counter);
  return 0;
}
int my_release(struct inode *inode, struct file *file) {
  printk(" my_release(%s)\n", MYDEV_NAME);
  return 0;
}
ssize_t my_read(struct file *file, char __user *buf,
    size_t lbuf, loff_t *ppos) {
  int nbytes, maxbytes, bytes_to_do;
  maxbytes = KBUF_SIZE - *ppos;
  bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
  if (bytes_to_do == 0) {
    printk("reach end of the device\n");
    return -ENOSPC;
  }
  nbytes = bytes_to_do - copy_to_user(buf, kbuf + *ppos,
        bytes_to_do);
  *ppos += nbytes;
  printk("\n my_read, nbytes = %d, pos = %d\n",
    nbytes, (int)*ppos);
  return nbytes;
}
char to_lower(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
  return c;
}
char to_upper(char c) { 
  if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
  return c;
} 
int convert(char *kbuf_str, size_t lbuf, int minor) {
  int i;
  char c;
  for (i = 0; i < lbuf; i++) {
    c = *(kbuf_str + i); 
    switch (minor) {
    case 1: c = to_lower(c); break;
    case 2: c = to_upper(c); break;
    }
    *(kbuf_str + i) = c;
  }
  return lbuf;
} 
ssize_t my_write(struct file *file, const char __user *buf,
    size_t lbuf, loff_t *ppos) {
  struct inode *inode = file->f_dentry->d_inode;
  int minor = MINOR(inode->i_rdev);
  int nbytes, maxbytes, bytes_to_do;
  maxbytes = KBUF_SIZE - *ppos;
  bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
  if (bytes_to_do == 0) {
    printk("Reach end of the device\n");
    return -ENOSPC;
  } 
  nbytes = bytes_to_do - copy_from_user(kbuf + *ppos,
      buf, bytes_to_do);
  convert(kbuf + *ppos, nbytes, minor);
  *ppos += nbytes;

  printk("\n my_write : nbytes = %d, pos = %d\n",
    nbytes, (int)*ppos);
  return nbytes;
}
struct file_operations my_fops = {
  .owner = THIS_MODULE,
  .read = my_read,
  .write = my_write,
  .open = my_open,
  .release = my_release,
};

int __init my_init(void) {
  int i;
  dev_t node_no;

  if (alloc_chrdev_region(&first, 0, count, MYDEV_NAME)<0) {
    printk("Failed to allocate character device\n");
    return -1;
  }
  if (!(my_cdev = cdev_alloc())) {
    printk("cdev_alloc() failed\n");
    unregister_chrdev_region(first, count);
    return -1;
  }
  cdev_init(my_cdev, &my_fops);
  kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
  if (cdev_add(my_cdev, first, count) < 0) {
    printk("cdev_add() failed\n");
    unregister_chrdev_region(first, count);
    return -1;
  } 

  my_class = class_create(THIS_MODULE, "mydrv");
  for (i = 0; i < count; i++) {
    node_no = MKDEV(MAJOR(first), i);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,13)
    device_create(my_class, node_no, NULL,
      "mydrv%d", i);
#else
    device_create(my_class, NULL, node_no,
      NULL, "mydrv%d", i);
#endif
  }

  return 0;
} 
void __exit my_exit(void) {
  int i;
  dev_t node_no;
  for (i = 0; i < count; i++) {
    node_no = MKDEV(MAJOR(first), i);
    device_destroy(my_class, node_no); 
  }
  class_destroy(my_class);

  if (my_cdev)
    cdev_del(my_cdev);
  unregister_chrdev_region(first, count);
  if (kbuf) kfree(kbuf);
}     
module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
