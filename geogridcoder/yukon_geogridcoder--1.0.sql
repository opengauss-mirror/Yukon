--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION yukon_geogridcoder" to load this file. \quit

------------------------------------------------geosotgrid----------------------------------------------------
CREATE OR REPLACE FUNCTION geosotgrid_in(cstring)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0','geosotgrid_in'
	LANGUAGE 'c'  IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geosotgrid_out(geosotgrid)
	RETURNS cstring
	AS '$libdir/yukon_geogridcoder-1.0','geosotgrid_out'
	LANGUAGE 'c'  IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geosotgrid_recv(internal)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0','geosotgrid_recv'
	LANGUAGE 'c'  IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geosotgrid_send(geosotgrid)
	RETURNS bytea
	AS '$libdir/yukon_geogridcoder-1.0','geosotgrid_send'
	LANGUAGE 'c'  IMMUTABLE STRICT ;

CREATE TYPE geosotgrid(
	internallength = variable,
	input = geosotgrid_in,
	output = geosotgrid_out,
	send = geosotgrid_send,
	receive = geosotgrid_recv,
	alignment = int4,
	storage = main
);

----------------------------------------geosotgrid type----------------------------------------

CREATE OR REPLACE FUNCTION grid_lt(geosotgrid, geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION grid_le(geosotgrid, geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION grid_gt(geosotgrid, geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION grid_ge(geosotgrid, geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION grid_eq(geosotgrid, geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION grid_cmp(geosotgrid, geosotgrid)
RETURNS integer
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OPERATOR < (
LEFTARG = geosotgrid, RIGHTARG = geosotgrid, PROCEDURE = grid_lt,
COMMUTATOR = '>', NEGATOR = '>=',
RESTRICT = contsel, JOIN = contjoinsel
);

CREATE OPERATOR <= (
LEFTARG = geosotgrid, RIGHTARG = geosotgrid, PROCEDURE = grid_le,
COMMUTATOR = '>=', NEGATOR = '>',
RESTRICT = contsel, JOIN = contjoinsel
);

CREATE OPERATOR = (
LEFTARG = geosotgrid, RIGHTARG = geosotgrid, PROCEDURE = grid_eq,
COMMUTATOR = '=', -- we might implement a faster negator here
RESTRICT = contsel, JOIN = contjoinsel
);

CREATE OPERATOR >= (
LEFTARG = geosotgrid, RIGHTARG = geosotgrid, PROCEDURE = grid_ge,
COMMUTATOR = '<=', NEGATOR = '<',
RESTRICT = contsel, JOIN = contjoinsel
);

CREATE OPERATOR > (
LEFTARG = geosotgrid, RIGHTARG = geosotgrid, PROCEDURE = grid_gt,
COMMUTATOR = '<', NEGATOR = '<=',
RESTRICT = contsel, JOIN = contjoinsel
);

CREATE OPERATOR CLASS btree_grid_ops
DEFAULT FOR TYPE geosotgrid USING btree AS
OPERATOR	1	< ,
OPERATOR	2	<= ,
OPERATOR	3	= ,
OPERATOR	4	>= ,
OPERATOR	5	> ,
FUNCTION	1	grid_cmp (geosotgrid, geosotgrid);

----------------------------------------geosotgrid array type----------------------------------------

CREATE OR REPLACE FUNCTION gridarray_cmp(geosotgrid, geosotgrid)
RETURNS integer
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_overlap(_geosotgrid, _geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_contains(_geosotgrid, _geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_contained(_geosotgrid, _geosotgrid)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_extractvalue(anyarray, internal, internal)
RETURNS internal
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_extractquery(_geosotgrid, internal, int2, internal, internal, internal, internal)
RETURNS internal
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION gridarray_consistent(internal, int2, _geosotgrid, int4, internal, internal, internal, internal)
RETURNS bool
AS '$libdir/yukon_geogridcoder-1.0'
LANGUAGE 'c' IMMUTABLE STRICT ;


CREATE OPERATOR && (
	LEFTARG = _geosotgrid,
	RIGHTARG = _geosotgrid,
	PROCEDURE = gridarray_overlap,
	COMMUTATOR = '&&',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR @> (
	LEFTARG = _geosotgrid,
	RIGHTARG = _geosotgrid,
	PROCEDURE = gridarray_contains,
	COMMUTATOR = '<@',
	RESTRICT = contsel,
	JOIN = contjoinsel
);

CREATE OPERATOR <@ (
	LEFTARG = _geosotgrid,
	RIGHTARG = _geosotgrid,
	PROCEDURE = gridarray_contained,
	COMMUTATOR = '@>',
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
	STORAGE 		geosotgrid;

----------------------------------------geosotgrid function----------------------------------------

CREATE OR REPLACE FUNCTION ST_GeoSOTGrid(geom geometry, level int)
	RETURNS _geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgrid'
	LANGUAGE 'c'  IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GeoSOTGridZ(altitude float8, level int)
	RETURNS int4
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgrid_z'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_AltitudeFromGeoSOTGridZ(z_num int4, level int)
	RETURNS float8
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_as_altitude'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GeoSOTGridFromText(geosotgrid2d cstring)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgrid_from_text'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GeoSOTGridFromText(geosotgrid2d cstring, geosotgrid_z cstring)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgrid3d_from_text'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_AsText(grid geosotgrid)
	RETURNS text
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geosotgrid_as_text'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GeomFromGeoSOTGrid(grid geosotgrid)
	RETURNS geometry
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geom_from_geosotgrid'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GeomFromGeoSOTGrid(gridarray _geosotgrid)
	RETURNS geometry[]
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_geom_from_geosotgrid_array'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_HasZ(grid geosotgrid)
	RETURNS bool
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_has_z'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_GetLevel(grid geosotgrid)
	RETURNS int2
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_get_level'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_Degenerate(grid geosotgrid, level int)
	RETURNS geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_degenerate'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_Degenerate(gridarray _geosotgrid, level int)
	RETURNS _geosotgrid
	AS '$libdir/yukon_geogridcoder-1.0' ,'gsg_degenerate_array'
	LANGUAGE 'c' IMMUTABLE STRICT;
----------------------------------------geomhash function----------------------------------------

CREATE OR REPLACE FUNCTION ST_GeoHash(boundary box2d,geom geometry,level int default 1)
	RETURNS int8[]
	AS '$libdir/yukon_geogridcoder-1.0' ,'UgComputerGeoHash'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION UGBBoxGeoHash(UGbbox box2d,UGquerybox box2d,level int default 1)
	RETURNS int8[]
	AS '$libdir/yukon_geogridcoder-1.0' ,'UgBBoxGeoHash'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION UGGetFilter1(indexbound box2d,geobound box2d,level int default 25,keysizelimit int default 10)
	RETURNS text
	AS '$libdir/yukon_geogridcoder-1.0','UgGetFilter1'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

-- tables 要合并的数据表
-- level hash 划分等级
-- schema 表所在的模式名
create or replace function geogridcoder_init(tables text[], level integer default 1, schema text default 'public')
    RETURNS boolean
AS
$$
DECLARE
    _max          integer;
    _sqls         text;
    kw            text;
    _status       boolean;
    _schema       text;
    _count        integer;
    _globalleft   float8;
    _globalright  float8;
    _globaltop    float8;
    _globalbottom float8;
    _ext          box2d;
BEGIN

    _max := array_length(tables, 1);
    -- 判断是否是一个空数组
    if _max is null then
        raise 'no table in the tables';
        return false;
    end if;

    -- 判断所有的表是否都包含 geometry 列
    <<kwloop>>
    FOR x in 1.._max
        LOOP
            kw := quote_literal(tables[x]);
            _count := 0;
            _sqls := 'select count(*) from geometry_columns where f_table_schema= ' || quote_literal(schema) ||
                     ' and f_table_name=' || kw;
            execute _sqls into _count;

            if _count = 0 then
                raise 'the table % not contain a geometry column',kw;
            end if;

        END LOOP kwloop;

    _sqls = 'drop schema if exists yk_geogridcoder cascade';
    execute _sqls;
    _sqls = 'create schema yk_geogridcoder';
    execute _sqls;

    _sqls = 'create table yk_geogridcoder.geohash(id serial,tablename text,smid bigint,geom geometry,hash bigint[])';
    execute _sqls;

    -- 将各个表收集到 geohash
    _sqls := 'select st_extent(smgeometry) from ' || quote_ident(tables[1]);
    execute _sqls into _ext;
    _globalbottom := st_ymin(_ext);
    _globaltop := st_ymax(_ext);
    _globalleft := st_xmin(_ext);
    _globalright := st_xmax(_ext);

    <<kwloop>>
    FOR x in 1.._max
        LOOP
            _count := 0;
            _sqls = 'insert into yk_geogridcoder.geohash (tablename,smid,geom) ' || '(select ' || quote_literal(tables[x]) ||
                    ',smid ,smgeometry from ' || quote_ident(schema) || '.' || quote_ident(tables[x]) || ')';
            execute _sqls;

            -- 计算整体范围
            _sqls = 'select st_extent(smgeometry) from ' || quote_ident(tables[x]);

            execute _sqls into _ext;

            _globalbottom := LEAST(_globalbottom, st_ymin(_ext));
            _globaltop := GREATEST(_globaltop, st_ymax(_ext));
            _globalleft := LEAST(_globalleft, st_xmin(_ext));
            _globalright := GREATEST(_globalright, st_xmax(_ext));


            raise notice 'collect data from :%',kw;

        END LOOP kwloop;

    -- 更新 geohash 中的 hash 列

    _sqls = 'update yk_geogridcoder.geohash set hash = st_geohash(''BOX(' || _globalleft || ' ' || _globalbottom || ',' ||
            _globalright || ' ' || _globaltop || ')'',geom,' || level || ')';

    execute _sqls;


    _sqls = 'alter table yk_geogridcoder.geohash drop column geom';
    execute _sqls;

	_sqls = 'create index geohash_index on yk_geogridcoder.geohash using gin(hash);';
    execute _sqls;

    RETURN TRUE;
END ;
$$ LANGUAGE 'plpgsql' VOLATILE;

create or replace function geogridcoder_intersects(globalbox box2d, querybox box2d, level integer default 10)
    RETURNS json
AS
$$
DECLARE
    _count integer;
    _sql   text;
    _res json;
BEGIN
    -- 检查是否有 yk_geogridcoder.geohash 表
    _sql = 'select count(*) from pg_tables where tablename=''geohash'' and schemaname=''yk_geogridcoder''';
    execute _sql into _count;

    if _count = 0 then
        raise 'you should use geogridcoder_init first';
    end if;

    _sql = 'drop table if exists tempgeohash';

    execute _sql;

    _sql = 'create temporary table tempgeohash as (select tablename,smid from yk_geogridcoder.geohash where UGBBoxGeoHash(' || quote_literal(globalbox) ||
           '::box2d,' || quote_literal(querybox) || '::box2d,' || level || ') && hash)';
    --raise notice '%',_sql;
    execute _sql;

    _sql = 'select json_agg(tempgeohash) from tempgeohash';
    execute _sql into _res;

    return _res;

END ;
$$ LANGUAGE 'plpgsql' VOLATILE;
