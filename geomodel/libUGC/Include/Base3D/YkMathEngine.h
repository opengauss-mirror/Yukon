/*
 *
 * YkMathEngine.h
 *
 * Copyright (C) 2021 SuperMap Software Co., Ltd.
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

#if !defined(AFX_YKMATHENGINE_H__90331450_3347_42D5_B0A0_41353F9437C8__INCLUDED_)
#define AFX_YKMATHENGINE_H__90331450_3347_42D5_B0A0_41353F9437C8__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Base3D/YkBoundingSphere.h"

namespace Yk {
//! \brief 三维数学函数类。
class BASE3D_API YkMathEngine  
{
public:
	//! \brief 构造函数
	YkMathEngine();
	//! \brief 析构函数
	virtual ~YkMathEngine();

	//! \brief 地理坐标转换为笛卡尔坐标系，默认地球半径为6378137 
	//! \param dLongitude 地理坐标经度[in]。
	//! \param dLatitude 地理坐标维度[in]。
	//! \param GLOBAL_RADIUS 常量6378137[in]。
	//! \return 笛卡尔坐标系的坐标。
	//! \attention 球为圆球而不是椭球。
	static YkVector3d SphericalToCartesian(YkDouble dLongitude, YkDouble dLatitude,YkDouble dRadius = GLOBAL_RADIUS);

	//! \brief 地理坐标转换为笛卡尔坐标系，默认地球半径为6378137 
	//! \param sceneType 场景类型[in]。
	//! \param dLongitude 地理坐标经度[in]。
	//! \param dLatitude 地理坐标维度[in]。
	//! \param GLOBAL_RADIUS 常量6378137[in]。
	//! \return 笛卡尔坐标系的坐标。
	//! \attention 球为圆球而不是椭球。
	static YkVector3d SphericalToCartesian(SceneType sceneType, YkDouble dLongitude, YkDouble dLatitude,YkDouble dRadius = GLOBAL_RADIUS);

	//! \brief 笛卡尔坐标转换为球面坐标（地理坐标）。
	//! \param x 笛卡尔坐标x[in]。
	//! \param y 笛卡尔坐标y[in]。
	//! \param z 笛卡尔坐标z[in]。
	//! \return 球面坐标系坐标。
	static YkVector3d CartesianToSphericalD(YkDouble x, YkDouble y, YkDouble z);

	//! \brief 笛卡尔坐标转换为球面坐标（地理坐标）。
	//! \param x 笛卡尔坐标x[in]。
	//! \param y 笛卡尔坐标y[in]。
	//! \param z 笛卡尔坐标z[in]。
	//! \return 球面坐标系坐标。
	static YkVector3d CartesianToSphericalD(YkDouble x, YkDouble y, YkDouble z, SceneType sceneType);	

	//! \brief 球面坐标（地理坐标）转换成以球心为原点的直角坐标系。
	//! \param dLongitude [in] 地理坐标经度。
	//! \param dLatitude [in] 地理坐标纬度。
	//! \param dHigh [in] 相对地面高度。
	//! \param dRadius [in] 球半径，默认GLOBAL_RADIUS。
	//! \return 以球心为原点的直角坐标系的坐标。
	static YkVector3d SphericalToGlobalCartesian(
		YkDouble dLongitude, 
		YkDouble dLatitude, 
		YkDouble dHigh, 
		YkDouble dRadius = GLOBAL_RADIUS);

	//! \brief  返回比a大的，并且是最接近a的2的次方的数。
 	static YkUint NextP2 (YkUint a);
 
	//! \brief 通过线性插值和箱过滤缩放。from gluScaleImage
	//! \param components 颜色成分数量，RGBA是4
	//! \param widthin 输入图像的宽度
	//! \param heightin 输入图像的高度
	//! \param datain 输入图像的指针
	//! \param widthout 输出图像的宽度
	//! \param heightout 输出图像的高度
	//! \param dataout 输出图像的指针
	//! \param nsizedatain datain的大小
	static void ScaleImageInternal(YkInt components, YkInt widthin, YkInt heightin,
		const YkUchar *datain,
		YkInt widthout, YkInt heightout,
		YkUchar* dataout, const YkInt nsizedatain = -1);

	//! \brief 将图像数据内容进行对齐处理，暂时只对24位图像处理
	//! \param components 颜色成分数量，RGBA是4
	//! \param width 输入图像的宽度
	//! \param height 输入图像的高度
	//! \param datain 输入图像的指针
	//! \param dataout 输出图像的指针
	static YkBool ClipImageData(YkInt components, YkInt width, YkInt height, 
		const YkUchar *datain, const YkInt ndatainsize, YkUchar *dataout);

	//! \brief 通过线性插值和箱过滤缩放。from gluScaleImage
	//! \param components 颜色成分数量，RGBA是4
	//! \param widthin 输入图像的宽度
	//! \param heightin 输入图像的高度
	//! \param datain 输入图像的指针
	//! \param dataout 输出图像的指针
	static void HalveImage(YkInt components, YkUint width, YkUint height,
		const YkUchar *datain, YkUchar *dataout);

public:
	static SceneType m_msSceneType;
};
}
#endif

