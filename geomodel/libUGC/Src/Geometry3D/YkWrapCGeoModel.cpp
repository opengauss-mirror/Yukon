/*
 *
 * YkWrapCGeoModel.cpp
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

#include "Geometry3D/YkWrapCGeoModel.h"

YkBool SaveShell2Stream(YkGeoModel* pGeoModelPro, YkStream& stream)
{	
	return pGeoModelPro->SaveGeoData(stream, YkDataCodec::encNONE);	
}

YkBool SaveShell(YkGeoModel* pGeoModelPro, char*& pData, YkInt& nLen)
{
	YkMemoryStream stream;
	stream.Open(YkStreamSave);	
	if( ! pGeoModelPro->SaveGeoData(stream, YkDataCodec::encNONE))
	{
		return false;
	}
	YkUchar* udata=0;
	YkUint size=0;
	stream.TakeBuffer(udata, size);
	size = stream.GetLength();
	pData = (char*)udata;
	nLen = size;
	return true;
}

YkGeoModel* LoadShellFromStream(YkStream& stream)
{
	YkGeoModel* pGeoModelPro = new YkGeoModel();
	pGeoModelPro->LoadGeoData(stream);
	return pGeoModelPro;
}

YkGeoModel* LoadShell(char* pData, const YkInt nLen)
{
	YkMemoryStream stream;
	if(!stream.Open(YkStreamLoad, nLen, (YkUchar*)pData))
	{
		return NULL;
	}
	return LoadShellFromStream(stream);
}

YkBool SaveElement(YkModelElement* pElement,  char*& pData, YkInt& nLen)
{
	YkMemoryStream stream;
	stream.Open(YkStreamSave);	
	if( ! SaveElement2Stream(pElement, stream))
	{
		return false;
	}
	YkUchar* udata=0;
	YkUint size=0;
	stream.TakeBuffer(udata, size);
	size = stream.GetLength();
	pData = (char*)udata;
	nLen = size;
	return true;
}

YkBool SaveElement2Stream(YkModelElement* pElement, YkStream& stream)
{
	YkModelElement::METype meType = pElement->GetType();
	stream << (YkInt)meType;
	switch (meType)
	{
	case YkModelElement::etSkeleton:
		((YkModelSkeleton* )pElement)->Save(stream);
		break;
	case  YkModelElement::etMaterial:
		((YkModelMaterial* )pElement)->Save(stream);
		break;
	case  YkModelElement::etTexture:
		((YkModelTexture* )pElement)->Save(stream);
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
	return TRUE;
}

YkModelElement* LoadElementFromStream(YkStream& stream)
{
	YkInt meType = YkModelElement::etUnknown;
	stream >> meType;
	YkModelElement* pElement = NULL;
	switch (meType)
	{
	case YkModelElement::etSkeleton:
		{
			pElement = new YkModelSkeleton();
			((YkModelSkeleton*)pElement)->Load(stream);
		}
		break;
	case  YkModelElement::etMaterial:
		{
			pElement = new YkModelMaterial();
			((YkModelMaterial*)pElement)->Load(stream);
		}
		break;
	case  YkModelElement::etTexture:
		{
			pElement = new YkModelTexture();
			((YkModelTexture*)pElement)->Load(stream);
		}
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
	return pElement;
}

YkModelElement* LoadElement(char* pData, const YkInt nLen)
{
	YkMemoryStream stream;
	if(!stream.Open(YkStreamLoad, nLen, (YkUchar*)pData))
	{
		return NULL;
	}
	return LoadElementFromStream(stream);
}