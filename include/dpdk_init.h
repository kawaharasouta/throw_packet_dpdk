#ifndef __DPDK_INIT_H
#define __DPDK_INIT_H

#include "dpdk.h"

static const struct rte_eth_conf port_conf_default = { 
  .rxmode = { .max_rx_pkt_len = ETHER_MAX_LEN }
};

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
  struct rte_mempool *mbuf_pool;
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

#endif
