#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

#define MYDEV_NAME "mycdrv"
#define KBUF_SIZE (size_t)(10*PAGE_SIZE)

static char *kbuf;
static dev_t first;
static unsigned int count = 1;
static struct cdev *my_cdev;

static int mycdrv_open(struct inode *inode, struct file *file)
{        
  static int counter = 0;
  printk(" attempting to open device: %s:\n", MYDEV_NAME);
  printk(" MAJOR number = %d, MINOR number = %d\n",
         imajor(inode), iminor(inode));
  counter++;

  printk(" successfully open  device: %s:\n\n", MYDEV_NAME);
  printk("I have been opened  %d times since being loaded\n", counter);
  printk("ref=%ld\n", module_refcount(THIS_MODULE));

  return 0;
}   

static int mycdrv_release(struct inode *inode, struct file *file)
{ 
  printk(" CLOSING device: %s:\n\n", MYDEV_NAME);
  return 0;
}

static ssize_t
mycdrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
  int nbytes, maxbytes, bytes_to_do;
  maxbytes = KBUF_SIZE - *ppos;
  bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
  if (bytes_to_do == 0)
    printk("Reached end of the device on a read");
  nbytes = bytes_to_do - copy_to_user(buf, kbuf + *ppos, bytes_to_do);
  *ppos += nbytes;
  printk("\n Leaving the   READ function, nbytes=%d, pos=%d\n", nbytes,
         (int)*ppos);
  return nbytes;
}

static ssize_t
mycdrv_write(struct file *file, const char __user * buf, size_t lbuf,
       loff_t * ppos)
{
  int nbytes, maxbytes, bytes_to_do;
  maxbytes = KBUF_SIZE - *ppos;
  bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
  if (bytes_to_do == 0)
    printk("Reached end of the device on a write");
  nbytes = bytes_to_do - copy_from_user(kbuf + *ppos, buf, bytes_to_do);
  *ppos += nbytes;
  printk("\n Leaving the   WRITE function, nbytes=%d, pos=%d\n", nbytes,
         (int)*ppos);
  return nbytes;
}

static loff_t mycdrv_lseek(struct file *file, loff_t offset, int orig)
{
  loff_t testpos;
  switch (orig) {
  case 0:   /* SEEK_SET */
    testpos = offset;
    break;
  case 1:   /* SEEK_CUR */
    testpos = file->f_pos + offset;
    break;
  case 2:   /* SEEK_END */
    testpos = KBUF_SIZE + offset;
    break;
  default:
    return -EINVAL;
  }
  testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
  testpos = testpos >= 0 ? testpos : 0;
  file->f_pos = testpos;
    printk("Seeking to pos=%ld\n", (long)testpos);
  return testpos;
}

static const struct file_operations mycdrv_fops = {
  .owner = THIS_MODULE,
  .read = mycdrv_read,
  .write = mycdrv_write,
  .open = mycdrv_open,
  .release = mycdrv_release,
  .llseek = mycdrv_lseek
};

static int __init my_init(void)
{
  if (alloc_chrdev_region(&first, 0, count, MYDEV_NAME) < 0) {
    printk("failed to allocate character device region\n");
    return -1;
  }
  if (!(my_cdev = cdev_alloc())) {
    printk("cdev_alloc() failed\n");
    unregister_chrdev_region(first, count);
    return -1;
  }
  cdev_init(my_cdev, &mycdrv_fops);
  kbuf = kmalloc(KBUF_SIZE, GFP_KERNEL);
  if (cdev_add(my_cdev, first, count) < 0) {
    printk("cdev_add() failed\n");
    cdev_del(my_cdev);
    unregister_chrdev_region(first, count);
    kfree(kbuf);
    return -1;
  }

  printk("\nSucceeded in registering character device %s\n", MYDEV_NAME);
  printk("Major number = %d, Minor number = %d\n",
         MAJOR(first), MINOR(first));
  return 0;
}

static void __exit my_exit(void)
{
  if (my_cdev)
    cdev_del(my_cdev);
  unregister_chrdev_region(first, count);
  if (kbuf)
    kfree(kbuf);
  printk("\ndevice unregistered\n");
}

module_init(my_init);
module_exit(my_exit);
