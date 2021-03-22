#!/bin/bash
./configure --with-pgconfig=/opt/huawei/cluster/app_392c0438/bin/pg_config --with-gdalconfig=/usr/local/gdal-1.11.0/bin/gdal-config --with-geosconfig=/usr/local/geos-3.6.2/bin/geos-config --with-xml2config=/usr/local/libxml2-2.7.1/bin/xml2-config --with-jsondir=/usr/local/jsonc-0.12.0 --with-projdir=/usr/local/proj-4.9.2 --without-topology CFLAGS='-O2 -fpermissive -pthread' CC=g++
