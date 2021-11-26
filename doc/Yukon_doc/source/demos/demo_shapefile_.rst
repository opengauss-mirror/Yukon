shapefile 数据导入及空间查询
-----------------------------

导入数据
~~~~~~~~~~

`范例数据 <../_static/files/sampledata.rar>`__ 包含纽约市的街道、地铁站等信息，路径: \nyc_shapefile 

使用 shp2pgsql导入数据
^^^^^^^^^^^^^^^^^^^^^^^^^^^

**导入 nyc_census_blocks 数据**

.. code:: sh

    shp2pgsql \
      -D \
      -I \
      -s 26918 \
      nyc_census_blocks.shp \
      nyc_census_blocks \
      | gsql dbname=yukontutorial

其中：

-  ``-D``: 使用 ``dump format`` 方式，这个要比默认的 ``insert format``
   快很多。
-  ``-I``: 加载数据完成后，为 geometry 类型的列创建一个索引
-  ``-s``: 数据的 srid 值。
-  ``nyc_census_blocks.shp``: 要加载的数据集
-  ``nyc_census_blocks``: 加载后在数据库中创建的表名
-  ``gsql dbname=yukontutorial`` : 将数据导入到 ``yukontutorial``
   数据库中

当你看到有如下输出时，则说明数据导入成功

.. code:: text

    Field popn_total is an FTDouble with width 11 and precision 0
    Field popn_white is an FTDouble with width 11 and precision 0
    Field popn_black is an FTDouble with width 11 and precision 0
    Field popn_nativ is an FTDouble with width 11 and precision 0
    Field popn_asian is an FTDouble with width 11 and precision 0
    Field popn_other is an FTDouble with width 11 and precision 0
    Shapefile type: Polygon
    Postgis type: MULTIPOLYGON[2]
    SET
    SET
    BEGIN
    NOTICE:  CREATE TABLE will create implicit sequence "nyc_census_blocks_gid_seq" for serial column "nyc_census_blocks.gid"
    CREATE TABLE
    NOTICE:  ALTER TABLE / ADD PRIMARY KEY will create implicit index "nyc_census_blocks_pkey" for table "nyc_census_blocks"
    ALTER TABLE
                             addgeometrycolumn
    --------------------------------------------------------------------
     public.nyc_census_blocks.geom SRID:26918 TYPE:MULTIPOLYGON DIMS:2
    (1 row)

    CREATE INDEX
    COMMIT
    ANALYZE

然后重新连接到 ``yukontutorial`` 数据库，输入 ``\d`` 查看新创建的表：

.. code:: text

    yukontutorial=# \d
                                        List of relations
     Schema |           Name            |   Type   | Owner |             Storage
    --------+---------------------------+----------+-------+----------------------------------
     public | geography_columns         | view     | omm   |
     public | geometry_columns          | view     | omm   |
     public | geomodel_columns          | view     | omm   |
     public | nyc_census_blocks         | table    | omm   | {orientation=row,compression=no}
     public | nyc_census_blocks_gid_seq | sequence | omm   |
     public | raster_columns            | view     | omm   |
     public | raster_overviews          | view     | omm   |
     public | spatial_ref_sys           | table    | omm   | {orientation=row,compression=no}
    (8 rows)

同理，依次导入剩余的数据集：

-  nyc_neighborhoods.shp
-  nyc_streets.shp
-  nyc_subway_stations.shp

**你可能会好奇 .shp 文件到底是什么？**

通常来说 ``shp`` 文件一般指的是 ``.shp``, ``.shx``,\ ``.dbf``
还有一些其他类型文件且前缀名称一致的文件集合。\ ``shp`` 即 shape file
,通常单独的一个 ``shp``
文件时没有什么用的，必须和其他类型的文件结合使用。

必要文件：

-  ``.shp``\ ：存储地理要素的几何信息
-  ``.shx``\ ：存储要素几何图形的索引信息
-  ``.dbf``: 存储地理要素的属性信息（非几何信息）

可选文件：

-  ``.prj``\ ：存储空间参考信息，即地理坐标系统信息和投影坐标系统信息。使用
   well-known 文本格式进行描述。

**那 SRID 又是什么？**

大多数导入过程都是不言自明的，但即使是经验丰富的 GIS 专业人员也可能被
SRID 难倒。

