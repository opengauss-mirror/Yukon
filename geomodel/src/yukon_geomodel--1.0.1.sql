--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION get_sum" to load this file. \quit


CREATE OR REPLACE FUNCTION geomodel_typmod_srid(integer)
	RETURNS integer
	AS '$libdir/yukon_geomodel-1.0','geomodel_typmod_srid'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geomodel_typmod_dims(integer)
	RETURNS integer
	AS '$libdir/yukon_geomodel-1.0','geomodel_typmod_dims'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geomodel_typmod_type(integer)
	RETURNS text
	AS '$libdir/yukon_geomodel-1.0','geomodel_typmod_type'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(catalogn_name varchar,schema_name varchar,table_name varchar,column_name varchar,new_srid_in integer)
	RETURNS text
	AS
$$
DECLARE
	myrec RECORD;
	okay boolean;
	cname varchar;
	real_schema name;
	unknown_srid integer;
	new_srid integer := new_srid_in;
	gtype varchar;
	gcoord_dimension  integer;
	sql text;
	count integer;
BEGIN


	-- Find, check or fix schema_name
	IF ( schema_name IS NOT NULL ) THEN
		okay = false;

		FOR myrec IN SELECT nspname FROM pg_namespace WHERE text(nspname) = schema_name LOOP
			okay := true;
		END LOOP;

		IF ( okay <> true ) THEN
			RAISE EXCEPTION 'Invalid schema name';
		ELSE
			real_schema = schema_name;
		END IF;
	ELSE
		SELECT INTO real_schema current_schema()::text;
	END IF;

	-- Ensure that column_name is in geomodel_columns
	okay = false;
	
	EXECUTE 'SELECT count(*) FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
	' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
	INTO count;


	
	if(count) then
		EXECUTE 'SELECT type FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
		INTO gtype;
		
		EXECUTE 'SELECT coord_dimension FROM geomodel_columns WHERE f_table_schema ='|| quote_literal(real_schema) ||
		' and f_table_name = '|| quote_literal(table_name) ||'  and f_geomodel_column = '|| quote_literal(column_name)||';'
		INTO gcoord_dimension;
		okay := true;
	else
		okay := false;
	END IF;

	IF (NOT okay) THEN
		RAISE EXCEPTION 'column not found in geomodel_columns table';
		RETURN false;
	END IF;

	-- Ensure that new_srid is valid
	IF ( new_srid > 0 ) THEN
		IF ( SELECT count(*) = 0 from spatial_ref_sys where srid = new_srid ) THEN
			RAISE EXCEPTION 'invalid SRID: % not found in spatial_ref_sys', new_srid;
			RETURN false;
		END IF;
	ELSE
		new_srid = 0;
	END IF;
	
    EXECUTE 'ALTER TABLE ' || quote_ident(real_schema) || '.' || quote_ident(table_name) ||
        ' ALTER COLUMN ' || quote_ident(column_name) || ' TYPE  geomodel(' || gtype ||',' ||new_srid::text || ') USING ST_SetSRID(' || quote_ident(column_name) || ',' || new_srid::text || ');';

	RETURN real_schema || '.' || table_name || '.' || column_name ||' SRID changed to ' || new_srid::text;

END;
$$
LANGUAGE 'plpgsql' VOLATILE;



-----------------------------------------------------------------------
-- UpdateGeomodelSRID
--   <schema>, <table>, <column>, <srid>
-----------------------------------------------------------------------
CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(varchar,varchar,varchar,integer)
	RETURNS text
	AS $$
DECLARE
	ret  text;
BEGIN
	SELECT UpdateGeoModelSRID('',$1,$2,$3,$4) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

-----------------------------------------------------------------------
-- UpdateGeoModelSRID
--   <table>, <column>, <srid>
-----------------------------------------------------------------------
CREATE OR REPLACE FUNCTION UpdateGeoModelSRID(varchar,varchar,integer)
	RETURNS text
	AS $$
DECLARE
	ret  text;
BEGIN
	SELECT UpdateGeoModelSRID('','',$1,$2,$3) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;



