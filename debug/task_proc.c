/* task_proc.c */
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <asm/switch_to.h>
#include <linux/pid_namespace.h>

#define NODE_DIR	"my_task_dir"
#define NODE_1		"pid"
#define NODE_2		"nr_hashed"

struct proc_dir_entry *proc_taskdir, *proc_pid, *proc_nr_hashed;
int pid_no = -1;
struct task_struct *p_task = NULL;

ssize_t my_proc_read(struct file *file, char __user * buf,
		     size_t len, loff_t * offset)
{
	char data[1024];
	struct pid_namespace *pid_ns = NULL;
	unsigned int nr_hashed = 0;

	if (pid_no != -1) {
		p_task = pid_task(find_pid_ns(pid_no, &init_pid_ns), PIDTYPE_PID);
		pid_ns = task_active_pid_ns(p_task);
		nr_hashed = pid_ns->pid_allocated;
	}
	if (PDE_DATA(file_inode(file)) == &proc_pid)
		sprintf(data, "%d\n", pid_no);
	if (PDE_DATA(file_inode(file)) == &proc_nr_hashed)
		sprintf(data, "%u\n", nr_hashed);

	if (*offset > strlen(data))
		return 0;
	*offset = strlen(data) + 1;
	return strlen(data) - copy_to_user(buf, data, strlen(data));

}

ssize_t my_proc_write(struct file * file, const char __user * buffer,
		      size_t count, loff_t * pos)
{
	char *str = kmalloc(count, GFP_KERNEL);
        struct pid_namespace *pid_ns = NULL;
        unsigned int nr_hashed = 0;

        if (pid_no != -1) {
                p_task = pid_task(find_pid_ns(pid_no, &init_pid_ns), PIDTYPE_PID);
                pid_ns = task_active_pid_ns(p_task);
                nr_hashed = pid_ns->pid_allocated;
        }

	if (copy_from_user(str, buffer, count)) {
		kfree(str);
		return -EFAULT;
	}
	if (PDE_DATA(file_inode(file)) == &proc_pid) {
		sscanf(str, "%d", &pid_no);
		printk("proc_pid has been set to %d\n", pid_no);
		kfree(str);
		return count;
	}
	if (PDE_DATA(file_inode(file)) == &proc_nr_hashed) {
		sscanf(str, "%d", &nr_hashed);
		printk("nr_hashed has been set to %d\n", nr_hashed);
		if (pid_ns) {
			pid_ns->pid_allocated = pid_ns->pid_allocated + nr_hashed;
		}
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
	proc_taskdir = proc_mkdir(NODE_DIR, NULL);

	proc_pid =
	    proc_create_data(NODE_1, 0666, proc_taskdir, &myfops, &proc_pid);

	proc_nr_hashed =
	    proc_create_data(NODE_2, 0666, proc_taskdir, &myfops, &proc_nr_hashed);

	return 0;
}

void __exit my_exit(void)
{
	remove_proc_entry(NODE_1, proc_taskdir);
	remove_proc_entry(NODE_2, proc_taskdir);
	remove_proc_entry(NODE_DIR, NULL);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
