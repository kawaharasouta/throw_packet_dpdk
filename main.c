#include<stdio.h>
#include<stdlib.h>
#include<string.h>
/*
#include<arpa/inet.h>

#include<netinet/if_ether.h>
#include<netinet/ip.h>
*/
#include<netinet/in.h>

//#include"include/mk_ether.h"

#include"include/knet/ethernet.h"
//#include"include/knet/arp.h"
#include"include/knet/ip.h"
//#include"include/knet/tcp.h"

#include"include/dpdk.h"
#include"include/dpdk_init.h"

/*
struct ip_packet {
	struct ether_header eth_hdr;
	struct ip ip_hdr; 
};
*/

void mk_ether(struct eth_hdr *hdr, u_int8_t *d_addr, u_int8_t *s_addr){
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

#if 1
//u_int16_t packet[1000] = {
//	0xffff, 0xffff, 0xffff, 0xdeb4,
//	0x6b16, 0x904c, 0x0800, 0x4500,	//up to service type
//	0x0014, 0x1000, 0x4000, 0x4006, //length, ID, Flags, offset, TTL, proto
//	0x16e2, 0x0a00, 0x0002, 0x0a00,
//	0x0001
//};
//source ip 10.0.0.2
//dest ip 10.0.0.1
#else
u_int16_t packet[1000] = {
	0xdeb4, 0x6b16, 0x904c, 0xffff,
	0xffff, 0xffff, 0x0800, 0x4500,	//up to service type
	0x0014, 0x1000, 0x4000, 0x4006, //length, ID, Flags, offset, TTL, proto
	0x16e2, 0x0a00, 0x0002, 0x0a00,
	0x0001
};
#endif

//u_int16_t icmp_packet[50] = {
//	0x0036, 0x9f3e, 0xa08a, 0xa036,
//	0x9f3f, 0x6202, 0x0800, 0x4500,
//	0x0054, 0x670b, 0x4000, 0x4001,
//	0xbf91, 0x0a00, 0x0007, 0x0a00,
//	0x0006, 0x0800, 0x3687, 0x7ca3,
//	0x0001, 0x31d1, 0xdd5a, 0x0000,
//	0x0000
//};
//
u_char packet[64] = {
	0x00, 0x36, 0x9f, 0x3e, 0xa0, 0x8a, 0xa0, 0x36,
	0x9f, 0x3f, 0x62, 0x02, 0x08, 0x00, 0x45, 0x00,
	0x00, 0x54, 0x67, 0x0b, 0x40, 0x00, 0x40, 0x01,
	0xbf, 0x91, 0x0a, 0x00, 0x00, 0x07, 0x0a, 0x00,
	0x00, 0x06, 0x08, 0x00, 0x36, 0x87, 0x7c, 0xa3,
	0x00, 0x01, 0x31, 0xd1, 0xdd, 0x5a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //2
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

struct test_packet {
	struct eth_hdr ethhdr;
	struct ip_hdr iphdr;
	u_char pate[26];
} __attribute__ ((packed));

int main(int argc, char **argv){
	dpdk_init(argc, argv);


	int sock, size;//, sock_eth3, sock_eth2;
	u_char buf[2048];
	u_int8_t s_addr[6] = {
		0xa0, 0x36, 0x9f, 0x3e, 0xa0, 0x8a
	};
	u_int8_t d_addr[6] = {
		0xa0, 0x36, 0x9f, 0x3f, 0x62, 0x02
	};

//	printf("initrawsock\n");
//  if ((sock = initrawsock(argv[1], 1, 0)) < 0){ 
//  //if ((sock_eth3 = initrawsock("eth3", 1, 0)) < 0){ 
//    fprintf(stderr, "InitRawSocket:error:%s\n", argv[1]);
//    //fprintf(stderr, "InitRawSocket:error:eth3\n");
//    exit(1);
//  }
//	printf("initrawsock fin\n");
// // if ((sock_eth2 = initrawsock("eth2", 1, 0)) < 0){ 
// //   //fprintf(stderr, "InitRawSocket:error:%s\n", argv[1]);
// //   fprintf(stderr, "InitRawSocket:error:eth3\n");
// //   exit(1);
// // }

	//struct ip_packet packet;
	//write(sock, icmp_packet, strlen(icmp_packet));

	//struct sockaddr_in to;
	//to.sin_addr.s_addr = (u_long)0x0a000006;
	//to.sin_family = AF_INET;
	//to.sin_port = ;


//	if (((size = read(sock_eth3, buf, sizeof(buf))) <= 0)){
//		fprintf(stderr, "readerr\n");
//		exit(1);
//	}
//	printf("read success\n");

	struct test_packet *p;
	p = (struct test_packet *)malloc(sizeof(struct test_packet));

	//struct ether_hdr ethhdr;
	mk_ether(&(p->ethhdr), d_addr, s_addr);

	//struct ip_hdr iphdr;
	p->iphdr.version = 0x4;
	p->iphdr.hdr_len = 0x5;
	//p->iphdr.lv = 0x45;
	p->iphdr.type_of_service = 0x00;
	p->iphdr.total_len = htons(0x003c);
	p->iphdr.id = htons(0x209a);
	p->iphdr.frag = htons(0x4000);
	p->iphdr.ttl = 0x40;
	p->iphdr.proto = 0x01;
	p->iphdr.check = htons(0x0000);
	p->iphdr.src_addr = htonl(0x0a000003);
	p->iphdr.dest_addr = htonl(0x0a000005);

	int l;
	for (l = 0; l < 26; l++){
		p->pate[l] = 0x00;
	}



	//int n;
#if 0
	printf("write\n");
	if ((n = write(sock, (u_char *)p, 60)) <= 0){
	//if ((n = write(sock_eth2, buf, size)) <= 0){
		fprintf(stdout, "can not send packet\n");
		exit(1);
	}
	//fflush(sock_eth2);
	printf("write fin\n");

	FILE *fp;
	fp = fopen("aaa", "wb");
	fwrite(p, sizeof(struct test_packet), 1, fp);
	fflush(fp);
	close(fp);

//#else
	if ((n = sendto(sock, (char *)icmp_packet, strlen(icmp_packet), 0, (struct sockaddr *)&to, sizeof(to))) <= 0){
		fprintf(stdout, "can not send packet\n");
		exit(1);
	}
#endif
	
	//close(sock);
	//close(sock_eth3);

	return 0;
}
