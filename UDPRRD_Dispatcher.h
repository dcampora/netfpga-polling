/* 
 * File:   TCPRRD_Dispatcher.h
 * Author: root
 *
 * Created on September 13, 2012, 4:22 PM
 */

#ifndef UDPRRD_DISPATCHER_H
#define	UDPRRD_DISPATCHER_H

#include <iostream>
#include <string>
#include <list>

#include <rrd.h>

#include "GenericDispatcher.h"
#include "udp_headers.h"
#include "Tools.h"
#include "PacketCapture.h"

using namespace std;

class UDPRRD_Dispatcher : public GenericDispatcher {
private:
    string _filename;
    u_long _pdp_step;
    time_t _last_up;
    
    list<pair<time_t, int> > _packet_buffer;
    int _buffer_size;
    int _rrd_update_size;
    
    vector<string> _updates;

public:
    UDPRRD_Dispatcher(string);
        
    void dispatchPacket(GenericPacket*);
    
    void updateRRD();
};

#endif	/* UDPRRD_DISPATCHER_H */

