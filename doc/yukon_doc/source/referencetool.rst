参考工具
========

SuperMap GIS
------------

数据对接
~~~~~~~~

SuperMap GIS 平台通过 SuperMap SDX+ for YukonDB
实现数据的对接，支持的空间数据类型包括： 

+ 二/三维点、线、面 
+ 三维模型数据

暂不支持栅格数据。

安装部署
~~~~~~~~

| SuperMap iManager 10.2 支持定制 YukonDB 站点。
| 编排文件： `imgr_yukondb.yaml <_static/files/yukon.yaml>`__ 。

QGIS
----

QGIS 是一个自由软件的桌面 GIS
软件，提供空间数据的显示、编辑和分析功能。可以通过 PostGIS 插件连接
YukonDB，支持的空间数据类型包括： 

+ 二/三维点、线、面 
+ 栅格数据

三维模型数据无法识别。