-------------------------------------- {{ geomodel_columns view---------------------------------------------
-- 用于查询那些表中的那些字段包含 geomodel 类型

CREATE OR REPLACE VIEW geomodel_columns AS
	SELECT
		current_database() AS f_table_catalog, 
		n.nspname AS f_table_schema, 
		c.relname AS f_table_name, 
		a.attname AS f_geomodel_column,
		geomodel_typmod_dims(a.atttypmod) AS coord_dimension,
		geomodel_typmod_srid(a.atttypmod) AS srid,
		geomodel_typmod_type(a.atttypmod) AS type
	FROM 
		pg_class c, 
		pg_attribute a, 
		pg_type t, 
		pg_namespace n
	WHERE t.typname = 'geomodel'
		AND a.attisdropped = false
		AND a.atttypid = t.oid
		AND a.attrelid = c.oid
		AND c.relnamespace = n.oid
		AND c.relkind = ANY (ARRAY['r'::"char", 'v'::"char", 'm'::"char", 'f'::"char", 'p'::"char"] )
		AND NOT pg_is_other_temp_schema(c.relnamespace)
		AND has_table_privilege( c.oid, 'SELECT'::text );

--------------------------------------geomodel_columns view }}---------------------------------------------


----------------------------------------- {{ function ------------------------------------------------------

---用于给已有的数据表添加 geomodel 类型的列，并创建子表
---CREATE OR REPLACE FUNCTION AddGeoModelColumn(schema_name varchar, table_name varchar, column_name varchar)
CREATE OR REPLACE FUNCTION AddGeoModelColumn(schema_name varchar,table_name varchar,column_name varchar,new_srid_in integer,_type varchar DEFAULT 'ANYTYPE')
RETURNS text
AS $$
DECLARE
  --- 数据库个数
  table_cnt int;
  --- geomodel 列个数
  geocol_cnt int;
  --- sql 语句
  sql_str   varchar;
  sr varchar;
  real_schema name;
  new_srid integer;
  count integer;
  new_type varchar;
BEGIN

  --- 判断类型是否合法
  new_type = upper(_type);
  IF new_type = 'MESH' OR new_type = 'SURFACE' OR new_type = 'ANYTYPE'  THEN
	raise notice 'the type is %',new_type;
  ELSE
    raise  'the type  % not supported,please check and try it again!',new_type;
	return 'fail';
  END IF;

  --- 判断 SRID 是否合法
  IF ( new_srid_in > 0 ) THEN
		IF new_srid_in > 998999 THEN
			RAISE EXCEPTION 'AddGeoModelColumn() - SRID must be <= %', 998999;
		END IF;
		new_srid := new_srid_in;
		SELECT COUNT(*) FROM spatial_ref_sys WHERE SRID = new_srid INTO count;
		IF(count) THEN
			SELECT SRID INTO sr FROM spatial_ref_sys WHERE SRID = new_srid;
		ELSE
			RAISE EXCEPTION 'AddGeoModelColumn() - invalid SRID';
			RETURN 'fail';
		END IF;
    ELSE
		new_srid := 4326;		
    END IF;

   -- Verify schema
	IF ( schema_name IS NOT NULL) THEN
		sql_str := 'SELECT count(*) FROM pg_namespace ' ||
			'WHERE text(nspname) = ' || quote_literal(schema_name) ||
			'LIMIT 1';
		EXECUTE sql_str INTO count;
		
		if(count) then
			sql_str := 'SELECT nspname FROM pg_namespace ' ||
				'WHERE text(nspname) = ' || quote_literal(schema_name) ||
				'LIMIT 1';
			RAISE DEBUG '%', sql_str;
			EXECUTE sql_str INTO real_schema;

		else
			RAISE EXCEPTION 'Schema % is not a valid schemaname', quote_literal(schema_name);
			RETURN 'fail';
		END IF;
    END IF;

  --- 检查数据表是否存在
  --- select count(*) from information_schema.tables where table_schema='schema_name'
  --- and table_type='BASE TABLE' and table_name='table_name'
  sql_str := 'select count(*) from information_schema.tables where table_schema=' 
  || quote_literal(schema_name) 
  || ' and table_type=' || quote_literal('BASE TABLE')
  || ' and table_name=' || quote_literal(table_name);  
  EXECUTE sql_str into table_cnt;
  IF(table_cnt <= 0)THEN  
    --- 数据表不存在，直接报错
    RAISE  'the table %.% does not exist!',schema_name,table_name;  
    RETURN 'fail';
  END IF;  
  
  --- 检查数据库中是否已经存在 geomodel 列
  --- select count(*) from geomodel_columns where table_name='table_name' and schema_name='schema_name'
  sql_str := 'select count(*) from geomodel_columns where f_table_name=' 
  || quote_literal(table_name) 
  || ' and f_table_schema =' || quote_literal(schema_name);
  EXECUTE sql_str into geocol_cnt;  
  IF(geocol_cnt > 0)THEN
    --- 数据表中已含有 geomodel 数据类型的列,有则报错
    RAISE WARNING 'the table %.% already has a geomodel type column!',schema_name,table_name;  
    RETURN 'fail';
  END IF;
  
  --- 添加新列并创建子表
  --- alter table "schema_name"."table_name" add "column_name" geomodel;
  sql_str := 'alter table ' || quote_ident(schema_name) || '.' || quote_ident(table_name) || ' add ' 
 			 || quote_ident(column_name) || ' geomodel('|| new_type || ','|| new_srid::TEXT || ')';  
  EXECUTE sql_str;
  --- create table "schema_name"."table_name_elem"(id bigint,elemcol model_elem)
  sql_str := 'create table ' || quote_ident(schema_name) || '.' ||quote_ident(concat(table_name,'_elem')) 
 				|| '(id BIGINT primary key,elemcol model_elem('|| new_type || ','|| new_srid::TEXT || '))';
 			
  EXECUTE sql_str;
  
  RETURN 'success';
END; $$ LANGUAGE 'plpgsql';



--- 用于删除 geomodel 列，如果存在对应子表，则一起删除
CREATE OR REPLACE FUNCTION DropGeoModelColumn(schema_name varchar, table_name varchar)
RETURNS boolean
AS $$
DECLARE
  --- 数据库个数
  table_cnt int;
  --- geomodel 列个数
  geocol_cnt int;
  --- sql 语句
  sql_str   varchar;
  --- 要删除的字段名
  field_name varchar;
BEGIN

  --- 检查数据表是否存在
  --- select count(*) from information_schema.tables where table_schema='schema_name'
  --- and table_type='BASE TABLE' and table_name='table_name'
  sql_str := 'select count(*) from information_schema.tables where table_schema=' 
  || quote_literal(schema_name) 
  || ' and table_type=' || quote_literal('BASE TABLE')
  || ' and table_name=' || quote_literal(table_name);
  EXECUTE sql_str into table_cnt;
  IF(table_cnt <= 0)THEN  
    --- 数据表不存在，直接报错
    RAISE  'the table %.% does not exist!',schema_name,table_name;  
    RETURN FALSE;
  END IF;
  
  --- 检查数据库中是否已经存在 geomodel 列
  --- select count(*) from geomodel_columns where table_name='table_name' and schema_name='schema_name'
  sql_str := 'select count(*) from geomodel_columns where f_table_name=' 
  || quote_literal(table_name) 
  || ' and f_table_schema=' || quote_literal(schema_name);
  EXECUTE sql_str into geocol_cnt;  
  IF(geocol_cnt <= 0)THEN
    --- 数据表中不含有 geomodel 数据类型的列,无则报错
    RAISE  'the table %.% does not have a geomodel type column!',schema_name,table_name;  
    RETURN FALSE;
  END IF;
  
  --- 获取 geomodel 列的字段名
  sql_str:= 'select f_geomodel_column from geomodel_columns where f_table_name='
  || quote_literal(table_name) || ' and f_table_schema= '
  || quote_literal(schema_name);  
  EXECUTE sql_str into field_name;
  
  --- 删除 geomodel 列
  --- alter table "schema_name"."table_name" drop "field_name";
  sql_str := 'alter table ' || quote_ident(schema_name) || '.' || quote_ident(table_name) || ' drop column ' || quote_ident(field_name) ;  
  EXECUTE sql_str;
  
  --- 查看子表是否存在
  --- select count(*) from information_schema.tables where table_schema='schema_name'
  --- and table_type='BASE TABLE' and table_name='table_name_elem'
  sql_str := 'select count(*) from information_schema.tables where table_schema=' 
  || quote_literal(schema_name) 
  || ' and table_type=' || quote_literal('BASE TABLE')
  || ' and table_name=' || quote_literal(concat(table_name,'_elem'));
  EXECUTE sql_str into table_cnt;
  IF(table_cnt <= 0)THEN  
    --- 数据表不存在，直接报错
    RAISE NOTICE 'the child table %.%_elem does not exist!',schema_name,table_name;  
    RETURN FALSE;
  END IF;
  
  --- 删除子表
  --- drop table "schema_name"."table_name_elem"
  sql_str := 'drop table ' || quote_ident(schema_name) || '.' ||quote_ident(concat(table_name,'_elem'));
  EXECUTE sql_str;
  
  RETURN TRUE;
END; $$ LANGUAGE 'plpgsql';


/***********************************
删除带geomodel列的表及其子表
************************************/
CREATE OR REPLACE FUNCTION DropGeomodelTable(catalog_name varchar, schema_name varchar, table_name varchar)
RETURNS boolean
AS $$
DECLARE
  --- 数据库个数
  table_cnt int;
  --- geomodel 列个数
  geocol_cnt int;
  --- sql 语句
  sql_str   varchar;
  real_schema name;
  
BEGIN

	IF ( schema_name IS NULL ) THEN
		SELECT current_schema() into real_schema;
	ELSE
		real_schema = schema_name;
	END IF;
  --- 检查数据表是否存在
  	sql_str := 'select count(*) from information_schema.tables where table_schema=' 
  	|| quote_literal(real_schema) 
  	|| ' and table_type=' || quote_literal('BASE TABLE')
  	|| ' and table_name=' || quote_literal(table_name);
  	EXECUTE sql_str into table_cnt;
  	IF(table_cnt <= 0)THEN  
    --- 数据表不存在，直接报错
    RAISE 'the table %.% does not exist!',real_schema,table_name;  
    RETURN FALSE;
  	END IF;
  
  --- 检查数据库中是否已经存在 geomodel 列
  	sql_str := 'select count(*) from geomodel_columns where f_table_name=' 
  	|| quote_literal(table_name) 
  	|| ' and f_table_schema=' || quote_literal(real_schema);
  	EXECUTE sql_str into geocol_cnt;  
  	IF(geocol_cnt <= 0)THEN
    --- 数据表中不含有 geomodel 数据类型的列,无则报错
    RAISE 'the table %.% does not have a geomodel type column!',real_schema,table_name;  
    RETURN FALSE;
  	END IF;
  
  	--- 删除 geomodel 表
  	EXECUTE 'drop table if exists ' || quote_ident(real_schema) || '.' || quote_ident(table_name) || ' restrict';
  
  	--- 查看子表是否存在
  	sql_str := 'select count(*) from information_schema.tables where table_schema=' 
  	|| quote_literal(real_schema) 
  	|| ' and table_type=' || quote_literal('BASE TABLE')
  	|| ' and table_name=' || quote_literal(concat(table_name,'_elem'));
  	EXECUTE sql_str into table_cnt;
  	IF(table_cnt <= 0)THEN  
    --- 数据表不存在，提示
    RAISE NOTICE 'the child table %.%_elem does not exist!',real_schema,table_name;  
    RETURN FALSE;
  	END IF;
  
 	--- 删除子表
  	sql_str := 'drop table ' || quote_ident(real_schema) || '.' ||quote_ident(concat(table_name,'_elem'));
  	EXECUTE sql_str;
  	RETURN TRUE;
	  
END;
$$ 
LANGUAGE 'plpgsql';

CREATE OR REPLACE FUNCTION DropGeomodelTable(schema_name varchar, table_name varchar)
	RETURNS boolean
	AS $$
DECLARE
	ret  boolean;
BEGIN
	SELECT DropGeomodelTable('',$1,$2) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;
--
CREATE OR REPLACE FUNCTION DropGeomodelTable(table_name varchar)
	RETURNS boolean
	AS $$
DECLARE
	ret  boolean;
BEGIN
	SELECT DropGeomodelTable('', '', $1) into ret;
	RETURN ret;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

------------------------------------------Yukon_Version--------------------------------------------------

CREATE OR REPLACE FUNCTION Yukon_Lib_Version()
RETURNS text
	AS '$libdir/yukon_geomodel-1.0','yukon_lib_version'
LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION Yukon_Full_Version()
	RETURNS text
	AS $$
	DECLARE
	libver text;
	svnver text;
	projver text;
	geosver text;
	sfcgalver text;
	cgalver text;
	gdalver text;
	libxmlver text;
	liblwgeomver text;
	dbproc text;
	relproc text;
	fullver text;
	rast_lib_ver text;
	rast_scr_ver text;
	topo_scr_ver text;
	json_lib_ver text;
	protobuf_lib_ver text;
	sfcgal_lib_ver text;
	sfcgal_scr_ver text;
	pgsql_scr_ver text;
	pgsql_ver text;
BEGIN
	SELECT Yukon_Lib_Version() INTO libver;
	SELECT postgis_proj_version() INTO projver;
	SELECT postgis_geos_version() INTO geosver;
	SELECT postgis_libjson_version() INTO json_lib_ver;
	SELECT _postgis_scripts_pgsql_version() INTO pgsql_scr_ver;
	SELECT _postgis_pgsql_version() INTO pgsql_ver;


	SELECT postgis_libxml_version() INTO libxmlver;
	SELECT postgis_scripts_installed() INTO dbproc;
	SELECT postgis_scripts_released() INTO relproc;
	select postgis_lib_version() INTO svnver;

	fullver = 'YUKON="' || libver;
	fullver = fullver || '"';

	fullver = fullver || ' PGSQL="' || pgsql_scr_ver || '"';
	IF pgsql_scr_ver != pgsql_ver THEN
		fullver = fullver || ' (procs need upgrade for use with "' || pgsql_ver || '")';
	END IF;

	IF  geosver IS NOT NULL THEN
		fullver = fullver || ' GEOS="' || geosver || '"';
	END IF;

	IF  sfcgalver IS NOT NULL THEN
		fullver = fullver || ' SFCGAL="' || sfcgalver || '"';
	END IF;

	IF  projver IS NOT NULL THEN
		fullver = fullver || ' PROJ="' || projver || '"';
	END IF;

	IF  gdalver IS NOT NULL THEN
		fullver = fullver || ' GDAL="' || gdalver || '"';
	END IF;

	IF  libxmlver IS NOT NULL THEN
		fullver = fullver || ' LIBXML="' || libxmlver || '"';
	END IF;

	IF json_lib_ver IS NOT NULL THEN
		fullver = fullver || ' LIBJSON="' || json_lib_ver || '"';
	END IF;

	IF topo_scr_ver IS NOT NULL THEN
		fullver = fullver || ' TOPOLOGY';
		IF topo_scr_ver != relproc THEN
			fullver = fullver || ' (topology procs from "' || topo_scr_ver || '" need upgrade)';
		END IF;
	END IF;

	IF rast_lib_ver IS NOT NULL THEN
		fullver = fullver || ' RASTER';
		IF rast_lib_ver != relproc THEN
			fullver = fullver || ' (raster lib from "' || rast_lib_ver || '" need upgrade)';
		END IF;
	END IF;

	IF rast_scr_ver IS NOT NULL AND rast_scr_ver != relproc THEN
		fullver = fullver || ' (raster procs from "' || rast_scr_ver || '" need upgrade)';
	END IF;

	IF sfcgal_scr_ver IS NOT NULL AND sfcgal_scr_ver != relproc THEN
    fullver = fullver || ' (sfcgal procs from "' || sfcgal_scr_ver || '" need upgrade)';
	END IF;
	return fullver;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

------------------------------------------Yukon_Version--------------------------------------------------

/*
--- 事件触发器函数
--- 当删除一个主表时，如果存在相应的子表，则也将其删除
CREATE FUNCTION DropGeoModelTableTriggerFun()
        RETURNS event_trigger LANGUAGE plpgsql AS $$
DECLARE
    tab_sch varchar;	--- 用来保存 schemas 的名字
    tab_name varchar;	--- 用来保存 子 table 的名字
    tab_cnt int;		  --- 用来保存 子 table 的个数
    sql_str varchar;	--- 最后执行删除的语句
begin
	--- 保存删除的主表名和 shcemas 名字
    select schema_name,object_name into tab_sch,tab_name from pg_event_trigger_dropped_objects() limit 1;
	--- 构建 sql 语句
   	sql_str := pg_catalog.concat(tab_sch,'.',tab_name,'_elem') ;
    ---	查找子表数量
    select count(*) into tab_cnt from pg_class where relname = tab_name||'_elem';
   --- 如果存在子表则删除
    if(tab_cnt > 0) then
	    raise notice 'delete % % table',tab_cnt,sql_str;
	    sql_str = concat('drop table ',sql_str);
	    execute sql_str;
    end if;
end
$$;

--- 创建触发器
CREATE EVENT TRIGGER DropGeoModelTableTrigger
   ON sql_drop
   EXECUTE PROCEDURE DropGeoModelTableTriggerFun();
*/

-----------------------------------------  function }} ------------------------------------------------------


