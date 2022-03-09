/*
 *
 * YkGeometry.h
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

#ifndef AFX_YKGEOMETRY_H__D3759421_CB43_43B5_B9EF_D0C18B47B838__INCLUDED_
#define AFX_YKGEOMETRY_H__D3759421_CB43_43B5_B9EF_D0C18B47B838__INCLUDED_

#pragma once

#include "Toolkit/YkDataCodec.h"
#include "Toolkit/YkTextureInfo.h"
#include "Base3D/YkBoundingBox.h"

namespace Yk {
//! \brief 几何对象基类 
class GEOMETRY3D_API YkGeometry
{
public:
	//! \brief 几何对象类型定义
	enum Type
	{
		//! 空类型
		Null				= 0, 
		//! \brief 点类型	
		GeoPoint			= 1, 
		//! \brief 复合点
		GeoMultiPoint		= 2, 
		//! \brief 线类型
		GeoLine				= 3, 
		//! \brief 面类型
		GeoRegion			= 5, 
		//! \brief 文本类型
		GeoText				= 7, 	
		//! \brief 参数化线对象
		GeoParamLine		=16,
		//! \brief 参数化面对象
		GeoParamRegion		=17,
		//! \brief 复合的参数化线对象
		GeoCompoundLine		=8, 
		//! \brief 复合的参数化面对象
		GeoCompoundRegion	=9,
		//! \brief 矩形和斜矩形统一为带旋转角的矩形
		GeoRect				=12,
		//! \brief 带旋转角的圆角矩形
		GeoRectRound		=13,	
		//! \brief 圆
		GeoCircle			=15,
		//! \brief 椭圆和斜椭圆统一为带旋转角的椭圆
		GeoEllipse			=20,	
		//! \brief 椭圆扇面
		GeoPie				=21,	
		//! \brief 弦
		GeoChord			=23,
		//! \brief 圆弧
		GeoArc				=24,	
		//! \brief 椭圆弧
		GeoEllipticArc		=25,	
		//! \brief Cardinal曲线,保证曲线一定通过控制点
		GeoCardinal			=27,	
		//! \brief 曲线
		GeoCurve			=28,	
		//! \brief 贝叶斯曲线
		GeoBSpline			=29,	
		//! \brief 含线性坐标系的线(即带有M值的GeoLine)
		GeoLineM			=35, 
		//! \brief 三维点
		GeoPoint3D			=101,	
		//! \brief 三维多点
		GeoMultiPoint3D		=102,	
		//! \brief 三维线
		GeoLine3D			=103,	
		//! \brief 三维面
		GeoRegion3D			=105,	
		//! \brief 三维文本
		GeoText3D			=107,
		//! \brief 三维图标
		GeoPlacemark        =108,

		//! \brief 三角形
		GeoTriangle			=109,	
		//! \brief 立方体
		GeoCube				=111,	
		//! \brief 测量用
		GeoTraverseZ		=133,	
		//! \brief 贝叶斯曲面
		GeoBSurface			=135,	
		//! \brief 多面体
		GeoPolyhedron		=137,	
		//! \brief 不规则三角网(TIN地形)
		GeoTINTile = 138,
		//! \brief 不规则三角面, Tin
		GeoTinBlock			=139,
		//! \brief 不规则四面体结构
		GeoTIM = 140,
		//! \brief 复合对象, 该值同时代表Geometry对象类型的上限
		GeoCompound			=1000,
		GeoUser				=1001,	
		GeoUserSVG          =1002, //sgv对象类型
		GeoPicture			=1101, 
		GeoModel			=1201,
		//! \brief 三维中依地形的图片
		GeoGroundOverlay    =1202,
		//! fengzhh三维规则几何体
		GeoSphere           =1203,
		GeoHemiSphere       =1204,
		GeoBox              =1205,
		GeoCylinder         =1206,
		GeoCone             =1207,
		GeoPyramid          =1208,
		GeoPie3D            =1209,
		GeoCircle3D         =1210,
		GeoPieCylinder      =1211,
		GeoEllipsoid        =1212,
		//! \brief 粒子
		GeoParticle			=1213,

		GeoTexture			=1214,

		GeoModelElementSkeleton		= 1215,
		GeoModelElementTexture		=1216,
		GeoModelElementMaterial3D	=1217,
		GeoModelPro					=1218,
		GeoBillboard				=1220,

		GeoPaper			=2000,
		GeoMap				=2001,
		GeoTable			=2002,
		GeoTableCell		=2003,
		GeoScale            =2005,
		GeoDirection		=2008,	
		GeoMapDecorateBorder=2009,
		GeoArtText			=2010,
		GeoLegend			=2011,
		GeoMapGrid			=2012,

		GraphicObject		= 3000,
		GraphicCompound		= 3001,

		GeoPointEPS			=4000,
		GeoLineEPS			=4001,
		GeoRegionEPS		=4002,
		GeoTextEPS			=4003,

		GeoChart            = 5000, // 图表对象类型
	};

	class YkGeoModel;
public: //! 构造和析构函数
	//! \brief 构造函数
	GEOMETRY_APIF YkGeometry();

	//! \brief 析构函数
	virtual GEOMETRY_APIF ~YkGeometry();
	GEOMETRY_APIF YkGeometry(const YkGeometry& geometry);

public:
	//! \brief 清空地理数据
	virtual GEOMETRY_APIF void Clear();

	//! \brief 得到维度, 点为0维, 线/曲线等为1维, 面/曲面等为2维, GeoCompound/GeoText等为-1
	virtual GEOMETRY_APIF YkInt GetDimension() const = 0;

	//! \brief 得到类型
	virtual GEOMETRY_APIF Type GetType() const = 0;

	//! \brief 得到内点（在Geometry内部，靠近中央的点）
	//! \return 返回内点
	virtual GEOMETRY_APIF YkPoint2D GetInnerPoint() const = 0;

	//! \brief 得到内点（可能是Geometry3D的质点）
	//! \return 返回内点
	virtual GEOMETRY_APIF YkPoint3D GetInnerPoint3D() const = 0;

	//! \brief 几何对象是否有效
	//! \return 有效返回true,无效返回false
	virtual GEOMETRY_APIF YkBool IsValid() const = 0;

	
	//! \brief 得到对象的位置，这个函数只对模型和规则几何体意义，位置为模型等物体的底面中心点
	//! \return 对象的位置。
	virtual GEOMETRY_APIF YkPoint3D GetPosition() const;

	//! \brief 设置对象的位置，这个函数只对模型和规则几何体意义，位置为模型等物体的底面中心点
	//! x,y为经纬度，z是指到球面的距离而不是到球心的距离
	//! \param value [in] 指定的位置点，含义见上。
	virtual GEOMETRY_APIF void SetPosition(const YkPoint3D& value);	
	//! \brief 平移
	//! \param dX 平移的X偏移量
	//! \param dY 平移的Y偏移量
	virtual GEOMETRY_APIF void Offset( YkDouble dX, YkDouble dY);

	//! \brief 平移
	//! \param 偏移量(X,Y方向相同)
	virtual GEOMETRY_APIF void Offset( YkDouble dOffset);

	//! \brief 平移
	//! \param 平移的偏移量(分别用YkSize2D中的x,y代表X,Y方向的偏移量)
	virtual GEOMETRY_APIF void Offset( const YkSize2D &szOffset);

	//! \brief 调整几何数据,限定在新的Bounds中
	//! \param rcNewBounds 新的,限定范围的Bounds
	//! \return 成功返回true,失败返回false
	virtual GEOMETRY_APIF YkBool Resize(const YkRect2D& rcNewBounds);

public:


	//! \brief 几何对象的存储空间
	//! \return 返回几何对象的存储空间
	virtual GEOMETRY_APIF YkInt GetDataSize() const {return 0;};

public: //! 存储到外部介质中,或者从外部介质中生成

	//! \brief 得到ID
	//! \return 返回ID
	GEOMETRY_APIF YkInt GetID() const;

	//! \brief 设置ID
	//! \param 要设置的ID
	GEOMETRY_APIF void SetID( YkInt nID );

	//! \brief 得到外接矩形
	//! \return 返回外接矩形
	const GEOMETRY_APIF YkRect2D& GetBounds() const;

	//! \brief 设置外接矩形,一般不要使用, 内部会维护Bounds的正确性
	//! \param rcBounds 要设置的Bounds
	GEOMETRY_APIF void SetBounds( const YkRect2D& rcBounds );

	//! \brief 设置对象的缩放大小，这个函数只对模型和规则几何体意义
	//! \param fScaleX [in] X方向缩放比率。
	//! \param fScaleY [in] Y方向缩放比率。
	//! \param fScaleZ [in] Z方向缩放比率。
	virtual void SetScale(YkDouble fScaleX,YkDouble fScaleY,YkDouble fScaleZ);	

	//! \brief 设置对象的缩放大小，这个函数只对模型和规则几何体意义 
	//! \param fScaleXYZ [in] 物体在三个方向上进行同样大小的缩放 
	virtual void SetScale(YkDouble fScaleXYZ);

	//! \brief 设置对象的旋转量，这个函数只对模型和规则几何体意义 
	//! \param fRotateX [in] X方向的旋转角度。
	//! \param fRotateY [in] Y方向的旋转角度。
	//! \param fRotateZ [in] Z方向的旋转角度。
	virtual void SetRotation(YkDouble fRotateX,YkDouble fRotateY,YkDouble fRotateZ);	

	//! \brief 得到对象各方向的缩放比率，这个函数只对模型和规则几何体有意义
	//! \return 对象各方向的缩放比率。
	YkPoint3D GetScale() const;	
	//! \brief 得到对象各方向的旋转角度，这个函数只对模型和规则几何体有意义
	//! \return 对象各方向的旋转角度。
	YkPoint3D GetRotation() const;

	//! \brief 设置三维几何对象的包围盒。
	//! \param box [in] 新包围盒的值。
	virtual void SetBoundingBox(const YkBoundingBox& box)	{m_BoundingBox = box;};

	//! \brief 得到三维几何对象的包围盒。
	//! \return 返回包围盒的值。
	// virtual const YkBoundingBox& GetBoundingBox()	const;
	virtual const YkBoundingBox& GetBoundingBox()	const;

protected:
	//! \brief 真正计算Bounds
	//! \return 返回得到的Bounds
	//! \remarks 转成2维的Geometry后，调用它的方法实现
	virtual YkRect2D ComputeBounds() const = 0;	

	//! \brief 真正计算BoundingBox
	//! \return 返回得到的BoundingBox
	virtual YkBoundingBox ComputeBoundingBox() const = 0;
public:
	//! \brief 拷贝Geometry基类中的成员变量, 包括ID, Style, Bounds等
	//! \param geo 提供信息的Gemetry
	GEOMETRY_APIF void CopyBaseData(const YkGeometry& geo);

	//! \brief 拷贝Geometry基类中的成员变量, 包括ID, Style, Bounds,以及Geometry3D中的成员变量,包括Style3D, BoundingBox, Point3D对象等
	//! \param geo3d [in] 提供信息的Gemetry3D
	//! \return 如果有对自身的拷贝，或者有指针为空，则返回false
	GEOMETRY_APIF YkBool CopyAllBaseData(const YkGeometry& geo);

	//! \brief 保存到存储流重
	//! \param stream 存储用的流, 调用之前应该先Open
	//! \param eCodecType 存储的编码方式
	//! \param bIgnoreStyle 是否忽略对象风格
	//! \return 成功返回true,失败返回false
	virtual GEOMETRY_APIF YkBool Save(YkStream& stream, YkDataCodec::CodecType eCodecType=YkDataCodec::encNONE 
		/*,YkBool bIgnoreStyle = TRUE*/) const;

	//! \brief 从流中装载数据
	//! \param stream 流
	//! \param iJuge新增一个参数EPS调用不含属性名字的数据
	//! \remarks Load时,由于很多信息已经存储在stream中了,所以存储编码和是否保存风格等参数就不需要了
	//! \return 成功返回true,失败返回false
	GEOMETRY_APIF YkBool Load(YkStream& stream,YkInt iJuge=0);

