OBJS = \
    address_parser.o \
    address_standardizer.o \
    std_pg_hash.o \
    analyze.o \
    err_param.o \
    export.o \
    gamma.o \
    hash.o \
    lexicon.o \
    pagc_tools.o \
    parseaddress-api.o \
    standard.o \
    tokenize.o

OBJS_test_main = \
    test_main.o \
    analyze.o \
    err_param.o \
    export.o \
    gamma.o \
    hash.o \
    lexicon.o \
    pagc_tools.o \
    standard.o \
    tokenize.o

POSTGIS_PGSQL_VERSION=92
# SQL preprocessor
SQLPP = /opt/rh/devtoolset-8/root/usr/bin/cpp -traditional-cpp -w -P
GREP=/bin/grep
PERL=/bin/perl
EXTVERSION    = 2.4.2
MINORVERSION  = 2.4
MODULE_big = address_standardizer-2.4
EXTENSION = address_standardizer
DATA = address_standardizer_data_us.control sql/$(EXTENSION).sql sql/$(EXTENSION)_data_us.sql

all:  sql/address_standardizer.sql sql/address_standardizer--1.0--$(EXTVERSION).sql sql/address_standardizer--$(EXTVERSION).sql sql/address_standardizer--$(EXTVERSION)--$(EXTVERSION)next.sql sql/address_standardizer--$(EXTVERSION)next--$(EXTVERSION).sql \
 sql/$(EXTENSION)_data_us.sql sql/address_standardizer--2.4.2.sql sql/$(EXTENSION)_data_us--2.4.2.sql  sql/$(EXTENSION)_data_us--$(EXTVERSION)--$(EXTVERSION)next.sql \
sql/$(EXTENSION)_data_us--$(EXTVERSION)next--$(EXTVERSION).sql \
sql/test-init-extensions.sql sql/test-parseaddress.sql sql/test-standardize_address_1.sql sql/test-standardize_address_2.sql

