/* morse_io.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");

#define MORSE_CLK_FREQ  (1193180L)
#define MORSE_PIO       (0x61)
#define MORSE_PIT_CMD   (0x43)
#define MORSE_PIT_DATA  (0x42)
#define MORSE_SETUP     (0xB6)
#define MORSE_TONE_ON   (0x03)
#define MORSE_TONE_OFF  (0xFC)

void sound(int freq)
{
  unsigned int value = inb(MORSE_PIO);
  freq = MORSE_CLK_FREQ / freq;
  if ((value & MORSE_TONE_ON) == 0) {
    outb(value | MORSE_TONE_ON, MORSE_PIO);
    outb(MORSE_SETUP, MORSE_PIT_CMD);
  }
  outb(freq & 0xff, MORSE_PIT_DATA);
  outb((freq >> 8) & 0xff, MORSE_PIT_DATA);
}

void nosound(void)
{
  unsigned int value = inb(MORSE_PIO);
  value &= MORSE_TONE_OFF;
  outb(value, MORSE_PIO);
}

#define MORSE_SPACE_MASK  (1 << 15)
#define MORSE_BIT_MASK  (0xFE)
#define MORSE_UNIT_TIME (60)
#define MORSE_FREQUENCY (2000)

void morse_mdelay(int ms)
{
  set_current_state(TASK_INTERRUPTIBLE);
  schedule_timeout((ms * HZ) / 1000);
}

void send_dot(int minor_no)
{
  sound(MORSE_FREQUENCY * minor_no);
  morse_mdelay(MORSE_UNIT_TIME);
  nosound();
  morse_mdelay(MORSE_UNIT_TIME);
}

void send_dash(int minor_no)
{
  sound(MORSE_FREQUENCY * minor_no);
  morse_mdelay(MORSE_UNIT_TIME * 3);
  nosound();
  morse_mdelay(MORSE_UNIT_TIME);
}

void letter_space(int minor_no)
{
  morse_mdelay(MORSE_UNIT_TIME * 2);
}

void word_space(int minor_no)
{
  morse_mdelay(MORSE_UNIT_TIME * 4);
}

void morse(char *cp, int minor_no)
{
  unsigned int c;
  static unsigned int codes[64] = {
    MORSE_SPACE_MASK, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 115, 49, 106, 41,
    63, 62, 60, 56, 48, 32, 33, 35,
    39, 47, 0, 0, 0, 0, 0, 76,
    0, 6, 17, 21, 9, 2, 20, 11,
    16, 4, 30, 13, 18, 7, 5, 15,
    22, 27, 10, 8, 3, 12, 24, 14,
    25, 29, 19
  };
  while ((c = *cp++) != '\0') {
    if (c < 'A')
      continue;
    if (c >= 'a' && c <= 'z')
      c = c - 'a' + 'A';
    c -= ' ';
    if (c > 58)
      continue;
    c = codes[c];
    if (c & MORSE_SPACE_MASK) {
      word_space(minor_no);
      continue;
    }
    while (c & MORSE_BIT_MASK) {
      if (c & 1)
        send_dash(minor_no);
      else
        send_dot(minor_no);
      c >>= 1;
    }
    letter_space(minor_no);
  }
}

ssize_t m_write(struct file *filp, const char *buffer,
    size_t length, loff_t * offset)
{
  struct inode *inode = filp->f_dentry->d_inode;
  int minor_no = MINOR(inode->i_rdev) + 1;

  char *data = (char *)kmalloc(length, GFP_KERNEL);
  if (data == NULL)
    return 0;
  length = length - copy_from_user(data, buffer, length);
  data[length] = 0;
  morse(data, minor_no);
  kfree(data);

  return length;
}

struct file_operations m_fops = {
  .write = m_write
};

int major_no = 0;
module_param(major_no, int, 0);
MODULE_PARM_DESC(major_no, "major number");
int minor_count = 3;

struct cdev *morse_cdev;

dev_t dev_id;
#define DEV_NAME    "morse"

struct class *morse_class;

static int __init morse_init(void)
{
  int retval;
  int i;

  if (major_no) {
    dev_id = MKDEV(major_no, 0);
    retval = register_chrdev_region(dev_id, minor_count, DEV_NAME);
  } else {
    retval = alloc_chrdev_region(&dev_id, 0, minor_count, DEV_NAME);
    major_no = MAJOR(dev_id);
  }

  if (retval) {
    return -1;
  }

  morse_cdev = cdev_alloc();
  if (!morse_cdev) {
    unregister_chrdev_region(dev_id, minor_count);
    return -1;
  }

  cdev_init(morse_cdev, &m_fops);
  cdev_add(morse_cdev, dev_id, minor_count);

  morse_class = class_create(THIS_MODULE, DEV_NAME);
  for (i = 0; i < minor_count; i++) {
    dev_t node_no = MKDEV(MAJOR(dev_id), i);
    device_create(morse_class, NULL, node_no, NULL, DEV_NAME "%d",
            i);
  }

  return 0;
}

static void __exit morse_exit(void)
{
  int i;
  for (i = 0; i < minor_count; i++) {
    dev_t node_no = MKDEV(MAJOR(dev_id), i);
    device_destroy(morse_class, node_no);
  }
  class_destroy(morse_class);

  cdev_del(morse_cdev);
  kfree(morse_cdev);
  unregister_chrdev_region(dev_id, minor_count);
}

module_init(morse_init);
module_exit(morse_exit);
