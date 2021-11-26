附录
====

A.1 Release Yukon 1.0.0 Alpha
------------------------------
发布日期：2021/11/30

A.1.1 功能列表
~~~~~~~~~~~~~~
+ 适配 PostGIS 2.4 的三个模块：postgis、postgis_raster、postgis_sfcgal
+ 新增 yukon_geomodel 模块，开放 geomodel 对象的存储结构 

.. _compatibility_label:

A.1.2 已知问题
~~~~~~~~~~~~~~
由于 openGauss 和 PostgreSQL 的差异，PostGIS的矢量和栅格功能还有以下测试未通过:

1. postgis模块

-  loader/Latin1 : varchar 默认为字节数而不是字符数

-  loader/Latin1-implicit ： varchar 默认为字节数而不是字符数

-  cluster ：目前不支持 ``window`` 函数

-  long\_xact ：和 WEB 相关，暂未测试

-  typmod ： copy 不支持 exception

-  同时目前不支持中断操作。

2. postgis_raster模块

-  rt\_tile ： 聚合函数不支持 ``internal`` 参数，导致 ``st_union``
   无法使用，待修复

-  rt\_summarystats ： 缺少 ``st_summarystatsagg`` 聚合函数，待修复

-  rt\_histogram ： 缺少聚合函数 ``st_summarystatsagg`` ，待修复

-  rt\_quantile ： 缺少聚合函数 ``st_summarystatsagg`` ，待修复

-  rt\_createoverview ： 聚合函数不支持 ``internal`` 参数，导致
   ``st_union`` 无法使用，待修复

-  rt\_union ： 聚合函数不支持 ``internal`` 参数，导致 ``st_union``
   无法使用，待修复

-  rt\_elevation\_functions ： 聚合函数不支持 ``internal`` 参数，导致
   ``st_union`` 无法使用，待修复

-  rt\_iscoveragetile ： 聚合函数不支持 ``internal`` 参数，导致
   ``st_union`` 无法使用，待修复

-  rt\_mapalgebra ： 聚合函数不支持 ``internal`` 参数，导致 ``st_union``
   无法使用，待修复

-  rt\_mapalgebrafct ：函数已废弃，推荐使用 ``ST_MapAlgebra``
