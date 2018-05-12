#include"mk_ether.h"
//#include	<sys/ioctl.h>
#include<sys/types.h>
#include<string.h>
#include"knet/ethernet.h"
//#include <stdint.h>
extern void mk_ether(struct ether_hdr *hdr, u_int8_t *d_addr, u_int8_t *s_addr){
	//struct ifreq	ifreq;
	//struct sockaddr_ll	sa;
	//memset(&ifreq,0,sizeof(struct ifreq));
	//strncpy(ifreq.ifr_name,device,sizeof(ifreq.ifr_name)-1);
	//if(ioctl(soc,SIOCGIFINDEX,&ifreq)<0){
	//	perror("ioctl");
	//	close(soc);
	//	return(-1);
	//}
	//sa.sll_family=PF_PACKET;

	//hdr->ether_dest_addr = d_addr;
	//hdr->ether_src_addr = s_addr;
	printf("mk_ether head \n");

	strncpy(hdr->ether_dest_addr, d_addr, 6);
	strncpy(hdr->ether_src_addr, s_addr, 6);
	//hdr->ether_dest_addr
	printf("strncpy fin\n");
	hdr->type = ntohs(0x0800);
	//hdr->type1 = 0x08;
	//hdr->type2 = 0x00;

	printf("mk_ether fin\n");
}
/*
#include	<unistd.h>
#include	<arpa/inet.h>
#include	<sys/socket.h>
#include	<linux/if.h>
#include	<net/ethernet.h>
#include	<netpacket/packet.h>
#include	<netinet/if_ether.h>
#include	<netinet/ip.h>

#include"initrawsock.h"

int initrawsock(char *device,int promiscFlag,int ipOnly)
{
int	soc;

	if(ipOnly){
		if((soc=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_IP)))<0){
			perror("socket");
			return(-1);
		}
	}
	else{
		if((soc=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0){
			perror("socket");
			return(-1);
		}
	}

	if(ipOnly){
		sa.sll_protocol=htons(ETH_P_IP);
	}
	else{
		sa.sll_protocol=htons(ETH_P_ALL);
	}
	sa.sll_ifindex=ifreq.ifr_ifindex;
	if(bind(soc,(struct sockaddr *)&sa,sizeof(sa))<0){
		perror("bind");
		close(soc);
		return(-1);
	}

	if(promiscFlag){
		if(ioctl(soc,SIOCGIFFLAGS,&ifreq)<0){
			perror("ioctl");
			close(soc);
			return(-1);
		}
		ifreq.ifr_flags=ifreq.ifr_flags|IFF_PROMISC;
		if(ioctl(soc,SIOCSIFFLAGS,&ifreq)<0){
			perror("ioctl");
			close(soc);
			return(-1);
		}
	}

	return(soc);
}*/
