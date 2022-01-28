# 适配openGauss 2.1 + PostGIS 3.2

## 当前进展
postgis模块已适配；其它模块暂未适配

### 遗留的问题：

1）内存管理相关

libpgcommon/lwgeom_transform.c
Line 117 
CacheMemoryContext
ALLOCSET_SMALL_SIZES


libpgcommon/lwgeom_pg.c
Line 83
F_OIDEQ

static postgisConstants *
getPostgisConstants()
Line 151
CacheMemoryContext
ALLOCSET_SMALL_SIZES

postgis/lwgeom_geos_prepared.c
lwgeom_geos_prepared.c:241:3: error: ‘MemoryContextCallback’ was not declared in this scope    MemoryContextCallback *callback;

lwgeom_geos_prepared.c:252:3: error: ‘MemoryContextRegisterResetCallback’ was not declared in this scope
   MemoryContextRegisterResetCallback(prepcache->context_callback, callback);

2）Geometry brin 相关未编译
缺少  #include "access/brin_tuple.h"
postgis/postgis_brin.sql.in
postgis/brin_2d.c 等未编译；从makefile中移除
    brin_2d.o \
    brin_nd.o \
    brin_common.o \

3） GeoJson 相关未编译

postgis/lwgeom_out_geojson.c
从makefile中移除：
lwgeom_out_geojson.o \

4）postgis--3.2.sql文件暂未处理完成，可临时用2.4.2版本的sql文件替换