----------------------------------------- {{ geomodel ------------------------------------------------------
CREATE OR REPLACE FUNCTION geomodel_in(cstring)
	RETURNS geomodel
	AS '$libdir/yukon_geomodel-1.0','geomodel_in'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

-- Availability: 1.5.0
CREATE OR REPLACE FUNCTION geomodel_out(geomodel)
	RETURNS cstring
	AS '$libdir/yukon_geomodel-1.0','geomodel_out'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geomodel_recv(internal)
	RETURNS geomodel
	AS '$libdir/yukon_geomodel-1.0','geomodel_recv'
	LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geomodel_send(geomodel)
	RETURNS bytea
	AS '$libdir/yukon_geomodel-1.0','geomodel_send'
	LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION geomodel_typmod_in(cstring[])
	RETURNS integer
	AS '$libdir/yukon_geomodel-1.0','geomodel_typmod_in'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

-- Availability: 1.5.0
CREATE OR REPLACE FUNCTION geomodel_typmod_out(integer)
	RETURNS cstring
	AS '$libdir/yukon_geomodel-1.0','geomodel_typmod_out'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION ST_SRID(geom geomodel)
	RETURNS integer
	AS '$libdir/yukon_geomodel-1.0', 'geomodel_get_srid'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION ST_SETSRID(geom geomodel, srid integer)
	RETURNS geomodel
	AS '$libdir/yukon_geomodel-1.0', 'geomodel_set_srid'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;	

CREATE TYPE geomodel (
	internallength = variable,
	input = geomodel_in,
	output = geomodel_out,
	send = geomodel_send,
	receive = geomodel_recv,
	typmod_in = geomodel_typmod_in,
	typmod_out = geomodel_typmod_out,
	-- delimiter = ':',
	alignment = double,
	-- analyze = geomodel_analyze,
	storage = main
);

----------------------------------------- }} geomodel ----------------------------------------------------


