--complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION yukon_vector_pyramid" to load this file. \quit


CREATE TABLE pyramid_columns (
	 id bigserial primary key,
     f_schema_name varchar(256),
	 f_table_name varchar(256),
	 f_geometry_column varchar(256),
	 f_pyramid_table_name varchar(256),
     resolution float8,
	 config text
);


CREATE OR REPLACE FUNCTION yukon_pyramid_version()
RETURNS text
	AS '$libdir/yukon_vector_pyramid-1.0','yukon_pyramid_version'
LANGUAGE 'c' IMMUTABLE STRICT ;



CREATE OR REPLACE FUNCTION ST_BuildPyramid(schemaname text, tablename text, columnname text, config text)
RETURNS boolean
	AS '$libdir/yukon_vector_pyramid-1.0','buildpyramid'
LANGUAGE 'c' IMMUTABLE STRICT ;


CREATE OR REPLACE FUNCTION ST_ListPyramid(schemaname text, tablename text, columnname text)
RETURNS SETOF text
AS
$$
DECLARE
    sql_statment text;
    item         text;
BEGIN
    sql_statment = 'SELECT config FROM pyramid_columns WHERE f_table_name = ' || QUOTE_LITERAL(tablename) ||
                   ' and f_geometry_column=' || QUOTE_LITERAL(columnname) || ' and f_schema_name =' || QUOTE_LITERAL(schemaname) ;
    FOR item IN EXECUTE sql_statment
        LOOP
            RETURN NEXT item;
        END LOOP;
    RETURN;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

-- 查看是否有矢量金字塔层
CREATE OR REPLACE FUNCTION ST_HasPyramid(schemaname text, tablename text, columnname text)
    RETURNS boolean
AS
$$
DECLARE
    ret integer;
    tmp VARCHAR(512);
BEGIN
    tmp = 'SELECT count(*) FROM pyramid_columns WHERE f_table_name = ' || QUOTE_LITERAL(tablename) ||
                   ' and f_geometry_column=' || QUOTE_LITERAL(columnname) || ' and f_schema_name =' || QUOTE_LITERAL(schemaname) ;
    execute tmp into ret;
    return ret != 0;
END;
$$
    LANGUAGE 'plpgsql' VOLATILE STRICT;


CREATE OR REPLACE FUNCTION ST_DeletePyramid(schemaname text, tablename text, columnname text)
	RETURNS void
	AS $$
	DECLARE
    tmp VARCHAR(512);
    rec record;
BEGIN
	tmp:= 'select * from pyramid_columns where f_table_name = ' || QUOTE_LITERAL(tablename) ||
          ' and f_geometry_column =' || QUOTE_LITERAL(columnname) || ' and f_schema_name =' || QUOTE_LITERAL(schemaname);

  FOR rec IN EXECUTE tmp LOOP
    -- tmp := 'DROP TABLE '|| quote_ident(rec.f_pyramid_table_name) || ' CASCADE;';
    -- 删除数据表
    EXECUTE 'DROP TABLE '|| quote_ident(rec.f_schema_name) || '.' ||quote_ident(rec.f_pyramid_table_name) || ' CASCADE;';
    -- 删除元信息表中的记录
    EXECUTE 'DELETE from pyramid_columns where f_schema_name='|| quote_literal(rec.f_schema_name) || ' and f_pyramid_table_name=' ||quote_literal(rec.f_pyramid_table_name);

  END LOOP;
END;
$$
LANGUAGE 'plpgsql' VOLATILE STRICT;

-- 为了使与 opengauss 保持兼容，不使用触发器

-- /*
--  * 创建触发器函数，用于当删除数据表时，同时删除 pyramid 表
--  * 当手动删除 pyramid 表时,删除 pyramid_columns 中的记录
-- */

-- CREATE OR REPLACE FUNCTION drop_pyramid_table_trigger_function()
--     RETURNS EVENT_TRIGGER
--     LANGUAGE PLPGSQL AS
-- $$
-- DECLARE
--     obj       record;
--     tablename record;
--     sql_text  text;
-- BEGIN
--     FOR obj in SELECT * FROM pg_event_trigger_dropped_objects()
--         LOOP
--             -- 如果当前删除的是一个表
--             IF obj.object_type = 'table' then
--                 -- 在元信息表中查找当前表是否有 pyramid 的表
--                 FOR tablename in SELECT f_pyramid_table_name FROM pyramid_columns WHERE f_table_name = obj.object_name
--                     LOOP
--                         -- 删除表
--                         sql_text = 'DROP TABLE IF EXISTS ' || quote_ident(tablename.f_pyramid_table_name) || ' CASCADE';
--                         EXECUTE sql_text;
--                         -- 删除 pyramid_columns 中的记录
--                         sql_text = 'DELETE FROM pyramid_columns WHERE f_table_name = ' ||
--                                    quote_literal(obj.object_name);
--                         EXECUTE sql_text;
--                     END LOOP;
--                 -- 查找当前的表是否是 pyramid 的表
--                 FOR tablename in SELECT f_pyramid_table_name FROM pyramid_columns WHERE f_pyramid_table_name = obj.object_name
--                     LOOP
--                         -- 删除 pyramid_columns 中的记录
--                         sql_text = 'DELETE FROM pyramid_columns WHERE f_pyramid_table_name = ' ||
--                                    quote_literal(obj.object_name);
--                         EXECUTE sql_text;
--                     END LOOP;

