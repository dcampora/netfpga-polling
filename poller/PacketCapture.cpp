
#include "PacketCapture.h"

int PacketCapture::count = 0;
vector<pair<GenericPacket*, GenericDispatcher*> > PacketCapture::dispatch_queue;

void PacketCapture::capture(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    count++;
    bool processed = 0;
 
    // Dispatcher in action! :D
    for(int i=0; i<dispatch_queue.size(); i++){
        GenericPacket* packetProcesser = dispatch_queue[i].first;
        GenericDispatcher* dispatcher = dispatch_queue[i].second;
        
        if(packetProcesser->processPacket(header, packet)){
            // cout << "Processing a " << it->first->packet_type << " packet";
            // cout << ".";
            // cout << flush;
            
            dispatcher->dispatchPacket(packetProcesser);
            processed = 1;
            // return;
        }
        
        packetProcesser->freePacketAndHeader();
    }

    if(processed){
        // cout << ".";
        // cout << flush;

        return;
    }
    
    cout << "x";
    cout << flush;
}

void PacketCapture::addDispatcher(GenericPacket* packet_headers, GenericDispatcher* dispatcher){
    dispatch_queue.push_back( make_pair(packet_headers, dispatcher) );
}

void PacketCapture::clearDispatchVector(){
    dispatch_queue.clear();
}
