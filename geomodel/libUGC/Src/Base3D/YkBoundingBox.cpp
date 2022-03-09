/*
 *
 * YkBoundingBox.cpp
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

#include "Base3D/YkBoundingBox.h"
#include "Base3D/YkVector3d.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkMathEngine.h"

namespace Yk {
YkBoundingBox::YkBoundingBox()
{   
	SetMin(-0.5,-0.5,-0.5);
	SetMax(0.5,0.5,0.5);
	m_bNull = TRUE;
} 
YkBoundingBox::YkBoundingBox(YkVector3d vMin,YkVector3d vMax)
{
	SetExtents(vMin,vMax);
}

YkBoundingBox& YkBoundingBox::operator = (const YkBoundingBox& boundingBox)
{
	if (this == &boundingBox)
	{
		return *this;	
	}
	const YkVector3d vMin = boundingBox.GetMin();
	const YkVector3d vMax = boundingBox.GetMax();
	SetExtents(vMin,vMax);
	m_bNull = boundingBox.IsNULL();
	return *this;
}

YkBool YkBoundingBox::operator == (const YkBoundingBox &boundingBox)
{
	if (this == &boundingBox)
	{
		return TRUE;	
	}

	return ((boundingBox.GetMin() == m_vMin) && (boundingBox.GetMax() == m_vMax));
}

YkBoundingBox::~YkBoundingBox()
{
	
}

void YkBoundingBox::UpdateCorners(void)
{
	for(YkInt pos=0; pos<8; ++pos)
	{
		m_vCorner[pos] = YkVector3d(pos&1?m_vMax.x:m_vMin.x,pos&2?m_vMax.y:m_vMin.y,pos&4?m_vMax.z:m_vMin.z);
	}
	m_bNull=FALSE;	
}
void YkBoundingBox::SetMin(YkVector3d vMin)
{
	m_bNull = FALSE;
	m_vMin = vMin;
	UpdateCorners();
}
void YkBoundingBox::SetMax(YkVector3d vMax)
{
	m_bNull = FALSE;
	m_vMax = vMax;
	UpdateCorners();
	
}
void YkBoundingBox::SetMin(YkDouble x,YkDouble y,YkDouble z)
{
	m_bNull = FALSE;
	m_vMin.x = x;
	m_vMin.y = y;
	m_vMin.z = z;
	UpdateCorners();
}
void YkBoundingBox::SetMax(YkDouble x,YkDouble y,YkDouble z)
{
	m_bNull = FALSE;
	m_vMax.x = x;
	m_vMax.y = y;
	m_vMax.z = z;
	UpdateCorners();
}
const YkVector3d& YkBoundingBox::GetMin(void) const
{
	return m_vMin;
}
const YkVector3d& YkBoundingBox::GetMax(void) const
{
	return m_vMax;
}
void YkBoundingBox::SetExtents(const YkVector3d& vMin,const YkVector3d& vMax)
{
	m_bNull = FALSE;
	m_vMin = vMin;
	m_vMax = vMax;
	UpdateCorners();
}

const YkVector3d* YkBoundingBox::GetCorners()const
{
	return (const YkVector3d*)m_vCorner;
}

YkVector3d YkBoundingBox::GetCenter() const
{
	YkVector3d center;
	center.x =  ( m_vMin.x + m_vMax.x ) / 2.0 ;
	center.y =  ( m_vMin.y + m_vMax.y ) / 2.0 ;
	center.z =  ( m_vMin.z + m_vMax.z ) / 2.0 ;
	
	return center;
}

void YkBoundingBox::Transform(YkMatrix4d& matrix)
{
	if(m_bNull)
		return;
	YkBool bFirst = TRUE;
	YkVector3d vMax, vMin,vTemp;
	int i = 0; 
	for(i = 0; i < 8; i ++)
	{
		vTemp = m_vCorner[i].MultiplyMatrix(matrix);
		if(bFirst||vTemp.x > vMax.x)
			vMax.x = vTemp.x;
		if(bFirst||vTemp.y > vMax.y)
			vMax.y = vTemp.y;
		if(bFirst||vTemp.z > vMax.z)
			vMax.z = vTemp.z;
		if(bFirst||vTemp.x < vMin.x)
			vMin.x = vTemp.x;
		if(bFirst||vTemp.y < vMin.y)
			vMin.y = vTemp.y;
		if(bFirst||vTemp.z < vMin.z)
			vMin.z = vTemp.z;
		bFirst = FALSE;
	}
	SetExtents(vMin,vMax);
}
void YkBoundingBox::Merge(const YkBoundingBox& BoundBox)
{
	if(BoundBox.m_bNull)
		return;
	else if(m_bNull)
		SetExtents(BoundBox.m_vMin,BoundBox.m_vMax);
	else
	{
		YkVector3d vMin = m_vMin;
		YkVector3d vMax = m_vMax;
		vMax.MakeCeil(BoundBox.m_vMax);
		vMin.MakeFloor(BoundBox.m_vMin);
		SetExtents(vMin,vMax);
	}
	
}
void YkBoundingBox::SetNULL(void)
{
	m_bNull = TRUE;
}
YkBool YkBoundingBox::IsNULL(void)const
{
	return m_bNull;
}


YkBool YkBoundingBox::IsVaild()
{
	return (IsRealNaN(m_vMax.x) && IsRealNaN(m_vMax.y) && IsRealNaN(m_vMax.z) && IsRealNaN(m_vMin.x) && IsRealNaN(m_vMin.y) && IsRealNaN(m_vMin.z));
}
YkBool YkBoundingBox::IsRealNaN(YkDouble dValue)
{
	if (dValue>YK_DBLMAX || dValue< -YK_DBLMAX || YKIS0(dValue-YK_DBLMAX) || YKIS0(dValue-YK_DBLMIN)) 
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}
}