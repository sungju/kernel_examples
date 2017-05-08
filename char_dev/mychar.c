#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h> 

#include <linux/slab.h>
#include <asm/uaccess.h>

#define MYDEV_NAME  "mycdrv"
#define KBUF_SIZE 10 * PAGE_SIZE

static char *kbuf;
static dev_t first; 
static unsigned int count = 10;
static int my_major = 900, my_minor = 0;
static struct cdev *my_cdev;

static int my_open(struct inode *inode, struct file *file) {
  printk(" OPEN device : %s\n", MYDEV_NAME);
  return 0;
} 
static int my_release(struct inode *inode, struct file *file) {
  printk(" CLOSE device : %s\n", MYDEV_NAME);
  return 0; 
} 

static ssize_t my_read(struct file *file, char __user *buf,
    size_t lbuf, loff_t *ppos) {
  int nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);
  *ppos += nbytes;
  printk("\n my_read, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
  return nbytes;
}
static ssize_t my_write(struct file *file, const char __user *buf,
    size_t lbuf, loff_t *ppos) {
  int nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
  *ppos += nbytes;
  printk("\n my_write, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
  return nbytes;
}
static loff_t my_lseek(struct file *file, loff_t offset, int orig) {
  loff_t testpos;
  switch (orig) {
    case 0: testpos = offset; break;
    case 1: testpos = file->f_pos + offset; break;
    case 2: testpos = KBUF_SIZE + offset; break;
    default: return -EINVAL;
  }
  testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
  testpos = testpos >= 0 ? testpos : 0;
  file->f_pos = testpos;
  return testpos;
}
static struct file_operations my_fops = {
  .llseek = my_lseek,
  .owner = THIS_MODULE,
  .open = my_open,
  .release = my_release,
  .read = my_read,
  .write = my_write,
};
static int __init my_init(void) {
  kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
  first = MKDEV(my_major, my_minor);
  register_chrdev_region(first, count, MYDEV_NAME);
  my_cdev = cdev_alloc();
  cdev_init(my_cdev, &my_fops);
  cdev_add(my_cdev, first, count);
  printk("Success\n");
  return 0;
}
static void __exit my_exit(void) {
  cdev_del(my_cdev);
  unregister_chrdev_region(first, count);
  kfree(kbuf);
  printk("\nunregistered\n");
}
module_init(my_init);
module_exit(my_exit);
