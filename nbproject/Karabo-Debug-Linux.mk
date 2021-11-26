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
	${OBJECTDIR}/src/CameraImageSource.o \
	${OBJECTDIR}/src/ImageSource.o \
	${OBJECTDIR}/src/Scene.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${TESTDIR}/TestFiles/f1

# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++14
CXXFLAGS=-std=c++14

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L${KARABO}/lib -L${KARABO}/extern/lib -L${KARABO}/extern/lib64 -Wl,-rpath,${KARABO}/lib -Wl,-rpath,${KARABO}/extern/lib -Wl,-rpath,${KARABO}/extern/lib64 -lkarabo -ljpeg -lopencv_imgproc -lopencv_core `pkg-config --libs karaboDependencies`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libimageSource.${CND_DLIB_EXT}

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libimageSource.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libimageSource.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/src/CameraImageSource.o: src/CameraImageSource.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CameraImageSource.o src/CameraImageSource.cc

${OBJECTDIR}/src/ImageSource.o: src/ImageSource.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ImageSource.o src/ImageSource.cc

${OBJECTDIR}/src/Scene.o: src/Scene.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Scene.o src/Scene.cc

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${TESTDIR}/TestFiles/f1: ${TESTDIR}/src/tests/ImageSourceTest.o ${TESTDIR}/src/tests/test_runner.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${TESTDIR}/TestFiles
	${LINK.cc}   -o ${TESTDIR}/TestFiles/f1 $^ ${LDLIBSOPTIONS} `pkg-config cppunit --libs`   


${TESTDIR}/src/tests/ImageSourceTest.o: src/tests/ImageSourceTest.cc 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/ImageSourceTest.o src/tests/ImageSourceTest.cc


${TESTDIR}/src/tests/test_runner.o: src/tests/test_runner.cc 
	${MKDIR} -p ${TESTDIR}/src/tests
	${RM} "$@.d"
	$(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -MMD -MP -MF "$@.d" -o ${TESTDIR}/src/tests/test_runner.o src/tests/test_runner.cc


${OBJECTDIR}/src/CameraImageSource_nomain.o: ${OBJECTDIR}/src/CameraImageSource.o src/CameraImageSource.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/CameraImageSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/CameraImageSource_nomain.o src/CameraImageSource.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/CameraImageSource.o ${OBJECTDIR}/src/CameraImageSource_nomain.o;\
	fi

${OBJECTDIR}/src/ImageSource_nomain.o: ${OBJECTDIR}/src/ImageSource.o src/ImageSource.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/ImageSource.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ImageSource_nomain.o src/ImageSource.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/ImageSource.o ${OBJECTDIR}/src/ImageSource_nomain.o;\
	fi

${OBJECTDIR}/src/Scene_nomain.o: ${OBJECTDIR}/src/Scene.o src/Scene.cc 
	${MKDIR} -p ${OBJECTDIR}/src
	@NMOUTPUT=`${NM} ${OBJECTDIR}/src/Scene.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} "$@.d";\
	    $(COMPILE.cc) -g -I${KARABO}/include -I${KARABO}/extern/include -I${KARABO}/extern/include/opencv4 `pkg-config --cflags karaboDependencies`   -fPIC  -Dmain=__nomain -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Scene_nomain.o src/Scene.cc;\
	else  \
	    ${CP} ${OBJECTDIR}/src/Scene.o ${OBJECTDIR}/src/Scene_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/TestFiles/f1 || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libimageSource.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
