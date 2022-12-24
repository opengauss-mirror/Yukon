.. image:: /_static/images/Yukon-2.png
   :scale: 30%
   :align: center

产品简介
========
Yukon（禹贡 [#yukon]_），基于openGauss数据库 [#openGauss]_ 扩展地理空间数据的存储和管理能力，
并提供专业的GIS（Geographic Information System）功能，赋能传统关系型数据库。

源码托管在openGauss社区的\ `Yukon仓库 <https://gitee.com/opengauss/Yukon>`__。  

许可说明参见 \ `LICENSE.TXT <https://gitee.com/opengauss/Yukon/blob/master/LICENSE.TXT>`__。

模块组织结构
============

目前，Yukon 基于 openGauss 扩展的模块包括： 

1. postgis：与 openGauss 适配的 PostGIS 矢量模块； 
2. postgis_raster：与 openGauss 适配的 PostGIS 栅格模块； 
3. postgis_sfcgal：与 openGauss 适配的 PostGIS 三维算法相关模块； 
4. yukon_geomodel：Yukon自有的三维模型数据模块。

模块之间的依赖关系如图：

.. figure:: /_static/images/modstruct.png
   :alt: 模块依赖图
   :align: center
   
   模块依赖图
    

   
对于与PostGIS适配的三个模块（postgis、postgis_raster、postgis_sfcgal），本文档提供入门级教程，
更详细的文档可参考 `PostGIS 文档 <http://postgis.net/docs/manual-2.4/>`__；
此外，当前版本尚未适配的能力清单见 :ref:`compatibility_label` 。

.. [#yukon] 《禹贡》是《尚书》中的一篇，是中国古代文献中最古老和最有系统性地理观念的著作。
.. [#openGauss]  见 `openGauss 官网 <https://opengauss.org/zh/>`__
