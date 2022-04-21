/* spindead.c */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpu.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

struct kthread {
    unsigned long flags;
    unsigned int cpu;
    int (*threadfn)(void *);
    void *data;
    struct completion parked;
    struct completion exited;
#ifdef CONFIG_BLK_CGROUP
    struct cgroup_subsys_state *blkcg_css;
#endif
};

static inline struct kthread *to_kthread(struct task_struct *k)
{
    WARN_ON(!(k->flags & PF_KTHREAD));
    return (__force void *)k->set_child_tid;
}

struct task_struct *kthread_create_on_cpu(int (*threadfn)(void *data),
                      void *data, unsigned int cpu,
                      const char *namefmt)
{
    struct task_struct *p;

    p = kthread_create_on_node(threadfn, data, cpu_to_node(cpu), namefmt,
                   cpu);
    if (IS_ERR(p))
        return p;
    kthread_bind(p, cpu);
    /* CPU hotplug need to bind once again when unparking the thread. */
    to_kthread(p)->cpu = cpu;
    return p;
}

DEFINE_SPINLOCK(wrong_lock);
int count = 0;

int spintest_fn(void *data) {
	int cpu = (int)data;
	printk("I am from CPU %d\n", cpu);
	spin_lock(&wrong_lock);
	count++;
	if (count == 1) {
		current->__state = TASK_INTERRUPTIBLE;
		schedule();
		current->__state = TASK_RUNNING;
	}
	spin_unlock(&wrong_lock);

	return 0;
}


int create_kernel_threads(void) {
	unsigned int cpu;
	struct task_struct *task;
	char comm[256];

	for_each_online_cpu(cpu) {
		sprintf(comm, "badspin/%d", cpu);
		task = kthread_create_on_cpu(spintest_fn, (void *)cpu, cpu, comm);
		if (IS_ERR(task)) {
			return PTR_ERR(task);
		}
		wake_up_process(task);
	}

	return 0;
}

int destroy_kernel_threads(void) {
	return 0;
}

int __init my_init(void) {
  printk("Hello, I'm first\n");
  create_kernel_threads();

  return 0;
}
void __exit my_exit(void) {
  destroy_kernel_threads();
  printk("Bye, I'm first\n");
}

module_init(my_init);
module_exit(my_exit);
