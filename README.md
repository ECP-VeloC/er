# Encode/Rebuild (ER)

# Overview
This module combines the funcitonality of shuffile and redset,
which lets both migrate and rebuild files within an MPI job.
It is useful for moving and rebuilding data files on distributed storage, e.g., node-local storage,
particularly when MPI ranks may be running on different compute nodes than when they originally created their files.

To encode, one defines a redundancy scheme and then applies the redundancy scheme to a list of files also providing a name for the encoded set.
To rebuild, one specifies the name of the encoded set.
Additionally, there is a function to remove the encoding information, which is needed when removing a dataset.

Currently, er assumes the group of processes participating is the same as MPI_COMM_WORLD.

Usage is documented in [src/er.h](src/er.h) and in the [user API](https://ecp-veloc.github.io/component-user-docs/group__er.html) docs.

# Building

To build dependencies:

    git clone git@github.com:ECP-VeloC/KVTree.git   KVTree.git
    git clone git@github.com:ECP-VeloC/rankstr.git  rankstr.git
    git clone git@github.com:ECP-VeloC/redset.git   redset.git
    git clone git@github.com:ECP-VeloC/shuffile.git shuffile.git

    mkdir install

    rm -rf build
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../install -DMPI=ON ../KVTree.git
    make clean
    make
    make install
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

    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./install -DWITH_KVTREE_PREFIX=`pwd`/install -DWITH_REDSET_PREFIX=`pwd`/install -DWITH_SHUFFILE_PREFIX=`pwd`/install .
    make

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
