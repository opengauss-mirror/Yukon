/*
 *
 * rasterize_grid.cpp
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
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

#include "rasterize_grid.h"

/**
 * Return geosotgrids of the provided geometry
 * @param wkb : WKB representation of the geometry to convert
 * @param wkb_len : length of the WKB representation of the geometry
 * @param srs : the geometry's coordinate system in OGC WKT
 * @param level ：geosotgrid精度等级
 * @param geom_type ：geometry类型
 * @param results : 网格坐标点结果集
 */
void GridRasterize(const uint8_t *wkb, uint32_t wkb_len, const char *srs,
	      			uint32_t level, uint32_t geom_type, vector<GridPos> &results, GBOX *src_box)
{
	/* raster 相关参数 */
	OGRSpatialReferenceH *src_sr = nullptr;
	char **options = nullptr;

	/* 一定要指定最后一个指针为 nullptr，否则会造成段错误 */
	options = (char **)palloc(sizeof(char *) * 1);
	options[0] = (char *)palloc(sizeof(char *) * (strlen("ALL_TOUCHED=TRUE") + 1));
	options[0] = (char *)"ALL_TOUCHED=TRUE";
	options = (char **)repalloc(options, sizeof(char *) * 2);
	options[1] = nullptr;

	OGRErr ogrerr;
	OGRGeometryH src_geom;
	OGREnvelope src_env;
	GDALDriverH _drv = nullptr;
	int unload_drv = 0;
	/* 栅格化 handler */
	GDALDatasetH _ds = nullptr;

    if (nullptr == wkb)
        rterror("rasterize_hash: wkb cannot be nullptr.");

    if (0 == wkb_len)
        rterror("rasterize_hash: wkb_len cannot be 0.");

    if (nullptr != srs && strlen(srs))
    {
        src_sr = (OGRSpatialReferenceH *)OSRNewSpatialReference(nullptr);
        if (OSRSetFromUserInput(src_sr, srs) != OGRERR_NONE)
        {
            rterror("rasterize_hash: Could not create OSR spatial reference using the provided srs: %s", srs);
            return;
        }
    }

    /* convert WKB to OGR Geometry */
    ogrerr = OGR_G_CreateFromWkb((unsigned char *)wkb, src_sr, &src_geom, wkb_len);

    if (ogrerr != OGRERR_NONE)
    {
        rterror("rasterize_hash: Could not create OGR Geometry from WKB");
        return;
    }

    if (OGR_G_IsEmpty(src_geom))
    {
        rtinfo("Geometry provided is empty. Returning empty raster");
        OGR_G_DestroyGeometry(src_geom);
        return;
    }

    /* 获取 geometry 边界 */
    OGR_G_GetEnvelope(src_geom, &src_env);
    if (src_env.MinX < -180 || src_env.MaxX > 180)
        lwpgerror("Longitude must be in the range of (-180,180)");

    if (src_env.MinY < -88 || src_env.MaxY > 88)
        lwpgerror("Latitude must be in the range of (-88,88)");

    //根据等级划分出来的栅格数不是线性递增
	uint64_t num = 0;
	if (12 < level && level < 19)
		num = 15 * pow(2, level - 4);
	else if (18 < level)
		num = 900 * pow(2, level - 10);
	else
		num = pow(2, level);

	//判断是否会有超出1G数量的格子数
	if ((src_env.MaxX - src_env.MinX) * (src_env.MaxY - src_env.MinY) * pow(num, 2) > 0x08000000L * 512 * 512)
		lwpgerror("The current geometry does not support this precision, please reduce the precision level");

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
		return;
	}

	/* unload driver from GDAL driver manager */
	if (unload_drv)
	{
		RASTER_DEBUG(4, "Deregistering MEM driver");
		GDALDeregisterDriver(_drv);
	}

	vector<GridPos> record;
	vector<GridPos> degree_record;
	vector<GridPos> minute_record;

	rt_envelope subextent;
	/* 栅格化后的 width 和 height */
	uint64_t grid_num[2] = {0};
	/* 栅格化后的像素大小 */
	double pixel_size = 0.;
	//等级在10-12 不规则栅格化，先定位到度级，取规则划分的位置
	if(10 <= level && level <= 12)
		grid_num[0] = grid_num[1] = 512;
	//等级在16-18 不规则栅格化，先定位到分级，取规则划分的位置
	else if(16 <= level && level <= 18)
		grid_num[0] = grid_num[1] = 30720;
	else
		grid_num[0] = grid_num[1] = num;

	pixel_size = 512.0 / grid_num[0];

	if (src_box)
	{
		src_env.MinX = src_box->xmin;
		src_env.MaxX = src_box->xmax;
		src_env.MinY = src_box->ymin;
		src_env.MaxY = src_box->ymax;

		subextent.MinX = src_box->xmin;
		subextent.MaxX = src_box->xmax;
		subextent.MinY = src_box->ymin;
		subextent.MaxY = src_box->ymax;
	}
	else
	{
		/* 计算 geometry 的包围网格 */
		subextent.MinX = floor(src_env.MinX / pixel_size) * pixel_size;
		if (subextent.MinX > src_env.MinX)
			subextent.MinX -= pixel_size;
		subextent.MinY = floor(src_env.MinY / pixel_size) * pixel_size;
		if (subextent.MinY > src_env.MinY)
			subextent.MinY -= pixel_size;
		subextent.MaxX = ceil(src_env.MaxX / pixel_size) * pixel_size;
		subextent.MaxY = ceil(src_env.MaxY / pixel_size) * pixel_size;
	}

	GridPos pos;
	pos.x = subextent.MinX;
	pos.y = subextent.MinY;
	record.push_back(pos);

	//如果是规则划分，则传入的为当前等级下的整个栅格左下角的坐标，否则为上一层级下的左下角坐标点
	if (10 <= level && level <= 12)
	{
		//如果精度等级在10-12，为不规则栅格，其上层为规则栅格，先计算度级的网格并存储起来
		RegularRasterize(_drv, src_geom, src_env, subextent, options, 9, UNIFIED_, geom_type, record, degree_record);
		//计算每个度级网格里的不规则栅格
		int minutes_level = level - 9;
		RegularRasterize(_drv, src_geom, src_env, subextent, options, minutes_level, MINUTE_, geom_type, degree_record, results);
	}
	else if (16 <= level && level <= 18)
	{
		//如果精度等级在16-18，为不规则栅格，其上层为规则栅格，先计算分级的网格并存储起来
		RegularRasterize(_drv, src_geom, src_env, subextent, options, 15, UNIFIED_, geom_type, record, minute_record);
		//计算每个分级网格里的不规则栅格
		int seconds_level = level - 15;
		RegularRasterize(_drv, src_geom, src_env, subextent, options, seconds_level, SECOND_, geom_type, minute_record, results);
	}
	else
		RegularRasterize(_drv, src_geom, src_env, subextent, options, level, UNIFIED_, geom_type, record, results);

    OGR_G_DestroyGeometry(src_geom);
    GDALClose(_ds);

    if (unload_drv)
        GDALDestroyDriver(_drv);
}

