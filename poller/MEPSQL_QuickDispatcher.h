/* 
 * File:   MEPSQL_Dispatcher.h
 * Author: root
 *
 * Created on October 15, 2012, 1:09 PM
 */

#ifndef MEPSQL_QUICKDISPATCHER_H
#define	MEPSQL_QUICKDISPATCHER_H

#include <iostream>
#include <string>
#include <list>
#include <set>
#include <map>

#include "GenericDispatcher.h"
#include "mep_headers.h"
#include "Tools.h"
#include "PacketCapture.h"

#include <arpa/inet.h>
#include <mysql++.h>

using namespace std;

class MEPSQL_QuickDispatcher : public GenericDispatcher {
private:
    const char* db, *server, *user, *pass;
    string table_name;
    
    mysqlpp::Connection conn;
    
    int _max_id, _current_id;
    
    string _filename, _packet_ip;
    int _last_timestamp;
    
    int _prev_seqno;
    int _last_rx_seqno;
    
    MEPPacket* _packet;
    
    map<string, int> seqnos;
    
    /*list<MEPPacket*> _packet_buffer;
    int _buffer_size;
    int _db_update_size;
    
    int counter, max_counter;*/
    

public:
    MEPSQL_QuickDispatcher();
    
    void dispatchPacket(GenericPacket*);
    void updateSQL(int lost);
};

#endif	/* MEPSQL_DISPATCHER_H */

