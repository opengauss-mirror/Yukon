/*
 *
 * rasterize_hash.cpp
 *
 * Copyright (C) 2021-2024 SuperMap Software Co., Ltd.
 *
 * Yukon is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>. *
 */

#include "postgres.h"
#include "fmgr.h"
#include <cmath>
#include "RasterizeHash.h"

/**
 * HashRasterize 函数修改自 rt_raster.c 中的 rt_raster_gdal_rasterize 函数
 * 修改：
 * 1. 因 _rti_rasterize_arg_t 中原有的指针部分已修改为基本类型，无需释放内存，移除本函数中的释放内存函数 
 * 2. 移除一些不需要的参数
 * 3. 修改 arg 作为参数的函数调用
 * 4. 移除 width 和  height 相关代码
 * 5. 移除 skew_x 和 skew_y 相关代码
 * 6. 移除因旋转产生的重新计算 extent 相关代码
 * 7. 移除 scale_x scale_y 的相关代码,scale_x scale_y 在函数内计算
 * 8. 移除 ul_xw,ul_yw 相关代码，由 global_extent 指定
 */

/**
 * Return a raster of the provided geometry
 *
 * @param wkb : WKB representation of the geometry to convert
 * @param wkb_len : length of the WKB representation of the geometry
 * @param srs : the geometry's coordinate system in OGC WKT
 * @param global_extent 全局 hash 范围
 * @param level hash 划分等级
 * @param data : 栅格化后的原始数据
 */
