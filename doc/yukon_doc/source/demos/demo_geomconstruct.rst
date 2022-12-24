geometry 对象定义及构造
------------------------

geometry 类型
~~~~~~~~~~~~~~~~~~~

定义geometry列，可以不指定子类型，则表示可以存储任意子类型。

.. code:: SQL

    -- 指定srid 为 4326
    create table testgeoms ( id int primary key, geom geometry);

geometry列，也可以指定子类型，支持的类型名见下一小节，示例如下：

.. code:: SQL

    -- 指定 geom 列存储 linestring
    create table testlinestring (id serial, geom geometry(linestring, 4326) );

指定子类型时，可带单引号、双引号或不带引号，也不区分大小写。

geometry 子类型
~~~~~~~~~~~~~~~~~~~

geometry 有15种子类型，包括：

.. csv-table:: geometry 类型
   :header: "子类型", "描述", "构成"

   "POINT","点", "/"
   "LINESTRING","线，折线","由点串构成"
   "POLYGON","面","由 n 个部分组成，每个部分是首尾相连的线串"
   "POLYHEDRALSURFACE","多面体表面","由 n 个部分组成，每个部分都是一个 Polygon"
   "MULTIPOINT","多点","由 n 个 Point 子对象组成"
   "MULTILINESTRING","多线","由 n 个 LineString 子对象组成"
   "MULTIPOLYGON","多面","由 n 个 Polygon 子对象组成"
   "CIRCULARSTRING","线，由圆弧连接而成","用点串描述。三个点确定一段圆弧，前一个圆弧的最后一个点与后一个圆弧的第一个点共用；特别地，如果圆弧的第一个点与第三个点重合，则第二个点表示圆心，以此来表达圆形"
   "COMPOUNDCURVE","复合线","由 n 个部分组成，每个部分可以是LineString 或 CircularString，且前一部分的最后一个点与后续部分的第一个点重合，保证复合线对象的连续性"
   "MULTICURVE","多(曲)线","由 n 个子对象组成，每个子对象可以是CircularString 或 LineString 或 CompoundCurve"
   "CURVEPOLYGON","复合面","由 n 个部分组成，每个部分是首尾相连的 CircularString 或 LineString 或 CompoundCurve。与 Polygon 类似，都表达一个闭合的区域，区别在于是否有CircularString对象参与构造"
   "MULTISURFACE","多面","由 n 个子对象组成，每个子对象可以是Polygon 或 CurvePolygon 类型。与 MultiPolygon类似，都表达多面对象，区别在于是否有CircularString对象参与构造"
   "GEOMETRYCOLLECTION","复合对象","由任意子类型构成"
   "TRIANGLE","三角形","由首尾相连的4个点构成"
   "TIN","不规则三角网","由 n 个 Triangle 组成"




| 以上每种类型，可以指定是否带Z或M值。例如，点可以指定为：POINT、POINTZ、POINTM、POINTZM。
| 以上15种子类型，有一部分容易理清其关系，见下图左；当加入新的类型
  **CircularString**
  后，衍生出带参数化对象的线和面，进而构成PostGIS的全部15种子类型，见下图右。

.. figure:: /_static/images/PostGIS-GeometryType.png
   :alt: PostGIS-GeometryType
   :align: center

   PostGIS-GeometryType

构造示例：Point LineString Polygon
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Point、LineString、Polygon、MultiPoint、MultiLineString、MultiPolygon容易理解，构造示例如下：

