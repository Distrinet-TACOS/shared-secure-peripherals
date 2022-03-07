#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/tee_drv.h>
#include <linux/uuid.h>
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/delay.h>
#include <asm/arch_timer.h>
#include <linux/kthread.h>

#define DRIVER_NAME "Benchmarker"

#define stamp(x)                                                               \
	isb();                                                                 \
	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(x))

static const uuid_t pta_uuid = UUID_INIT(0x6ce85888, 0xdb4e, 0x41f9, 0xb4, 0x4f,
					 0x03, 0x6e, 0x21, 0x4d, 0xdf, 0x3a);

static enum bench_cmd { RTT, SINGLE, RTT_MEM, PRINT_MEM };
#define MAX_STAMPS 100

static struct task_struct *kthread;

static struct timestamps {
	u32 counter;
	u32 start[MAX_STAMPS];
	u32 middle[MAX_STAMPS];
	u32 end[MAX_STAMPS];
} stamps;
static struct bench {
	u32 sess_id;
	struct tee_context *ctx;
	bool done;
} bench = { .done = false };

// static u32 stamp(void)
// {
// 	u32 ccounter;

// 	asm volatile("mrc p15, 0, %0, c9, c13, 0" : "=r"(ccounter));

// 	return ccounter;
// }

static void bench_rtt(void)
{
	int ret;
	struct tee_ioctl_invoke_arg inv_arg;
	struct tee_param params[4];
	int i;
	u32 start;
	u32 end;

	memset(&inv_arg, 0, sizeof(inv_arg));
	memset(&params, 0, sizeof(params));

	inv_arg.func = RTT;
	inv_arg.session = bench.sess_id;
	inv_arg.num_params = 4;

	params[0].attr = TEE_IOCTL_PARAM_ATTR_TYPE_VALUE_OUTPUT;

	pr_info("i: Start, Middle, End\n");

	for (i = 0; i < MAX_STAMPS; i++) {
		stamp(start);

		ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);

		stamp(end);

		if ((ret < 0) || (inv_arg.ret != 0)) {
			pr_err("RTT invoke error: %x.\n", inv_arg.ret);
			return;
		}

		pr_info("%d: %u, %llu, %u\n", i, start, params[0].u.value.a,
			end);
	}
}

static void bench_rtt_mem(void)
{
	int ret;
	struct tee_ioctl_invoke_arg inv_arg;
	struct tee_param params[4];
	int i;

	memset(&inv_arg, 0, sizeof(inv_arg));
	memset(&params, 0, sizeof(params));

	inv_arg.func = RTT_MEM;
	inv_arg.session = bench.sess_id;
	inv_arg.num_params = 4;

	stamps.counter = 0;

	for (i = 0; i < MAX_STAMPS; i++) {
		stamp(stamps.start[stamps.counter]);

		ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);

		stamp(stamps.end[stamps.counter++]);

		if ((ret < 0) || (inv_arg.ret != 0)) {
			pr_err("RTT invoke error: %x.\n", inv_arg.ret);
			return;
		}
	}

	pr_info("i: Start, End\n");
	for (i = 0; i < stamps.counter; i++) {
		pr_info("%d: %u, %u\n", i, stamps.start[i], stamps.end[i]);
	}

	inv_arg.func = PRINT_MEM;
	ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);
	if ((ret < 0) || (inv_arg.ret != 0)) {
		pr_err("PRINT_MEM invoke error: %x.\n", inv_arg.ret);
		return;
	}
}

static void enable_counters(void *data);

static int callback(void *s)
{
	stamp(stamps.end[stamps.counter]);
	// stamps.end[stamps.counter] = ktime_get_ns();
	stamps.middle[stamps.counter++] = *(u32 *)s;

	// pr_info("cpu: %u\n", smp_processor_id());

	if (stamps.counter >= MAX_STAMPS) {
		bench.done = true;
	}

	return 0;
}

static void bench_single_way(void)
{
	int ret;
	struct tee_ioctl_invoke_arg inv_arg;
	struct tee_param params[4];
	int i;
	u32 overflow;
	u32 c;

	ret = register_callback(callback, 9);
	if (ret < 0) {
		pr_err("Registering notification callback resulted in error: %x\n",
		       ret);
		return;
	}

	memset(&inv_arg, 0, sizeof(inv_arg));
	memset(&params, 0, sizeof(params));

	inv_arg.func = SINGLE;
	inv_arg.session = bench.sess_id;
	inv_arg.num_params = 4;

	stamps.counter = 0;

	// ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);
	// if ((ret < 0) || (inv_arg.ret != 0)) {
	// 	pr_err("SINGLE invoke error: %x.\n", inv_arg.ret);
	// 	return;
	// }
	// return;

	// pr_info("Start cpu: %u\n", smp_processor_id());

	pr_info("Dit is een test!\n");
	// enable_counters(NULL);

	for (i = 0; i < MAX_STAMPS; i++) {
		// stamps.start[i] = ktime_get_ns();
		stamp(stamps.start[i]);
		ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);
		if ((ret < 0) || (inv_arg.ret != 0)) {
			pr_err("SINGLE invoke error: %x.\n", inv_arg.ret);
			return;
		}
		msleep(10);
		if (kthread_should_stop()) {
			return;
		}
	}

	while (!bench.done) {
		if (kthread_should_stop()) {
			return;
		}
		msleep(100);

		stamp(c);
		pr_info("stamp: %u", c);
	}

	pr_info("i: Invoke, Driver, CA\n");
	for (i = 0; i < stamps.counter; i++) {
		pr_info("%d: %u, %u, %u\n", i, stamps.start[i],
			stamps.middle[i], stamps.end[i]);
	}

	// Read overflow bit.
	asm volatile("mcr p15, 0, %0, c9, c12, 3" : "=r"(overflow));
	pr_info("Overflow    : %u\n", (overflow & (1 << 31)) >> 31);

	memset(&inv_arg, 0, sizeof(inv_arg));
	memset(&params, 0, sizeof(params));

	inv_arg.func = PRINT_MEM;
	inv_arg.session = bench.sess_id;
	inv_arg.num_params = 4;

	ret = tee_client_invoke_func(bench.ctx, &inv_arg, params);
	if ((ret < 0) || (inv_arg.ret != 0)) {
		pr_err("PRINT_MEM invoke error: %x.\n", inv_arg.ret);
		return;
	}

	unregister_callback();
}

