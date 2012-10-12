/* 
 * File:   GenericDispatcher.h
 * Author: root
 *
 * Created on September 13, 2012, 3:23 PM
 */

#ifndef GENERICDISPATCHER_H
#define	GENERICDISPATCHER_H

#include "GenericPacket.h"
    
class GenericDispatcher{
public:
    virtual void dispatchPacket(GenericPacket*){}
};

#endif	/* GENERICDISPATCHER_H */

