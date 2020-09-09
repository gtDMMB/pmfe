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
CXXFLAGS += --std=c++14 -D_GLIBCXX_USE_CXX11_ABI=0 -DABI=0
CXXFLAGS += -fPIC
CXXFLAGS += -fopenmp
CXXFLAGS += -Wall
CXXFLAGS += -g
CXXFLAGS += -O3
CXXFLAGS += -Iinclude $(INCLUDES) $(shell readlink -f ../cgal/*/include | tr "\n" " " | sed -e 's/ / -I/g') \
		-DBOOST_FILESYSTEM_NO_DEPRECATED \
		-I$(shell readlink -f ../BoostLocalInstall/include) \
		-frounding-math -DBOOST_LOG_DYN_LINK=0 -DCGAL_HEADER_ONLY

# library paths
LIBS += -L/usr/local/lib # For Homebrew
LIBS += -lgmp -lgmpxx
LIBS += -lCGAL
LIBS += -lm
LIBS += -lboost_filesystem
LIBS += -lboost_program_options
LIBS += -lboost_system
LIBS += -lboost_log
LIBS += -frounding-math -lboost_log_setup -lboost_log-mt

BIN = pmfe-findmfe pmfe-scorer pmfe-parametrizer pmfe-subopt pmfe-tests
all: $(OBJ) $(BIN)

-include $(DEP)

debug: CXXFLAGS += -Og
debug: all

pmfe-findmfe: $(LIBOBJ) src/bin-findmfe.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

pmfe-scorer: $(LIBOBJ) src/bin-scorer.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

pmfe-parametrizer: $(LIBOBJ) src/bin-parametrizer.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

pmfe-subopt: $(LIBOBJ) src/bin-subopt.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

pmfe-tests: $(LIBOBJ) $(TESTOBJ) src/bin-tests.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $^ -o $@ $(LIBS)

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
