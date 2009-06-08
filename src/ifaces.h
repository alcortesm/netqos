#ifndef _IFACES_H_
#define _IFACES_H_

#include <linux/netdevice.h>


struct netqos_ifaces {
    int count;
    char * data; /* array of char[IFNAMSIZ] */
};

void netqos_ifaces_print(struct netqos_ifaces * ifaces);
struct netqos_ifaces * netqos_ifaces_create(struct net * net_ns);
void netqos_ifaces_destroy(struct netqos_ifaces * ifaces);

#endif /* _IFACES_H_ */
