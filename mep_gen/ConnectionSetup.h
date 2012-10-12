/* 
 * File:   ConnectionSetup.h
 * Author: root
 *
 * Created on September 7, 2012, 11:33 AM
 */

#ifndef CONNECTIONSETUP_H
#define	CONNECTIONSETUP_H

#include <pcap.h>
#include <libnet.h>

#include <string>
#include <iostream>

using namespace std;

class ConnectionSetup {
private:
    libnet_t * _l;
    u_int32_t _tx_ip;
    u_int32_t _rx_ip;
    u_int8_t* _tx_mac;
    u_int8_t* _rx_mac;
    
    void getMacsFromIPs();
    
public:
    string tx_mac;
    string tx_ip;
    string rx_mac;
    string rx_ip;
    bool mac_manual;
    
    // TODO: This is private
    int _length;
    
    ConnectionSetup(libnet_t * l);
    
    // TODO: Add the getters for types other than string
    u_int32_t get_tx_ip();
    u_int32_t get_rx_ip();
    u_int8_t* get_tx_mac();
    u_int8_t* get_rx_mac();
    
    void set_tx_mac(string tx_mac);
    void set_tx_ip(string tx_ip);
    void set_rx_mac(string rx_mac);
    void set_rx_ip(string rx_ip);
    
    void set_parameters(string tx_mac, string tx_ip, string rx_mac, string rx_ip);
    void set_default_parameters();
    
    void set_rx_parameters_interactively();
    void set_tx_parameters_interactively();
    
};

#endif	/* CONNECTIONSETUP_H */