EXTRA_CLEAN += sql/*.sql



DOCS = README.address_standardizer
PG_CPPFLAGS =  -I/usr/local/geos-3.6.2/include -I/usr/local/proj-4.9.2/include  -I/usr/local/libxml2-2.7.1/include/libxml2  -I/usr/local/jsonc-0.12.0/include   -I/home/hn/code/postgis/postgis-xc/postgis-2.4.2/include -g -O0

SHLIB_LINK =  -L/usr/local/geos-3.6.2/lib -L/usr/local/proj-4.9.2/lib -lgeos_c -lproj -L/usr/local/jsonc-0.12.0/lib -ljson-c  -L/usr/local/libxml2-2.7.1/lib -lxml2 -lm   -Wl,--exclude-libs,ALL -lpcre
EXTRA_CLEAN = usps-st-city-name.txt mk-st-regexp mk-city-regex test_main
REGRESS = test-init-extensions test-parseaddress test-standardize_address_1 test-standardize_address_2

# PGXS information
PG_CONFIG = /opt/huawei/cluster/app_392c0438/bin/pg_config

PGVER := $(shell $(PG_CONFIG) --version)


# Set PERL _after_ the include of PGXS
PERL=/bin/perl

# Borrow the $libdir substitution from PGXS but customise by running the preprocessor
# and adding the version number
sql/%.sql: %.sql.in
	mkdir -p sql
	$(SQLPP) -I../libpgcommon $< | grep -v '^#' | \
	$(PERL) -lpe "s'MODULE_PATHNAME'\$$libdir/${EXTENSION}-${MINORVERSION}'g" > $@

sql/address_standardizer--2.4.2.sql: sql/address_standardizer_types.sql \
    sql/address_standardizer_functions.sql
	cat $^ > $@

sql/address_standardizer.sql: sql/address_standardizer_types.sql \
    sql/address_standardizer_functions.sql
	cat $^ > $@

sql/address_standardizer--1.0--$(EXTVERSION).sql:  sql/address_standardizer_functions.sql
	cat $^ > $@

# upgrade logic for us data extension (to go to next cludge for dev upgrading)
sql/$(EXTENSION)--$(EXTVERSION)--$(EXTVERSION)next.sql: sql/address_standardizer_functions.sql
	cat $^ > $@

# upgrade logic for us data extension (to go to original round-robin yoyo cludge for dev upgrading)
sql/$(EXTENSION)--$(EXTVERSION)next--$(EXTVERSION).sql: sql/address_standardizer_functions.sql
	cat $^ > $@

# build us data extension
sql/$(EXTENSION)_data_us--$(EXTVERSION).sql: us_lex.sql us_gaz.sql us_rules.sql sql_bits/address_standardizer_data_us_mark_editable_objects.sql.in
	mkdir -p sql
	cat $^ > $@

sql/$(EXTENSION)_data_us.sql: us_lex.sql us_gaz.sql us_rules.sql sql_bits/address_standardizer_data_us_mark_editable_objects.sql.in
	mkdir -p sql
	cat $^ > $@

# upgrade logic for us data extension (to go to next cludge for dev upgrading)
sql/$(EXTENSION)_data_us--$(EXTVERSION)--$(EXTVERSION)next.sql: sql/$(EXTENSION)_data_us--$(EXTVERSION).sql
	cat $^ > $@

# upgrade logic for us data extension (to go to original round-robin yoyo cludge for dev upgrading)
sql/$(EXTENSION)_data_us--$(EXTVERSION)next--$(EXTVERSION).sql: sql/$(EXTENSION)_data_us--$(EXTVERSION).sql
	cat $^ > $@

mk-st-regexp: mk-st-regexp.pl
	$(PERL) -c mk-st-regexp.pl
	rm -f mk-st-regexp
	echo "#! " $(PERL) > mk-st-regexp
	cat mk-st-regexp.pl >> mk-st-regexp
	chmod ugo+x mk-st-regexp

mk-city-regex: mk-city-regex.pl usps-st-city-name.txt
	$(PERL) -c mk-city-regex.pl
	rm -f mk-city-regex
	echo "#! " $(PERL) > mk-city-regex
	cat mk-city-regex.pl >> mk-city-regex
	chmod ugo+x mk-city-regex

usps-st-city-name.txt: usps-st-city-orig.txt usps-st-city-adds.txt
	cat usps-st-city-orig.txt usps-st-city-adds.txt | sort -u >usps-st-city-name.txt

#parseaddress-stcities.h: mk-city-regex
#	./mk-city-regex > parseaddress-stcities.h

#parseaddress-regex.h: mk-st-regexp
#	./mk-st-regexp > parseaddress-regex.h

dist-clean:
	rm -f mk-st-regexp mk-city-regex usps-st-city-name.txt test_main Makefile


test_main: $(OBJS_test_main)
	gcc -o test_main $(OBJS_test_main) $(LDFLAGS) $(LIBS)

test_main.o: test_main.c pagc_api.h pagc_std_api.h

address_parser.o: address_parser.c parseaddress-api.h

address_standardizer.o: address_standardizer.c std_pg_hash.h pagc_api.h pagc_std_api.h

analyze.o: analyze.c pagc_api.h

err_param.o: err_param.c pagc_api.h

export.o: export.c pagc_api.h pagc_tools.h

gamma.o: gamma.c pagc_api.h pagc_std_api.h gamma.h

hash.o: hash.c hash.h khash.h

lexicon.o: lexicon.c pagc_api.h pagc_std_api.h

pagc_tools.o: pagc_tools.c pagc_tools.h pagc_common.h

parseaddress-api.o: parseaddress-api.c parseaddress-api.h parseaddress-stcities.h parseaddress-regex.h

standard.o: standard.c pagc_api.h

tokenize.o: tokenize.c pagc_api.h

std_pg_hash.o: std_pg_hash.c std_pg_hash.h pagc_api.h pagc_std_api.h

#only extension files
DATA += $(wildcard sql/*--*.sql)
EXTRA_CLEAN += sql/*.sql
EXTRA_CLEAN += $(wildcard expected/*--*.out)
EXTRA_CLEAN += sql/$(EXTENSION)--$(EXTVERSION).sql sql/$(EXTENSION)--unpackaged--$(EXTVERSION).sql
EXTRA_CLEAN += sql/$(EXTENSION)--$(EXTVERSION).sql sql/$(EXTENSION)--unpackaged--$(EXTVERSION).sql *.a *.so *.dll

PGXS := /opt/huawei/cluster/app/lib/postgresql/pgxs/src/makefiles/pgxs.mk
include $(PGXS)
PERL = /bin/perl
