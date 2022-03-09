/*
 *
 * YkBoundingBox.h
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

#if !defined(AFX_YKBOUNDINGBOX_H__29C05AF1_76E7_49B0_A62C_239A57A92F80__INCLUDED_)
#define AFX_YKBOUNDINGBOX_H__29C05AF1_76E7_49B0_A62C_239A57A92F80__INCLUDED_

#pragma once

#include "Base3D/YkVector3d.h"

namespace Yk {
//! \brief 包围盒类
class BASE3D_API YkBoundingBox  
{
public:
	//! \brief 构造函数
	YkBoundingBox();

	//! \brief 构造函数
	//! \param vMin 包围盒的最小向量[in]。
	//! \param vMax 包围盒的最大向量[in]。
	YkBoundingBox(YkVector3d vMin,YkVector3d vMax);

	YkBoundingBox& operator = (const YkBoundingBox& boundingBox);

	//! \brief 判断两个包围盒是否相等。
	//! \param &boundingBox 另外一个包围盒[in]。
	//! \return 若相等返回为TRUE，若不相等则返回为FALSE。
	YkBool operator == (const YkBoundingBox &boundingBox);

	//! \brief 析构函数
	virtual ~YkBoundingBox();
	
	//! \brief 设置构成包围盒的最小向量。
	//! \param vMin 构成包围盒的最小向量[in]。
	void SetMin(YkVector3d vMin);	
	
	//! \brief 设置构成包围盒的最大向量。
	//! \param vMax 构成包围盒的最大向量[in]。
	void SetMax(YkVector3d vMax);
	
	//! \brief 设置构成包围盒的最小向量。
	//! \param x,y,z 构成包围盒的最小向量[in]。
	void SetMin(YkDouble x,YkDouble y,YkDouble z);
	
	//! \brief 设置构成包围盒的最大向量。
	//! \param x,y,z 构成包围盒的最大向量[in]。
	void SetMax(YkDouble x,YkDouble y,YkDouble z);

	//! \brief  得到构成包围盒的最小向量
	const YkVector3d& GetMin(void) const;
	
	//! \brief 得到构成包围盒的最大向量
	const YkVector3d& GetMax(void) const;
	
	//! \brief 设置构成包围盒的最大、最小向量。
	//! \param vMin 构成包围盒的最大最小向量[in]。
	//! \param vMax 构成包围盒的最大向量[in]。
	void SetExtents(const YkVector3d& vMin,const YkVector3d& vMax);	
	
	//! \brief 得到构成包围盒的8各点。
	const YkVector3d* GetCorners()const;	
	
	//! \brief 得到包围盒的中心点。
	//! \remarks 这个中心点是构成包围盒的最小向量和最大向量的平均值。
	YkVector3d GetCenter() const;	
	
	//! \brief 对包围盒进行矩阵变换
	//! \param matrix 对包围盒进行变换的矩阵[in]。
	void Transform(YkMatrix4d& matrix);	
	
	//! \brief 将两个包围盒进行合并。
	//! \param BoundBox 要进行合并的包围盒[in]。
	void Merge(const YkBoundingBox& BoundBox);	
	
	//! \brief 设置包围盒为空
	void SetNULL(void);
	
	//! \brief 判断包围盒是否为空
	YkBool IsNULL(void)const;
	
	//! \brief 判断有效性
	//! \return 包围盒是否有效
	YkBool IsVaild();

protected:
	//! \brief 根据最大最小向量更新包围盒的8个角点。
	void UpdateCorners(void);
			
	//! \brief 判断是否是合法实数。
	//! \param dValue 实数[in]。
	//! \return true为合法，false为非法。
	YkBool IsRealNaN(YkDouble dValue);

protected:
	//! \brief 构成包围盒的最小向量。
	YkVector3d m_vMin;
	//! \brief 构成包围盒的最大向量。
	YkVector3d m_vMax;
	//! \brief 构成包围盒8个角点。
	YkVector3d m_vCorner[8];
	//! \brief 标志包围盒是否为空。
	YkBool m_bNull;
};
}
#endif

