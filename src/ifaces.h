#ifndef _IFACES_H_
#define _IFACES_H_

#include <linux/netdevice.h>


struct netqos_ifdata {
    char name[IFNAMSIZ];
    int bw;
    struct kobj_attribute bw_attr;
    int delay;
    struct kobj_attribute delay_attr;
    int jitter;
    struct kobj_attribute jitter_attr;
    int price;
    struct kobj_attribute price_attr;
};

struct netqos_ifaces {
    int count;
    struct netqos_ifdata * data; /* array */
};

void netqos_ifaces_print(struct netqos_ifaces * ifaces);
struct netqos_ifaces * netqos_ifaces_create(struct net * net_ns);
void netqos_ifaces_destroy(struct netqos_ifaces * ifaces);

#endif /* _IFACES_H_ */
