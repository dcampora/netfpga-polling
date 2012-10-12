/* 
 * File:   MEPRRD_Dispatcher.h
 * Author: root
 *
 * Created on October 9, 2012, 12:41 PM
 */

#ifndef MEPRRD_DISPATCHER_H
#define	MEPRRD_DISPATCHER_H

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <map>

#include <rrd.h>

#include "GenericDispatcher.h"
#include "mep_headers.h"
#include "Tools.h"
#include "PacketCapture.h"

#include <arpa/inet.h>

using namespace std;

class MEPRRD_Dispatcher : public GenericDispatcher {
private:
    string _filename, _packet_ip;
    u_long _pdp_step;
    time_t _last_timestamp;
    int _last_seqno;
    int _last_rx_seqno;
    
    // list<pair<int, UDPPacket*> > _packet_buffer;
    list<UDPPacket*> _packet_buffer;
    int _buffer_size;
    int _rrd_update_size;
    int _granularity_divider;
    string _containing_folder;
    
    // vector<string> _updates;
    vector<string> _options;
    set<string> _filenames;
    

public:
    MEPRRD_Dispatcher();
        
    void dispatchPacket(GenericPacket*);
    
    void createRRDFile(int pdp_step, string filename, vector<string>& options);
    
    void updateDataSets();
    void updateIPSpecificRRDs(int no_elems, list<UDPPacket*>::iterator it_last);
    void updateAggregateRRD(int no_elems, list<UDPPacket*>::iterator it_last);
    
    void updateRRD(string filename, vector<string>& updates);
    
    void createOrAddToEntry(list<pair<time_t, pair<int, int> > >& updates,
        time_t update_time, int no_mep, int no_lost_mep);
    void createOrAddToEntry(map<in_addr_t, map<time_t, int> >& updates,
        UDPPacket* packet, int no_mep);
};

#endif	/* MEPRRD_DISPATCHER_H */

