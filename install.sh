#!/bin/bash
./configure --with-pgconfig=/opt/huawei/cluster/app_392c0438/bin/pg_config --without-address-standardizer --without-topology CFLAGS='-O2 -fpermissive -pthread' CC=g++
