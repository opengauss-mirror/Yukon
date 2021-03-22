# **********************************************************************
# *
# * PostGIS - Spatial Types for PostgreSQL
# * http://postgis.net
# * 
# * Copyright (C) 2010-2011 Sandro Santilli <strk@kbt.io>
# * Copyright (C) 2008 Mark Cave-Ayland
# * Copyright (C) 2005 Refractions Research Inc.
# *
# * This is free software; you can redistribute and/or modify it under
# * the terms of the GNU General Public Licence. See the COPYING file.
# *
# **********************************************************************

#
# PostGIS PGXS build system
#

POSTGIS_PGSQL_VERSION=92

MODULE_big=postgis_topology-2.4
MODULEDIR=contrib/postgis-2.4

# Files to be copied to the contrib/ directory
DATA_built=topology.sql topology_upgrade.sql uninstall_topology.sql

# SQL preprocessor
SQLPP = /opt/rh/devtoolset-8/root/usr/bin/cpp -traditional-cpp -w -P

# SQL objects (files requiring pre-processing)
SQL_OBJS = \
  topology.sql \
  topology_upgrade.sql \
  topology_drop_before.sql \
  topology_drop_after.sql

# Objects to build using PGXS
OBJS = postgis_topology.o

# Libraries to link into the module (proj, geos)
#
# Note: we specify liblwgeom.a directly in SHLIB_LINK rather than using
# -L... -l options to prevent issues with some platforms trying to link
# to an existing liblwgeom.so in the PostgreSQL $libdir supplied by an
# older version of PostGIS, rather than with the static liblwgeom.a
# supplied with newer versions of PostGIS
PG_CPPFLAGS += -I../liblwgeom -I../libpgcommon  -I/usr/local/geos-3.6.2/include -I/usr/local/proj-4.9.2/include  -I/usr/local/libxml2-2.7.1/include/libxml2  -I/usr/local/jsonc-0.12.0/include   -I/home/hn/code/postgis/postgis-xc/postgis-2.4.2/include -fPIC
SHLIB_LINK_F = ../libpgcommon/libpgcommon.a ../liblwgeom/.libs/liblwgeom.a  -L/usr/local/geos-3.6.2/lib -L/usr/local/proj-4.9.2/lib -lgeos_c -lproj -L/usr/local/jsonc-0.12.0/lib -ljson-c  -L/usr/local/libxml2-2.7.1/lib -lxml2 -lm   -Wl,--exclude-libs,ALL

# Add SFCGAL Flags if defined
ifeq (,sfcgal)
PG_CPPFLAGS += 
SHLIB_LINK_F += 
endif


# Extra files to remove during 'make clean'
EXTRA_CLEAN=$(SQL_OBJS) topology_upgrade.sql.in

# PGXS information
PG_CONFIG = /opt/huawei/cluster/app_392c0438/bin/pg_config 
PGXS := /opt/huawei/cluster/app/lib/postgresql/pgxs/src/makefiles/pgxs.mk
# NO_TEMP_INSTALL is a workaround for a 9.5dev bug. See:
# http://www.postgresql.org/message-id/CAB7nPqTsR5o3g-fBi6jbsVdhfPiLFWQ_0cGU5=94Rv_8W3qvFA@mail.gmail.com
NO_TEMP_INSTALL=yes
include $(PGXS)

# Set PERL _after_ the include of PGXS
PERL=/bin/perl

# This is to workaround a bug in PGXS 8.4 win32 link line,
# see http://trac.osgeo.org/postgis/ticket/1158#comment:57
SHLIB_LINK := $(SHLIB_LINK_F) $(SHLIB_LINK)

$(OBJS): ../liblwgeom/.libs/liblwgeom.a ../libpgcommon/libpgcommon.a ../postgis_config.h

# If REGRESS=1 passed as a parameter, change the default install paths
# so that no prefix is included. This allows us to relocate to a temporary
# directory for regression testing.
ifeq ($(REGRESS),1)
        bindir=/bin
        pkglibdir=/lib
        datadir=/share
        datamoduledir=contrib/postgis
endif



# Generate any .sql file from .sql.in.c files by running them through the SQL pre-processor 
%.sql: %.sql.in
	$(SQLPP) $< | grep -v '^#' | \
	$(PERL) -lpe "s'MODULE_PATHNAME'\$$libdir/postgis_topology-2.4'g" > $@

#Generate upgrade script by stripping things that can't be reinstalled
#e.g. don't bother with tables, types, triggers, and domains
# TODO: use postgis_proc_upgrade.pl
topology_upgrade.sql.in:  topology.sql
	$(PERL) -0777 -ne 's/^(CREATE|ALTER) (CAST|OPERATOR|TYPE|TABLE|SCHEMA|DOMAIN|TRIGGER).*?;//msg;print;' $< > $@

topology_upgrade.sql: topology_drop_before.sql topology_upgrade.sql.in topology_drop_after.sql
	cat $^ > $@

topology.sql: \
	sql/sqlmm.sql.in \
	sql/populate.sql.in \
	sql/polygonize.sql.in \
	sql/export/gml.sql.in \
	sql/export/TopoJSON.sql.in \
	sql/query/getnodebypoint.sql.in \
	sql/query/getedgebypoint.sql.in \
	sql/query/getfacebypoint.sql.in \
	sql/query/GetRingEdges.sql.in \
	sql/query/GetNodeEdges.sql.in \
	sql/manage/TopologySummary.sql.in \
	sql/manage/CopyTopology.sql.in \
	sql/manage/ManageHelper.sql.in \
	sql/topoelement/topoelement_agg.sql.in \
	sql/topogeometry/type.sql.in \
	sql/topogeometry/totopogeom.sql.in \
	sql/topogeometry/cleartopogeom.sql.in \
	sql/topogeometry/topogeom_edit.sql.in \
	sql/topogeometry/simplify.sql.in \
	sql/predicates.sql.in \
	../postgis/sqldefines.h \
	../postgis_svn_revision.h

uninstall_topology.sql: topology.sql ../utils/create_undef.pl 
	$(PERL) ../utils/create_undef.pl $< $(POSTGIS_PGSQL_VERSION) > $@

check: topology.sql
	$(MAKE) -C test $@

distclean: clean
	rm -f Makefile test/Makefile
