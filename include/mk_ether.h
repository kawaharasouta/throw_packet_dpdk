#ifndef __MK_ETHER_H
#define __MK_ETHER_H

#include<sys/types.h>
//#include"knet/ethernet.h"

extern void mk_ether(struct ether_hdr *hdr, u_int8_t *d_addr, u_int8_t *s_addr);

#endif
