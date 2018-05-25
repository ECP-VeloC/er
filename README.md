# Encode/Rebuild (ER)

# Overview
This module defines an interface lets one define redundancy schemes,
which are identified by an integer ID.
To encode, one can then apply the redundancy scheme to a list of files and provide a name for the encoded set.
To rebuild, one specifies the name of the encoded set.

Currently, it assumes the parent group of processes is MPI_COMM_WORLD.

Usage is documented in src/er.h.

# Building

To build dependencies:

    git clone git@github.com:LLNL/KVTree.git KVTree.git
    git clone git@xgitlab.cels.anl.gov:ecp-veloc/rankstr.git rankstr.git
    git clone git@xgitlab.cels.anl.gov:ecp-veloc/redset.git redset.git
    git clone git@xgitlab.cels.anl.gov:ecp-veloc/shuffile.git shuffile.git

    mkdir install

    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../install -DMPI=ON ../KVTree.git
    make clean
    make
    make install
    make test
    cd ..

    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../install -DMPI=ON ../rankstr.git
    make clean
    make
    make install
    cd ..

    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../install -DMPI=ON ../redset.git
    make clean
    make
    make install
    cd ..

    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../install -DMPI=ON ../shuffile.git
    make clean
    make
    make install
    cd ..

To build ER:

    cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_KVTREE_PREFIX=`pwd`/install -DWITH_REDSET_PREFIX=`pwd`/install -DWITH_SHUFFILE_PREFIX=`pwd`/install .

# Testing
Some simple test programs exist in the test directory.

To build a test for the ER API:

    mpicc -g -O0 -o test_er test_er.c -I../install/include -L../install/lib64 -lkvtree -lshuffile -lredset -lrankstr -I../src -L../src -ler

## Release

Copyright (c) 2018, Lawrence Livermore National Security, LLC.
Produced at the Lawrence Livermore National Laboratory.
<br>
Copyright (c) 2018, UChicago Argonne LLC, operator of Argonne National Laboratory.


For release details and restrictions, please read the [LICENSE]() and [NOTICE]() files.

`LLNL-CODE-751725` `OCEC-18-060`
