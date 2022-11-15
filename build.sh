#!/bin/bash

set -e
DIR=`pwd`

cd $DIR
./waf configure --with-flavors=x11-glesv2 --no-opt
./waf build -j 10
./waf install --destdir=$PWD/release
