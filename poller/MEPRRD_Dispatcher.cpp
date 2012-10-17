
#include "MEPRRD_Dispatcher.h"

/* TODO
 * 
 * check why the pop_front list is 25 (more than 20) (actually more than 1).
 * 
 */

/* Constructor - Generate RRD files */
MEPRRD_Dispatcher::MEPRRD_Dispatcher(){
    _pdp_step = 1;
    _rrd_update_size = 10; // Every n seconds
    
    _calculate_lost_meps_buff_size = 10;
    _buffer_size = 10 + _calculate_lost_meps_buff_size;
    
    _last_seqno = 0;
    _granularity_divider = 1; // second accuracy
    _containing_folder = "rrd//";
    _with_IP_specific_RRDs = 0;
    
    max_counter = 10000;
    counter = 0;
    
    /* RRD options
     * DS - Data Source, RRA - RR Archive
     * 
     * We generate one pdp each 'step' number of seconds.
     * The value stored in each pdp is the <type> in the
     * RRA option.
     * 
     * The number of RRAs stored is DS_options * RRA_options.
     * 
     * DS:<name>:<type>:<heartbeat>:<min_val>:<max_val>
     * <type>: GAUGE | COUNTER
     * 
     * RRA:<type>:<default_value>:<num_pdp>:<num_cdp>
     * <type>: AVERAGE | LAST
     */
    
    /* (not now) 1m  60.000.000 (every us)
     * 1h   3.600.000 (every ms)
     * 3h         180 (every min)
     * 24h        288 (every 5 mins)
     */
    _options.push_back("DS:mep:GAUGE:10:0:U");
    _options.push_back("RRA:AVERAGE:0.5:300:288");
    _options.push_back("RRA:AVERAGE:0.5:60:180");
    _options.push_back("RRA:AVERAGE:0.5:1:3600");
    _options.push_back("RRA:MAX:0.5:300:288");
    _options.push_back("RRA:MAX:0.5:60:180");
    // options.push_back("RRA:AVERAGE:0.5:1:60000000");
    
    // Create the aggregation RRD
    // Option only for the agg RRD
    _options.push_back("DS:lost_mep:GAUGE:10:0:U");
    createRRDFile(_pdp_step, "aggregation", _options);
    _options.pop_back();
}

void MEPRRD_Dispatcher::createRRDFile(int pdp_step, string filename, vector<string>& options){
    int status;
    // filename = filename + ".rrd";
    filename = _containing_folder + filename + ".rrd";
    
    // Calc of argc and argv
    char ** argv = NULL;
    int argc = Tools::vec2arg(options, argv);
    
    rrd_clear_error();
    
    /* RRD create */
    status = rrd_create_r (const_cast<const char*>(filename.c_str()),
            pdp_step, NULL,
            argc, (const char**) argv);
    
    if (status != 0) {
        cerr << "rrdtool plugin: rrd_create_r " << filename << " failed: "
             << rrd_get_error() << endl;
    }
    else {
        cout << "Created " << filename << endl;
    }
}

void MEPRRD_Dispatcher::dispatchPacket(GenericPacket* receivedPacket){
    pair<time_t, int> item;
    string update;
    
    MEPPacket* packet = new MEPPacket(receivedPacket);
    
    packet->time = time(&packet->timestamp.tv_sec); // ;
    _packet_ip = string(inet_ntoa(packet->ip->ip_src));
    
    if(_last_seqno == 0){
        _last_seqno = packet->mep->seqno - 1;
        _last_rx_seqno = _last_seqno;
        _last_timestamp = packet->time;
        // TODO! :)
        // _last_seqno = packet->mep->seqno - 1;
    }
    
    counter++;
    if(counter == max_counter){
        counter = 0;
        cout << _aggregate_updates.size() << " " << _filenames.size() << " " << _options.size() << " " << _packet_buffer.size() << endl;
    }
    
    // TODO:
    // Avoid garbage udp packets (the ones I don't want, for testing purposes)
    // Garbage udp packets == those with a mep->seqno value of 0 or differing in more than say 500.
    /* if(atoi((const char*) (packet->mep->seqno)) == 0 || (_last_rx_seqno + 500) < atoi((const char*) (packet->mep->seqno)))
        return; */
    
    _last_rx_seqno = packet->mep->seqno;
    
    // cout << "mep->seqno: " << atoi((const char*)packet->mep->seqno) << endl;
        
    
    /* Measure the MEP lost packets and update the rrd files for every statistic.
     * - Avoid out of order (keep an ordered buffer of, say, 10 elements;
     * we update a subset when we reach the capacity of the buffer, and take them out of the list.
     * idea: if problems, keep a dynamic-sized buffer).
     * - Handle also same timestamp problems (e.g. can happen with millisecond accuracy)
     */
    
    // If it's from a package we didn't receive before, create the RRD file:
    // TODO: What? packet doesn't like the "inttostr" bit for some reason.
    // cout << _packet_ip << endl;
    if (_filenames.find(_packet_ip) == _filenames.end() && _with_IP_specific_RRDs){
        // cout << Tools::inttostr(packet->ip->ip_src.s_addr) << " ";
        createRRDFile(_pdp_step, _packet_ip, _options);
        _filenames.insert(_packet_ip);
    }
    
    // Insert elements in the list by arrival order
    _packet_buffer.push_back(packet);
    if(_packet_buffer.size() >= _buffer_size){
        updateDataSets();
    }
    
    // Post data for last _rrd_update_size seconds
    if(_aggregate_updates.size() > _rrd_update_size)
        convertAggregateRRDAndPostUpdate();
}

