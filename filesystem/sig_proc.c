/* sig_proc.c */
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <asm/switch_to.h>

#define NODE_DIR	"my_sig_dir"
#define NODE_1		"pid"
#define NODE_2		"signal"

struct proc_dir_entry *proc_sigdir, *proc_pid, *proc_signal;
int sig_pid = -1, sig_tosend = SIGUSR1;
struct task_struct *sig_tsk = NULL;

ssize_t my_proc_read(struct file *file, char __user * buf,
		     size_t len, loff_t * offset)
{
	char data[1024];

	if (PDE_DATA(file_inode(file)) == &proc_pid)
		sprintf(data, "%d\n", sig_pid);
	if (PDE_DATA(file_inode(file)) == &proc_signal)
		sprintf(data, "%d\n", sig_tosend);

	if (*offset > strlen(data))
		return 0;
	*offset = strlen(data) + 1;
	return strlen(data) - copy_to_user(buf, data, strlen(data));

}

ssize_t my_proc_write(struct file * file, const char __user * buffer,
		      size_t count, loff_t * pos)
{
	char *str = kmalloc(count, GFP_KERNEL);
	if (copy_from_user(str, buffer, count)) {
		kfree(str);
		return -EFAULT;
	}
	if (PDE_DATA(file_inode(file)) == &proc_pid) {
		sscanf(str, "%d", &sig_pid);
		printk("sig_pid has been set to %d\n", sig_pid);
//              sig_tsk = find_task_by_pid_ns(sig_pid, &init_pid_ns);
		sig_tsk =
		    pid_task(find_pid_ns(sig_pid, &init_pid_ns), PIDTYPE_PID);
		kfree(str);
		return count;
	}
	if (PDE_DATA(file_inode(file)) == &proc_signal) {
		sscanf(str, "%d", &sig_tosend);
		printk("sig_tosend has been set to %d\n", sig_tosend);
		if (!sig_tsk) {
			sig_tsk = current;
			sig_pid = (int)current->pid;
		}
		printk("Send signal %d to process %d\n", sig_tosend, sig_pid);
		send_sig(sig_tosend, sig_tsk, 0);
		kfree(str);
		return count;
	}
	kfree(str);
	return -EINVAL;
}

struct file_operations myfops = {
	.read = my_proc_read,
	.write = my_proc_write,
};

int __init my_init(void)
{
	proc_sigdir = proc_mkdir(NODE_DIR, NULL);

	proc_pid =
	    proc_create_data(NODE_1, 0666, proc_sigdir, &myfops, &proc_pid);

	proc_signal =
	    proc_create_data(NODE_2, 0666, proc_sigdir, &myfops, &proc_signal);

	return 0;
}

void __exit my_exit(void)
{
	remove_proc_entry(NODE_1, proc_sigdir);
	remove_proc_entry(NODE_2, proc_sigdir);
	remove_proc_entry(NODE_DIR, NULL);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
