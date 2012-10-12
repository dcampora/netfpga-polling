/* 
 * File:   GenericPacket.h
 * Author: root
 *
 * Created on September 13, 2012, 3:06 PM
 */

#ifndef GENERICPACKET_H
#define	GENERICPACKET_H

#include <pcap.h>

#include <vector>
#include <iostream>
#include <cstdlib>
#include <string>

using namespace std;

class GenericPacket {
public:
    GenericPacket(){}
    GenericPacket(GenericPacket* packet);
    
    struct pcap_pkthdr* _header;
    u_char* _packet;
    
    void cloneHeaderAndPacket(const struct pcap_pkthdr* header, const u_char* packet);
    
    string packet_type;
    virtual bool processPacket(const struct pcap_pkthdr *header, const u_char *packet){}
    
};

#endif	/* GENERICPACKET_H */

