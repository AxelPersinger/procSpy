#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Axel Persinger");
MODULE_DESCRIPTION("Spy on Linux Processes");
MODULE_VERSION("0.01");

#define MAX_SYMBOL_LEN 64

static char symbol[MAX_SYMBOL_LEN] = "_do_fork";
static unsigned int precounter = 0;
static unsigned int postcounter = 0;
static struct kprobe kp = {
	.symbol_name = symbol,
};

static int kp_PreHandler(struct kprobe *p, struct pt_regs *regs)
{
	printk("[%s][%s]: precounter=%u\n", __FILE__,  __FUNCTION__, precounter++);
	return 0;
}

static void kp_PostHandler(struct kprobe *p, struct pt_regs *regs, unsigned long flags)
{

	printk("[%s][%s]: postcounter=%u\n", __FILE__,  __FUNCTION__, postcounter++);
}

static int __init lkm_procSpy_init(void) 
{
	int ret;

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
