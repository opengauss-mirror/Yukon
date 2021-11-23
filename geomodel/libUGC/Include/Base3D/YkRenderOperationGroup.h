/*
 *
 * YkRenderOperationGroup.h
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

#ifndef AFX_YKRENDEROPERATIONGROUP_H__8404354C_CEE3_4325_A120_C26C63FEEE71__INCLUDED_
#define AFX_YKRENDEROPERATIONGROUP_H__8404354C_CEE3_4325_A120_C26C63FEEE71__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Base3D/YkVector3d.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkBoundingSphere.h"
#include "Base3D/YkMaterial3D.h"
#include "Base3D/YkVertexDataPackageTemplate.h"

namespace Yk {
class YkVertexDeclaration;

typedef YkVertexDataPackageTemplate<YkFloat, YkVertexDeclaration> YkVertexDataPackage;

//! \brief double精度的顶点
typedef YkVertexDataPackageTemplate<YkDouble, YkVertexDeclaration> YkVertexDataPackageExact;

//! \brief 描述VertexDataPacke顶点结构的元信息
class BASE3D_API YkVertexDeclaration
{
	template<typename T, class VertexDeclaration>
	friend class YkVertexDataPackageTemplate;
public:	
	//! \brief 重载==
	YkBool operator== (const YkVertexDeclaration& other) const;

	//! \brief 重载!=
	YkBool operator!= (const YkVertexDeclaration& other) const;

protected:
	//! \brief 构造函数
	YkVertexDeclaration();
	YkVertexDeclaration(YkVertexDataPackage* pVertexDataPackage);
	YkVertexDeclaration(const YkVertexDeclaration& other);

	//! \brief 根据YkVertexDataPackage构造元定义
	void MakeWith(YkVertexDataPackage* pVertexDataPackage);

private:
	//! \brief 顶点维数
	YkUshort m_nVertexDimension;
	//! \brief 贴图坐标的维数
	YkArray<YkUshort> m_arrTexDimensions;
	//! \brief 是否有法线坐标
	YkBool m_bHasNormal;
	//! \brief 是否有顶点颜色
	YkBool m_bHasColor;
	//! \brief 是否有副顶点颜色
	YkBool m_bHasSecondColor;
};

//! \brief 顶点索引数组
struct BASE3D_API YkIndexPackage
{
	//! \brief 索引数目
	YkUint m_nIndexesCount;
	//! \brief 索引数组
	YkUshort *m_pIndexes;
	//! 是否使用无符整型数据类型
	VertexIndexType m_enIndexType;
	//! 是否使用索引
	YkBool m_bUseIndex;
	//! \brief 使用的YkPass的名称数组
	YkArray<YkString> m_strPassName;
	//! 数据绘制的方式
	OperationType m_OperationType;

	YkIndexPackage();
	YkIndexPackage(const YkIndexPackage& other);
	YkIndexPackage& operator = (const YkIndexPackage& other);
	~YkIndexPackage();
	void SetIndexNum(YkInt nIndexNum);

	YkInt GetDataSize();

	void Save(YkStream& stream,YkBool bAlign = false);

	YkBool Load(YkStream& stream,YkBool bAlign = false);


	//! \brief 是否带属性
	YkBool HasAtt();

};
}

#endif