--             END IF;
--         END LOOP;
-- END;
-- $$;


-- /*
--  * 创建触发器
--  */
-- CREATE EVENT TRIGGER drop_pyramid_triger
--     ON sql_drop
-- EXECUTE FUNCTION drop_pyramid_table_trigger_function();




CREATE OR REPLACE FUNCTION ST_BuildTile(schemaname text, tablename text, columnname text, maxlevel int)
RETURNS boolean
	AS '$libdir/yukon_vector_pyramid-1.0','buildTile'
LANGUAGE 'c' VOLATILE ;

CREATE OR REPLACE FUNCTION ST_UpdatePyramid(schemaname text, tablename text, columnname text, updateextent BOX2D, maxlevel int)
RETURNS boolean
	AS '$libdir/yukon_vector_pyramid-1.0','updatePyramid'
LANGUAGE 'c' VOLATILE ;


CREATE OR REPLACE FUNCTION ST_AsTile(schemaname text, tablename text, columnname text, z int8, x int8, y int8)
    RETURNS bytea
AS
$$
DECLARE
    sql      VARCHAR(512);
    pydtable text;
    id       bigint;
    cnt      int8;
    srid     int8;
    res      bytea;
BEGIN
    -- 检查是否包含 geometry 数据
    sql = 'select count(*) from geometry_columns where f_table_schema=' || quote_literal(schemaname) ||
          ' and f_table_name=' || quote_literal(tablename) || ' and f_geometry_column = ' || quote_literal(columnname);
    EXECUTE sql into cnt;

    if cnt = 0 then
        raise '% is not a geometry table',tablename;
    end if;

    -- 获取 srid,目前只支持 4326 和 3857 坐标系
    sql = 'select srid from geometry_columns where f_table_schema=' || quote_literal(schemaname) ||
          ' and f_table_name=' || quote_literal(tablename) || ' and f_geometry_column = ' || quote_literal(columnname);
    EXECUTE sql into srid;

    if srid != 4326 and srid != 3857 then
        raise 'only support 4326 or 3857 coordinate';
    end if;

    -- 先检查瓦片表是否存在
    pydtable := 'tile_' || tablename || '_' || columnname;

    sql := 'select count(*) from pg_class where relnamespace = (select oid from pg_namespace where nspname =' ||
           quote_literal(schemaname) || ') and relname = ' || quote_literal(pydtable);
    EXECUTE sql into cnt;
    -- 说明不存在矢量金字塔表，则直接生成
    if cnt = 0 then
        -- 这里我们提示没有矢量金字塔表
        raise '% does not have a pyramid table',tablename;
    end if;

    -- 这里我们组装 ld
    id = (z * pow(2, 50))::bigint + (x * pow(2, 25))::bigint + y;

    -- 检查一下在矢量金字塔表中是否存在相应的数据
    sql := 'select count(*) from ' || quote_ident(schemaname) || '.' || quote_ident(pydtable) || ' where id = ' || id;
    EXECUTE sql into cnt;
    if cnt = 0 then
        -- 这是我们根据原表直接生成
        if srid = 3857 then
            sql := 'WITH mvtgeom AS ( SELECT ST_AsMVTGeom(' || quote_ident(columnname)
                       || ', ST_TileEnvelope( ' || z || ',' || x || ',' || y || ')) AS geom  FROM '
                       || quote_ident(schemaname) || '.' || quote_ident(tablename) ||
                   ' WHERE ' || quote_ident(columnname) || ' && ST_TileEnvelope(' || z || ',' || x || ',' || y || ')
                   ) SELECT ST_AsMVT(mvtgeom.*) FROM mvtgeom;';
            raise notice 'generate:z:%,x:%,y:%',z,x,y;
            execute sql into res;
            return res;
        else
            sql := 'WITH mvtgeom AS ( SELECT ST_AsMVTGeom('
                       || quote_ident(columnname)
                       || ', ST_TileEnvelope( ' || z || ',' || x || ',' || y ||
                   ',ST_MakeEnvelope(-180, -270, 180, 90, 4326))) AS geom  FROM '
                       || quote_ident(schemaname) || '.' || quote_ident(tablename) ||
                   ' WHERE ' || quote_ident(columnname) || ' && ST_TileEnvelope(' || z || ',' || x || ',' || y || ',ST_MakeEnvelope(-180, -270, 180, 90, 4326))
                   ) SELECT ST_AsMVT(mvtgeom.*) FROM mvtgeom;';
            raise notice 'generate:z:%,x:%,y:%',z,x,y;
            execute sql into res;
            return res;

        end if;

    else
        sql := 'select mvt from ' || quote_ident(pydtable) || ' where id = ' || id;
        execute sql into res;
        return res;
    end if;
END ;
$$
    LANGUAGE 'plpgsql' VOLATILE
                       STRICT;
