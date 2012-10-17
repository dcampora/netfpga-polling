/* 
 * File:   TCPRRD_Dispatcher.h
 * Author: root
 *
 * Created on September 13, 2012, 4:22 PM
 */

#ifndef UDP_TEST_H
#define	UDP_TEST_H

#include <iostream>
#include <string>
#include <list>

#include "GenericDispatcher.h"
#include "mep_headers.h"
#include "Tools.h"
#include "PacketCapture.h"

using namespace std;

class UDP_test : public GenericDispatcher {
public:
    UDP_test();
    void dispatchPacket(GenericPacket*);
};

#endif	/* UDPRRD_DISPATCHER_H */