void HashRasterize(
    const unsigned char *wkb, uint32_t wkb_len,
    const char *srs,
    const Yk::YkRect2D global_extent,
    const unsigned int level, RawHashData &data)
{
    int i = 0;
    int err = 0;

    /* 栅格化后的 width 和 height */
    int _dim[2] = {0};
    /* 栅格化后的像素大小 */
    double _scale[2] = {0};
    /* 栅格旋转参数，均为 0  */
    double _skew[2] = {0};

    /* raster 相关参数 */
    uint8_t noband = 0;
    uint32_t numbands = 1;
    rt_pixtype pixtype = (rt_pixtype)4;
    double init = 0;
    double nodata = 0;
    uint8_t hasnodata = 1;
    double value = 1;
    int bandlist = 1;
    OGRSpatialReferenceH *src_sr = nullptr;

    char **options = nullptr;

    /* 一定要指定最后一个指针为 nullptr，否则会造成段错误 */
    options = (char **)palloc(sizeof(char *) * 1);
    options[0] = palloc(sizeof(char *) * (strlen("ALL_TOUCHED=TRUE") + 1));
    options[0] = "ALL_TOUCHED=TRUE";
    options = (char **)repalloc(options, sizeof(char *) * 2);
    options[1] = nullptr;

    OGRErr ogrerr;
    OGRGeometryH src_geom;
    OGREnvelope src_env;
    rt_envelope extent;
    rt_envelope subextent;
    OGRwkbGeometryType wkbtype = wkbUnknown;

    int ul_user = 0;

    CPLErr cplerr;
    /* 变换矩阵，用于在后续为 @_ds 设置 */
    double _gt[6] = {0};
    GDALDriverH _drv = nullptr;
    int unload_drv = 0;
    /* 栅格化 handler */
    GDALDatasetH _ds = nullptr;
    GDALRasterBandH _band = nullptr;

    uint16_t _width = 0;
    uint16_t _height = 0;

    if (nullptr == wkb)
    {
        rterror("rasterize_hash: wkb cannot be nullptr.");
    }

    if (0 == wkb_len)
    {
        rterror("rasterize_hash: wkb_len cannot be 0.");
    }

    if (!global_extent.IsValid())
    {
        rterror("rasterize_hash: global_extent is invalid.");
    }

    /* OGR spatial reference */
    if (nullptr != srs && strlen(srs))
    {
        src_sr = OSRNewSpatialReference(nullptr);
        if (OSRSetFromUserInput(src_sr, srs) != OGRERR_NONE)
        {
            rterror("rasterize_hash: Could not create OSR spatial reference using the provided srs: %s", srs);
            return;
        }
    }

    /* convert WKB to OGR Geometry */
    ogrerr = OGR_G_CreateFromWkb((unsigned char *)wkb, src_sr, &src_geom, wkb_len);
    delete wkb;
    wkb = nullptr;
    
    if (ogrerr != OGRERR_NONE)
    {
        rterror("rasterize_hash: Could not create OGR Geometry from WKB");
        /* OGRCleanupAll(); */
        return;
    }

    /* OGR Geometry is empty */
    if (OGR_G_IsEmpty(src_geom))
    {
        rtinfo("Geometry provided is empty. Returning empty raster");

        OGR_G_DestroyGeometry(src_geom);
        /* OGRCleanupAll(); */

        return;
    }

    _dim[0] = (std::pow(2, level));
    _dim[1] = (std::pow(2, level));

    /* user-defined scale */
    _scale[0] = (global_extent.right - global_extent.left) / _dim[0];
    _scale[1] = (global_extent.top - global_extent.bottom) / _dim[1];

    /* 获取 geometry 边界 */
    OGR_G_GetEnvelope(src_geom, &src_env);
    rt_util_from_ogr_envelope(src_env, &extent);

    /* 计算 geometry 的包围网格 */
    subextent.MinX = floor((extent.MinX - global_extent.left) / _scale[0]) * _scale[0] + global_extent.left;
    subextent.MinY = floor((extent.MinY - global_extent.bottom) / _scale[1]) * _scale[1] + global_extent.bottom;
    subextent.MaxX = ceil((extent.MaxX - global_extent.left) / _scale[0]) * _scale[0] + global_extent.left;
    subextent.MaxY = ceil((extent.MaxY - global_extent.bottom) / _scale[1]) * _scale[1] + global_extent.bottom;

    /* 先做减法，再做除法，尽量避免 double 除法产生的漂移问题 */
    data.offsetx = floor(subextent.MinX / _scale[0] - global_extent.left / _scale[0] + 0.1);
    data.offsety = floor(subextent.MinY / _scale[1] - global_extent.bottom / _scale[1] + 0.1);

    RASTER_DEBUGF(3, "scale (x, y) = %f, %f", _scale[0], -1 * _scale[1]);
    RASTER_DEBUGF(3, "dim (x, y) = %d, %d", _dim[0], _dim[1]);

    /* load GDAL mem */
    if (!rt_util_gdal_driver_registered("MEM"))
    {
        RASTER_DEBUG(4, "Registering MEM driver");
        GDALRegister_MEM();
        unload_drv = 1;
    }
    _drv = GDALGetDriverByName("MEM");
    if (nullptr == _drv)
    {
        rterror("rasterize_hash: Could not load the MEM GDAL driver");

        OGR_G_DestroyGeometry(src_geom);
        /* OGRCleanupAll(); */

        return;
    }

    /* unload driver from GDAL driver manager */
    if (unload_drv)
    {
        RASTER_DEBUG(4, "Deregistering MEM driver");
        GDALDeregisterDriver(_drv);
    }

    /* 重新计算 width 和 height */
    _dim[0] = ceil((subextent.MaxX - subextent.MinX) / _scale[0]);
    _dim[1] = ceil((subextent.MaxY - subextent.MinY) / _scale[1]);

    data.width = _dim[0];
    data.height = _dim[1];

    /**
     * @brief 设置变形矩阵
     * 如果 _gt[3] 为 miny 则 _gt[5] = _scale[1] raster 数据与图形方向相反
     * 如果 _gt[3] 为 maxy 则 _gt[5] = - _scale[1], 数据与图形方向相同
     * 
     */
    _gt[0] = subextent.MinX;
    _gt[1] = _scale[0];
    _gt[2] = 0;
    _gt[3] = subextent.MinY;
    _gt[4] = 0;
    _gt[5] = _scale[1];

    _ds = GDALCreate(_drv, "", _dim[0], _dim[1], 0, GDT_Byte, nullptr);
    if (nullptr == _ds)
    {
        rterror("rasterize_hash: Could not create a GDALDataset to rasterize the geometry into");

        OGR_G_DestroyGeometry(src_geom);
        /* OGRCleanupAll(); */
        if (unload_drv)
            GDALDestroyDriver(_drv);

        return;
    }

    /* set geotransform */
    cplerr = GDALSetGeoTransform(_ds, _gt);
    if (cplerr != CE_None)
    {
        rterror("rasterize_hash: Could not set geotransform on GDALDataset");

        OGR_G_DestroyGeometry(src_geom);

        /* OGRCleanupAll(); */

        GDALClose(_ds);
        if (unload_drv)
            GDALDestroyDriver(_drv);

        return;
    }

    /* set SRS */
    if (nullptr != src_sr)
    {
        char *_srs = nullptr;
        OSRExportToWkt(src_sr, &_srs);

        cplerr = GDALSetProjection(_ds, _srs);
        CPLFree(_srs);
        if (cplerr != CE_None)
        {
            rterror("rasterize_hash: Could not set projection on GDALDataset");

            OGR_G_DestroyGeometry(src_geom);

            /* OGRCleanupAll(); */

            GDALClose(_ds);
            if (unload_drv)
                GDALDestroyDriver(_drv);

            return;
        }
    }

    /* set bands */
    err = 0;

    do
    {
        /* add band */
        cplerr = GDALAddBand(_ds, GDT_Byte, nullptr);
        if (cplerr != CE_None)
        {
            rterror("rasterize_hash: Could not add band to GDALDataset");
            err = 1;
            break;
        }

        _band = GDALGetRasterBand(_ds, i + 1);
        if (nullptr == _band)
        {
            rterror("rasterize_hash: Could not get band %d from GDALDataset", i + 1);
            err = 1;
            break;
        }

        /* nodata value */
        if (hasnodata)
        {
            RASTER_DEBUGF(4, "Setting NODATA value of band %d to %f", i, nodata[i]);
            cplerr = GDALSetRasterNoDataValue(_band, nodata);
            if (cplerr != CE_None)
            {
                rterror("rasterize_hash: Could not set nodata value");
                err = 1;
                break;
            }
            RASTER_DEBUGF(4, "NODATA value set to %f", GDALGetRasterNoDataValue(_band, nullptr));
        }

        /* initial value */
        cplerr = GDALFillRaster(_band, init, 0);
        if (cplerr != CE_None)
        {
            rterror("rasterize_hash: Could not set initial value");
            err = 1;
            break;
        }
    } while (0);

    if (err)
    {

        OGR_G_DestroyGeometry(src_geom);

        /* OGRCleanupAll(); */

        GDALClose(_ds);
        if (unload_drv)
            GDALDestroyDriver(_drv);

        return;
    }

    /* burn geometry */
    cplerr = GDALRasterizeGeometries(
        _ds,
        numbands, &bandlist,
        1, &src_geom,
        nullptr, nullptr,
        &value,
        options,
        nullptr, nullptr);
    if (cplerr != CE_None)
    {
        rterror("rasterize_hash: Could not rasterize geometry");

        OGR_G_DestroyGeometry(src_geom);

        /* OGRCleanupAll(); */

        GDALClose(_ds);
        if (unload_drv)
            GDALDestroyDriver(_drv);

        return;
    }

    /* convert gdal dataset to raster */
    GDALFlushCache(_ds);
    RASTER_DEBUG(3, "Converting GDAL dataset to raster");

    RawFromGdalDataset(_ds, data);
    //rast = rt_raster_from_gdal_dataset(_ds);

    OGR_G_DestroyGeometry(src_geom);
    /* OGRCleanupAll(); */

    GDALClose(_ds);
    if (unload_drv)
        GDALDestroyDriver(_drv);
}