/**
 * @brief 从 GDALDatasetH 中获取原始的栅格化数据
 *
 * @param ds GDALDataset handler
 * @param grid_data 栅格化后的原始数据
 */
void RawFromGdalDataSet(GDALDatasetH ds, RawGridData &grid_data)
{
    double gt[6] = {0};
    CPLErr cplerr;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t numBands = 0;
    uint32_t i = 0;

    GDALRasterBandH gdband = nullptr;
    GDALDataType gdpixtype = GDT_Unknown;
    uint32_t ptlen = 0;

    int x;
    int y;

    uint nXBlocks, nYBlocks;
    int nXBlockSize, nYBlockSize;
    uint iXBlock, iYBlock;
    int nXValid, nYValid;

    uint32_t valueslen = 0;

    if (nullptr == ds)
        rterror("RawFromGdalDataSet: ds cannot be nullptr.");

    /* raster size */
    width = GDALGetRasterXSize(ds);
    height = GDALGetRasterYSize(ds);
    RASTER_DEBUGF(3, "Raster dimensions (width x height): %d x %d", width, height);

    grid_data.val.resize(width * height);

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

    RASTER_DEBUGF(3, "Raster geotransform (%f, %f, %f, %f, %f, %f)", gt[0], gt[1], gt[2], gt[3], gt[4], gt[5]);

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
            rterror("RawFromGdalDataSet: Could not get GDAL band");
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

        // RASTER_DEBUGF(3, "values @ %p of length = %d", values, valueslen);

        for (iYBlock = 0; iYBlock < nYBlocks; iYBlock++)
        {
            for (iXBlock = 0; iXBlock < nXBlocks; iXBlock++)
            {
                x = iXBlock * nXBlockSize;
                y = iYBlock * nYBlockSize;
                RASTER_DEBUGF(4, "(iXBlock, iYBlock) = (%d, %d)", iXBlock, iYBlock);
                RASTER_DEBUGF(4, "(x, y) = (%d, %d)", x, y);

                // memset(values, 0, valueslen);

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
                    gdband, GF_Read, x, y, nXValid, nYValid,
                    &grid_data.val[0] + valueslen * iYBlock,
                    nXValid, nYValid, gdpixtype, 0, 0);

                if (cplerr != CE_None)
                {
                    rterror("RawFromGdalDataSet: Could not get data from GDAL raster");
                    // rtdealloc(values);
                    return;
                }
            }
        }
    }
}

