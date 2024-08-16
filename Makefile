VPATH=src include DIY tutorial
CC=g++ --std=c++17
obj_dir=obj/
bin_dir=bin/
INCLUDE_PATH=include DIY tutorial
UTILS_PATH=include/Utils/Includes include/Utils/STV_Tools

GCCFLAGS= ${addprefix -I,${INCLUDE_PATH} ${UTILS_PATH} } 

ROOTFLAGS=`root-config --cflags --libs`
ROOTLIBS=-lEG

SRC_DIRS = src DIY tutorial

cxxfiles=$(wildcard *.cxx src/*cxx)
CXXOBJS=$(addprefix ${obj_dir}, $(notdir $(notdir $(patsubst %.cxx,%.o,$(cxxfiles)))))
#cppfiles=$(filter-out DIY/BinScheme.cpp, $(wildcard $(addsuffix /*.cpp,$(SRC_DIRS))))
cppfiles=$(wildcard $(addsuffix /*.cpp,$(SRC_DIRS)))
CPPOBJS=$(addprefix ${obj_dir}, $(notdir $(notdir $(patsubst %.cpp,%.o,$(cppfiles)))))
Cfiles=$(wildcard *.C src/*C)
TOBJS=$(addprefix ${obj_dir}, $(notdir $(notdir $(patsubst %.C,%.C.o,$(Cfiles)))))
TARGETS=$(addprefix ${bin_dir}, $(notdir $(notdir $(patsubst %.C, %,$(Cfiles)))))

OBJS=${CPPOBJS} ${CXXOBJS} obj/stv_root_dict.o

ALL : prepare ${CPPOBJS} ${CXXOBJS} ${TOBJS}  ${TARGETS} scripts 


${CXXOBJS}: ${obj_dir}%.o : src/%.cxx
	${CC} -c $< -o $@ ${GCCFLAGS} ${ROOTFLAGS} ${ROOTLIBS}

${CPPOBJS}: ${obj_dir}%.o : %.cpp
	${CC} -c $< -o $@ ${GCCFLAGS} ${ROOTFLAGS} ${ROOTLIBS}

${TARGETS}: ${bin_dir}% : obj/%.C.o ${OBJS}
	${CC} -o $@ $^ ${GCCFLAGS} ${ROOTFLAGS}  ${ROOTLIBS} 


${TOBJS}: ${obj_dir}%.C.o : src/%.C
	${CC} -c $< -o $@ ${GCCFLAGS} ${ROOTFLAGS} ${ROOTLIBS} 

prepare: 
	@if [ ! -d ${bin_dir} ]; then \
		mkdir -p ${bin_dir} ${obj_dir} lib; \
		rootcling -f stv_root_dict.cc -c include/LinkDef.h; \
		g++ `root-config --cflags --libs` -O3 -fPIC -o stv_root_dict.o -c stv_root_dict.cc;\
		mv stv_root_dict.o obj; \
		mv stv_root_dict_rdict.pcm lib; \
		rm stv_root_dict.cc; \
	fi

scripts:
	@cp Scripts/* bin/ 

.PHONY: ALL clean

clean:
	-rm -rf  ${bin_dir} ${obj_dir} lib
