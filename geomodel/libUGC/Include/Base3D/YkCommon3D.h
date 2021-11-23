/*
 *
 * YkCommon3D.h
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

#ifndef AFX_YKCOMMON3D_H__6F193A74_24E8_4929_BCC6_81DD499A7425__INCLUDED_
#define AFX_YKCOMMON3D_H__6F193A74_24E8_4929_BCC6_81DD499A7425__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"

namespace Yk {
//! \brief 地球半径。
#undef GLOBAL_RADIUS
#define GLOBAL_RADIUS  6378137.0

//! \brief 多重纹理最大个数。
#define SMSCN_MAX_TEXTURE_COORD_SETS 8

//! \brief 三维场景最大灯光数目。
#define MAX_LIGHTS                   8        // 定义光源最多个数

//! \brief Filtering options for textures / mipmaps. 
enum FilterOptions
{
    //! \brief No filtering, used for FILT_MIP to turn off mipmapping
    FO_NONE,
    //! \brief Use the closest pixel
    FO_POINT,
    //! \brief Average of a 2x2 pixel area, denotes bilinear for MIN and MAG, trilinear for MIP
    FO_LINEAR,
	/// Equal to: min=FO_LINEAR, mag=FO_LINEAR, mip=FO_LINEAR
	FO_TRILINEAR,
    //! \brief Similar to FO_LINEAR, but compensates for the angle of the texture plane
    FO_ANISOTROPIC
};

//! \brief
enum Yk3DPixelFormat
{
	//! \brief 未知像素格式.
	PF_UNKNOWN = 0,
	//! \brief 8位像素，用于亮度.
	PF_L8 = 1,
	PF_BYTE_L = PF_L8,
	//! \brief 16位像素，用于亮度.
	PF_L16 = 2,
	PF_SHORT_L = PF_L16,
	//! \brief 8位像素，用于alpha值.
	PF_A8 = 3,
	PF_BYTE_A = PF_A8,
	//! \brief 8位像素, 4位alpha值, 4位亮度.
	PF_A4L4 = 4,
	//! \brief 两字节像素, 一个用于alpha值，一个用于亮度
	//! \brief 8位像素, 4位亮度, 4位alpha.
	PF_L4A4 = 35,
	//! \brief 16位像素, 每4位用于BGRA.
	PF_B4G4R4A4 = 36,
	//! \brief 24位像素, 每8位用于BRG.
	PF_B8R8G8 = 37,
	//! \brief 32位像素, 每10位用于BGR, 2位用于alpha.
	PF_B10G10R10A2 = 38,
	PF_BYTE_LA = 5,
	//! \brief 16位像素格式, R,G,B为5，6，5.
	PF_R5G6B5 = 6,
	//! \brief 16位像素格式, B,G,R为5，6，5.
	PF_B5G6R5 = 7,
	//! \brief 8位像素, B,G,R为2，3，3.
	PF_R3G3B2 = 31,
	//! \brief 16位像素, alpha，R,G,B为4,4,4,4.
	PF_A4R4G4B4 = 8,
	//! \brief 16位像素, alpha，R,G,B为1,5,5,5.
	PF_A1R5G5B5 = 9,
	//! \brief 24位像素, R,G,B为8,8,8.
	PF_R8G8B8 = 10,
	//! \brief 24位像素, B,G,R为8,8,8.
	PF_B8G8R8 = 11,
	//! \brief 32位像素, alpha,R,G,B为8,8,8,8.
	PF_A8R8G8B8 = 12,
	//! \brief 32位像素, B, G, R,alpha为8,8,8,8.
	PF_A8B8G8R8 = 13,
	//! \brief 32位像素, B, G, R,alpha为8,8,8,8.
	PF_B8G8R8A8 = 14,
	//! \brief 32位像素, R, G, B,alpha为8,8,8,8.
	PF_R8G8B8A8 = 28,
	// 128位像素, 每个元素占32位
	PF_FLOAT32_RGBA = 25,
	//! \brief 32位像素, R, G, B为8，8,8。alpha无效
	PF_X8R8G8B8 = 26,
	//! \brief 32位像素, B G, R为8，8,8。alpha无效
	PF_X8B8G8R8 = 27,
	//! \brief 3字节像素, 每个颜色占一个字节
	PF_BYTE_RGB = PF_B8G8R8,
	//! \brief 3字节像素, 每个颜色占一个字节
	PF_BYTE_BGR = PF_R8G8B8,
	//! \brief 4字节像素, 每个颜色和alpha各占一个字节
	PF_BYTE_BGRA = PF_A8R8G8B8,
	//! \brief 4字节像素, 每个颜色和alpha各占一个字节
	PF_BYTE_RGBA = PF_A8B8G8R8,
	//! \brief 32位像素 两位用于alpha，每个颜色占10位
	PF_A2R10G10B10 = 15,
	//! \brief 32位像素, 两位用于alpha，每个颜色占10位
	PF_A2B10G10R10 = 16,
	//! \brief DirectDraw Surface (DDS) DXT1 format
	PF_DXT1 = 17,
	//! \brief DirectDraw Surface (DDS) DXT2 format
	PF_DXT2 = 18,
	//! \brief DirectDraw Surface (DDS) DXT3 format
	PF_DXT3 = 19,
	//! \brief DirectDraw Surface (DDS) DXT4 format
	PF_DXT4 = 20,
	//! \brief DirectDraw Surface (DDS)) DXT5 format
	PF_DXT5 = 21,
	// 16位像素, 用于R
	PF_FLOAT16_R = 32,
	// 48位像素, 每16位float用于RGB
	PF_FLOAT16_RGB = 22,
	// 64位像素,每16位float用于RGBA
	PF_FLOAT16_RGBA = 23,
	// 16位像素，float用于R
	PF_FLOAT32_R = 33,
	// 96位像素, 每32位float用于RGB
	PF_FLOAT32_RGB = 24,
	// 128位像素, 每32位float用于RGBA
	// 用于深度像素
	PF_DEPTH = 29,
	// 64位像素, 每16位float用于RGBA
	PF_SHORT_RGBA = 30,
	// 当前定义像素格式的数目
	PF_COUNT = 34						
};

//! \brief 比较方式
enum CompareFunction
{
    CMPF_ALWAYS_FAIL,
    CMPF_ALWAYS_PASS,
    CMPF_LESS,
    CMPF_LESS_EQUAL,
    CMPF_EQUAL,
    CMPF_NOT_EQUAL,
    CMPF_GREATER_EQUAL,
    CMPF_GREATER
};

//! \brief 渲染引擎用的的裁剪模式
enum CullingMode
{
    //! \brief Hardware never culls triangles and renders everything it receives.
    CULL_NONE = 1,
    //! \brief Hardware culls triangles whose vertices are listed clockwise in the view (default).
    CULL_CLOCKWISE = 2,
    //! \brief Hardware culls triangles whose vertices are listed anticlockwise in the view.
    CULL_ANTICLOCKWISE = 3
};

//! \brief 渲染引擎用的多边形显示模式
enum PolygonMode
{
	//! \brief Only points are rendered.
    PM_POINTS = 1,
	//! \brief Wireframe models are rendered.
    PM_WIREFRAME = 2,
	//! \brief Solid polygons are rendered.
    PM_SOLID = 3
};

//! \brief 图像绘制的反走样模式
enum SmoothHintMode
{
	//不使用抗锯齿
	SHM_NONE,
	//由具体OpenGL的实现来决定作用那种方式达到点，线的平滑效果
	SHM_DONT_CARE,
	//运行速度最快
	SHM_FASTEST,
	//显示效果最好
	SHM_NICEST
};

//! \brief 数据绘制的方式
enum OperationType
{
	/// A list of points, 1 vertex per point
	OT_POINT_LIST = 1,
	/// A list of lines, 2 vertices per line
	OT_LINE_LIST = 2,
	/// A strip of connected lines, 1 vertex per line plus 1 start vertex
	OT_LINE_STRIP = 3,
	/// A list of triangles, 3 vertices per triangle
	OT_TRIANGLE_LIST = 4,
	/// A strip of triangles, 3 vertices for the first triangle, and 1 per triangle after that 
	OT_TRIANGLE_STRIP = 5,
	/// A fan of triangles, 3 vertices for the first triangle, and 1 per triangle after that
	OT_TRIANGLE_FAN = 6,

	OT_QUAD_STRIP = 8,

	OT_QUAD_LIST = 9,

	OT_POLYGON = 10,

	OT_PICTURE = 12,
};

//! \brief 索引的类型
enum VertexIndexType
{
	IT_16BIT,
	IT_32BIT,

	//! \brief 附带属性的索引号
	IT_16BIT_2,
	IT_32BIT_2,
};

//! \brief 数据绘制的属性
enum VertexOptions
{
	VO_NORMALS = 1,                  // GL_NORMAL_ARRAY + glNormalPointer()
	VO_TEXTURE_COORDS = 2,           // GL_TEXTURE_COORD_ARRAY + glTexCoordPointer()
	VO_DIFFUSE_COLOURS = 4,          // GL_COLOR_ARRAY + glColorPointer()
	VO_SPECULAR_COLOURS = 8,         // Secondary Color
	VO_BLEND_WEIGHTS = 16,      
	VO_USE_SINGLE_COLOR = 32,        // use only one kind of color
	VO_USE_POINT_SMOOTHING = 64,     // 启动点反走样
	VO_MATERIAL = 128,               // 使用材质
	VO_TEXTURE_COLOR = 256,          // 使用材质
	VO_VERTEX_DOUBLE = 512,          // 使用Double类型的顶点
	VO_TEXTURE_COORD_Z_IS_MATRIX = 1024,  // 表示顶点属性的Z值是一个矩阵
};

enum SceneType
{
	ST_EARTH_SPHERICAL   = 0,       //球场景
	ST_EARTH_PROJECTION = 1,	//平面的投影场景
	ST_NONEARTH  = 2,           //平面的局部坐标场景
};

//! \brief LOD切换模式
enum YkRangeMode
{
	DISTANCE_FROM_EYE_POINT, // 根据到相机的距离切换
	PIXEL_SIZE_ON_SCREEN	 // 根据屏幕像素大小切换
};

}

#endif

