# source files
SRC := $(wildcard src/*.cc)
OBJ := $(SRC:.cc=.o)

BINSRC := $(wildcard src/bin-*.cc)
BINOBJ := $(BINSRC:.cc=.o)

TESTSRC := $(wildcard src/test-*.cc)
TESTOBJ := $(TESTSRC:.cc=.o)

LIBOBJ := $(OBJ)
LIBOBJ := $(filter-out $(BINOBJ),$(LIBOBJ))
LIBOBJ := $(filter-out $(TESTOBJ),$(LIBOBJ))

DEP := $(SRC:.cc=.P)
HDR := $(wildcard src/*.h)

# include directories
INCLUDES += -IiB4e
INCLUDES += -I/usr/local/include # For Homebrew

# C++ compiler flags
CXXFLAGS += -Wall -fvisibility=hidden -fvisibility-inlines-hidden \
		-D_GLIBCXX_USE_CXX11_ABI=0 -DABI=0
CXXFLAGS += -fPIC
CXXFLAGS += -fopenmp
CXXFLAGS += -Wall
CXXFLAGS += -g
CXXFLAGS += -O3
CXXFLAGS += -Iinclude $(INCLUDES) -I$(shell readlink -f ../cgal/*/include | tr "\n" " " | sed -e 's/ / -I/g') \
		-DBOOST_FILESYSTEM_NO_DEPRECATED \
		-I$(shell readlink -f ../BoostLocalInstall/include) \
		-frounding-math -DCGAL_HEADER_ONLY -DBOOST_LOG_DYN_LINK -BOOST_ALL_DYN_LINK \
		$(shell pkg-config gmp --cflags) $(shell pkg-config gmpxx --cflags)
CXXFLAGS_BASE := $(CXXFLAGS)

# library paths
LIBS += -L/usr/local/lib # For Homebrew
LIBS += -L/opt/rh/devtoolset-9/root/usr/lib/gcc/x86_64-redhat-linux/9 -lstdc++
LIBS += $(shell pkg-config gmp --libs) $(shell pkg-config gmpxx --libs)
LIBS += -L$(shell readlink -f ../BoostLocalInstall/lib)
LIBS += -lm
LIBS += -lboost_filesystem
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_log
LIBS += -frounding-math -lboost_log_setup -lboost_thread -lboost_atomic -lboost_regex -lboost_chrono
LIBS += -Wl,-Bdynamic -lpthread

BIN = pmfe-findmfe pmfe-scorer pmfe-parametrizer pmfe-subopt pmfe-tests
all: $(OBJ) $(BIN)

-include $(DEP)

debug: CXXFLAGS += -Og
debug: all

## Note we are having to (re)set the C++ standard to get compatibility in the 
## broken, non-cohesive feature sets on the local compiler. This is not an 
## exact science, but rather success by trial and error (Sigh.)
pmfe-findmfe: CXXFLAGS= -std=c++0x $(CXXFLAGS_BASE)
pmfe-findmfe: $(LIBOBJ) src/bin-findmfe.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

pmfe-scorer: CXXFLAGS= -std=c++0x $(CXXFLAGS_BASE)
pmfe-scorer: $(LIBOBJ) src/bin-scorer.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

pmfe-parametrizer: CXXFLAGS= -std=c++11 $(CXXFLAGS_BASE)
pmfe-parametrizer: $(LIBOBJ) src/bin-parametrizer.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

pmfe-subopts: CXXFLAGS= -std=c++0x $(CXXFLAGS_BASE) 
pmfe-subopt: $(LIBOBJ) src/bin-subopt.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

pmfe-tests: CXXFLAGS= -std=c++0x $(CXXFLAGS_BASE) 
pmfe-tests: $(LIBOBJ) $(TESTOBJ) src/bin-tests.o
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

%.o: %.cc
	$(CXX) -MD $(CXXFLAGS) $(INCLUDES) -o $@ -c $<
	@cp $*.d $*.P; \
        sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
            -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P; \
        rm -f $*.d

clean:
	-rm -vf $(EXEC) $(OBJ) $(DEP) $(BIN)

install:

uninstall:

.PHONY: clean
