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

My intention with this set of documentation and installer instructions is to prevent painful
breakage when system components on the campus math machines are upgraded (or changed) down the line. 
I want to ensure that those users in the group following along can continue to run this modified 
*PMFE* software for non-development computations. Morover, the best way to make sure users in a few 
years that do need to modify these instructions can do so easily and well is to explain why we are 
doing things this way. So bear with me as I explain things step-by-step, that it may make the next 
code RA's life an existential breeze!

**NOTE:** This documentation set is currently housed in a separate ``math-mulberry-testing`` 
branch. To view these instructions offline, users will need to refer to the new files within 
this branch, and compile from the modified ``Makefile`` contained therein. 

*Now, let us get started with the documentation procedure below.*

## Things to ask your Linux sysadmin for before we can dig in and get our terminals in the mix ...

I am going to keep a short log of the system software that **IS** (very much) needed to be installed 
systemwide to get this local install method working. Unfotunately, there is an important need to get 
recent, sane compiler toolchains, e.g., at least ``g++-9``, more recent ``cmake-3.x.x`` versions, ideally 
a matching ``gdb``, and upgraded GNU ``binutils`` packages. Performing this particular step in the user's 
home directory is complicated, can take days to bootstrap and build (from experience), and is at any rate something the 
local sysadmin team can do quickly. Therefore, we minimally require a ``sudo`` enabled user of our target Linux 
system to make the following list of tools available globally:
* At least ``g++-9`` (RH can be upgraded to this toolchain easily). The ``c++11`` or ``gnu++1x`` standard support is 
required to build recent ``libboost`` support locally.
* Upgraded ``cmake`` installations. The local CGAL as of today (*2020.09.08*) needs at least a version of the 
following: ``CMake 3.1 or higher is required.``
* Other local install prerequisites to be documented here (**TODO**)

### Logging in from campus or over VPN

```bash
ssh my-username-mds@ssh.math.gatech.edu
ssh math-mulberry
```

### Run once commands to configure your math machine terminal

```bash
scl enable devtoolset-9 /bin/bash
```

### Make a wrapper directory for the local PMFE install files

```bash
mkdir GTDMMBSoftware2020
cd GTDMMBSoftware2020
git clone https://github.com/gtDMMB/pmfe.git
cd pmfe
git checkout math-mulberry-testing
cd ../
```

## Fetch and extract core external library sources

### Build the interface (B2 script) for recent Boost C++ libraries

```bash
git clone https://github.com/boostorg/build.git
cd build
CXX="g++" CXXFLAGS="-std=gnu++0x -D_GLIBCXX_USE_CXX11_ABI=0 -DABI=0" ./bootstrap.sh cxx
./b2 install --prefix=$(readlink -f ~/GTDMMBSoftware2020/BoostLocalInstall)
cd ..
```

### Build and install a sane local CMake toolchain

```bash
wget https://github.com/Kitware/CMake/releases/download/v3.18.2/cmake-3.18.2.tar.gz
tar xvzf cmake-3.18.2.tar.gz
cd cmake-3.18.2
./bootstrap
make
make install
```

#### Install CGAL tools -- multiprocessing library sources and headers (depends on Boost and CMake)

```bash
git clone https://github.com/CGAL/cgal.git
cd cgal && mkdir -p LocalCGALBuildDir && cd LocalCGALBuildDir
cmake ../ -DCMAKE_BUILD_TYPE=Release

```



