/*
 *
 * YkBoundingSphere.cpp
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

#include "Base3D/YkBoundingSphere.h"
#include "Base3D/YkMathEngine.h"

namespace Yk {
YkBoundingSphere::YkBoundingSphere(void)
	:m_center(0.0,0.0,0.0),m_radius(-1)
{
}

YkBoundingSphere::~YkBoundingSphere(void)
{
}

void YkBoundingSphere::ExpandBy(const YkBoundingSphere& bs)
{
	//原球无效
	if (!valid())
	{
		m_center = bs.m_center;
		m_radius = bs.m_radius;
		return;
	}

	//两包围球球心间的距离
	YkDouble d = ( m_center - bs.m_center ).Length();

	//bs在原球之内
	if ( d + bs.m_radius <= m_radius )  
	{
		return;
	}

	//bs包围原球 
	if ( d + m_radius <= bs.m_radius )  
	{
		m_center = bs.m_center;
		m_radius = bs.m_radius;
		return;
	}

	YkDouble new_radius = (m_radius + d + bs.m_radius ) * 0.5;
	YkDouble ratio = ( new_radius - m_radius ) / d ;

	m_center[0] += ( bs.m_center[0] - m_center[0] ) * ratio;
	m_center[1] += ( bs.m_center[1] - m_center[1] ) * ratio;
	m_center[2] += ( bs.m_center[2] - m_center[2] ) * ratio;

	m_radius = new_radius;
}
}
