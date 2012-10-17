
#include "UDP_test.h"

/* Constructor - Generate RRD files */
UDP_test::UDP_test(){
}

void UDP_test::dispatchPacket(GenericPacket* receivedPacket){
    pair<time_t, int> item;
    string update;
    
    MEPPacket* packet = new MEPPacket(receivedPacket);
    delete packet;
    
    time_t arrival_time = time(&packet->timestamp.tv_sec);
    
    // cout << "payload size: " << packet->size_payload << endl;
    
    // cout << "udp size: " << ntohs(packet->udp->ud_length) << endl;
    
    // cout << "payload: " << Tools::toString<const u_char*>(packet->payload) << endl;
    
    
}