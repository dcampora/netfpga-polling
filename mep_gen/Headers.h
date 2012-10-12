/* 
 * File:   Headers.h
 * Author: root
 *
 * Created on August 30, 2012, 8:21 PM
 */

#ifndef HEADERS_H
#define	HEADERS_H

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

// In a computer, it's LSB first, aligned to the byte.
// http://www.erg.abdn.ac.uk/~gorry/eg3561/lan-pages/mac.html

/* Ethernet header */
struct ethernet_headers {
        u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct ip_headers {
        u_char  ip_header  : 4;         /* header */ // These are swapped, LSB first
        u_char  ip_version : 4;         /* version */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off : 13;            /* fragment offset field */ // LSB first
        u_char  ip_more_fragments : 1;         /* more fragments flag */
        u_char  ip_dont_fragment : 1;         /* dont fragment flag */
        u_char  ip_reserved_bit : 1;     /* reserved fragment flag */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src, ip_dst;  /* source and dest address */
};

/* TCP header */
struct tcp_headers {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        u_int th_seq;                 /* sequence number */
        u_int th_ack;                 /* acknowledgement number */
        u_char  th_reserved : 4;        /* reserved */
        u_char  th_offset : 4;          /* data offset */
        u_char  th_f_fin : 1;           /* Flags */ // LSB first
        u_char  th_f_syn : 1;
        u_char  th_f_reset : 1;
        u_char  th_f_push : 1;
        u_char  th_f_ack : 1;
        u_char  th_f_urgent : 1;
        u_char  th_f_ece : 1;
        u_char  th_f_cwr : 1;
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};
#define TH_FLAGS(th) (th.th_cwr<<7 | th.th_ece<<6 | th.th_urgent<<5 | th.th_ack<<4 | \
                      th.th_push<<3 | th.th_reset<<2 | th.th_syn << 1 | th.th_fin)

#endif	/* HEADERS_H */

