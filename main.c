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
//#include"include/dpdk_init.h"

/*
struct ip_packet {
	struct ether_header eth_hdr;
	struct ip ip_hdr; 
};
*/

static const struct rte_eth_conf port_conf_default = { 
	.rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

struct rte_mempool *mbuf_pool;

static inline int
port_init(uint16_t port, struct rte_mempool *mbuf_pool)
{ 
  struct rte_eth_conf port_conf = port_conf_default;
  const uint16_t rx_rings = 1, tx_rings = 1;
  uint16_t nb_rxd = RX_RING_SIZE;
  uint16_t nb_txd = TX_RING_SIZE;
  int retval;
  uint16_t q;
  
  if (port >= rte_eth_dev_count())
    return -1;
  
  /* Configure the Ethernet device. */ 
  retval = rte_eth_dev_configure(port, rx_rings, tx_rings, &port_conf);
  if (retval != 0)
    return retval;
  
  retval = rte_eth_dev_adjust_nb_rx_tx_desc(port, &nb_rxd, &nb_txd);
  if (retval != 0)
    return retval;
  
  /* Allocate and set up 1 RX queue per Ethernet port. */
  for (q = 0; q < rx_rings; q++) {
    retval = rte_eth_rx_queue_setup(port, q, nb_rxd,
        rte_eth_dev_socket_id(port), NULL, mbuf_pool);
    if (retval < 0)
      return retval;
  }
  
  /* Allocate and set up 1 TX queue per Ethernet port. */
  for (q = 0; q < tx_rings; q++) {
    retval = rte_eth_tx_queue_setup(port, q, nb_txd,
        rte_eth_dev_socket_id(port), NULL);
    if (retval < 0)
      return retval;
  }
  
  /* Start the Ethernet port. */
  retval = rte_eth_dev_start(port);
  if (retval < 0)
    return retval;
  
  /* Display the port MAC address. */
  struct ether_addr addr;
  rte_eth_macaddr_get(port, &addr);
  printf("Port %u MAC: %02" PRIx8 " %02" PRIx8 " %02" PRIx8
         " %02" PRIx8 " %02" PRIx8 " %02" PRIx8 "\n",
      port,
      addr.addr_bytes[0], addr.addr_bytes[1],
      addr.addr_bytes[2], addr.addr_bytes[3],
      addr.addr_bytes[4], addr.addr_bytes[5]);
  
  /* Enable RX in promiscuous mode for the Ethernet device. */
  rte_eth_promiscuous_enable(port);
  
  return 0;
}

void dpdk_init(int argc, char **argv){
  //struct rte_mempool *mbuf_pool;
  int ret;
  unsigned nb_ports;
  uint16_t portid;

  ret = rte_eal_init(argc, argv);
  if (ret < 0)
    rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");

  argc -= ret;
  argv += ret;

  nb_ports = rte_eth_dev_count();
  if (nb_ports != 1)
    rte_exit(EXIT_FAILURE, "Error: number of ports must be even\n");

  /* Creates a new mempool in memory to hold the mbufs. */
  mbuf_pool = rte_pktmbuf_pool_create("MBUF_POOL", NUM_MBUFS * nb_ports,
    MBUF_CACHE_SIZE, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());

  if (mbuf_pool == NULL)
    rte_exit(EXIT_FAILURE, "Cannot create mbuf pool\n");

  /* Initialize all ports. */
  for (portid = 0; portid < nb_ports; portid++)
    if (port_init(portid, mbuf_pool) != 0)
      rte_exit(EXIT_FAILURE, "Cannot init port %"PRIu16 "\n",
          portid);

  if (rte_lcore_count() > 1)
    printf("\nWARNING: Too many lcores enabled. Only 1 used.\n");
}

void hexdump(u_int16_t *buf, int size){
  int i;
	printf("***** hexdump *****\n");
  for (i = 0;i < size; i++){
    fprintf(stdout, "%04x ", *(buf + i));
    if ((i + 1) % 8 == 0){ 
      fprintf(stdout, "\n");
    }   
  }
  fprintf(stdout, "\n***** hexdump fin *****\n");
}

void print_mbuf(const struct rte_mbuf *bufs){
	printf("-----print_mbuf-----\n");

	//printf("rearm_data: %u\n", bufs->rearm_data);

	printf("data_off: %u\n", bufs->data_off);
	printf("refcnt: %u\n", bufs->refcnt);
	printf("nb_segs: %u\n", bufs->nb_segs);
	printf("port: %u\n", bufs->port);
	printf("ol_flags: %u\n", bufs->ol_flags);
	printf("packet_type: %u\n", bufs->packet_type);
	printf("pkt_len: %u\n", bufs->pkt_len);
	printf("data_len: %u\n", bufs->data_len);
	printf("vlan_tci: %u\n", bufs->vlan_tci);
	printf("rss: %u\n", bufs->hash.rss);
	printf("vlan_tci_outer: %u\n", bufs->vlan_tci_outer);
	printf("buf_len: %u\n", bufs->buf_len);
	printf("timestamp: %u\n", bufs->timestamp);
	printf("udata: %u\n", bufs->udata64);
	printf("tx_offlead: %u\n", bufs->tx_offload);
	printf("priv_size: %u\n", bufs->priv_size);
	printf("timesync: %u\n", bufs->timesync);
	printf("seqn: %u\n", bufs->seqn);
	printf("--------------------\n");
}

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

	struct test_packet *buffer;
	buffer = (struct test_packet *)malloc(sizeof(struct test_packet));

	//struct ether_hdr ethhdr;
	mk_ether(&(buffer->ethhdr), d_addr, s_addr);

	//struct ip_hdr iphdr;
	buffer->iphdr.version = 0x4;
	buffer->iphdr.hdr_len = 0x5;
	//p->iphdr.lv = 0x45;
	buffer->iphdr.type_of_service = 0x00;
	buffer->iphdr.total_len = htons(0x003c);
	buffer->iphdr.id = htons(0x209a);
	buffer->iphdr.frag = htons(0x4000);
	buffer->iphdr.ttl = 0x40;
	buffer->iphdr.proto = 0x01;
	buffer->iphdr.check = htons(0x0000);
	buffer->iphdr.src_addr = htonl(0x0a000003);
	buffer->iphdr.dest_addr = htonl(0x0a000005);

	int l;
	for (l = 0; l < 26; l++){
		buffer->pate[l] = 0x00;
	}

	
	/*** packet Transfer flow ***/
	printf("head of device_output\n");
	struct rte_mbuf *bufs[BURST_SIZE];
	const uint16_t nb_ports = rte_eth_dev_count();
	uint16_t port;
	size_t length = 60;

	/*
   * Check that the port is on the same NUMA node as the polling thread
   * for best performance.
   */ 
  for (port = 0; port < nb_ports; port++)
    if (rte_eth_dev_socket_id(port) > 0 &&
        rte_eth_dev_socket_id(port) !=
            (int)rte_socket_id())
      printf("WARNING, port %u is on remote NUMA node to "
          "polling thread.\n\tPerformance will "
          "not be optimal.\n", port);	


	printf("\nCore %u forwarding packets. [Ctrl+C to quit]\n",
      rte_lcore_id());

	port = 0;
	/* Send burst of TX packets */
	//struct rte_mbuf *bufs[BURST_SIZE];
	
	printf("rte_pktmbuf_alloc\n");
	bufs[0] = rte_pktmbuf_alloc(mbuf_pool);
	print_mbuf(bufs[0]);
	bufs[0]->pkt_len = 60;
	bufs[0]->data_len = 60;
	//print_mbuf(bufs[0]);

	printf("strncpy\n");
	//uint8_t *p = rte_pktmbuf_append(bufs[0], length);
	//uint8_t *p = rte_pktmbuf_prepend(bufs[0], length);
	uint8_t *p = rte_pktmbuf_mtod(bufs[0], uint8_t*);

	//if (p == pp) printf("p <=> pp\n");
	memcpy(p, buffer, length);
	
	/******/
	//struct ether_arp *pp = p + sizeof(struct ether_header);
	//struct ether_arp *buffer_e += sizeof(struct ether_header);
	//strncpy(pp->ea_hdr, buffer_e->ea_hdr, strlen(buffer->ea_hdr));
	/******/

	print_mbuf(bufs[0]);
	printf("port\n");
	bufs[0]->port = 0;
	printf("packet_type\n");
	bufs[0]->packet_type = 1;
	print_mbuf(bufs[0]);
	printf("len\n");
	//printf("rte_pktmbuf_read\n");
	//bufs[0] = rte_pktmbuf_read(bufs[0], 0, length, buffer);
	////print_mbuf(bufs[0]);

	//printf("rte_get_ptypte_name\n");
	////bufs[0]->packet_type = (uint32_t)193;
	//if (rte_get_ptype_name(bufs[0]->packet_type, buffer, length) == -1){
	//	printf("rte_get_ptype_name\n");
	//	exit(1);
	//}

	//print_mbuf(bufs[0]);
	
	//printf("mbuf test\n");
	//uint8_t *p = rte_pktmbuf_mtod(bufs[0], uint8_t*);
	hexdump(buffer, length);
	printf("*****\n");
	hexdump(p, length);

	printf("before tx_burst\n");

	rte_eth_tx_burst(port, 0, bufs, 1);

	printf("fin\n");


	FILE *fp;
	fp = fopen("mbuf_dump.txt", "w");
	rte_pktmbuf_dump(fp, bufs[0], bufs[0]->buf_len);


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