----------------------------------------- {{ model_elem ------------------------------------------------------
CREATE OR REPLACE FUNCTION model_elem_in(cstring)
	RETURNS model_elem
	AS '$libdir/yukon_geomodel-1.0','model_elem_in'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

-- Availability: 1.5.0
CREATE OR REPLACE FUNCTION model_elem_out(model_elem)
	RETURNS cstring
	AS '$libdir/yukon_geomodel-1.0','model_elem_out'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION model_elem_recv(internal)
	RETURNS model_elem
	AS '$libdir/yukon_geomodel-1.0','model_elem_recv'
	LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION model_elem_send(model_elem)
	RETURNS bytea
	AS '$libdir/yukon_geomodel-1.0','model_elem_send'
	LANGUAGE 'c' IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION model_elem_typmod_in(cstring[])
	RETURNS integer
	AS '$libdir/yukon_geomodel-1.0','model_elem_typmod_in'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

-- Availability: 1.5.0
CREATE OR REPLACE FUNCTION model_elem_typmod_out(integer)
	RETURNS cstring
	AS '$libdir/yukon_geomodel-1.0','model_elem_typmod_out'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE TYPE model_elem (
	internallength = variable,
	input = model_elem_in,
	output = model_elem_out,
	send = model_elem_send,
	receive = model_elem_recv,
	typmod_in = model_elem_typmod_in,
	typmod_out = model_elem_typmod_out,
	-- delimiter = ':',
	alignment = double,
	-- analyze = ,
	storage = main
);
----------------------------------------- }} model_elem ------------------------------------------------------



