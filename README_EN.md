<img src=./doc/image/Yukon.png height=250% width=25% alt=Yukon-logo align=center>  
  
Yukon is an extension of openGauss that provides capabilities for geospatial data storage and management, along with professional Geographic Information System (GIS) functions, empowering traditional relational databases.
> Note: Yukong is a reference to a chapter in the *Book of Documents*, the oldest and most systematic work on geography in ancient Chinese literature.
## Module Organization

Yukon currently extends the following modules based on openGauss:
1. postgis: PostGIS vector module adapted for use with openGauss
2. postgis_raster: PostGIS raster module adapted for use with openGauss
3. postgis_sfcgal: PostGIS 3D algorithm module adapted for use with openGauss
4. yukon_geomodel: 3D model data module of Yukon
5. yukon_geogridcoder: grid encoding module of Yukon

The following figure shows the dependencies between the modules.

![Module dependencies](doc/html/images/yukon_model_info.png "Yukon模块依赖图.png")

For details, see [Yukon documentation](https://yukon.supermap.io/).

## License
See [LICENSE.TXT](https://gitee.com/opengauss/Yukon/blob/master/LICENSE.TXT).


## Contributions

The initial version of PostGIS adapted to openGauss is from https://github.com/pg-extension/postgis-xc.git.

It has been upgraded to PostGIS 3.2.
