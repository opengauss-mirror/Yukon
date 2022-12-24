Geography与Geometry
------------------------

geography 类型
~~~~~~~~~~~~~~~~~~~
Geography数据类型基于大地坐标系对经纬度坐标（也可称为大地坐标或者地理坐标）数据提供支持，大地坐标是用角度单位表示的球面坐标。
而Geometry是基于二维笛卡尔坐标系-Cartesian Coordinate System（平面坐标系，投影坐标系）的，投影坐标通常是以米（meter）、千米（kilometer）等作为单位。如下图所示：

.. figure::  ..\_static\images\co_sy.png
   :alt: 平面坐标和球面坐标
   :align: center

geometry与geography对比
~~~~~~~~~~~~~~~~~~~
.. csv-table:: geometry与geography支持的数据类型
   :header: "数据类型", "geometry", "geography"

   "POINT","√", "√"
   "MULTIPOINT","√","√"

   "LINESTRING","√","√"
   "MULTILINESTRING","√","√"
   "CIRCULARSTRING","√", "×"
   "ELLIPSESTRING","√", "×"
   "COMPOUNDCURVE","√","×"
   "MULTICURVE","√", "×"
   "TRIANGLE","√","×"
   "TIN","√","×"

   "POLYGON","√","√"
   "MULTIPOLYGON","√","√"
   "CURVEPOLYGON","√","×"
   "MULTISURFACE","√","×"
   "POLYHEDRALSURFACE","√","×"
   "GEOMETRYCOLLECTION","√","√"

（注意:GeometryCollection数据类型仅包含以上（多）点、（多）线、（多）面类型）

众所周知，在球面上的计算比在平面上的计算更复杂，代价更高，所以Geography支持的空间函数比Geometry支持的空间函数少（PostGIS依赖的GEOS库实现的函数，均不支持Geography数据类型）。

Geography仅支持格式转换、量算、空间关系判定等几大类函数运算。
二者所支持的函数列表 `参考 <http://postgis.net/docs/PostGIS_Special_Functions_Index.html#PostGIS_TypeFunctionMatrix>`__ ：

为了扩展使用场景，可Geography数据类型与Geometry数据类型相互转换解决这一问题。
如下图流程所示，将geom转换到EPSG:4326（lon，lat），再将其转换为Geog。

.. figure:: /_static/images/geom_geog.png
   :alt: geom转换为geog
   :align: center

以范例数据 nyc_subway_stations为例：

.. code:: SQL

    CREATE TABLE nyc_subway_stations_geog AS
    SELECT
       Geography(ST_Transform(geom,4326)) AS geog,
       name,
       routes
    FROM nyc_subway_stations;

相反，也可采用（geog：：geometry）函数将geog转为geom。以下举例当需要通过ST_X(point)函数从geog中获取坐标时的做法:

.. code:: SQL

    SELECT code, ST_X(geog::geometry) AS longitude FROM airports;

得到以下结果：

.. figure:: /_static/images/geog_geom_result.png
   :alt: geog转换为geom
   :align: center



Geometry和Geography这两种数据类型的最大不同之处是它们对空间数据的计算方式不同，这也是最能体现Geography数据类型价值的地方。
例如，计算两个坐标点的距离，Geometry是基于平面坐标系使用勾股定理公式计算它们之间的平面距离。而Geography是基于球面坐标系，将两个坐标点看作地球参考椭球体上的两个坐标点，计算它们之间的
**大圆航线** 距离。
以下通过两种数据类型计算两点之间的距离：

.. code:: SQL

    SELECT ST_Distance(
      ST_GeometryFromText('Point(-118.4079 33.9434)'),
      ST_GeometryFromText('Point(139.733 35.567)'))
         AS geometry_distance,
    ST_Distance(
      ST_GeographyFromText('Point(-118.4079 33.9434)'),
      ST_GeographyFromText('Point(139.733 35.567)'))
         AS geography_distance;

基于geog的距离计算结果得到正确答案：

.. figure:: /_static/images/geog_geom_distance.png
   :alt: geog与geom距离计算
   :align: center

综上，针对geometry与Geography两种数据类型进行对比，使用者可根据不同的适用场景灵活选择：

.. figure:: /_static/images/geom_geog compare.png
   :alt: Geometry与Geography对比
   :align: center