CREATE OR REPLACE FUNCTION model_elem(model_elem, integer, boolean)
	RETURNS model_elem
	AS '$libdir/yukon_geomodel-1.0','model_elem_enforce_typmod'
	LANGUAGE 'c' NOT FENCED IMMUTABLE;


CREATE CAST (model_elem AS model_elem) WITH FUNCTION model_elem(model_elem, integer, boolean) AS IMPLICIT;


CREATE OR REPLACE FUNCTION ST_Boundary(geomodel)
	RETURNS box3D
	AS '$libdir/yukon_geomodel-1.0','boundary'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION ST_CombineBBox(box3d, geomodel)
		RETURNS box3D
		AS '$libdir/yukon_geomodel-1.0', 'BOX3D_combine'
		LANGUAGE 'c' NOT FENCED IMMUTABLE ;		

CREATE AGGREGATE ST_Extent(geomodel) (
	sfunc = ST_CombineBBox,
	stype = box3d,
	finalfunc = box2d
);

CREATE AGGREGATE ST_3DExtent(geomodel) (
	sfunc = ST_CombineBBox,
	stype = box3d	
);


CREATE OR REPLACE FUNCTION ST_GeoModelType(geomodel)
	RETURNS text
	AS '$libdir/yukon_geomodel-1.0','geomodel_geomodeltype'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

