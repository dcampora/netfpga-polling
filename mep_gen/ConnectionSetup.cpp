
#include "ConnectionSetup.h"

ConnectionSetup::ConnectionSetup(libnet_t * l){
    this->_l = l;
    this->tx_mac = "";
    this->tx_ip = "";
    this->rx_mac = "";
    this->rx_ip = "";
    this->mac_manual = 0;
}

u_int32_t ConnectionSetup::get_tx_ip(){
    return this->_tx_ip;
}

u_int32_t ConnectionSetup::get_rx_ip(){
    return this->_rx_ip;
}

u_int8_t* ConnectionSetup::get_tx_mac(){
    return this->_tx_mac;
}

u_int8_t* ConnectionSetup::get_rx_mac(){
    return this->_rx_mac;
}


void ConnectionSetup::set_tx_mac(string tx_mac){
    this->tx_mac = tx_mac;
    this->_tx_mac = libnet_hex_aton(this->tx_mac.c_str(), &this->_length);
}

void ConnectionSetup::set_tx_ip(string tx_ip){
    this->tx_ip = tx_ip;
    this->_tx_ip = libnet_name2addr4(this->_l, const_cast<char*>(this->rx_ip.c_str()), LIBNET_DONT_RESOLVE);
}

void ConnectionSetup::set_rx_mac(string rx_mac){
    this->rx_mac = rx_mac;
    if (this->rx_mac[0] == 'y')
        this->rx_mac = string("00:00:00:00:00:00");
    
    this->_rx_mac = libnet_hex_aton(this->rx_mac.c_str(), &this->_length);
}

void ConnectionSetup::set_rx_ip(string rx_ip){
    this->rx_ip = rx_ip;
    if (this->rx_ip[0] == 'y')
        this->rx_ip = string("127.0.0.1");
    
    this->_rx_ip = libnet_name2addr4(this->_l, const_cast<char*>(this->rx_ip.c_str()), LIBNET_DONT_RESOLVE);
}

void ConnectionSetup::set_parameters(string tx_mac, string tx_ip, string rx_mac, string rx_ip){
    this->set_rx_ip(rx_ip);
    this->set_rx_mac(rx_mac);
    this->set_tx_ip(tx_ip);
    this->set_tx_mac(tx_mac);
}

void ConnectionSetup::set_default_parameters(){
    // TODO
    this->tx_mac = "";
    this->tx_ip = "";
    this->rx_mac = "";
    this->rx_ip = "";
}

void ConnectionSetup::set_rx_parameters_interactively(){
    string temp;
    cout << "Rx IP address [y]-127.0.0.1 : ";
    cin.width(15);
    cin >> temp;
    this->set_rx_ip(temp);
    
    temp = "";
    cout << "Rx MAC address (hex separated by ':') [y]-00:00:00:00:00:00 : ";
    cin.width(17);
    cin >> temp;
    this->set_rx_mac(temp);
}

void ConnectionSetup::set_tx_parameters_interactively(){
    string temp;
    cout << "Tx IP address: ";
    cin.width(15);
    cin >> temp;
    this->set_tx_ip(temp);
    
    temp = "";
    cout << "Tx MAC address (hex separated by ':'): ";
    cin.width(17);
    cin >> temp;
    this->set_tx_mac(temp);
}

void ConnectionSetup::getMacsFromIPs(){
    /* TODO: This is a pain:
     * 1. For every IP in the list of IPs of this machine, get it by libnet.
     * 2. For every other IP, ARP Request through the default link,
     * sniff the result.
     */
}