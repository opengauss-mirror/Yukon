/*
 *
 * YkGeometry.cpp
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

#include "Geometry3D/YkGeometry.h"

namespace Yk {

//! \brief 构造函数
YkGeometry::YkGeometry()
{
	m_nID			= 0;
	m_bBoundsDirty	= TRUE;
	m_bModifyVertexSeq = FALSE;
	m_nValidState = 0;
    m_bClip = false;
	m_bPJConverted = FALSE;

	m_pntPos = YkPoint3D(0,0,0);
	m_pntScale = YkPoint3D(1,1,1);
	m_pntRotate = YkPoint3D(0,0,0);
	m_BoundingBox.SetMax(0.0, 0.0, 0.0);
	m_BoundingBox.SetMin(0.0, 0.0, 0.0);
}
YkGeometry::YkGeometry(const YkGeometry& geometry)
{
	m_nID			= 0;
	m_bBoundsDirty	= TRUE;
	m_bModifyVertexSeq = FALSE;
	m_nValidState = geometry.m_nValidState;
    m_bClip = false;
	m_bPJConverted = geometry.m_bPJConverted;

	m_pntPos = geometry.m_pntPos;
	m_pntScale = geometry.m_pntScale;
	m_pntRotate = geometry.m_pntRotate;
	m_BoundingBox.SetMax(geometry.m_BoundingBox.GetMax());
	m_BoundingBox.SetMin(geometry.m_BoundingBox.GetMin());

}
//! \brief 析构函数
YkGeometry::~YkGeometry()
{
	YkGeometry::Clear();
}

void YkGeometry::Clear()
{

}

//! \brief 得到ID
YkInt YkGeometry::GetID() const
{
	return m_nID;
}

//! \brief 设置ID
void YkGeometry::SetID(YkInt nID)
{
	m_nID = nID;
}

//! \brief 获取Geometry的最小外接矩形
const YkRect2D& YkGeometry::GetBounds() const
{
	if (m_bBoundsDirty)
	{
		YkRect2D rcBounds = ComputeBounds();
		if( !rcBounds.IsValid() )
		{
			rcBounds.SetEmpty();
		}
		((YkGeometry*)this)->SetBounds(rcBounds);
	}
	return m_rcBounds;
}


YkPoint3D YkGeometry::GetPosition()const
{
	return m_pntPos;
}

void YkGeometry::SetPosition(const YkPoint3D& value)
{
	m_pntPos = value;
	SetBoundsDirty();
}

void YkGeometry::Offset( YkDouble dX, YkDouble dY)
{
	YkPoint3D pntTemp = GetPosition();
	if (this->IsValid())
	{
		if (m_rcBounds.IsEmpty())
		{
			pntTemp.x += dX;
			pntTemp.y += dY;
		}
		else
		{
			m_rcBounds.Offset(dX,dY);
			pntTemp.x += dX;
			pntTemp.y += dY;
		}

		SetPosition(pntTemp);
		SetBoundsDirty();
	}
}

void YkGeometry::Offset( YkDouble dOffset)
{
	Offset(dOffset, dOffset);
}

void YkGeometry::Offset( const YkSize2D &szOffset)
{
	Offset( szOffset.cx, szOffset.cy);
}

YkBool YkGeometry::Resize(const YkRect2D& rcNewBounds)
{
	YkPoint3D pnt3D = GetInnerPoint3D();
	pnt3D.x = rcNewBounds.CenterPoint().x;
	pnt3D.y = rcNewBounds.CenterPoint().y;

	SetPosition(pnt3D);

	SetBoundsDirty();
	return TRUE;
}

//! \brief 设置最小外接矩形
//! \remarks 用于知道Geometry的Bounds时, 设置Bounds, 不会带来Geometry其他数据的改变
//! 一般在数据引擎中使用
void YkGeometry::SetBounds(const YkRect2D& rcBounds)
{
	m_rcBounds = rcBounds;
	m_bBoundsDirty = FALSE;
}

void YkGeometry::SetBoundsDirty(YkBool bBoundsDirty)
{
	m_bBoundsDirty = bBoundsDirty;
}

//! \brief		判断Bounds是否脏了
//! \return		如果脏了, 返回true; 不脏返回false
YkBool YkGeometry::IsBoundsDirty() const
{
	return m_bBoundsDirty;
}

//! \brief 存储到Stream中
YkBool YkGeometry::Save(YkStream& stream, YkDataCodec::CodecType eCodecType/*, YkBool bIgnoreStyle*/) const
{
	{
		stream << 0;
	}

	// Spatial Data 
	SaveGeoData(stream, eCodecType);

	return TRUE;
}

//! \brief 从Stream中生成Geometry
YkBool YkGeometry::Load(YkStream& stream,YkInt iJuge)
{
	if(iJuge==0)
	{
		// 对于点而言， eCodecType没有作用，都用double存储
		YkInt nStyleSize=0;
		stream >> nStyleSize;
		if(nStyleSize<0 || nStyleSize>100000)
		{
			return FALSE;
		}

		if (!LoadGeoData(stream))
		{
			return FALSE;
		}
		
	}
	else
	{
		if (!LoadGeoData(stream,iJuge))
		{
			return FALSE;
		}
	}

	SetBoundsDirty();

	//刚加载的状态应该是 未验证的
	m_nValidState = 0;

	return TRUE;
}

//! \brief 拷贝Geometry基类中的成员变量, 包括ID, Style, Bounds等
void YkGeometry::CopyBaseData(const YkGeometry& geo)
{
	m_nID = geo.m_nID;
	
	SetBounds(geo.GetBounds());
	m_bPJConverted = geo.m_bPJConverted;
}

