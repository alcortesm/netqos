#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alberto Cortes <alcortes@it.uc3m.es>");
MODULE_VERSION("0.2");

/* declaration of attribute access functions */
static ssize_t foo_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf);
static ssize_t foo_store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf,
        size_t count);
static ssize_t bar_show(struct kobject *kobj,
        struct kobj_attribute *attr, char *buf);
static ssize_t bar_store(struct kobject *kobj,
        struct kobj_attribute *attr, const char *buf,
        size_t count);

/* attributes */
static struct kobj_attribute foo_attribute =
    __ATTR(foo, 0666, foo_show, foo_store);
static struct kobj_attribute bar_attribute =
    __ATTR(bar, 0666, bar_show, bar_store);
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
foo_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    return sprintf(buf, "%d\n", foo);
}

static ssize_t
foo_store(struct kobject *kobj, struct kobj_attribute * attr,
        const char *buf, size_t count)
{
    sscanf(buf, "%du", &foo);
    return count;
}

static ssize_t
bar_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buf)
{
    return sprintf(buf, "%d\n", bar);
}

static ssize_t
bar_store(struct kobject *kobj, struct kobj_attribute * attr,
        const char *buf, size_t count)
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
    kobject_put(netqos_kobj);
    return;
}

module_init(netqos_init);
module_exit(netqos_exit);