void MEPRRD_Dispatcher::updateDataSets(){
    // There are two things to update: #mep and #lost_meps.
    // Each update is a pair of int, int therefore.
    // eg. 1397862342:<mep>:<lost_mep>
    
    // Grab all the elements up until the time (t) of the nth element with the
    // granularity we want.
    int i, no_elems = _calculate_lost_meps_buff_size;
    list<MEPPacket*>::iterator it_last = _packet_buffer.begin();
    for(i=0; i<no_elems; i++) it_last++;
    
    // Update no_elems
    updateAggregateRRD(no_elems, it_last);
    
    if(_with_IP_specific_RRDs)
        updateIPSpecificRRDs(no_elems, it_last);
    
    // Pop the first no_elems from the list
    for(int i=0; i<no_elems; i++)
        _packet_buffer.pop_front();
}

// Updates single IP RRDs (only for #mep)
// with the info in the first no_elems from _packet_buffer.
void MEPRRD_Dispatcher::updateIPSpecificRRDs(int no_elems, list<MEPPacket*>::iterator it_last){
    map<in_addr_t, map<time_t, int> > updates;
    in_addr temp;
    
    for(list<MEPPacket*>::iterator it = _packet_buffer.begin(); it != it_last; it++){
        // fill in #mep
        createOrAddToEntry(updates, (*it), 1);
    }
    
    // Make something updateRRD can understand
    vector<string> rrd_updates;
    for(map<in_addr_t, map<time_t, int> >::iterator it = updates.begin(); it != updates.end(); it++){
        for(map<time_t, int>::iterator it2 = (it->second).begin(); it2 != (it->second).end(); it2++){
            rrd_updates.push_back(Tools::toString<time_t>(it2->first) + ":" + Tools::toString<int>(it2->second));
        }
        
        temp.s_addr = it->first;
        
        cout << "Updating " << string(inet_ntoa(temp)) << endl;
        
        updateRRD(string(inet_ntoa(temp)), rrd_updates);
        rrd_updates.clear();
    }
}

// TODO: Clean this code.
// Updates the aggregate RRD with the info in the first no_elems from _packet_buffer.
void MEPRRD_Dispatcher::updateAggregateRRD(int no_elems, list<MEPPacket*>::iterator it_last){
    // list<pair<time_t, pair<int, int> > > updates;
    vector<string> rrd_updates;
    list<MEPPacket*>::iterator it;
    
    // cout << "no of elems: " << no_elems << endl;
    // cout << "current last_seqno: " << _last_seqno << endl;
    
    // Fill updates:
    int prev_last_seqno = _last_seqno;
    for(it = _packet_buffer.begin(); it != it_last; it++){
        // TODO: Change to MEP seqno
        if((*it)->mep->seqno > _last_seqno)
            _last_seqno = (*it)->mep->seqno;
        
        // fill in #mep
        createOrAddToEntry(_aggregate_updates, (*it)->time, 1, 0);
    }
    
    // cout << "new last seqno: " << _last_seqno << endl;
    
    // Order seqno's received until _last_seqno
    list<pair<int, time_t> > ordered_seqnos;
    int seqno;
    bool inserted;
    for(list<MEPPacket*>::iterator it1 = _packet_buffer.begin(); it1 != _packet_buffer.end(); it1++){
        seqno = (*it1)->mep->seqno;
        
        // cout << "mep->seqno: " << atoi((const char*) (*it1)->mep->seqno) << ", ";
        // cout << seqno << ", ";
        
        
        time_t last_time = (*it_last)->time;
        if(seqno < _last_seqno){
            inserted = 0;
            for(list<pair<int, time_t> >::iterator it2 = ordered_seqnos.begin(); it2 != ordered_seqnos.end(); it2++){
                if(it2->first > seqno){
                    if((*it1)->time < last_time)
                        ordered_seqnos.insert(it2, make_pair(seqno, (*it1)->time));
                    else
                        ordered_seqnos.insert(it2, make_pair(seqno, last_time));
                    inserted = 1;
                    break;
                }
            }

            if(inserted == 0){
                if((*it1)->time < last_time)
                    ordered_seqnos.push_back(make_pair(seqno, (*it1)->time));
                else
                    ordered_seqnos.push_back(make_pair(seqno, last_time));
            }
        }
    }
    
    /* cout << "Ordered seqnos:" << endl;
    for(list<pair<int, time_t> >::iterator it2 = ordered_seqnos.begin(); it2 != ordered_seqnos.end(); it2++){
        cout << (*it2).first << "," << it2->second << " - ";
    }
    cout << endl; */
    
    // Iterate seqnos and push updates to rrd for the lost ones
    int prev_seqno = prev_last_seqno;
    for(list<pair<int, time_t> >::iterator seqit = ordered_seqnos.begin(); seqit!=ordered_seqnos.end(); seqit++){
        if( seqit->first > prev_seqno+1 ){
            // fill in #lost_mep
            createOrAddToEntry(_aggregate_updates, seqit->second, 0, seqit->first-(prev_seqno+1));
            
            /* if(seqit->first > prev_seqno+5){
                cout << "Something weird happened:" << endl
                     << seqit->first << " - " << prev_seqno << endl;
            } */
        }

        prev_seqno = seqit->first;
    }
}

