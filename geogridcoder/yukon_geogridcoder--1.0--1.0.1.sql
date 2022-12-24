--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION yukon_geogridcoder" to load this file. \quit

CREATE OR REPLACE FUNCTION gridarray_spanoverlap(_geosotgrid, _geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;


CREATE OR REPLACE FUNCTION gridarray_comparepartial(geosotgrid, geosotgrid, int2, internal)
RETURNS internal
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;


CREATE OR REPLACE FUNCTION ST_GeoSOTGridAgg(geom geometry, levelmax int, levelmin int)
	RETURNS _geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgridagg'
	LANGUAGE 'c'  IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION ST_GeoSOTGrid(geom geometry)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_mingeosotgrid'
	LANGUAGE 'c'  IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION ST_GetLevelextremum(grids geosotgrid[])
	RETURNS int2[]
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_get_level_extremum'
	LANGUAGE 'c' IMMUTABLE STRICT PARALLEL SAFE;


DROP OPERATOR CLASS gin_grid_ops  USING gin;
DROP OPERATOR && (_geosotgrid,_geosotgrid) CASCADE;

CREATE  OPERATOR && (
	LEFTARG = _geosotgrid,
	RIGHTARG = _geosotgrid,
	PROCEDURE = gridarray_spanoverlap,
	COMMUTATOR = '&&',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR CLASS gin_grid_ops
DEFAULT FOR TYPE _geosotgrid USING gin
AS
    OPERATOR	3	&&,
	OPERATOR	7	@>,
	OPERATOR	8	<@,
    OPERATOR	18	= (anyarray, anyarray),
	OPERATOR	19	!= (anyarray, anyarray),
    FUNCTION    1   gridarray_cmp(geosotgrid, geosotgrid),
    FUNCTION	2	gridarray_extractvalue(anyarray, internal, internal),
	FUNCTION	3	gridarray_extractquery(_geosotgrid, internal, int2, internal, internal, internal, internal),
	FUNCTION	4	gridarray_consistent(internal, int2, _geosotgrid, int4, internal, internal, internal, internal),
    FUNCTION	5	gridarray_comparepartial(geosotgrid, geosotgrid, int2, internal),
	STORAGE 		geosotgrid;