public:
	//! \brief 只保存几何数据部分
	//! \param stream 存储流, 调用之前应该先Open
	//! \param eCodecType 存储的编码方式
	//! \param bSDBPlus 是否存储为SDBPlus的格式
	//! \remarks 由于SDBPlus引擎和数据库引擎的存储上差别较大,所以必须直接调用该函数,
	//! 并增加变量进行分别处理
	//! \return 成功返回true,失败返回false
	virtual GEOMETRY_APIF YkBool SaveGeoData(YkStream& stream, 
		YkDataCodec::CodecType eCodecType,YkBool bSDBPlus = FALSE) const = 0;

	//! \brief 通过几何对象中保存的纹理路径初始化纹理信息
	//! \return 成功返回TRUE,失败返回FLASE
	virtual GEOMETRY_APIF YkBool InitialTextureInfo(YkArray<YkTextureInfo>& arrayTextureInfos);

	//! \brief 只转载几何数据部分
	//! \param stream 流 
	//! \param eCodecType 编码方式
	//! \attention 还需要增加 YkBool bSDBPlus = FALSE 的参数
	//! \return 成功返回true,失败返回false
	virtual GEOMETRY_APIF YkBool LoadGeoData( YkStream& stream , 
		YkDataCodec::CodecType eCodecType = YkDataCodec::encNONE,YkBool bSDBPlus = FALSE ) = 0;

	//! \brief 只转载几何数据部分
	//! \param stream 流 
	//! \param eCodecType 编码方式
	//! \param eCodecType 用户采用的Load方式的nGuge判断参数
	//! \attention 还需要增加 YkBool bSDBPlus = FALSE 的参数
	//! \return 成功返回true,失败返回false
	virtual GEOMETRY_APIF YkBool LoadGeoData( YkStream& stream , YkInt iGuge,
		YkDataCodec::CodecType eCodecType = YkDataCodec::encNONE,YkBool bSDBPlus = FALSE);

	//! \brief 是否是3D类型的几何对象。
	virtual GEOMETRY_APIF YkBool Is3D( ) const;	