static void enable_counters(void *data)
{
	/* Enable EL0 access to PMU counters */
	asm volatile("mcr p15, 0, %0, c9, c14, 0" ::"r"(1));
	/* Enable all PMU counters and clear PMCCNTR */
	asm volatile("mcr p15, 0, %0, c9, c12, 0" ::"r"((1 | 4 | 16 | 8)));
	/* Disable counter overflow interrupts */
	asm volatile("mcr p15, 0, %0, c9, c12, 1" ::"r"(0x8000000f));
	/* Clear overflow bit */
	asm volatile("mcr p15, 0, %0, c9, c12, 3" ::"r"(1 << 31));
	isb();
}

static void disable_counters(void *data)
{
	isb();
	/* Disable all PMU counters */
	asm volatile("mcr p15, 0, %0, c9, c12, 0" ::"r"(0));
	/* Enable counter overflow interrupts */
	asm volatile("mcr p15, 0, %0, c9, c12, 2" ::"r"(0x8000000f));
	/* Disable EL0 access to PMU counters. */
	asm volatile("mcr p15, 0, %0, c9, c14, 0" ::"r"(0));
}

static int benchmark(void *_)
{
	u64 start_time;
	u32 start_ticks;
	u64 end_time;
	u32 end_ticks;
	int i;

	on_each_cpu(enable_counters, NULL, 1);

	start_time = ktime_get_ns();
	end_time = ktime_get_ns();
	pr_info("start: %llu", start_time);
	pr_info("end: %llu", end_time);

	// Determine averate ticks per second.
	start_time = ktime_get_ns();
	stamp(start_ticks);
	for (i = 0; i < 1000000000; i++)
		;
	end_time = ktime_get_ns();
	stamp(end_ticks);

	pr_info("\n\n### Average ticks ###\n");
	pr_info("Time (ns)   : %llu\n", end_time - start_time);
	pr_info("Start ticks : %u\n", start_ticks);
	pr_info("End ticks   : %u\n", end_ticks);

	// on_each_cpu(enable_counters, NULL, 1);

	// pr_info("\n### RTT ###\n\n");
	// bench_rtt();

	// if (kthread_should_stop()) {
	// 	return -1;
	// }

	// on_each_cpu(enable_counters, NULL, 1);

	// pr_info("\n\n### RTT_MEM ###\n\n");
	// bench_rtt_mem();

	// if (kthread_should_stop()) {
	// 	return -1;
	// }

	on_each_cpu(enable_counters, NULL, 1);

	pr_info("\n\n### SINGLE ###\n\n");
	bench_single_way();

	if (kthread_should_stop()) {
		return -1;
	}

	on_each_cpu(disable_counters, NULL, 1);

	return 0;
}

static int open_session(const char *name, u32 *sess_id)
{
	struct tee_ioctl_open_session_arg sess_arg;
	struct tee_param param[4];
	int ret;

	memset(&sess_arg, 0, sizeof(sess_arg));
	export_uuid(sess_arg.uuid, &pta_uuid);
	sess_arg.clnt_login = TEE_IOCTL_LOGIN_PUBLIC;
	sess_arg.num_params = 4;

	memset(&param, 0, sizeof(param));

	ret = tee_client_open_session(bench.ctx, &sess_arg, param);
	if ((ret < 0) || (sess_arg.ret != 0)) {
		pr_err("tee_client_open_session failed for device %s, err: %x\n",
		       name, sess_arg.ret);
		return -EINVAL;
	}
	*sess_id = sess_arg.session;

	return 0;
}

static int close_session(u32 sess_id)
{
	tee_client_close_session(bench.ctx, sess_id);
	return 0;
}

static int optee_ctx_match(struct tee_ioctl_version_data *ver, const void *data)
{
	if (ver->impl_id == TEE_IMPL_ID_OPTEE) {
		return 1;
	} else {
		return 0;
	}
}

static int create_context(void)
{
	bench.ctx = tee_client_open_context(NULL, optee_ctx_match, NULL, NULL);
	if (IS_ERR(bench.ctx)) {
		return -ENODEV;
	}

	return 0;
}

static int destroy_context(void)
{
	tee_client_close_context(bench.ctx);

	return 0;
}

static int __init controller_init(void)
{
	int ret;

	if ((ret = create_context()) < 0) {
		pr_err("Could not create a context.\n");
		return ret;
	}

	if ((ret = open_session(DRIVER_NAME, &bench.sess_id)) < 0) {
		pr_err("Could not open a session.\n");
		return ret;
	}

	kthread = kthread_create(benchmark, NULL, "benchmarker");
	kthread_bind(kthread, 0);
	wake_up_process(kthread);

	return 0;
}

static void __exit controller_exit(void)
{
	close_session(bench.sess_id);
	destroy_context();
}

module_init(controller_init);
module_exit(controller_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Tom Van Eyck");