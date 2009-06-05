#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#include "netqos.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION(NETQOS_VERSION);

/* declaration of attribute access functions */
/* show() and store() are demultiplexers for the rest
 * using attr as the selector */
static ssize_t show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf);
static ssize_t store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf,
        size_t count);
static ssize_t foo_show(char *buf);
static ssize_t foo_store(const char *buf, size_t count);
static ssize_t bar_show(char *buf);
static ssize_t bar_store(const char *buf, size_t count);

/* attributes */
static struct kobj_attribute foo_attribute =
    __ATTR(foo, 0666, show, store);
static struct kobj_attribute bar_attribute =
    __ATTR(bar, 0666, show, store);
static struct attribute *attrs[] = {
    &foo_attribute.attr,
    &bar_attribute.attr,
    NULL,
};

/* attributes' storage */
static int foo;
static int bar;

/* definitions of attribute accesss functions */
static ssize_t
show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    if (attr == &foo_attribute) {
        return foo_show(buf);
    }
    else if (attr == &bar_attribute) {
        return bar_show(buf);
    } else {
        return -1;
    }
}

static ssize_t
store(struct kobject *kobj, struct kobj_attribute * attr,
        const char *buf, size_t count)
{
    if (attr == &foo_attribute) {
        return foo_store(buf, count);
    } else if (attr == &bar_attribute) {
        return bar_store(buf, count);
    } else {
        return -1;
    }
}

static ssize_t
foo_show(char *buf)
{
    return sprintf(buf, "%d\n", foo);
}

static ssize_t
foo_store(const char *buf, size_t count)
{
    sscanf(buf, "%du", &foo);
    return count;
}

static ssize_t
bar_show(char *buf)
{
    return sprintf(buf, "%d\n", bar);
}

static ssize_t
bar_store(const char *buf, size_t count)
{
    sscanf(buf, "%du", &bar);
    return count;
}

/* Unnamed attribute group; will put all attributes
 * directly under the kobject dir instead of creating
 * a subdir.
 */
static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *netqos_kobj;

static int __init
netqos_init(void)
{
    int r;

    printk(KERN_INFO "netqos init (API version %i)\n",
            NETQOS_VERSION);

    /* Create a simple kobject called "netqos" under /sys/kernel
     * 
     * As this is a simple directory, no uevent will be sent to
     * userspace.
     */
    netqos_kobj = kobject_create_and_add("netqos", kernel_kobj);
    if (!netqos_kobj)
        return -ENOMEM;

    /* Populate the directory with the attributes */
    r = sysfs_create_group(netqos_kobj, &attr_group);
    if (r)
        kobject_put(netqos_kobj);

    return r;
}

static void __exit
netqos_exit(void)
{
    printk(KERN_DEBUG "netqos exit\n");

    kobject_put(netqos_kobj);
    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
