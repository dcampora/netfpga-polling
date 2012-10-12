

#include "udp_headers.h"

UDPPacket::UDPPacket(GenericPacket* packet){
    // Assuming the packet is an UDPPacket
    this->processPacket(packet->_header, packet->_packet);
}

bool UDPPacket::processPacket(const struct pcap_pkthdr* header, const u_char* packet){
    this->cloneHeaderAndPacket(header, packet);
    
    // Generic type for all packets
    packet_type = "udp";
    
    // Get the timestamp
    timestamp = _header->ts;
            
    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
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
    
    if(protocol == "UDP"){
        /* define/compute tcp header offset */
        udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
        size_udp = ntohs(udp->ud_length);
        
        if (size_udp < 8) {
            // cerr << "Invalid UDP header length: " << size_udp << " bytes" << endl;
            return 0;
        }

        /* define/compute udp payload (segment) offset */
        payload = packet + SIZE_ETHERNET + size_ip + 8;

        /* compute udp payload (segment) size */
        size_payload = ntohs(ip->ip_len) - (size_ip + 8); /* size of udp headers is 8 */
    }
    else {
        // cerr << "Protocol specified is " << protocol << " (not UDP)" << endl;
        return 0;
    }
    
    return 1;
}