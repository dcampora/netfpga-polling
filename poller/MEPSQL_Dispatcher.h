/* 
 * File:   MEPSQL_Dispatcher.h
 * Author: root
 *
 * Created on October 15, 2012, 1:09 PM
 */

#ifndef MEPSQL_DISPATCHER_H
#define	MEPSQL_DISPATCHER_H

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

class MEPSQL_Dispatcher : public GenericDispatcher {
private:
    const char* db, *server, *user, *pass;
    string table_name;
    
    mysqlpp::Connection conn;
    
    int _max_id, _current_id;
    
    string _filename, _packet_ip;
    int _last_timestamp;
    int _last_seqno;
    int _last_rx_seqno;
    
    list<MEPPacket*> _packet_buffer;
    int _buffer_size;
    int _db_update_size;
    
    int counter, max_counter;
    

public:
    MEPSQL_Dispatcher();
    
    void dispatchPacket(GenericPacket*);
    
    void updateDataSets();
    void updateSQL(list<MEPPacket*>::iterator it_last);
};

#endif	/* MEPSQL_DISPATCHER_H */

