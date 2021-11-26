<img src=https://images.gitee.com/uploads/images/2021/1123/160022_8d571424_8511228.png height=250% width=25% alt=Yukon-logo align=center>  
  
Yukon（禹贡），基于openGauss数据库扩展地理空间数据的存储和管理能力，并提供专业的GIS（Geographic Information System）功能，赋能传统关系型数据库。
> 注：《禹贡》是《尚书》中的一篇，是中国古代文献中最古老和最有系统性地理观念的著作。
## 模块组织结构

目前，Yukon 基于openGauss扩展的模块包括：
1. postgis：与openGauss适配的 PostGIS 矢量模块；
2. postgis_raster：与 openGauss适配的PostGIS栅格模块；
3. postgis_sfcgal：与 openGauss适配的PostGIS三维算法相关模块；
4. yukon_geomodel：Yukon的三维模型数据模块。

模块之间的依赖关系如图：

![模块依赖图](https://images.gitee.com/uploads/images/2021/1123/154741_6f7258fb_8511228.png "Yukon模块依赖图.png")

帮助文档参见 [Yukon在线文档](https://yukon.supermap.io/)。

## 许可说明
参见[LICENSE.TXT](https://gitee.com/opengauss/Yukon/blob/master/LICENSE.TXT)


## 贡献

PostGIS与openGauss适配的初始版本源自https://github.com/pg-extension/postgis-xc.git