/*
 *
 * YkWrapCGeoModel.h
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

#ifndef AFX_YKWRAPCGEOMODEL_H__F5B3D7DE_A5C6_4E77_9432_1FBAED0270B3__INCLUDED_
#define AFX_YKWRAPCGEOMODEL_H__F5B3D7DE_A5C6_4E77_9432_1FBAED0270B3__INCLUDED_

#pragma once

#include "Stream/YkMemoryStream.h"
#include "Geometry3D/YkGeoModel.h"

using namespace Yk;

#ifdef __cplusplus
extern "C"{
#endif

#if !defined(__linux__)
//保存流
__declspec(dllexport) YkBool SaveElement2Stream(YkModelElement* pElement, YkStream& stream);
__declspec(dllexport) YkBool SaveShell2Stream(YkGeoModel* pGeoModelPro, YkStream& stream);

__declspec(dllexport) YkBool SaveElement(YkModelElement* pElement,  char*& pData, YkInt& nLen);
__declspec(dllexport) YkBool SaveShell(YkGeoModel* pGeoModelPro, char*& pData, YkInt& nLen);

//读取流
__declspec(dllexport) YkModelElement* LoadElementFromStream(YkStream& stream);
__declspec(dllexport) YkGeoModel* LoadShellFromStream(YkStream& stream);
__declspec(dllexport) YkModelElement* LoadElement(char* pData, const YkInt nLen);
__declspec(dllexport) YkGeoModel* LoadShell(char* pData, const YkInt nLen);
#else
//保存流
YkBool SaveElement2Stream(YkModelElement* pElement, YkStream& stream);
YkBool SaveShell2Stream(YkGeoModel* pGeoModelPro, YkStream& stream);

YkBool SaveElement(YkModelElement* pElement,  char*& pData, YkInt& nLen);
YkBool SaveShell(YkGeoModel* pGeoModelPro, char*& pData, YkInt& nLen);

//读取流
YkModelElement* LoadElementFromStream(YkStream& stream);
YkGeoModel* LoadShellFromStream(YkStream& stream);
YkModelElement* LoadElement(char* pData, const YkInt nLen);
YkGeoModel* LoadShell(char* pData, const YkInt nLen);
#endif
#ifdef __cplusplus
};
#endif
#endif