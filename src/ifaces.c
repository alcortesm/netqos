#include <linux/netdevice.h>
#include <linux/if.h>

#include "ifaces.h"

void
netqos_ifaces_print(struct netqos_ifaces * ifaces)
{
    int i;
    struct neqos_ifdata * cur;
    for (i = 0; i<ifaces->count; i++) {
        cur = ifaces->data+i;
        printk(KERN_DEBUG "%s\n", cur->name);
    }
}

struct netqos_ifaces *
netqos_ifaces_create(struct net * net_ns)
{
    struct netqos_ifaces * ifaces;
    struct net_device * net_dev;
    char * new;

    ifaces = kmalloc(sizeof(struct netqos_ifaces), GFP_KERNEL);
    if (!ifaces)
        return NULL;
    ifaces->count = 0;
    ifaces->data = NULL;

    for_each_netdev(net_ns, net_dev) {
        ifaces->count++;
        new = krealloc(ifaces->data, ifaces->count*sizeof(struct netqos_iface_data), GFP_KERNEL);
        if (!new) {
            kfree(ifaces->data);
            kfree(ifaces);
            return NULL;
        }
        ifaces->data = new;
        strncpy(ifaces->data+((ifaces->count-1)*IFNAMSIZ), net_dev->name, IFNAMSIZ);
    }

    return ifaces;
}

void
netqos_ifaces_destroy(struct netqos_ifaces * ifaces)
{
    kfree(ifaces->data);
    kfree(ifaces);
    return;
}

struct netqos_ifdata *
netqos_ifdata_create(char * name)
{
    struct netqos_ifdata * data;

    data =  kmalloc(sizeof(struct netqos_ifdata), GFP_KERNEL);
    if (!data)
        return NULL;
    data->name          = strndup(name, IFNAMSIZ);
    data->bw            = 0;
    data->bw_attr       = __ATTR();
}

void
netqos_ifdata_destroy()
{
}
