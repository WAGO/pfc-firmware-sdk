/*
 * Copyright (C) 2010 Nelson Elhage
 * Copyright (C) 2018-2022 WAGO GmbH & Co. KG
 *
 * Trivial kernel module to provoke a NULL/invalid pointer dereference.
 *
 * Based in part on provoke-crash.c
 * <http://thread.gmane.org/gmane.linux.kernel/942633>
 *
 * Changed by WAGO GmbH & Co. KG
 *
 * Author: Nelson Elhage <nelhage@ksplice.com>
 * Author: Patrick Enns
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2 of the
 * License.
 *
 */
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/hardirq.h>
#include <linux/debugfs.h>

/*
 * Define an 'ops' struct containing a single mostly-pointless
 * function. We just do this to try to make this code look vaguely
 * like something that the actual kernel might contain.
 */
struct my_ops {
	ssize_t (*do_it)(void);
};

/* Define a pointer to our ops struct, "accidentally" initialized to NULL. */
static struct my_ops *ops = NULL;
static struct my_ops *ops42 = (void *)42;

/*
 * Writing to the 'null_read' file just reads the value of the do_it
 * function pointer from the NULL ops pointer.
 */
static ssize_t null_read_write(struct file *f, const char __user *buf,
		size_t count, loff_t *off)
{
	f->private_data = ops->do_it;

	return count;
}

/*
 * Writing to the 'null_call' file calls the do_it member of ops,
 * which results in reading a function pointer from NULL and then
 * calling it.
 */
static ssize_t null_call_write(struct file *f, const char __user *buf,
		size_t count, loff_t *off)
{
	return ops->do_it();
}

/*
 * Writing to the 'fourtytwo_read' file just reads the value of the do_it
 * function pointer from the 42 ops pointer.
 */
static ssize_t fourtytwo_read_write(struct file *f, const char __user *buf,
    size_t count, loff_t *off)
{
  f->private_data = ops42->do_it;

  return count;
}

/*
 * Writing to the 'fourtytwo_call' file calls the do_it member of ops,
 * which results in reading a function pointer from 42 and then
 * calling it.
 */
static ssize_t fourtytwo_call_write(struct file *f, const char __user *buf,
    size_t count, loff_t *off)
{
  return ops42->do_it();
}

/* Handles to the files we will create */
static struct dentry *nullderef_root, *read_de, *call_de, *read42_de, *call42_de;

/* Structs telling the kernel how to handle writes to our files. */
static const struct file_operations null_read_fops = {
	.write = null_read_write,
};
static const struct file_operations null_call_fops = {
	.write = null_call_write,
};
static const struct file_operations fourtytwo_read_fops = {
  .write = fourtytwo_read_write,
};
static const struct file_operations fourtytwo_call_fops = {
  .write = fourtytwo_call_write,
};

/*
 * To clean up our module, we just remove the two files and the
 * directory.
 */
static void cleanup_debugfs(void) {
	if (read_de) debugfs_remove(read_de);
	if (call_de) debugfs_remove(call_de);
  if (read42_de) debugfs_remove(read42_de);
  if (call42_de) debugfs_remove(call42_de);
	if (nullderef_root) debugfs_remove(nullderef_root);
}

/*
 * This function is called at module load time, and creates the
 * directory in debugfs and the two files.
 */
static int __init nullderef_init(void)
{
	/* Create the directory our files will live in. */
	nullderef_root = debugfs_create_dir("nullderef", NULL);
	if (!nullderef_root) {
		printk(KERN_ERR "nullderef: creating root dir failed\n");
		return -ENODEV;
	}

	/*
	 * Create the null_read and null_call files. Use the fops
	 * structs defined above so that the kernel knows how to
	 * handle writes to them, and set the permissions to be
	 * writable by anyone.
	 */
	read_de = debugfs_create_file("null_read", 0222, nullderef_root,
				      NULL, &null_read_fops);
	call_de = debugfs_create_file("null_call", 0222, nullderef_root,
				      NULL, &null_call_fops);
  read42_de = debugfs_create_file("fourtytwo_read", 0222, nullderef_root,
                NULL, &fourtytwo_read_fops);
  call42_de = debugfs_create_file("fourtytwo_call", 0222, nullderef_root,
                NULL, &fourtytwo_call_fops);

	if (!read_de || !call_de || !read42_de || !call42_de)
		goto out_err;

	return 0;
out_err:
	cleanup_debugfs();

	return -ENODEV;
}

/*
 * This function is called on module unload, and cleans up our files.
 */
static void __exit nullderef_exit(void)
{
	cleanup_debugfs();
}

/*
 * These two lines register the functions above to be called on module
 * load/unload.
 */
module_init(nullderef_init);
module_exit(nullderef_exit);

MODULE_AUTHOR("Nelson Elhage <nelhage@ksplice.com>");
MODULE_DESCRIPTION("Provides debugfs files to trigger NULL pointer dereferences.");
MODULE_LICENSE("GPL");
