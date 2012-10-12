
#include "tcp_headers.h"

TCPPacket::TCPPacket(GenericPacket* packet){
    this->processPacket(packet->_header, packet->_packet);
}

bool TCPPacket::processPacket(const struct pcap_pkthdr* header, const u_char* packet){
    // Generic type for all packets
    packet_type = "tcp";
            
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
    
    if(protocol == "TCP"){
        /* define/compute tcp header offset */
        tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
        size_tcp = tcp->th_offset * 4;
        
        if (size_tcp < 20) {
            // cerr << "Invalid TCP header length: " << size_tcp << " bytes" << endl;
            return 0;
        }

        /* define/compute tcp payload (segment) offset */
        payload = packet + SIZE_ETHERNET + size_ip + size_tcp;

        /* compute tcp payload (segment) size */
        size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
    }
    else {
        // cerr << "Protocol specified is " << protocol << " (not TCP)" << endl;
        return 0;
    }
    
    return 1;
}