//! \brief 获取对象需要捕捉的点串
//! \param aryPnts 点串，传出去的要捕捉的点串
//! \return 如果是true表示有需要捕捉的点串，如果返回false表示该对象没有需要捕捉的点串
YkBool YkGeometry::GetSnapPoints(YkArray<YkPoint2D>& aryPnts)
{
	aryPnts.Add(this->GetInnerPoint());
	return true;
}

YkBool YkGeometry::Is3D( ) const
{
	return false;
}

YkPoint3D YkGeometry::Get3DPoint()
{
	return YkPoint3D();
}

YkBool YkGeometry::Set3DPoint( const YkPoint3D& pnt3d )
{
	return FALSE;
}

void YkGeometry::SetModifyVertexSeq(YkBool bModifyVertexSeq)
{
	m_bModifyVertexSeq = bModifyVertexSeq;
}

YkBool YkGeometry::HasModifiedVertexSeq()
{
	return m_bModifyVertexSeq;
}

void YkGeometry::SetIsClip(YkBool bClip)
{
    m_bClip = bClip;
}

YkBool YkGeometry::IsClip()
{
    return m_bClip;
}

YkBool YkGeometry::LoadGeoData( YkStream& stream ,YkInt iJuge,YkDataCodec::CodecType eCodecType,YkBool bSDBPlus )
{
	return TRUE;
}


void YkGeometry::SetValidState(YkInt nValidState)
{
	m_nValidState = nValidState;
}

YkBool YkGeometry::IsPJConverted()
{
	return m_bPJConverted;
}

void YkGeometry::SetPJConverted( YkBool bPJConverted )
{
	m_bPJConverted = bPJConverted;
}

YkPoint3D YkGeometry::SphericalToCartesian( const YkPoint3D& pnt3D )
{
	YkDouble dRadius = pnt3D.z + 6378137;
	YkDouble dLongitude = pnt3D.x * DTOR;
	YkDouble dLatitude = pnt3D.y * DTOR;

	YkDouble dRadCosLat = dRadius * (YkDouble)cos(dLatitude);
	return YkPoint3D((YkDouble)(dRadCosLat*sin(dLongitude)),
		(YkDouble)(dRadius*sin(dLatitude)),
		(YkDouble)(dRadCosLat*cos(dLongitude)));
}

YkPoint3D YkGeometry::CartesianToSpherical( const YkPoint3D& pnt3D )
{
	YkDouble rho = (YkDouble)sqrt(pnt3D.x * pnt3D.x + pnt3D.y * pnt3D.y + pnt3D.z * pnt3D.z);
	if (YKIS0(rho))
	{
		return YkPoint3D(0.0, 0.0, 0.0);
	}
	YkDouble longitude = (YkDouble)atan2(pnt3D.x, pnt3D.z);
	YkDouble latitude = (YkDouble)asin(pnt3D.y / rho);
	return YkPoint3D(longitude*RTOD, latitude*RTOD, rho-6378137);
}

YkBool YkGeometry::CopyAllBaseData(const YkGeometry& geo)
{
	// 对自己的拷贝
	if (this == &geo)
	{
		return FALSE;
	}

	// 先拷贝基类的成员
	CopyBaseData(geo);

	m_BoundingBox = geo.m_BoundingBox;
	m_pntPos = geo.m_pntPos;
	m_pntScale = geo.m_pntScale;
	m_pntRotate = geo.m_pntRotate;

	return TRUE;
}

YkBool YkGeometry::InitialTextureInfo(YkArray<YkTextureInfo>& arrayTextureInfos)
{
	arrayTextureInfos.RemoveAll();

	return TRUE;
}

YkBool YkGeometry::IsInLatLong() const
{
	if (m_pntPos.x >= -180 && m_pntPos.x <= 180 && m_pntPos.y >= -90 && m_pntPos.y <= 90)
	{
		return TRUE;
	}
	return FALSE;
}

const YkBoundingBox& YkGeometry::GetBoundingBox()	const
{
	YkBoundingBox bg = ComputeBoundingBox();
	if(((YkBoundingBox&)m_BoundingBox).IsVaild())
	{
		bg.Merge(m_BoundingBox);
	}
	((YkGeometry*)this)->SetBoundingBox(bg); 	
	return m_BoundingBox;
};

void YkGeometry::SetScale(YkDouble fScaleX,YkDouble fScaleY,YkDouble fScaleZ)
{
	if (m_pntScale.x !=fScaleX || m_pntScale.y !=fScaleY || m_pntScale.z != fScaleZ)
	{
		SetBoundsDirty(TRUE);
		m_pntScale.x=fScaleX;
		m_pntScale.y=fScaleY;
		m_pntScale.z=fScaleZ;
	} 
}
void YkGeometry::SetScale(YkDouble fScaleXYZ)
{
	if (m_pntScale.x !=fScaleXYZ || m_pntScale.y !=fScaleXYZ || m_pntScale.z != fScaleXYZ)
	{
		SetBoundsDirty(TRUE);
		m_pntScale.x=fScaleXYZ;
		m_pntScale.y=fScaleXYZ;
		m_pntScale.z=fScaleXYZ;
	}
}
void YkGeometry::SetRotation(YkDouble fRotateX,YkDouble fRotateY,YkDouble fRotateZ)
{
	m_pntRotate.x=fRotateX;
	m_pntRotate.y=fRotateY;
	m_pntRotate.z=fRotateZ;


}
YkPoint3D YkGeometry::GetScale()const
{
	return m_pntScale;
}

YkPoint3D YkGeometry::GetRotation()const
{
	return m_pntRotate;
}

}

