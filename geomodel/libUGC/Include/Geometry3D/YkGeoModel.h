/*
 *
 * YkGeoModel.h
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

#ifndef AFX_YKGEOMODEL_H__3AF5A4A2_D49A_47F2_8D63_2F32C4EEF181__INCLUDED_
#define AFX_YKGEOMODEL_H__3AF5A4A2_D49A_47F2_8D63_2F32C4EEF181__INCLUDED_

#pragma once

#include "Geometry3D/YkGeometry.h"
#include "Base3D/YkModelNode.h"

namespace Yk
{
class GEOMETRY3D_API YkGeoModel : public YkGeometry
{
public:
	//! \brief 构造函数
	YkGeoModel();
	YkGeoModel(YkModelNode* pNode, YkBool bDelegate=FALSE);

	//! \brief 析构函数
	virtual ~YkGeoModel();

	//! \brief 得到几何对象的类型，本接口返回YkGeometry::GeoModel
	virtual YkGeometry::Type GetType() const;

public:
	//! \brief 清空地理数据
	virtual void Clear();

	//! \brief 得到维度, 点为0维, 线/曲线等为1维, 面/曲面等为2维, GeoCompound/GeoText等为-1
	virtual YkInt GetDimension() const;

	//! \brief 得到内点（在Geometry内部，靠近中央的点）
	//! \remarks 为点数最多的子对象 的中间点(中间个数的点, 而不是中间距离的点)
	virtual YkPoint2D GetInnerPoint() const;

	//! \brief 得到内点，本函数返回模型的位置Position
	virtual YkPoint3D GetInnerPoint3D() const;

	//! \brief 得到内点，本函数返回模型的位置Position，点线面物体返回内点
	virtual YkPoint3D GetPosition() const;

	//! \brief 几何对象是否有效
	//! \return 有效返回true,无效返回false
	virtual YkBool IsValid() const;

	//! \brief 根据geometry类型，再调用各自的make方法去实现。
	//! \param pGeometry [in]。
	//! \return 。
	//! \remarks 。
	//! \attention 构造成功返回true。
	virtual YkBool MakeWithGeometry(const YkGeometry* pGeometry);	

	//! \brief 获取数据存储时的大小
	//! \return 返回存储大小
	//! \remarks 暂未考虑编码情况，与SaveGeoData()一起维护
	//! \attention 字符串的长度要+4
	virtual YkInt GetDataSize() const;
public:

	//! \brief 存储到Stream中
	virtual YkBool SaveGeoData(YkStream& stream, 
		YkDataCodec::CodecType eCodecType,YkBool bSDBPlus = FALSE) const;

	//! \brief 从Stream中生成Geometry
	virtual YkBool LoadGeoData( YkStream& stream , 
		YkDataCodec::CodecType eCodecType = YkDataCodec::encNONE,YkBool bSDBPlus = FALSE );

	//! \brief 获取GeoModel的版本号
	YkInt GetVersion(){ return m_nVersion;};

	//! \brief 设置三维几何对象的包围盒。
	//! \param box [in] 新包围盒的值。
	virtual void SetBoundingBox(const YkBoundingBox& box);

	//! \brief 得到三维几何对象的包围盒。
	//! \return 返回包围盒的值。
	virtual const YkBoundingBox& GetBoundingBox()	const;

	//! \brief 得到模型体积。
	//virtual YkDouble GetVolume();

	void SetSpherePlaced(YkBool bSpherePlaced);

	YkBool GetSpherePlaced() const;

	//! \brief 按用户指定的位置进行矩阵变换。
	void ReMultiplyMatrix(const YkPoint3D& pntUserDefined, const YkMatrix4d& mTrans);

	//! \brief 修改:模型的放置点位置(m_pntPos)-->模型的实际中心点
	//! \brief 在 SaveGeoData 中调用
	void TransformPntPosition() const;

	//! \brief 设置EM
	void SetEM(YkModelEM* pEM);

	//! \brief 根据传入的Geometry设置位置等信息，ModelNode不变
	YkBool MakeShell(const YkGeometry* pGeometry);

	//! \brief 模型合并
	static YkGeoModel* Compose(YkArray<YkGeoModel*> arrGeoModel);

protected:
	//! \brief 真正计算Bounds
	//! \return 返回得到的Bounds
	virtual YkRect2D ComputeBounds() const;	

	//!\brief 得到Boundingbox
	virtual YkBoundingBox ComputeBoundingBox() const;
public:

	void SetModelNode(YkModelNode* pGroup,YkBool bOwned = FALSE);
	YkModelNode* GetModelNode() const;

	//! \brief 根据m_pntScale m_pntRotate m_pntPos构造矩阵；
	//! \brief 该 矩阵+ModelNode=YkGeoModel
	YkMatrix4d GetLocalMatrix() const;

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

	//! \brief 从加载YkModelsElementsManager填充vector
	YkBool LoadElements(YkModelEM* pEM, YkInt nLOD = -1, 
		YkModelElement::METype mtype = YkModelElement::etUnknown);

	//! \brief 计算对象的二维Bounds使用
	YkBoundingBox GetGeoBoundingBox() const;

	//! \brief 获取对象海拔高度，缓存八叉树切分使用
	void GetAltitude( YkDouble& dbMaxZ, YkDouble& dbMinZ);

	//! \brief 设置模型文件的路径,主要用在KML中保持模型文件路径
	YkString GetFilePathName() const;

	//! \brief 设置模型文件的路径,主要用在KML中保持模型文件路径
	void SetFilePathName(YkString strFilePath);

	//! \brief 平移的基准点：球面上换算笛卡尔坐标使用
	//! \brief 模型对象平移，单位：米或弧度
	//! \brief 仅支持球面对象
	YkBool Translate(YkDouble dbX, YkDouble dbY, YkDouble dbZ, YkPoint3D pntBase);

	//! \brief pntBase [in] 旋转前统一基准点
	//! \brief dXAngle [in]	x轴旋转角度
	//! \brief dYAngle [in]	y轴旋转角度
	//! \brief dZAngle [in]	z轴旋转角度
	//! \brief rotatePoint [in] 旋转点
	//! \brief 仅支持球面对象
	YkBool Rotate(YkPoint3D rotatePoint, \
		YkDouble dXAngle, YkDouble dYAngle, YkDouble dZAngle,YkPoint3D pntBase);

private:
	//! \brief 处理Patch
	void TranslatePosition(YkModelPagedPatch* pPatch, YkMatrix4d& matSrc, YkMatrix4d& matLocalBase);

private:
	//! \GeoModel版本号
	YkInt m_nVersion;
	YkModelNode* m_pModelNode;
	YkBool m_bNodeOwned;

	//! \brief 是否放置在球面上，影响GeoModelPro计算Bounds
	YkBool m_bSpherePlaced;

	YkBool m_bPJInsertPntOnly;

	//! \brief 记录模型文件路径，主要用于KML文件保存
	YkString m_strModelFilePath;

	//! \brief 
	YkModelEM *m_pEM;
};


}

#endif
