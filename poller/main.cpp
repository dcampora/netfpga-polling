
#include <pcap.h>
#include <cstdlib>
#include <getopt.h>

#include <iostream>
#include <iomanip>

#include "Tools.h"
#include "udp_headers.h"
#include "MEPRRD_Dispatcher.h"
#include "MEPSQL_QuickDispatcher.h"
#include "PacketCapture.h"

#include "UDP_test.h"

/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

int main(int argc, char **argv)
{
    char *capture_dev = "lo";			/* capture device name */
    
    char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
    pcap_t *handle;				/* packet capture handle */

    char *filter_exp = "ip[9] == 17";                    /* filter expression [3] */
    struct bpf_program fp;			/* compiled filter program (expression) */
    bpf_u_int32 mask;                           /* subnet mask */
    bpf_u_int32 net;                            /* ip */
    int num_packets = -1;			/* number of packets to capture */
    
    string rrd_filename = "traffic_stats.rrd";  /* name of rrd file */
    
    /* Catch options */
    int c;
    while ((c = getopt(argc, argv, "i:f:")) != EOF)
    switch (c){
         case 'i': capture_dev = optarg; break;
         case '?': cerr << "Usage: " << argv[0]
           << " [-i <interface_to_sniff_on>]" << endl;
         return -1;
         break;
    }
    
    if (capture_dev==NULL){
        /* find a capture device if not specified on command-line */
         capture_dev= pcap_lookupdev(errbuf);
        if ( capture_dev== NULL) {
            cerr << "Couldn't find default device: " << errbuf << endl;
            exit(EXIT_FAILURE);
        }
    }

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet( capture_dev, &net, &mask, errbuf) == -1) {
        cerr << "Couldn't get netmask for device " << capture_dev << ": " << errbuf << endl;
        net = 0;
        mask = 0;
    }
    
    /* print capture info */
    cout << "Device: " << capture_dev << endl
         << "Number of packets: " << num_packets << endl
         << "Filter expression: " << filter_exp << endl << endl;
    
    /* open capture device */
    handle = pcap_open_live( capture_dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        cerr << "Couldn't open device " << capture_dev << ": " << errbuf << endl;
        exit(EXIT_FAILURE);
    }
    
    /* make sure we're capturing on an Ethernet device [2] */
    if (pcap_datalink(handle) != DLT_EN10MB) {
        cerr << capture_dev << " is not an Ethernet frame" << endl;
        exit(EXIT_FAILURE);
    }
    
    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        cerr << "Couldn't parse filter " << filter_exp << ": " << pcap_geterr(handle) << endl;
        exit(EXIT_FAILURE);
    }
    
    
    /* Get a packet description and a dispatcher and let the magic begin :D */
    // GenericPacket* packet_processor = new MEPPacket();
    GenericPacket* packet_processor = new UDPPacket();
    
    // GenericDispatcher* db_dispatcher = new MEPSQL_QuickDispatcher();
    GenericDispatcher* rrd_dispatcher = new MEPRRD_Dispatcher();
    // GenericDispatcher* udp_test_dispatcher = new UDP_test();
    
    PacketCapture::addDispatcher(packet_processor, rrd_dispatcher);
    // PacketCapture::addDispatcher(packet_processor, db_dispatcher);
    // PacketCapture::addDispatcher(packet_processor, udp_test_dispatcher);
    
    /* now we can set our callback function */
    pcap_loop(handle, num_packets, PacketCapture::capture, NULL);
    
    // CLEANUP
    pcap_freecode(&fp);
    pcap_close(handle);
    
    return 0;
}
