# Instructions for running the PMFE code on Math-Mulberry@GATech

The rationale for this separate set of instructions to run our *PMFE* code sources on the local 
campus Linux boxen is fairly straightforward. System upgrades and updates tend to break things 
easily, even during active development. Thus, we are going to create a new set of instructions 
that details how to perform a so-called *local install* of all requisite sources, libraries, and 
include files that does not depend on the changing state of the system-wide libraries, their 
versions, nor other state that computer support will have to intervene in the install process 
for longer term. This means that once the code compiles and runs based on these local 
install files in the user's home directory, we will have to do considerably much less to 
maintain working versions (or installations) of the *PMFE* code, e.g., this install method, 
while a little bit of extra work up front, should lead to longer term stability and 
functionality of the work based on this code. 

Let us get started with the documentation procedure below. 

**NOTE:** This documentation set is currently housed in a separate ``math-mulberry-testing`` 
branch. To view these instructions offline, users will need to refer to the new files within 
this branch, and compile from the modified ``Makefile`` contained therein. 

## Logging in from campus or over VPN

```bash
ssh my-username-mds@ssh.math.gatech.edu
ssh math-mulberry
whoami
```

## Cloning the PMFE code (and initial setup)

### Make a wrapper directory for the local install files

```bash
mkdir GTDMMBSoftware2020
cd GTDMMBSoftware2020
git clone https://github.com/gtDMMB/pmfe.git
cd pmfe
git checkout math-mulberry-testing
cd ../
```

### Fetch and extract library sources

#### Boost C++ libraries

```bash
git clone https://github.com/boostorg/build.git
cd build
CXX="g++" CXXFLAGS="-std=gnu++0x -D_GLIBCXX_USE_CXX11_ABI=0 -DABI=0" ./bootstrap.sh cxx
## TODO, below here for tonight ... 
./b2 install
```

#### CGAL (multiprocessing library sources, depends on Boost)

```bash
git clone https://github.com/CGAL/cgal.git
cd cgal && mkdir -p LocalCGALBuildDir && cd LocalCGALBuildDir
cmake ../ -DCMAKE_BUILD_TYPE=Release

```