----------------------------------------- {{ -- Gist (wrap) for geomodel --  ------------------------------------------------------
CREATE OR REPLACE FUNCTION geomodel_gist_consistent(internal,geomodel,int4) 
	RETURNS bool 
	AS '$libdir/postgis-3' ,'gserialized_gist_consistent'
	LANGUAGE 'c' NOT FENCED;

------ overlaps ------ 
CREATE OR REPLACE FUNCTION geomodel_overlaps(geomodel, geometry) 
	RETURNS boolean 
	AS '$libdir/postgis-3' ,'gserialized_overlaps'
	LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OPERATOR && (
	LEFTARG = geomodel, RIGHTARG = geometry, PROCEDURE = geomodel_overlaps,
	COMMUTATOR = '&&',
	RESTRICT = gserialized_gist_sel_nd,
	JOIN = gserialized_gist_joinsel_nd	
);

CREATE OPERATOR CLASS gist_geomodel_ops
	DEFAULT FOR TYPE geomodel USING GIST AS
	STORAGE 	gidx, -- PostGIS Type
  OPERATOR        3        &&	(geomodel, geometry),
--	OPERATOR        6        ~=	,
--	OPERATOR        7        ~	,
--	OPERATOR        8        @	,
	FUNCTION        1      geomodel_gist_consistent (internal, geomodel, int4),
	FUNCTION        2      geography_gist_union (bytea, internal),
	FUNCTION        3        geography_gist_compress (internal),
	FUNCTION        4        geography_gist_decompress (internal),
	FUNCTION        5        geography_gist_penalty (internal, internal, internal),
	FUNCTION        6        geography_gist_picksplit (internal, internal),
	FUNCTION        7        geography_gist_same (box2d, box2d, internal);
----------------------------------- -- Gist (wrap) for geomodel -- }} ------------------------------------------------------

CREATE OR REPLACE FUNCTION ST_MakeSkeletonFromTIN(geometry, cstring, cstring)
       RETURNS model_elem
       AS '$libdir/yukon_geomodel-1.0','make_skeleton_from_tin'
       LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;	   

CREATE OR REPLACE FUNCTION ST_MakeDefaultMaterial(cstring)
       RETURNS model_elem
       AS '$libdir/yukon_geomodel-1.0','make_default_material'
       LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT ;

CREATE OR REPLACE FUNCTION ST_MakeGeomodel(model_elem)
       RETURNS geomodel
       AS '$libdir/yukon_geomodel-1.0','make_geomodelshell'
       LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION ST_MakeHashID(cstring)
       RETURNS bigint
       AS '$libdir/yukon_geomodel-1.0','make_hash_from_string'
       LANGUAGE 'c' NOT FENCED IMMUTABLE STRICT;