/*
 *
 * YkBoundingSphere.h
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

#if !defined(AFX_YKBOUNDINGSPHERE_H__29C05AF1_76E7_49B0_A62C_239A57A92F80__INCLUDED_)
#define AFX_YKBOUNDINGSPHERE_H__29C05AF1_76E7_49B0_A62C_239A57A92F80__INCLUDED_

#pragma once

#include "Base3D/YkVector3d.h"
#include "Base3D/YkBoundingBox.h"

namespace Yk {
class BASE3D_API YkBoundingSphere
{
public:
	YkBoundingSphere(void);
	virtual ~YkBoundingSphere(void);

	//! \brief 判断包围球是否有效
	inline YkBool valid () const
	{
		return m_radius > 0.0;
	}


	inline YkVector3d GetCenter() const
	{
		return m_center;
	}

	inline YkDouble GetRadius() const
	{
		return m_radius;
	}

	//！ \brief 给据给定的包围球来扩展，球心、半径都改变
	//!  \param 待扩展的球
	void ExpandBy(const YkBoundingSphere& bs);

public:
	YkVector3d m_center;
	YkDouble m_radius;
};
}
#endif

