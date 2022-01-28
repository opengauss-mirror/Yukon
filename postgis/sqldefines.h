#ifndef _LWPGIS_DEFINES
#define _LWPGIS_DEFINES

#include "../postgis_revision.h"

/*
 * Define just the version numbers; otherwise we get some strange substitutions in postgis.sql.in
 */
#define POSTGIS_PGSQL_VERSION 96
#define POSTGIS_PGSQL_HR_VERSION 9.6
#define POSTGIS_GEOS_VERSION 30606
#define POSTGIS_PROJ_VERSION 72
#define POSTGIS_LIB_VERSION '3.2.0'
#define POSTGIS_LIBXML2_VERSION 2.9.12
#define POSTGIS_SFCGAL_VERSION 10308


/*
 * High costs can only be used for PostGIS 3/PgSQL 12
 * where the support functions have been used in
 * place of index SQL inlining.
 * See https://trac.osgeo.org/postgis/ticket/3675
 * for sideffects of costing inlined SQL.
 */
#if POSTGIS_PGSQL_VERSION >= 120
#define _COST_DEFAULT COST 1
#define _COST_LOW COST 50
#define _COST_MEDIUM COST 500
#define _COST_HIGH COST 10000
#else
#define _COST_DEFAULT COST 1
#define _COST_LOW COST 1
#define _COST_MEDIUM COST 10
#define _COST_HIGH COST 10
#endif

/*
 * Define the build date and the version number
 * (these substitiutions are done with extra quotes sinces CPP
 * won't substitute within apostrophes)
 */
#define _POSTGIS_SQL_SELECT_POSTGIS_VERSION 'SELECT ''3.2 USE_GEOS=1 USE_PROJ=1 USE_STATS=1''::text AS version'
#define _POSTGIS_SQL_SELECT_POSTGIS_BUILD_DATE 'SELECT ''2021-12-18 02:46:04''::text AS version'
#define _POSTGIS_SQL_SELECT_POSTGIS_PGSQL_VERSION 'SELECT ''96''::text AS version'

#ifdef POSTGIS_REVISION
/*
* Insert the PostGIS revision.  This is immensely goofy because FreeBSD doesn't recognized
* PostGIS_REVISION as a variable if anything like ` or $rev$ (sql dollar quoting) abutts it so we need to leave a space on both sides
* which means we need to then trim it to get rid of the extra spaces we added.
* Maybe someone smarter can come up with a less goofy solution that makes all OS happy
*/
#define _POSTGIS_SQL_SELECT_POSTGIS_SCRIPTS_VERSION $$ SELECT trim('3.2.0'::text || $rev$ POSTGIS_REVISION $rev$) AS version $$
#else
#define _POSTGIS_SQL_SELECT_POSTGIS_SCRIPTS_VERSION $$ SELECT '3.2.0'::text AS version $$
#endif

#define SRID_USR_MAX 998999

#endif /* _LWPGIS_DEFINES */


