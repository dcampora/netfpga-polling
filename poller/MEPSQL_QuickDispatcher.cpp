
#include "MEPSQL_QuickDispatcher.h"

/* Constructor - Generate RRD files */
MEPSQL_QuickDispatcher::MEPSQL_QuickDispatcher(){
    _prev_seqno = 0;
    _last_rx_seqno = 0;
    _current_id = 1;
    _max_id = 10000;
    
    _packet = new MEPPacket();
    
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
        `ip` varchar(100), \
        `timestamp_s` int(11) NOT NULL, \
        `timestamp_us` int(11) NOT NULL, \
        `lost_no` int(11) NOT NULL, \
        PRIMARY KEY  (`id`) \
    ) ENGINE=MyISAM DEFAULT CHARSET=latin1;";
    query.execute();
    query.reset();
    
    cout << "Database " << table_name << " created" << endl;
}

void MEPSQL_QuickDispatcher::dispatchPacket(GenericPacket* receivedPacket){
    _packet = new MEPPacket(receivedPacket);
    
    // cout << packet->timestamp.tv_sec << "." << packet->timestamp.tv_nsec;
    _packet_ip = string(inet_ntoa(_packet->ip->ip_dst));
    int current_seqno = htonl(_packet->mep->seqno);
    
    if(seqnos.find(_packet_ip) != seqnos.end()){
        _prev_seqno = seqnos[_packet_ip];
        seqnos[_packet_ip] = current_seqno;
        
        if(current_seqno - _prev_seqno > 1)
            updateSQL(current_seqno - _prev_seqno);
    }
    else
        seqnos[_packet_ip] = current_seqno;
    
    cout << _packet_ip << " - " << current_seqno << endl;
    
    delete _packet;
}

// TODO: Check the query is executed (check for errors).

void MEPSQL_QuickDispatcher::updateSQL(int lost){
    mysqlpp::Query query = conn.query();
    
    cout << _packet_ip << ": " << _packet->timestamp.tv_sec << "." << _packet->timestamp.tv_usec << ", " << lost << endl;
    
    query << "insert into " << table_name << " (id, ip, timestamp_s, timestamp_us, lost_no) values ("
          << _current_id << ", '" << _packet_ip << "', " << _packet->timestamp.tv_sec << ", " << _packet->timestamp.tv_usec << ", " << lost << ")";
    query.execute();
    query.reset();
    
    _current_id++;
    if(_current_id == _max_id)
        _current_id = 1;
}
