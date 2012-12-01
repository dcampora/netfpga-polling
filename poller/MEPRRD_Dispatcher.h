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

/* #include "tbb/tbb.h"
using namespace tbb;

class ParallelRRDWriting {
    float *const my_a;
public:
    void operator()( const blocked_range<size_t>& r ) const {
        float *a = my_a;
        for( size_t i=r.begin(); i!=r.end(); ++i )
           Foo(a[i]);
    }
    ApplyFoo( float a[] ) :
        my_a(a)
    {}
}; */

class MEPRRD_Dispatcher : public GenericDispatcher {
private:
    string _filename, _packet_ip;
    u_long _pdp_step;
    int _packet_seqno, _packet_time, _max_update_time,
        _threshold_minutes, _starting_time;
    
    // list<pair<int, UDPPacket*> > _packet_buffer;
    list<UDPPacket*> _packet_buffer;
    int _rrd_update_size, _buffer_size, _calculate_lost_meps_buff_size;
    int _granularity_divider;
    string _containing_folder;
    
    // vector<string> _updates;
    vector<string> _options;
    set<string> _filenames;
    bool _with_IP_specific_RRDs;
    
    list<pair<time_t, pair<int, int> > > _aggregate_updates;
    
    map<string, map<int, int> > lost_meps;
    map<string, map<int, int> > received_meps;
    map<string, int> last_seqnos;
    
    int counter, max_counter;
    
    // TODO
    // map<vector<string> > _IP_specific_RRD_updates
    

public:
    MEPRRD_Dispatcher();
        
    void dispatchPacket(GenericPacket*);
    
    void createRRDFile(int pdp_step, string filename, vector<string>& options);
    
    void updateDataSets();
    void updateIPSpecificRRDs();
    void updateAggregateRRD();
    
    void removeOldieUpdates();
    
    void updateRRD(string filename, vector<string>& updates);
};

#endif	/* MEPRRD_DISPATCHER_H */

