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
    
    // list<pair<int, MEPPacket*> > _packet_buffer;
    list<MEPPacket*> _packet_buffer;
    int _rrd_update_size, _buffer_size, _calculate_lost_meps_buff_size;
    int _granularity_divider;
    string _containing_folder;
    
    // vector<string> _updates;
    vector<string> _options;
    set<string> _filenames;
    bool _with_IP_specific_RRDs;
    
    list<pair<time_t, pair<int, int> > > _aggregate_updates;
    
    int counter, max_counter;
    
    // TODO
    // map<vector<string> > _IP_specific_RRD_updates
    

public:
    MEPRRD_Dispatcher();
        
    void dispatchPacket(GenericPacket*);
    
    void createRRDFile(int pdp_step, string filename, vector<string>& options);
    
    void updateDataSets();
    void updateIPSpecificRRDs(int no_elems, list<MEPPacket*>::iterator it_last);
    void updateAggregateRRD(int no_elems, list<MEPPacket*>::iterator it_last);
    
    void convertAggregateRRDAndPostUpdate();
    
    void updateRRD(string filename, vector<string>& updates);
    
    void createOrAddToEntry(list<pair<time_t, pair<int, int> > >& updates,
        time_t update_time, int no_mep, int no_lost_mep);
    void createOrAddToEntry(map<in_addr_t, map<time_t, int> >& updates,
        MEPPacket* packet, int no_mep);
};

#endif	/* MEPRRD_DISPATCHER_H */

