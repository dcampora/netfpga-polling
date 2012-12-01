
#include "MEPRRD_Dispatcher.h"

/* TODO
 * 
 * TBB multithreading
 * Consider timestamping from PF_RING
 * 
 */

tbb::task* WriteRRDs::execute(){
	cout << "Updating RRDs..." << endl;

	updateAggregateRRD();
	updateIPSpecificRRDs();

	cout << endl;

	return NULL; // or a pointer to a new task to be executed immediately
}

WriteRRDs::WriteRRDs(map<string, map<int, int> > lost_meps,
		map<string, map<int, int> > received_meps,
		int max_update_time){

	_containing_folder = "rrd//";

	/* RRD options
	 * DS - Data Source, RRA - RR Archive
	 *
	 * We generate one pdp each '_pdp_step' number of seconds.
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

	/* Eg. of kind of thinking that needs to be done (not current settings).
	 * (not now) 1m  60.000.000 (every us)
	 * 1h   3.600.000 (every ms)
	 * 3h         180 (every min)
	 * 24h        288 (every 5 mins)
	 */
	_pdp_step = 60;
    _options.push_back("DS:mep:GAUGE:120:0:U");
    _options.push_back("DS:lost_mep:GAUGE:120:0:U");
    _options.push_back("RRA:AVERAGE:0.5:1:10080"); // Per minute for one month
    _options.push_back("RRA:AVERAGE:0.5:60:2160"); // Per hour for three months
    _options.push_back("RRA:MAX:0.5:1:10080");
    _options.push_back("RRA:MAX:0.5:60:2160");

    if(!Tools::fileExists(_containing_folder + string("aggregation.rrd")))
    	createRRD(_pdp_step, "aggregation", _options);

	// Prepare the _lost_meps and _received_meps for the update
    for(map<string, map<int, int> >::iterator it = received_meps.begin(); it != received_meps.end(); ++it){
    	map<int, int> lost_copy;
    	map<int, int> received_copy;
        for(map<int, int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
        	if(it2->first < max_update_time){
				received_copy[it2->first] = it2->second;
				lost_copy[it2->first] = lost_meps[it->first][it2->first];
        	}
        }
        if(received_copy.size() > 0){
			_lost_meps[it->first] = lost_copy;
			_received_meps[it->first] = received_copy;
        }
    }
}

