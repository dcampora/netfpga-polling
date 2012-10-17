
#include "GenericPacket.h"
#include "Tools.h"

GenericPacket::GenericPacket(GenericPacket* packet){
    this->processPacket(packet->_header, packet->_packet);
}

GenericPacket::~GenericPacket(){
    free(_packet);
}

void GenericPacket::cloneHeaderAndPacket(const struct pcap_pkthdr* header, const u_char* packet){
    _header = new pcap_pkthdr;
    
    _header->caplen = header->caplen;
    _header->len = header->len;
    _header->ts = header->ts;
    
    _packet = (u_char*) malloc(_header->len * sizeof(u_char));
    
    string temp = string((char*)packet, _header->len);
    temp.copy((char*) _packet, _header->len, 0);
    
    // cout << Tools::bitfield2bin<string>(temp, _header->len * 8) << endl;
}
