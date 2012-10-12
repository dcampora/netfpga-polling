

// Oldie libraries
#include <libnet.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>

// New style libraries yey!
#include <cstdio>
#include <cstdlib>

// Awesomesauce C++ is on the way!
#include <iostream>

// My own awesomesauce
#include "Tools.h"
#include "Headers.h"
#include "ConnectionSetup.h"

using namespace std;

int main(int argc, char **argv)
{    
    char *tx_if = "eth0";	/* tx if */
    string pkt_type = "udp";	/* tx if */
    int pkt_per_second = 10;	/* number of packets to send p/s */
    
    libnet_t *l;  /* the libnet context */
    char libnet_errbuf[LIBNET_ERRBUF_SIZE];
    
    u_int16_t id, seq;
    string ip_addr_str = "127.0.0.1";
    int bytes_written;
    string payload =  "it works!";
    int protocol;
    
    /* Get options */
    int c;
    while ((c = getopt(argc, argv, "i:p:t:")) != -1)
    switch (c){  
         case 'i': tx_if = optarg; break;
         case 'p': pkt_per_second = atoi(optarg); break;
         case 't': pkt_type = optarg; break;
         case '?': cerr << "Usage: " << argv[0]
           << " [-i <tx_interface> -p <pkt_per_second> -t [udp,icmp]]" << endl;
         return -1;
         break;
    }

    /* LIBNET INIT */
    l = libnet_init(LIBNET_LINK, tx_if, libnet_errbuf);
    if ( l == NULL ) {
        fprintf(stderr, "libnet_init() failed: %s\n", libnet_errbuf);
        exit(EXIT_FAILURE);
    }
    
    /* Getting destination IP address */
    ConnectionSetup conn = ConnectionSetup(l);
    conn.set_rx_parameters_interactively();
    
    /* Generating a random id */
    libnet_seed_prand (l);
    id = (u_int16_t)libnet_get_prand(LIBNET_PR16);

    if ( conn.get_rx_ip() == -1) {
        cerr << "Invalid destination IP address" << endl;
        libnet_destroy(l);
        exit(EXIT_FAILURE);
    }
    
    if (pkt_type == "udp"){
        /* Building UDP header */
        
        if (libnet_build_udp(
                10000, 10000, 10000, 0, (u_int8_t*)payload.c_str(), payload.length(), l, 0) == -1)
        {
          fprintf(stderr, "Error building UDP header: %s\n",\
              libnet_geterror(l));
          libnet_destroy(l);
          exit(EXIT_FAILURE);
        }
        
        protocol = IPPROTO_UDP;
    } else if (pkt_type == "icmp"){
        /* Building ICMP header */
        
        seq = 1;
        if (libnet_build_icmpv4_echo(ICMP_ECHO, 0, 0, id, seq,\
              (u_int8_t*)payload.c_str(),payload.length(), l, 0) == -1)
        {
          fprintf(stderr, "Error building ICMP header: %s\n",\
              libnet_geterror(l));
          libnet_destroy(l);
          exit(EXIT_FAILURE);
        }
        
        protocol = IPPROTO_ICMP;
    }

    
    /* Manually Building IP header - doesn't work yet */
    /* if (libnet_build_ipv4(LIBNET_IPV4_H + LIBNET_ICMPV4_ECHO_H + payload.length(),\
          0             , \ // tos
          0             , \ // id
          1 << 1        , \ // framentation bits
          64            , \ // time to live 
          protocol      , \ // upper layer protocol
          0             , \ // checksum (0 is autofill)
          ip_addr_src   , \ // ip src
          ip_addr_dst   , \ // ip dst
          (u_int8_t*) payload.c_str(), \
          payload.length(), l, \
          0 ) == -1 ) // build new protocol tag (?)
    {
      fprintf(stderr, "Error building IP header: %s\n",\
          libnet_geterror(l));
      libnet_destroy(l);
      exit(EXIT_FAILURE);
    } */
    
    
    /* Automatically build IPv4 headers */                                       
    if (libnet_autobuild_ipv4(LIBNET_IPV4_H +\
          LIBNET_ICMPV4_ECHO_H + payload.length(),\
          protocol, conn.get_rx_ip(), l) == -1 )
    {
      fprintf(stderr, "Error building IP header: %s\n",\
          libnet_geterror(l));
      libnet_destroy(l);
      exit(EXIT_FAILURE);
    }
    
    /* Automatically build ethernet header */
    if ( libnet_autobuild_ethernet (conn.get_rx_mac(),\
                          ETHERTYPE_IP, l) == -1 )
    {
      fprintf(stderr, "Error building Ethernet header: %s\n",\
          libnet_geterror(l));
      libnet_destroy(l);
      exit(EXIT_FAILURE);
    }

    
    // Testing it works! :)
    
    /* cout << conn.rx_ip << endl << conn.tx_ip << endl
         << conn.rx_mac << endl << conn.tx_mac << endl
         << conn._length << endl; */
    
    u_int32_t conn_ip_temp = conn.get_rx_ip();
    u_int8_t* ip_addr_p = (u_int8_t*)(&conn_ip_temp);
    printf("IP address read: %d.%d.%d.%d\n", ip_addr_p[0],\
        ip_addr_p[1], ip_addr_p[2], ip_addr_p[3]);
    
    printf("MAC address read: ");
    if(conn._length && conn._length < 20){
        for ( int i=0; i < conn._length; i++) {
          printf("%02X", conn.get_rx_mac()[i]);
          if ( i < conn._length-1 )
            printf(":");
        }
    printf("\n");
    } // */
    
    
    
    // Pkt per second calc
    int usleep_units = 1000000 / pkt_per_second;
    
    /* Writing packets */
    cout << endl << "Sender: " << tx_if << endl
         << "Receiver: " << conn.rx_ip << endl
         << "Pkt type: " << pkt_type << endl;
    
    while(true){
        bytes_written = libnet_write(l);
        if ( bytes_written != -1 )
            cout << ".";
        else {
            cout << "e";
            cerr << "-" << libnet_geterror(l) << endl;
        }
        cout << flush;
        
        usleep(usleep_units);
    }
    
    cout << endl;
    
    libnet_destroy(l);
    return 0;
}
