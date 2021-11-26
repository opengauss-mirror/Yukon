.. _geomodelintroduce_label:

三维模型数据组织及存储格式
==================

对象组织结构
------------
三维模型对象存储为GeoModel类型，由带局部坐标系的模型对象（ModelNode）及其放置的位置、姿态等信息组成，对象组织结构见下图：

.. figure:: /_static/images/geomodel_uml.png
   :alt: geomodel对象组织结构
   :align: center
   
   图 geomodel对象组织结构

ModelNode由精细层和LOD层（用PagedLOD表示，可选）数据组成，精细层和LOD层的基本组成单元均为PagedPatch；

每个PagedPatch包含多个Geode；Geode是一个数据包，由实体对象（ModelElement）组成，通过Geode上的矩阵，可以把相同的实体放置在不同的位置，实现模型数据的实例化存储。

ModelElement的子类包括骨架（ModelSkeleton）、材质（ModelMaterial）和纹理（ModelTexture）。  

存储策略
--------
在存储策略上，GeoModel存储在主表中，Geode仅存储实体对象的名字；实体对象单独存储在数据集子表中，基于实体对象名字的64位HashCode编码作为对象的ID。

源码路径
--------
yukong_geomodel模块对应\ `Yukon仓库 <https://gitee.com/opengauss/Yukon>`__ 的/geomodel 目录；

geomodel对象相关代码实现在 /geomodel/libUGC。
