
#include "UDPRRD_Dispatcher.h"

/* Constructor - Generate RRD files */
UDPRRD_Dispatcher::UDPRRD_Dispatcher(string filename){
    int status;
    _filename = filename;
    _pdp_step = 1; // TODO: Check good sizes for pdp
    _rrd_update_size = 10;
    _buffer_size = 10 + _rrd_update_size;
    
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
    vector<string> options;
    options.push_back("DS:num_mep:GAUGE:120:0:U");
    options.push_back("DS:lost_mep:GAUGE:120:0:U");
    options.push_back("RRA:AVERAGE:0.5:1:300");
    options.push_back("RRA:LAST:0.5:1:300");
    
    // Calc of argc and argv
    char ** argv = NULL;
    int argc = Tools::vec2arg(options, argv);
    
    rrd_clear_error (); 
    
    /* RRD create */
    status = rrd_create_r (const_cast<const char*>(_filename.c_str()),
            _pdp_step, _last_up,
            argc, (const char**) argv);
    
    if (status != 0) {
        cerr << "rrdtool plugin: rrd_create_r " << _filename << " failed: "
             << rrd_get_error() << endl;
    }
}

void UDPRRD_Dispatcher::dispatchPacket(GenericPacket* receivedPacket){
    pair<time_t, int> item;
    string update;
    
    UDPPacket* packet = new UDPPacket(receivedPacket);
    
    /* Measure the UDP packets received at any point in time:
     * - Avoid out of order (keep a buffer of, say, 10 elements;
     * if problems, keep a dynamic-sized buffer).
     * - Post when second is higher than before (avoid same second post).
     */
    
    time_t arrival_time = time(&packet->timestamp.tv_sec);
    
    /* Idea: Buffer is a list
     * 
     * 1- Insert element in the list:
     *  a. If there is such an element already, increment counter. O(n log k)
     *  b. Otherwise, add in order the element.
     * 2- If buffer size is reached, push a batch update to rrdupdate.
     */
    // 1a
    bool found = 0;
    for (list<pair<time_t, int> >::iterator it = _packet_buffer.begin(); it != _packet_buffer.end(); it++){
        if((*it).first == arrival_time){
            found = 1;
            (*it).second++;
            break;
        }
    }
    
    // 1b
    list<pair<time_t, int> >::iterator location = _packet_buffer.end();
    if(!found){
        for (list<pair<time_t, int> >::iterator it = _packet_buffer.end(); it != _packet_buffer.begin(); it--){
            if(arrival_time > (*it).first){
                location = it; location++;
                break;
            }
        }
        _packet_buffer.insert(location, make_pair(arrival_time, 1));
    }
    
    // 2
    if(_packet_buffer.size() >= _buffer_size){
        // Populate _updates from the first _rrd_update_size elements from
        // _packet_buffer and delete them (pop_front)
        
        for(int i=0; i<_rrd_update_size; i++){
            item = _packet_buffer.front();
            update = Tools::toString<time_t>(item.first) + ":" + 
                    Tools::toString<int>(item.second);
            
            _updates.push_back(update);
            _packet_buffer.pop_front();
        }
        
        updateRRD();
        _updates.clear();
    }
}

void UDPRRD_Dispatcher::updateRRD(){
    int argc;
    char** argv = NULL;

    // Calc of argc and argv
    argc = Tools::vec2arg(_updates, argv);

    /* RRD update */
    int status = rrd_update_r(const_cast<const char*>(_filename.c_str()),
        NULL, // ? cont char* _template
        argc, (const char**) argv );
    if (status != 0) {
        cerr << "rrdtool plugin: rrd_update_r " << _filename << " failed: "
             << rrd_get_error() << endl;
    }
}