#!/bin/bash

g++ -L/home/dcampora/temp/tbb41_20121003oss/lib/intel64/cc4.1.0_libc2.4_kernel2.6.16.21 -ltbb -lrt -lnet -lpcap -lmysqlpp -lrrd -pthread -O2 GenericPacket.cpp main.cpp mep_headers.cpp MEPRRD_Dispatcher.cpp MEPSQL_Dispatcher.cpp MEPSQL_QuickDispatcher.cpp PacketCapture.cpp tcp_headers.cpp udp_headers.cpp UDPRRD_Dispatcher.cpp UDP_test.cpp