.. code:: SQL

    -- 指定 srid 为 4326
    CREATE TABLE testgeomobj (id serial, geom geometry NOT NULL);

    -- Point 对象 
    INSERT INTO testgeomobj (geom) VALUES ('SRID=4326;POINT(-95.363151 29.763374)');
    INSERT INTO testgeomobj (geom) VALUES ('SRID=4326;POINT(-95.363151 29.763374)');
    -- MultiPoint 对象 
    INSERT INTO testgeomobj (geom) VALUES ('SRID=4326;MULTIPOINT(-95.4 29.8,-95.4 29.8)');

    -- LineString 对象
    insert into testgeomobj (geom) values ('SRID=4326;LINESTRING(-71.1031880899493 42.3152774590236,-71.1031627617667 42.3152960829043,-71.102923838298 42.3149156848307,-71.1023097974109 42.3151969047397,-71.1019285062273 42.3147384934248)');
    -- MultiLineString 对象
    insert into testgeomobj (geom) values ('SRID=4326;MultiLineString (
    (-71.1031880899493 42.3152774590236,-71.1031627617667 42.3152960829043,-71.102923838298 42.3149156848307,-71.1023097974109 42.3151969047397,-71.1019285062273 42.3147384934248),
    (-71.1766585052917 42.3912909739571, -71.1766820268866 42.391370174323896, -71.1766063012595 42.3913825660754, -71.17658265830809 42.391303365353096) 
    )');

    -- Polygon 对象
    insert into testgeomobj(geom) values ('SRID=4326;
    POLYGON (
    (-71.1776585052917 42.3902909739571, -71.1776820268866 42.3903701743239, -71.1776063012595 42.3903825660754, -71.1775826583081 42.3903033653531,-71.1776585052917 42.3902909739571),
    (-71.1766585052917 42.3912909739571, -71.1766820268866 42.391370174323896, -71.1766063012595 42.3913825660754, -71.17658265830809 42.391303365353096, -71.1766585052917 42.3912909739571) 
    )');
    -- MultiPolygon 对象
    insert into testgeomobj(geom) values ('SRID=4326; MultiPolygon (
    ((-71.1776585052917 42.3902909739571, -71.1776820268866 42.3903701743239, -71.1776063012595 42.3903825660754, -71.1775826583081 42.3903033653531,-71.1776585052917 42.3902909739571)),
    ((-71.1766585052917 42.3912909739571, -71.1766820268866 42.391370174323896, -71.1766063012595 42.3913825660754, -71.17658265830809 42.391303365353096, -71.1766585052917 42.3912909739571)) 
    )');

构造示例：CircularString CompoundCurve
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: SQL

    CREATE TABLE testgeom (id serial,geom geometry NOT NULL);
    -- CircularString：由四段组成，见下图左
    INSERT INTO testgeom (geom) VALUES ('CIRCULARSTRING(0 2, -1 1, 0 0, 0.5 0, 1 0, 2 1, 1 2, 0.5 2, 0 2)');
    -- CompoundCurve：由圆弧和折线段组成，'LINESTRING'关键字可省略，生成的图形见下图右
    INSERT INTO testgeom (geom) VALUES ('COMPOUNDCURVE(CIRCULARSTRING(0 0, 1 1, 1 0),LINESTRING(1 0, 2 0))'); 

|CircularString| |CompoundCurve|

构造示例：CurvePolygon
~~~~~~~~~~~~~~~~~~~~~~~

.. code:: SQL

    -- 由首尾相连的圆弧线串和折线构成
    insert INTO testgeom (geom) VALUES('CURVEPOLYGON(CIRCULARSTRING(0 0, 4 0, 4 4, 0 4, 0 0),(1 1, 3 3, 3 1, 1 1))');

构造示例：PolyhedralSurface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

PolyhedralSurface 由n个 Polygon 构成，PolyhedralSurface
对象不一定是闭合的，可以用ST_IsClosed() 方法判断。

.. code:: SQL

    -- PolyhedralSurface 对象
    INSERT INTO testgeom (geom) VALUES ('POLYHEDRALSURFACE( ((0 0 0, 0 0 1, 0 1 1, 0 1 0, 0 0 0)), ((0 0 0, 0 1 0, 1 1 0, 1 0 0, 0 0 0)), ((0 0 0, 1 0 0, 1 0 1, 0 0 1, 0 0 0)), ((1 1 0, 1 1 1, 1 0 1, 1 0 0, 1 1 0)), ((0 1 0, 0 1 1, 1 1 1, 1 1 0, 0 1 0)), ((0 0 1, 1 0 1, 1 1 1, 0 1 1, 0 0 1)) )');

    SELECT ST_IsClosed(geom) from testgeom;

.. |CircularString| image:: /_static/images/CircularString.png
.. |CompoundCurve| image:: /_static/images/CompoundCurve.png