``SRID`` 表示
``Spatial Reference IDentifier（空间参考标识符)``\ 。它定义了我们数据的地理坐标系统和投影的所有参数。

SRID
很方便，因为它将有关地图投影的所有信息（可能非常复杂）打包（更具体的说应该是映射）到一个数字中。

你可以在以下链接中查找我们在上面使用的投影的定义：\ `SRID <http://spatialreference.org/ref/epsg/26918/>`__

数据操作
~~~~~~~~~~

我们想知道纽约市街道的总长度是多少应该怎么做？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: sql

    select SUM(ST_LENGTH(GEOM)) from nyc_streets; 

你可以看到如下输出：

.. code:: sql

    yukontutorial=# select  SUM(ST_LENGTH(GEOM)) from nyc_streets ;
          sum
    ---------------
     10418904.7172
    (1 row)

这样我们就可以知道纽约市街道总长度为 10418904.7172m

``ST_Length(geometry geom)``\ ：如果 geom 的类型是 ``LineString`` 或者
``MultiLineString``,那么就返回这个 geometry 的长度。

``sum``\ ：聚合函数，可以计算 ``nyc_streets`` 表中每一行 geometry
对象长度的总和。

我们想知道纽约市最西边的地铁站是哪一个？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: SQL

    select st_x(geom),name from nyc_subway_stations order by st_x(geom) limit 1;

输出结果：

.. code:: SQL

    yukontutorial=# select st_x(geom),name from nyc_subway_stations order by st_x(geom) limit 1;
           st_x       |    name
    ------------------+-------------
     563292.117258056 | Tottenville
    (1 row)

``ST_X(geometry a_point);``: 如果 geometry 是一个点，则返回它的 x
坐标，否则返回 NULL。

现在我们知道位于纽约市最西边的地铁站是 Tottenville。

我们想知道距离 Broad St 地铁站 10m 范围内的街道有那些？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

我们可以先查询一下 ``Broad St`` 地铁站的具体位置：

.. code:: sql

    select st_astext(geom) from nyc_subway_stations where name='Broad St';

输出结果：

.. code:: SQL

    yukontutorial=# select st_astext(geom) from nyc_subway_stations where name='Broad St';
                    st_astext
    ------------------------------------------
     POINT(583571.905921312 4506714.34119218)
    (1 row)

现在我们知道了 ``Borad St`` 地铁站的具体地点为
``POINT(583571.905921312 4506714.34119218)``\ ，
接下来可以查询距离地铁站 10m 范围内的街道了。

.. code:: sql

    SELECT name
    FROM nyc_streets
    WHERE ST_DWithin(
            geom,
            ST_GeomFromText('POINT(583571.905921312 4506714.34119218)',26918),
            10
          );

输出结果：

.. code:: text

       name
    -----------
     Wall St
     Broad St
     Nassau St
    (3 rows)

现在我们可以知道距离 ``Broad St`` 地铁站 10m 内的街道有这 3 个。

``ST_AsText(geometry g1)``: 返回 WKT 形式的 geometry 数据。

``ST_GeomFromText(text WKT, integer srid);`` : 返回由 WKT 形式表示的
geometry 数据类型。同时指定它的 SRID。

``ST_DWithin(geometry g1, geometry g2, double precision distance_of_srid)``
: 如果 g1 和 g2 相距 distance_of_srid 之内，则返回真，否则返回假。

我们想知道纽约市人口密度最大的社区是哪一个？
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: SQL

    SELECT
      n.name,
      Sum(c.popn_total) / (ST_Area(n.geom) / 1000000.0) AS popn_per_sqkm
    FROM nyc_census_blocks AS c
    JOIN nyc_neighborhoods AS n
    ON ST_Intersects(c.geom, n.geom)
    GROUP BY n.name, n.geom
    ORDER BY 2 DESC limit 1; 

输出结果：

.. code:: SQL

           name        |  popn_per_sqkm
    -------------------+------------------
     North Sutton Area | 68435.1328377268
    (1 row)

从结果中我们可以看到 North Sutton Area 的人口密度是最大的。

``ST_Intersects( geometry geomA , geometry geomB )``: 返回 geomA 和
geomB 是否相交