void MEPRRD_Dispatcher::convertAggregateRRDAndPostUpdate(){
    vector<string> rrd_updates;
    
    // Convert updates to updateRRD format
    list<pair<time_t, pair<int, int> > >::iterator last_item_pointer = _aggregate_updates.end();
    last_item_pointer--;
    for(list<pair<time_t, pair<int, int> > >::iterator upit = _aggregate_updates.begin(); upit != last_item_pointer; upit++){
        rrd_updates.push_back(string(
            Tools::toString<time_t>(upit->first) + ":" + Tools::toString<int>(upit->second.first) + ":" + 
            Tools::toString<int>(upit->second.second)
        ));
    }
    
    // Perform updates on aggregation rrd
    updateRRD(string("aggregation"), rrd_updates);
    
    pair<time_t, pair<int, int> > last_item = (*last_item_pointer);
    
    _aggregate_updates.clear();
    rrd_updates.clear();
    
    _aggregate_updates.push_front(last_item);
}

void MEPRRD_Dispatcher::createOrAddToEntry(list<pair<time_t, pair<int, int> > >& updates,
        time_t update_time, int no_mep, int no_lost_mep){
    
    bool inserted = 0;
    for(list<pair<time_t, pair<int, int> > >::iterator it = updates.begin(); it != updates.end(); it++){
        if(it->first > update_time){
            updates.insert(it, make_pair(update_time, make_pair(no_mep, no_lost_mep)));
            inserted = 1;
            break;
        }
        else if(it->first == update_time){
            it->second.first += no_mep;
            it->second.second += no_lost_mep;
            inserted = 1;
            break;
        }
    }
    
    if(!inserted)
        updates.push_back(make_pair(update_time, make_pair(no_mep, no_lost_mep)));
}

void MEPRRD_Dispatcher::createOrAddToEntry(map<in_addr_t, map<time_t, int> >& updates,
        MEPPacket* packet, int no_mep){
    
    map<time_t, int> ip_map;
    
    if(updates.find(packet->ip->ip_src.s_addr) != updates.end()){
        ip_map = updates[packet->ip->ip_src.s_addr];
        if(ip_map.find(packet->time) != ip_map.end())
            ip_map[packet->time] += no_mep;
        else
            ip_map[packet->time] = no_mep;
    }
    else {
        ip_map[packet->time] = no_mep;
        updates[packet->ip->ip_src.s_addr] = ip_map;
    }
}

void MEPRRD_Dispatcher::updateRRD(string filename, vector<string>& updates){
    int argc;
    char** argv = NULL;
    
    filename = _containing_folder + filename + ".rrd";

    // Calc of argc and argv
    argc = Tools::vec2arg(updates, argv);
    
    cout << "Update: ";
    for(vector<string>::iterator it = updates.begin(); it != updates.end(); it++)
        cout << (*it) << " ";
    cout << endl;

    /* RRD update */
    int status = rrd_update_r(const_cast<const char*>(filename.c_str()),
        NULL, // ? cont char* _template
        argc, (const char**) argv );
    if (status != 0) {
        cerr << "rrdtool plugin: rrd_update_r " << filename << " failed: "
             << rrd_get_error() << endl;
    }
    rrd_clear_error();
}
