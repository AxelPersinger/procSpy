/*INCLUDES
module - Kernel Module definitions
version - Linux Kernel Version definitions
init - ?
kernel - Standard kernel structs/vars
kprobes - Kprobe functions
*/
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>


/*MODULE
Module metadata
*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axel Persinger");
MODULE_DESCRIPTION("Spy on Linux Processes");
MODULE_VERSION("0.01");

/*DEFINES
MAX_SYMBOL_LEN - Maximum symbol len in kernel
*/
#define MAX_SYMBOL_LEN 64

/*DECLARATIONS
MonitoredPID - Struct containing information that we monitor about a PID
*/
struct MonitoredPID {
	void* in;
	void* out;
	void* err;
};
typedef struct MonitoredPID *pMonitoredPID;
static int kp_PreHandler(struct kprobe *, struct pt_regs *);
static void kp_PostHandler(struct kprobe *, struct pt_regs *, unsigned long);
void iterate_procs(void);
static int __init lkm_procSpy_init(void);
static void __exit lkm_procSpy_exit(void);

/*GLOBALS
symbol - Symbol to trap in kernel using kprobe
kp - Kernel probe struct
MonitoredPIDs - Array of MonitoredPID structs accessed by their pid
*/
static char symbol[MAX_SYMBOL_LEN] = "_do_fork";
static unsigned int precounter = 0;
static unsigned int postcounter = 0;
static struct kprobe kp = {
	.symbol_name = symbol,
};
pMonitoredPID MonitoredPIDs[PID_MAX_LIMIT];

// int for_each_task(void *data)
// {
// 	struct task_struct *g, *p;
// 	do_each_thread(g, p) 
// 	{
// 		if (MonitoredPIDs[p->pid] == NULL)
// 		{
// 			struct MonitoredPID *tempMonitoredPID = malloc(sizeof(MonitoredPID));
// 			tempMonitoredPID->in = NULL;
// 			tempMonitoredPID->out = NULL;
// 			tempMonitoredPID->err = NULL;

// 			MonitoredPIDs[p->pid] = tempMonitoredPID;
// 		}
// 	} while_each_thread(g, p);

// 	return 0;
// }

static int kp_PreHandler(struct kprobe *p, struct pt_regs *regs)
{
	// printk("[%s][%s]: precounter=%u\n", __FILE__,  __FUNCTION__, precounter++);
	return 0;
}

static void kp_PostHandler(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{
	// printk("[%s][%s]: Entry postcounter=%u\n", __FILE__,  __FUNCTION__, postcounter++);
    // //stop_machine(for_each_task, NULL, NULL);
	// printk("[%s][%s]: Exit postcounter=%u\n", __FILE__,  __FUNCTION__, postcounter);
	iterate_procs();
}

void iterate_procs(void)
{
        struct task_struct* task_list;
		pMonitoredPID tempMonitoredPID;

        for_each_process(task_list) {
				printk("[%s][%s]: Working on PID %d (%s)", __FILE__, __FUNCTION__, task_list->pid, task_list->comm);
				if (MonitoredPIDs[task_list->pid] != NULL)
				{
					printk("[%s][%s]: %d (%s) already monitored", __FILE__, __FUNCTION__, task_list->pid, task_list->comm);
				}
				else
				{
					printk("[%s][%s]: %d (%s) is now being monitored", __FILE__, __FUNCTION__, task_list->pid, task_list->comm);
					tempMonitoredPID = kmalloc(sizeof(struct MonitoredPID), GFP_KERNEL);
					MonitoredPIDs[task_list->pid] = tempMonitoredPID;
					printk("[%s][%s]: MonitoredPIDs[task_list->pid] (MonitoredPIDs[%d]) = %p", __FILE__, __FUNCTION__, task_list->pid, MonitoredPIDs[task_list->pid]);
				}
        }
}

static int __init lkm_procSpy_init(void) 
{
	int i;
	int ret;

	// Set each item in MonitoredPIDs to NULL on startup
	for(i = 0; i < PID_MAX_LIMIT; i++)
	{
		MonitoredPIDs[i] = NULL;
	}

	// Install kprobe
	printk(KERN_INFO "[%s][%s]: Installing kprobe\n", __FILE__,  __FUNCTION__);
	kp.pre_handler = kp_PreHandler;
	kp.post_handler = kp_PostHandler;
	ret = register_kprobe(&kp);
	if (ret < 0)
	{
		printk(KERN_INFO "[%s][%s] kprobe Installation failed\n", __FILE__,  __FUNCTION__);
		return ret;
	}
	printk(KERN_INFO "[%s][%s]: kprobe installed at %p\n",__FILE__,  __FUNCTION__, kp.addr);
	
	return 0;
}


static void __exit lkm_procSpy_exit(void)
{
	printk(KERN_INFO "[%s][%s]: Removing kprobe\n",__FILE__,  __FUNCTION__);
	unregister_kprobe(&kp);
	printk(KERN_INFO "[%s][%s]: kprobe removed\n",__FILE__,  __FUNCTION__);
}

module_init(lkm_procSpy_init);
module_exit(lkm_procSpy_exit);
