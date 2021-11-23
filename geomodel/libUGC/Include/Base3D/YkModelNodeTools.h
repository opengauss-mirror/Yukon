/*
 *
 * YkModelNodeTools.h
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

#if !defined(AFX_YKMODELNODETOOLS_H__5DE74FD6_B050_418F_A437_A0458B71BA80__INCLUDED_)
#define AFX_YKMODELNODETOOLS_H__5DE74FD6_B050_418F_A437_A0458B71BA80__INCLUDED_

#pragma once

#include "Base3D/YkModelNode.h"

namespace Yk {
class YkModelPagedLOD;
class YkModelNode;

class BASE3D_API YkModelNodeTools
{
public:
	//! \brief 构造函数
	YkModelNodeTools();

	~YkModelNodeTools();

public:

	//! \brief 根据m_pntScale m_pntRotate m_pntPos构造矩阵；
	//! \brief 该 矩阵+ModelNode=YkGeoModelPro
	static YkMatrix4d GetLocalMatrix(const YkPoint3D& pntPos, const YkPoint3D& pntScale, \
		const YkPoint3D& pntRotate, const YkBool bSpherePlaced);
};

}

#endif