void WriteRRDs::updateRRD(string filename, vector<string>& updates){
    int argc;
    char** argv = NULL;

    filename = _containing_folder + filename + ".rrd";

    // Calc of argc and argv
    argc = Tools::vec2arg(updates, argv);

    cout << "Update " << filename << " ";
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


void WriteRRDs::updateAggregateRRD(){
    vector<string> updates;
    map<int, pair<int, int> > updates_intermediate_format;

    for(map<string, map<int, int> >::iterator it = _received_meps.begin(); it != _received_meps.end(); ++it){
        string ip = it->first;

        for(map<int, int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            int time_mins = it2->first;

			if(updates_intermediate_format.find(time_mins) == updates_intermediate_format.end())
				updates_intermediate_format[time_mins] = make_pair<int, int>(0, 0);

			updates_intermediate_format[time_mins].first += it2->second;
			updates_intermediate_format[time_mins].second += _lost_meps[ip][time_mins];
        }
    }

    for(map<int, pair<int, int> >::iterator it = updates_intermediate_format.begin();
    		it != updates_intermediate_format.end(); ++it){

        // updates string
        updates.push_back(Tools::toString<int>(it->first) + ":" + Tools::toString<int>(it->second.first)
            + ":" + Tools::toString<int>(it->second.second));
    }

    // Perform updates
    updateRRD(string("aggregation"), updates);
}


void WriteRRDs::updateIPSpecificRRDs(){
    for(map<string, map<int, int> >::iterator it = _received_meps.begin(); it != _received_meps.end(); ++it){
        vector<string> updates;
        string ip = it->first;

        for(map<int, int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2){
            int time_mins = it2->first;

			// updates string
			updates.push_back(Tools::toString<int>(time_mins) + ":" + Tools::toString<int>(it2->second)
                + ":" + Tools::toString<int>(_lost_meps[ip][time_mins]));
        }

        if(!Tools::fileExists(_containing_folder + ip + string(".rrd")))
            createRRD(_pdp_step, ip, _options);

        updateRRD(ip, updates);
    }
}

void WriteRRDs::createRRD(int pdp_step, string filename, vector<string>& options){
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

/* Constructor - Generate RRD files */
MEPRRD_Dispatcher::MEPRRD_Dispatcher(){
    _pdp_step = 60;
    _rrd_update_size = 1; // Every n minutes
    _threshold_minutes = 0; // Update until threshold_minutes
    _starting_time = 0;
}

void MEPRRD_Dispatcher::dispatchPacket(GenericPacket* receivedPacket){
    UDPPacket* packet = new UDPPacket(receivedPacket);
    
    _packet_time = time(&packet->timestamp.tv_sec); // / 60;
    _packet_time = _packet_time - _packet_time % 60;
    _packet_ip = string(inet_ntoa(packet->ip->ip_src));
    // TODO: Change this!
    // int* payday = (int*) (&packet->payload[0]);
    // _packet_seqno = int(htonl(packet->mep->seqno));
    _packet_seqno = ++counter + (rand() % 2);
    
    // cout << _packet_time << " " << _packet_ip << " " << _packet_seqno << endl;
    
    if(_starting_time == 0)
        _starting_time = _packet_time;
    
    /* if (_filenames.find(_packet_ip) == _filenames.end() && _with_IP_specific_RRDs){
        // cout << Tools::inttostr(packet->ip->ip_src.s_addr) << " ";
        
        // TODO: Uncomment!
        // createRRDFile(_pdp_step, _packet_ip, _options);
        _filenames.insert(_packet_ip);
    } */
    
    // Insert the elements in the specific vector or create it if it doesn't exist
    updateDataSets();
    
    // Post data for last _rrd_update_size minutes
    
    int current_time = time(NULL); // / 60
    current_time = current_time - (current_time % 60);
    if(current_time - _starting_time > _rrd_update_size * 60){
        // Update until n minutes ago
        _max_update_time = current_time - _threshold_minutes * 60;

        // Update RRDs in a separate thread
        WriteRRDs* t = new (tbb::task::allocate_root())
        		WriteRRDs(lost_meps, received_meps, _max_update_time);
        tbb::task::enqueue(*t);

        // updateAggregateRRD();
        // updateIPSpecificRRDs();

        // Remove the updated bits
        removeOldieUpdates();
        
        _starting_time = current_time;
    }
    
    delete packet;
}

void MEPRRD_Dispatcher::removeOldieUpdates(){
    for(map<string, map<int, int> >::iterator it = received_meps.begin(); it != received_meps.end(); ++it)
        for(map<int, int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            if(it2->first < _max_update_time)
                // received_meps[it->first].erase(it2);
                it->second.erase(it2);
        
    for(map<string, map<int, int> >::iterator it = lost_meps.begin(); it != lost_meps.end(); ++it)
        for(map<int, int>::iterator it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            if(it2->first < _max_update_time)
                // lost_meps[it->first].erase(it2);
                it->second.erase(it2);
}

void MEPRRD_Dispatcher::updateDataSets(){
    // There are two things to update: #mep and #lost_meps, per device and time
    
    if(received_meps.find(_packet_ip) == received_meps.end()){
        // Create new received_meps and lost_meps for that packet
        map<int, int> initial_rec_meps, initial_lost_meps;
        initial_rec_meps[_packet_time] = 1;
        initial_lost_meps[_packet_time] = 0;
        
        received_meps[_packet_ip] = initial_rec_meps;
        lost_meps[_packet_ip] = initial_lost_meps;
    }
    else {
        map<int, int>* device_received_meps = &received_meps[_packet_ip];
        map<int, int>* device_lost_meps = &lost_meps[_packet_ip];
        
        // Check if rec_meps is incremented
        if(device_received_meps->find(_packet_time) == device_received_meps->end()){
            (*device_received_meps)[_packet_time] = 1;
            (*device_lost_meps)[_packet_time] = 0;
        }
        else {
            (*device_received_meps)[_packet_time]++;
            
            if(_packet_seqno > last_seqnos[_packet_ip] + 100)
            	cout << " - " << _packet_ip << " reports outlier " << last_seqnos[_packet_ip] << " -> " << _packet_seqno;

            else if(_packet_seqno > last_seqnos[_packet_ip] + 1)
                (*device_lost_meps)[_packet_time] += _packet_seqno - last_seqnos[_packet_ip] - 1;
        }
    }
    
    last_seqnos[_packet_ip] = _packet_seqno;
}