public:

	//! \brief 获取对象需要捕捉的点串
	//! \param aryPnts 点串，传出去的要捕捉的点串
	//! \return 如果是true表示有需要捕捉的点串，如果返回false表示该对象没有需要捕捉的点串
	virtual GEOMETRY_APIF YkBool GetSnapPoints(YkArray<YkPoint2D>& aryPnts);

	//! \brief		设置Bounds是否脏了
	//! \param		bBoundsDirty	Bounds是否脏, 默认为设置脏
	GEOMETRY_APIF void SetBoundsDirty(YkBool bBoundsDirty=true);

	//! \brief		判断Bounds是否脏了
	//! \return		如果脏了, 返回true; 不脏返回false
	GEOMETRY_APIF YkBool IsBoundsDirty() const;

	//! \brief		设置合法状态；UDB中直接调用各Geometry的LoadGeoData，缺一个状态设置
	GEOMETRY_APIF void SetValidState(YkInt nValidState);

	//! \brief		是否已经是投影转换过
	GEOMETRY_APIF YkBool IsPJConverted();

	GEOMETRY_APIF void SetPJConverted(YkBool bPJConverted);

public:
	//! \brief 返回一个point3d来描述这个对象，对于3维点就是他本身，对于其他未定义
	virtual GEOMETRY_APIF YkPoint3D Get3DPoint();
	virtual GEOMETRY_APIF YkBool	  Set3DPoint(const YkPoint3D& pnt3d);	

	YkBool HasModifiedVertexSeq();

