#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/MEPRRD_Dispatcher.o \
	${OBJECTDIR}/UDPRRD_Dispatcher.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/tcp_headers.o \
	${OBJECTDIR}/GenericPacket.o \
	${OBJECTDIR}/MEPSQL_Dispatcher.o \
	${OBJECTDIR}/PacketCapture.o \
	${OBJECTDIR}/UDP_test.o \
	${OBJECTDIR}/mep_headers.o \
	${OBJECTDIR}/udp_headers.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lpcap -lnet -lrrd -lmysqlpp

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/poller

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/poller: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/poller ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/MEPRRD_Dispatcher.o: MEPRRD_Dispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/MEPRRD_Dispatcher.o MEPRRD_Dispatcher.cpp

${OBJECTDIR}/UDPRRD_Dispatcher.o: UDPRRD_Dispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/UDPRRD_Dispatcher.o UDPRRD_Dispatcher.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/tcp_headers.o: tcp_headers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/tcp_headers.o tcp_headers.cpp

${OBJECTDIR}/GenericPacket.o: GenericPacket.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/GenericPacket.o GenericPacket.cpp

${OBJECTDIR}/MEPSQL_Dispatcher.o: MEPSQL_Dispatcher.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/MEPSQL_Dispatcher.o MEPSQL_Dispatcher.cpp

${OBJECTDIR}/PacketCapture.o: PacketCapture.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/PacketCapture.o PacketCapture.cpp

${OBJECTDIR}/UDP_test.o: UDP_test.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/UDP_test.o UDP_test.cpp

${OBJECTDIR}/mep_headers.o: mep_headers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/mep_headers.o mep_headers.cpp

${OBJECTDIR}/udp_headers.o: udp_headers.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -MMD -MP -MF $@.d -o ${OBJECTDIR}/udp_headers.o udp_headers.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/poller

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
