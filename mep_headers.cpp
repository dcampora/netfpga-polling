

#include "mep_headers.h"


MEPPacket::MEPPacket(GenericPacket* packet){
    this->processPacket(packet->_header, packet->_packet);
}

bool MEPPacket::processPacket(const struct pcap_pkthdr* header, const u_char* packet){
    this->cloneHeaderAndPacket(header, packet);
    
    // Generic type for all packets
    packet_type = "mep";
    
    // Get the timestamp
    timestamp = _header->ts;
            
    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(_packet);

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(_packet + SIZE_ETHERNET);
    size_ip = ip->ip_header * 4;
    if (size_ip < 20) {
            // cerr << "Invalid IP header length: " << size_ip << endl;
            return 0;
    }

    /* determine protocol */
    string protocol = "unknown";
    
    if (size_ip < 20)
        protocol += " (not-IP)";
    else {
        switch(this->ip->ip_p) {
            case IPPROTO_TCP:
                protocol = "TCP";
                break;
            case IPPROTO_UDP:
                protocol = "UDP";
                break;
            case IPPROTO_ICMP:
                protocol = "ICMP";
                break;
            case IPPROTO_IP:
                protocol = "IP";
                break;
        }
    }
    
    if(protocol == " (not-IP)"){
        mep = (struct MEPReq*)(_packet + SIZE_ETHERNET + size_ip);
    }
    else {
        // cerr << "Protocol specified is " << protocol << " (not UDP)" << endl;
        return 0;
    }
    
    return 1;
}