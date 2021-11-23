/*
 *
 * YkModelNodeTools.cpp
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

#include "Toolkit/YkHashCode.h"
#include "Toolkit/YkCompress.h"
#include "Base3D/YkModelNodeTools.h"
#include "Base3D/YkModelNodeElements.h"
#include "Base3D/YkMathEngine.h"

namespace Yk
{
YkModelNodeTools::YkModelNodeTools()
{
}

YkModelNodeTools::~YkModelNodeTools()
{

}

YkMatrix4d YkModelNodeTools::GetLocalMatrix(const YkPoint3D& pntPos, const YkPoint3D& pntScale, \
						  const YkPoint3D& pntRotate, YkBool bSpherePlaced)
{
	YkMatrix4d transMatrix = YkMatrix4d::IDENTITY;
	YkMatrix4d mRotate = YkMatrix4d::IDENTITY;
	if(bSpherePlaced)
	{
		SceneType sceneTypeOrg = YkMathEngine::m_msSceneType;
		YkMathEngine::m_msSceneType = ST_EARTH_SPHERICAL;

		mRotate = YkMatrix4d::RotationXYZ(-pntPos.y*DTOR, pntPos.x*DTOR, 0);
		YkVector3d pnt_Car = YkMathEngine::SphericalToCartesian(pntPos.x*DTOR, 
			pntPos.y*DTOR, pntPos.z+GLOBAL_RADIUS);

		YkMathEngine::m_msSceneType = sceneTypeOrg;

		transMatrix = YkMatrix4d::Translation(pnt_Car.x, pnt_Car.y, pnt_Car.z);
	}
	else
	{
		transMatrix = YkMatrix4d::Translation(pntPos.x, pntPos.y, pntPos.z);
	}

	//插入点:缩放、旋转、平移
	YkMatrix4d mat = YkMatrix4d::Scaling(pntScale.x, pntScale.y, pntScale.z) *
		YkMatrix4d::RotationXYZ(pntRotate.x*DTOR, pntRotate.y*DTOR, pntRotate.z*DTOR) *
		mRotate * transMatrix;

	return mat;
}
}