
#include "MEPSQL_Dispatcher.h"

/* Constructor - Generate RRD files */
MEPSQL_Dispatcher::MEPSQL_Dispatcher(){
    _db_update_size = 10;
    _buffer_size = _db_update_size + 10;
    _max_id = 100000;
    _current_id = 1;
    
    db = "stats", server = "localhost", user = "root", pass = "";
    table_name = "mep_statistics";
    
    // Connect to the sample database.
    if (!conn.connect(db, server, user, pass)) {
        cerr << "DB connection failed: " << conn.error() << endl;
    }
    
    mysqlpp::Query query = conn.query();
    
    // Create the database types
    query << "DROP TABLE IF EXISTS `mep_statistics`;";
    query.execute();
    query.reset();
    
    query << "CREATE TABLE `" << table_name << "` ( \
        `id` int(11) NOT NULL auto_increment, \
        `timestamp_s` int(11) NOT NULL, \
        `timestamp_ns` int(11) NOT NULL, \
        PRIMARY KEY  (`id`) \
    ) ENGINE=MyISAM DEFAULT CHARSET=latin1;";
    query.execute();
    query.reset();
    
    cout << "Database " << table_name << " created" << endl;
}

void MEPSQL_Dispatcher::dispatchPacket(GenericPacket* receivedPacket){
    MEPPacket* packet = new MEPPacket(receivedPacket);
    
    // cout << packet->timestamp.tv_sec << "." << packet->timestamp.tv_nsec;
    _packet_ip = string(inet_ntoa(packet->ip->ip_dst));
    
    if(_last_seqno == 0){
        _last_seqno = packet->mep->seqno - 1;
        // _last_seqno = atoi((const char*) (packet->mep->seqno)) - 1;
        _last_rx_seqno = _last_seqno;
        // TODO! :)
        // _last_seqno = packet->mep->seqno - 1;
    }
    
    
    counter++;
    if(counter == max_counter){
        counter = 0;
        cout << _packet_buffer.size() << endl;
    }
    
    
    // TODO:
    // Avoid garbage udp packets (the ones I don't want, for testing purposes)
    // Garbage udp packets == those with a mep->seqno value of 0 or differing in more than say 500.
    /*if(atoi((const char*) (packet->mep->seqno)) == 0 || (_last_rx_seqno + 500) < atoi((const char*) (packet->mep->seqno)))
        return; */
    
    _last_rx_seqno = packet->mep->seqno;
    // _last_rx_seqno = atoi((const char*) (packet->mep->seqno));
    
    // cout << "mep->seqno: " << atoi((const char*)packet->mep->seqno) << endl;
        
    
    /* Measure the MEP lost packets and update the rrd files for every statistic.
     * - Avoid out of order (keep an ordered buffer of, say, 10 elements;
     * we update a subset when we reach the capacity of the buffer, and take them out of the list.
     */
    
    // Insert elements in the list by arrival order
    _packet_buffer.push_back(packet);
    if(_packet_buffer.size() >= _buffer_size){
        updateDataSets();
    }
}

void MEPSQL_Dispatcher::updateDataSets(){
    // There are two things to update: #mep and #lost_meps.
    // Each update is a pair of int, int therefore.
    // eg. 1397862342:<mep>:<lost_mep>
    
    // Grab all the elements up until the time (t) of the nth element with the
    // granularity we want.
    int i, no_elems = _db_update_size;
    list<MEPPacket*>::iterator it_last = _packet_buffer.begin();
    for(i=0; i<no_elems; i++) it_last++;
    
    updateSQL(it_last);
    
    // Remove and pop!
    for(list<MEPPacket*>::iterator it = _packet_buffer.begin(), int i=0; i<no_elems; it++, i++)
        delete (*it);
    
    // Pop the first no_elems from the list
    for(int i=0; i<no_elems; i++)
        _packet_buffer.pop_front();
}

// TODO: Check the query is executed (check for errors).

void MEPSQL_Dispatcher::updateSQL(list<MEPPacket*>::iterator it_last){
    list<MEPPacket*>::iterator it;
    mysqlpp::Query query = conn.query();
    
    // Fetch last seqno
    int prev_last_seqno = _last_seqno;
    for(it = _packet_buffer.begin(); it != it_last; it++){
        if((*it)->mep->seqno > _last_seqno)
             _last_seqno = (*it)->mep->seqno;
        // if(atoi((const char*)(*it)->mep->seqno) > _last_seqno)
        //     _last_seqno = atoi((const char*)(*it)->mep->seqno);
    }
    
    // Order seqno's received until _last_seqno
    list<pair<int, struct timespec> > ordered_seqnos;
    int seqno;
    bool inserted;
    for(list<MEPPacket*>::iterator it1 = _packet_buffer.begin(); it1 != _packet_buffer.end(); it1++){
        seqno = (*it1)->mep->seqno;
        // seqno = atoi((const char*)(*it1)->mep->seqno);
        
        if(seqno < _last_seqno){
            inserted = 0;
            for(list<pair<int, struct timespec> >::iterator it2 = ordered_seqnos.begin(); it2 != ordered_seqnos.end(); it2++){
                if(it2->first > seqno){
                    ordered_seqnos.insert(it2, make_pair(seqno, (*it1)->timestamp));
                    inserted = 1;
                    break;
                }
            }

            if(inserted == 0)
                ordered_seqnos.push_back(make_pair(seqno, (*it1)->timestamp));
        }
    }
    
    // Iterate seqnos and push updates to rrd for the lost ones
    int prev_seqno = prev_last_seqno;
    for(list<pair<int, struct timespec> >::iterator seqit = ordered_seqnos.begin(); seqit!=ordered_seqnos.end(); seqit++){
        if( seqit->first > prev_seqno+1 ){
            
            for(int i=0; i<(seqit->first-(prev_seqno+1)); i++){
                cout << "insert into " << table_name << " (id, timestamp_s, timestamp_ns) values (" << _current_id << ", " << seqit->second.tv_sec << ", " << seqit->second.tv_nsec << ")" << endl;
                
                // fill in #lost_mep
                query << "insert into " << table_name << " (id, timestamp_s, timestamp_ns) values (" << _current_id << ", " << seqit->second.tv_sec << ", " << seqit->second.tv_nsec << ")";
                query.execute();
                query.reset();
                
                if(_current_id > _max_id)
                    _current_id = 1;
                else
                    _current_id++;
            }
        }

        prev_seqno = seqit->first;
    }
}