/**
 * @brief 从 GDALDatasetH 中获取原始的栅格化数据
 * 
 * @param ds GDALDataset handler
 * @param data 栅格化后的原始数据
 */
void RawFromGdalDataset(GDALDatasetH ds, RawHashData &data)
{

    double gt[6] = {0};
    CPLErr cplerr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t numBands = 0;
    int i = 0;
    char *authname = nullptr;
    char *authcode = nullptr;

    GDALRasterBandH gdband = nullptr;
    GDALDataType gdpixtype = GDT_Unknown;
    rt_band band;
    int32_t idx;
    rt_pixtype pt = PT_END;
    uint32_t ptlen = 0;
    int hasnodata = 0;

    int x;
    int y;

    int nXBlocks, nYBlocks;
    int nXBlockSize, nYBlockSize;
    int iXBlock, iYBlock;
    int nXValid, nYValid;
    int iY;

    uint8_t *values = nullptr;
    uint32_t valueslen = 0;
    uint8_t *ptr = nullptr;

    if (nullptr == ds)
    {
        rterror("RawFromGdalDataset: ds cannot be nullptr.");
    }

    /* raster size */
    width = GDALGetRasterXSize(ds);
    height = GDALGetRasterYSize(ds);
    RASTER_DEBUGF(3, "Raster dimensions (width x height): %d x %d", width, height);

    data.val.resize(width * height);

    /* get raster attributes */
    cplerr = GDALGetGeoTransform(ds, gt);
    if (GDALGetGeoTransform(ds, gt) != CE_None)
    {
        RASTER_DEBUG(4, "Using default geotransform matrix (0, 1, 0, 0, 0, -1)");
        gt[0] = 0;
        gt[1] = 1;
        gt[2] = 0;
        gt[3] = 0;
        gt[4] = 0;
        gt[5] = -1;
    }

    RASTER_DEBUGF(3, "Raster geotransform (%f, %f, %f, %f, %f, %f)",
                  gt[0], gt[1], gt[2], gt[3], gt[4], gt[5]);

    numBands = GDALGetRasterCount(ds);

#if POSTGIS_DEBUG_LEVEL > 3
    for (i = 1; i <= numBands; i++)
    {
        GDALRasterBandH _grb = nullptr;
        double _min;
        double _max;
        double _mean;
        double _stddev;

        _grb = GDALGetRasterBand(ds, i);
        GDALComputeRasterStatistics(_grb, FALSE, &_min, &_max, &_mean, &_stddev, nullptr, nullptr);
        RASTER_DEBUGF(4, "GDAL Band %d stats: %f, %f, %f, %f", i, _min, _max, _mean, _stddev);
    }
#endif

    /* copy bands */
    for (i = 1; i <= numBands; i++)
    {
        RASTER_DEBUGF(3, "Processing band %d of %d", i, numBands);
        gdband = nullptr;
        gdband = GDALGetRasterBand(ds, i);
        if (nullptr == gdband)
        {
            rterror("RawFromGdalDataset: Could not get GDAL band");
            return;
        }
        RASTER_DEBUGF(4, "gdband @ %p", gdband);

        /* pixtype */
        gdpixtype = GDALGetRasterDataType(gdband);
        RASTER_DEBUGF(4, "gdpixtype, size = %s, %d", GDALGetDataTypeName(gdpixtype), GDALGetDataTypeSize(gdpixtype) / 8);

        ptlen = GDALGetDataTypeSize(gdpixtype) / 8;

        /* size: width and height */
        width = GDALGetRasterBandXSize(gdband);
        height = GDALGetRasterBandYSize(gdband);
        RASTER_DEBUGF(3, "GDAL band dimensions (width x height): %d x %d", width, height);

        /* this makes use of GDAL's "natural" blocks */
        GDALGetBlockSize(gdband, &nXBlockSize, &nYBlockSize);
        nXBlocks = (width + nXBlockSize - 1) / nXBlockSize;
        nYBlocks = (height + nYBlockSize - 1) / nYBlockSize;
        RASTER_DEBUGF(4, "(nXBlockSize, nYBlockSize) = (%d, %d)", nXBlockSize, nYBlockSize);
        RASTER_DEBUGF(4, "(nXBlocks, nYBlocks) = (%d, %d)", nXBlocks, nYBlocks);

        /* allocate memory for values */
        valueslen = ptlen * nXBlockSize * nYBlockSize;

        //RASTER_DEBUGF(3, "values @ %p of length = %d", values, valueslen);

        for (iYBlock = 0; iYBlock < nYBlocks; iYBlock++)
        {
            for (iXBlock = 0; iXBlock < nXBlocks; iXBlock++)
            {
                x = iXBlock * nXBlockSize;
                y = iYBlock * nYBlockSize;
                RASTER_DEBUGF(4, "(iXBlock, iYBlock) = (%d, %d)", iXBlock, iYBlock);
                RASTER_DEBUGF(4, "(x, y) = (%d, %d)", x, y);

                //memset(values, 0, valueslen);

                /* valid block width */
                if ((iXBlock + 1) * nXBlockSize > width)
                    nXValid = width - (iXBlock * nXBlockSize);
                else
                    nXValid = nXBlockSize;

                /* valid block height */
                if ((iYBlock + 1) * nYBlockSize > height)
                    nYValid = height - (iYBlock * nYBlockSize);
                else
                    nYValid = nYBlockSize;

                RASTER_DEBUGF(4, "(nXValid, nYValid) = (%d, %d)", nXValid, nYValid);

                cplerr = GDALRasterIO(
                    gdband, GF_Read,
                    x, y,
                    nXValid, nYValid,
                    &data.val[0] + valueslen * iYBlock, nXValid, nYValid,
                    gdpixtype,
                    0, 0);
                if (cplerr != CE_None)
                {
                    rterror("RawFromGdalDataset: Could not get data from GDAL raster");
                    //rtdealloc(values);
                    return;
                }
            }
        }
    }
}

/**
 * @brief 根据坐标快速计算 geohash
 * 
 * @param x 横坐标
 * @param y 纵坐标
 * @param level 划分等级
 * @return uint64_t 
 */
uint64_t HashCode(unsigned int x, unsigned int y, unsigned int level)
{
    if ((x > (int)std::pow(2, level)) || y > (int)std::pow(2, level))
    {
        // 划分等级和横纵坐标不匹配
        return 0;
    }

    std::bitset<64> code;
    std::bitset<56> _x(x);
    std::bitset<56> _y(y);
    std::bitset<6> _level(level);

    for (int i = 0, j = level - 1; i < 56 && j >= 0;)
    {
        code[61 - i] = _x[j];
        code[61 - i - 1] = _y[j];
        i += 2;
        j--;
    }

    for (int i = 5; i >= 0; i--)
    {
        code[i] = _level[i];
    }

    return code.to_ulong();
}