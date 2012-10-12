/* 
 * File:   PacketCapture.h
 * Author: root
 *
 * Created on September 13, 2012, 2:29 PM
 */

#ifndef PACKETCAPTURE_H
#define	PACKETCAPTURE_H

#include <pcap.h>
#include "GenericPacket.h"
#include "GenericDispatcher.h"

#include <vector>
#include <utility>

class PacketCapture {
public:
    static vector<pair<GenericPacket*, GenericDispatcher*> > dispatch_queue;
    
    static void addDispatcher(GenericPacket* packet_headers, GenericDispatcher* dispatcher);
    static void clearDispatchVector();
    
    static int count;
    // static PacketCapture(){ count = 0; }
    
    static void capture(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
};

#endif	/* PACKETCAPTURE_H */
