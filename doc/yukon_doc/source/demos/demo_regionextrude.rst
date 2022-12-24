
.. _demo_regionextrude_label:

矢量面拉伸构建三维模型对象
-----------------------------

本节用到的模块有：postgis、postgis_sfcgal、yukon_geomodel。
`范例数据 <../_static/files/sampledata.rar>`__ ：sampledata/region.shp，作为建筑物的矢量底面。 

导入底面数据
~~~~~~~~~~~~~~~~~

使用 shp2pgsql 工具导入 region.shp 数据，设置导入结果为 dt1, geometry 列名 smgeometry。

执行 SQL 脚本
~~~~~~~~~~~~~~~~~
构建函数：遍历矢量数据表，对每个面对象进行拉伸、三角化后写入指定的模型数据表。函数实现如下：

.. code:: SQL

    -- geomodel_table 待写入的模型数据表；polygon_table 二维面数据表； polygon_table 中待拉伸的面对象列名
    CREATE OR REPLACE FUNCTION ExtrudePolygon2GeoModel(geomodel_table varchar, polygon_table varchar, polygon_field varchar)
    RETURNS boolean
    AS $$
    DECLARE
    sql text;
    cnt int;
    begin
        sql = 'select count(*) from '||polygon_table||';';
        execute sql into cnt;

        -- 构造默认材质，并写入子表
        sql = 'insert into '|| geomodel_table ||'_elem ' || 'values (ST_MakeHashID('||quote_literal('material')||'),   ST_MakeDefaultMaterial('||quote_literal('material')||') );';
        raise notice '%',sql;
        execute sql;
        
        for i in 1..cnt
        loop
        -- 构造默认材质，矢量面拉伸 --> 转三角面 --> 转骨架对象，并写入子表
        sql = 'insert into '|| geomodel_table||'_elem '||'values (ST_MakeHashID('||quote_literal('skeleton_'||i-1)||'), 
        (select ST_MakeSkeletonFromTIN(ST_Tesselate(ST_Extrude('|| polygon_field ||',0, 0, 100)), '||quote_literal('skeleton_'||i-1)||', '||quote_literal('material')||') 
        from '|| polygon_table ||' where smid = '||i||'));';
        execute sql;
        -- raise notice '%',sql;
        -- 构造模型对象，写入主表
        sql = 'insert into '|| geomodel_table ||' values('||i||', (select  ST_MakeGeomodel( ( select elemcol from '|| geomodel_table ||'_elem ' ||' where id= ST_MakeHashID('||quote_literal('skeleton_'||i-1)||') ) ) ) );';
        execute sql;
        -- raise notice '%',sql;
        end loop;
        RETURN true;
    END; $$ LANGUAGE 'plpgsql';

构造模型数据表，执行 ExtrudePolygon2GeoModel 函数

.. code:: SQL

    -- 创建待写入的模型表，指定id字段为主键
    create table testgeomodel(id int primary key);
    -- 添加geomodel列，指定 SRID=3857
    select AddgeoModelcolumn('public', 'testgeomodel', 'geomodelcol', 3857);
    --执行拉伸构建函数
    select  ExtrudePolygon2GeoModel('testgeomodel', 'dt1', 'smgeometry');


查看模型数据
~~~~~~~~~~~~~~~~~

使用SuperMap桌面，打开Yukon数据源，可以在三维场景中加载testgeomodel数据：

| |模块依赖图|

.. |模块依赖图| image:: /_static/images/makegeomodel_extruded.png