public:

	void SetModifyVertexSeq(YkBool bModifyVertexSeq=TRUE);
	void SetIsClip(YkBool bClip);
	YkBool IsClip();
public:
	

	//! \brief 球面坐标系的点转换成笛卡尔坐标系的点。
	//! \param pnt3D [in] 球面坐标的三维点，x，y值必须是经纬数据，而且是度为单位，z值以米为单位。
	//! \return 返回以地球球心为原点的坐标系的点坐标。
	static YkPoint3D SphericalToCartesian(const YkPoint3D& pnt3D);

	//! \brief 笛卡尔坐标系的点转换成球面坐标系的点。
	//! \param pnt3D [in] 以地球球心为原点的坐标系点坐标。
	//! \return 返回的点x,y 为度为单位的经纬度坐标，z值以米为单位。
	static YkPoint3D CartesianToSpherical(const YkPoint3D& pnt3D);

protected:
	YkBool IsInLatLong() const; 
protected:
	//! \brief 几何对象ID，默认为0
	YkInt m_nID;

	//! \brief 几何对象BOUNDs
	YkRect2D m_rcBounds;
private:
	//! \brief 几何对象内部存储的Bounds是否已经脏了（和实际数据不相同了）
	YkBool m_bBoundsDirty;

	//\brief 是否修改了顶点索引序列
	YkBool m_bModifyVertexSeq;
	//是否已经被裁减过
	YkBool m_bClip;
protected:
	YkInt m_nValidState;  // 对象是否合法标志，0表示未验证，1表是合法，-1表示不合法

	YkBool m_bPJConverted;
protected:
	//! \brief 三维几何对象的包围盒
	YkBoundingBox m_BoundingBox;

	//! \brief 三维对象的位置，对规则几何体，模型，粒子有效
	YkPoint3D m_pntPos;

	//! \brief 三维对象的缩放系数，对规则几何体，模型，粒子有效
	YkPoint3D m_pntScale;

	//! \brief 三维对象的旋转量，对规则几何体，模型，粒子有效
	YkPoint3D m_pntRotate;
};

}

#endif