/**
 * Return raster results of geometry
 * @param _drv : gdal 驱动
 * @param src_geom : 源geometry
 * @param src_env : 源geometry的包围框
 * @param options ：操作方式
 * @param level ：栅格化等级
 * @param level_flag : 等级标志
 * @param geom_type : geometry类型
 * @param grid_record : 要栅格化的网格坐标点
 * @param grid_back : 栅格化后的网格坐标点
 */
void RegularRasterize(GDALDriverH _drv, OGRGeometryH src_geom, OGREnvelope &src_env, rt_envelope subextent,
                      char **options, const int level, LevelFlag level_flag, uint32_t geom_type,
                      vector<GridPos> &grid_record, vector<GridPos> &grid_back)
{
	uint64_t grid_num[2] = {0};		//单边网格数目
	double pixel_size = 0.;		//像素大小 
	double _gt[6] = {0};			//栅格矩阵参数	
	CPLErr cplerr;
	GridPos pos;
	double init = 0;
	double nodata = 0;
	GDALDatasetH _ds = nullptr;
	GDALRasterBandH _band = nullptr;
	RawGridData grid_data;
	vector<GridPos> pos_results;

    grid_num[0] = grid_num[1] = pow(2, level);

	switch (level_flag)
	{
	case UNIFIED_:
		// 此处是规则的栅格化，实际的格子数根据等级变化会有不同	
		pixel_size = GetPixSize(level);

		// 要生成的矩阵长宽
		grid_num[0] = ceil((subextent.MaxX - subextent.MinX) / pixel_size) + 1;
		grid_num[1] = ceil((subextent.MaxY - subextent.MinY) / pixel_size) + 1;
		break;
	case MINUTE_:
		// 分级格子像素
		pixel_size = pow(2, 6 - level) / 60.0;
		break;
	case SECOND_:
		// 秒级格子像素
		pixel_size = pow(2, 6 - level) / 3600.0;
		break;
	}

	grid_data.width = grid_num[0];
	grid_data.height = grid_num[1];

	// 循环取出每一个上层网格的位置
	for (GridPos grid : grid_record)
	{
		// 设置变形矩阵
		// 如果 _gt[3] 为 miny 则 _gt[5] = pixel_size[1] raster数据与图形方向相反
		// 如果 _gt[5] 为 maxy 则 _gt[5] = - pixel_size[1], 数据与图形方向相同
		_gt[0] = grid.x;
		_gt[1] = pixel_size;
		_gt[2] = 0;
		_gt[3] = grid.y;
		_gt[4] = 0;
		_gt[5] = pixel_size;

        //创建 长*宽 大小的矩阵
        _ds = GDALCreate(_drv, "", grid_num[0], grid_num[1], 0, GDT_Byte, nullptr);
        if (nullptr == _ds)
        {
            rterror("rasterize_hash: Could not create a GDALDataset to rasterize the geometry into");
            OGR_G_DestroyGeometry(src_geom);
            GDALDestroyDriver(_drv);
            return;
        }

        //根据矩阵大小和每个方格的坐标及像素，进行仿射
        cplerr = GDALSetGeoTransform(_ds, _gt);
        if (cplerr != CE_None)
        {
            rterror("rasterize_hash: Could not set geotransform on GDALDataset");
            OGR_G_DestroyGeometry(src_geom);
            GDALClose(_ds);
            GDALDestroyDriver(_drv);

            return;
        }

        /* set bands */
        int err = 0;

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

            int i = 0;
            _band = GDALGetRasterBand(_ds, i + 1);
            if (nullptr == _band)
            {
                rterror("rasterize_hash: Could not get band %d from GDALDataset", i + 1);
                err = 1;
                break;
            }

            RASTER_DEBUGF(4, "Setting NODATA value of band %d to %f", i, nodata[i]);
            cplerr = GDALSetRasterNoDataValue(_band, nodata);
            if (cplerr != CE_None)
            {
                rterror("rasterize_hash: Could not set nodata value");
                err = 1;
                break;
            }
            RASTER_DEBUGF(4, "NODATA value set to %f", GDALGetRasterNoDataValue(_band, nullptr));

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
            GDALClose(_ds);
            GDALDestroyDriver(_drv);
            return;
        }

        int bandlist = 1;
        uint32_t numbands = 1;
        double value = 1;
        /* burn geometry */
        cplerr = GDALRasterizeGeometries(
            _ds,
            numbands, &bandlist,
            1, &src_geom,
            nullptr, nullptr,
            &value, options,
            nullptr, nullptr);

        if (cplerr != CE_None)
        {
            rterror("rasterize_hash: Could not rasterize geometry");
            OGR_G_DestroyGeometry(src_geom);
            GDALClose(_ds);
            GDALDestroyDriver(_drv);

            return;
        }

        /* convert gdal dataset to raster */
        GDALFlushCache(_ds);
        RASTER_DEBUG(3, "Converting GDAL dataset to raster");

        //循环获取到了每一个方格内的格网数据
        RawFromGdalDataSet(_ds, grid_data);
        GDALClose(_ds);

		//读取每一个方格内的小格网坐标信息
		GridPos pos;
		for (int i = 0; i < grid_data.height; i++)
		{
			for (int j = 0; j < grid_data.width; j++)
			{
				if (grid_data.val[i * grid_data.width + j] == 1)
				{
					pos.x = grid.x + pixel_size * j;
					pos.y = grid.y + pixel_size * i;
					if (POINTTYPE == geom_type || LINETYPE == geom_type || MULTIPOINTTYPE == geom_type || MULTILINETYPE == geom_type)
					{
						if (pos.x <= src_env.MaxX && pos.y <= src_env.MaxY)
							grid_back.push_back(pos);
					}
					else
					{
						if (pos.x < src_env.MaxX && pos.y < src_env.MaxY && (pos.x + pixel_size) > src_env.MinX && (pos.y + pixel_size) > src_env.MinY)
							grid_back.push_back(pos);
					}
				}
			}
		}
		grid_data.val.clear();
	}
}
