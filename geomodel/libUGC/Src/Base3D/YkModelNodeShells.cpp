/*
 *
 * YkModelNodeShells.cpp
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

#include "Base3D/YkModelNodeShells.h"
#include "Base3D/YkModelNode.h"
#include "Toolkit/YkHashCode.h"

namespace Yk
{
//////////////////////////////////////////////////////////////////////////
YkCSkeletonID::YkCSkeletonID()
{
	//! \brief 初始化值为非法值
	Reset();
};

YkCSkeletonID::~YkCSkeletonID()
{
	//! \brief 初始化值为非法值
	Reset();
};

YkCSkeletonID::YkCSkeletonID(YkInt nL,YkInt nP, YkInt nG, YkInt nS)
{
	m_nIdx_LOD = nL;
	m_nIdx_Patch = nP;
	m_nIdx_Geode = nG;
	m_nIdx_Skeleton = nS;
}
YkCSkeletonID& YkCSkeletonID::operator = (const YkCSkeletonID& other)
{
	m_nIdx_LOD = other.m_nIdx_LOD;
	m_nIdx_Patch = other.m_nIdx_Patch;
	m_nIdx_Geode = other.m_nIdx_Geode;
	m_nIdx_Skeleton = other.m_nIdx_Skeleton;
	return *this;
};

YkBool YkCSkeletonID::operator == (const YkCSkeletonID &id)
{
	return (m_nIdx_LOD == id.m_nIdx_LOD &&
		m_nIdx_Patch == id.m_nIdx_Patch &&
		m_nIdx_Geode == id.m_nIdx_Geode &&
		m_nIdx_Skeleton == id.m_nIdx_Skeleton);
};

void YkCSkeletonID::Reset()
{
	//! \brief 初始化值为非法值
	m_nIdx_LOD = -2;
	m_nIdx_Patch = -1;
	m_nIdx_Geode = -1;
	m_nIdx_Skeleton = -1;
};

YkBool YkCSkeletonID::IsValid() const
{
	return (m_nIdx_LOD>=-1) && (m_nIdx_Patch >=0) 
		&& (m_nIdx_Geode >= 0) && (m_nIdx_Skeleton >= 0);
};
//////////////////////////////////////////////////////////////////////////
YkModelGeode::YkModelGeode():m_matLocalView(YkMatrix4d::IDENTITY)
{
	 m_nAttID = 0;
}

YkModelGeode::YkModelGeode(YkModelGeode& geode)
{
	m_strName = geode.m_strName;
	m_strAttTableName = geode.m_strAttTableName;
	m_nAttID = geode.m_nAttID;

	m_matLocalView = geode.GetPosition();
	m_BoundingBox = geode.GetBoundingBox();
	m_mapPack = geode.GetEMapPack();
}

YkModelGeode::~YkModelGeode()
{

}

void YkModelGeode::MakeFrom(YkModelSkeleton* pSkeleton, YkMatrix4d matGeode)
{
	if(pSkeleton == NULL)
	{
		YKASSERT(FALSE);
	}
	YKASSERT(pSkeleton->m_strMaterialName.IsEmpty());

	SetPosition(matGeode);

	//给默认材质
	YkModelMaterial* pMaterial = new YkModelMaterial();WhiteBox_Ignore
	pMaterial->MakeDefault();

	pSkeleton->SetMaterialName(pMaterial->m_strName);
	YKASSERT(!pSkeleton->m_strMaterialName.IsEmpty());

	AddSkeleton(YkModelSkeletonPtr(pSkeleton));
	AddMaterial(YkModelMaterialPtr(pMaterial));
}

void YkModelGeode::Clear()
{
	m_mapPack.Clear();
	m_BoundingBox.SetNULL();
	m_matSupplement = YkMatrix4d::IDENTITY;
	m_matLocalView = YkMatrix4d::IDENTITY;
}

//以实体本身的名字为主
void YkModelGeode::AddMapPack(YkModelEMapPack& mapPack)
{
	std::vector<YkString> vecMaterial;
	std::vector<YkString> vecTexture;
	YkString strElement;
	
	std::map<YkString, YkModelSkeletonPtr>::iterator itSke;
	for (itSke=mapPack.m_mapSkeleton.begin(); 
		itSke != mapPack.m_mapSkeleton.end(); itSke++)
	{
		vecMaterial.clear();
		vecTexture.clear();
		strElement = itSke->second.IsNull() ? itSke->first : itSke->second->m_strName;
		AddElementShell(YkModelElement::etSkeleton, strElement);
		
		////////直接操作属性//////////////////////////////////////////////////////////////////
		//材质
		mapPack.GetAtt(strElement, YkModelElement::etSkeleton, vecMaterial);

		//纹理
		for (YkInt i=0; i<vecMaterial.size(); i++)
		{
			AddElementShell(YkModelElement::etMaterial, vecMaterial[i]);

			vecTexture.clear();
			mapPack.GetAtt(vecMaterial[i], YkModelElement::etMaterial, vecTexture);
			for (YkInt j=0; j<vecTexture.size(); j++)
			{
				AddElementShell(YkModelElement::etTexture, vecTexture[j]);
			}
		}
	}
	LoadElements(mapPack);
}

void YkModelGeode::LoadElements(YkModelEMapPack& mapPack)
{
	m_mapPack.LoadElements(mapPack);
}

YkBool YkModelGeode::AddSkeleton(YkModelSkeletonPtr pGeometry)
{
	if(pGeometry.IsNull() || pGeometry->m_strName.IsEmpty())
	{
		YKASSERT(FALSE);
		return FALSE;
	}
	m_mapPack.AddSkeleton(pGeometry);
	m_BoundingBox.SetNULL();
	return TRUE;
}

YkBool YkModelGeode::AddMaterial(YkModelMaterialPtr pMaterial3D)
{
	if(pMaterial3D.IsNull() || pMaterial3D->m_strName.IsEmpty())
	{
		YKASSERT(FALSE);
		return FALSE;
	}	
	m_mapPack.AddMaterial(pMaterial3D);
	return TRUE;
}

YkBool YkModelGeode::AddTexture(YkModelTexturePtr pTexData)
{
	if(pTexData.IsNull() || pTexData->m_strName.IsEmpty())
	{
		YKASSERT(FALSE);
		return FALSE;
	}	
	m_mapPack.AddTexture(pTexData);
	return TRUE;
}

void YkModelGeode::AddElement(YkModelElement* pElement)
{
	if(pElement == NULL)
	{
		YKASSERT(FALSE);
		return;
	}
	YkString strName = pElement->GetName();
	switch (pElement->GetType())
	{
	case YkModelElement::etSkeleton:
		{
			m_mapPack.AddSkeleton((YkModelSkeleton*)pElement);
		}
		break;
	case YkModelElement::etMaterial:
		{
			m_mapPack.AddMaterial((YkModelMaterial*)pElement);
		}
		break;
	case YkModelElement::etTexture:
		{
			m_mapPack.AddTexture((YkModelTexture*)pElement);
		}
		break;
	default:
		YKASSERT(FALSE);
		break;
	}
}

void YkModelGeode::AddTexture(std::vector<YkModelTexture*>& vecElement)
{
	for (YkInt i=0; i<vecElement.size(); i++)
	{
		AddElement(vecElement[i]);
	}
}

YkModelSkeletonPtr YkModelGeode::CreateSkeleton(const YkString strName)
{
	return m_mapPack.CreateSkeleton(strName);
}

YkModelMaterialPtr YkModelGeode::CreateMaterial(const YkString strName)
{
	return m_mapPack.CreateMaterial(strName);
}

YkModelTexturePtr YkModelGeode::CreateTexture(const YkString strName)
{
	return m_mapPack.CreateTexture(strName);
}

void YkModelGeode::AddElementShell(const YkModelElement::METype eType, const YkString strName)
{
	m_mapPack.AddElementShell(eType, strName);
}


YkModelEMapPack& YkModelGeode::GetEMapPack()
{
	return m_mapPack;
}

YkInt YkModelGeode::GetElementNum(YkModelElement::METype metype) const
{
	YkInt nCount = 0;
	if(metype == YkModelElement::etSkeleton)
	{
		nCount = m_mapPack.m_mapSkeleton.size();
	}
	else if(metype == YkModelElement::etTexture)
	{
		nCount = m_mapPack.m_mapTexture.size();
	}
	else if(metype == YkModelElement::etMaterial)
	{
		nCount = m_mapPack.m_mapMaterial.size();
	}
	else if(metype == YkModelElement::etUnknown)
	{
		nCount = m_mapPack.m_mapSkeleton.size() + \
			m_mapPack.m_mapTexture.size() + m_mapPack.m_mapMaterial.size();
	}
	return nCount;
}

YkModelSkeletonPtr YkModelGeode::GetSkeleton(const YkString strName)
{
	return m_mapPack.GetSkeleton(strName);
}

YkModelMaterialPtr YkModelGeode::GetMaterial(const YkString strName)
{
	return m_mapPack.GetMaterial(strName);
}

YkModelTexturePtr YkModelGeode::GetTexture(const YkString strName)
{
	return m_mapPack.GetTexture(strName);
}

YkBool YkModelGeode::IsNameExisted(const YkString strName)
{
	return m_mapPack.IsExisted(strName);
}

YkString YkModelGeode::GetUnoccupiedName(YkString strName)
{
	return m_mapPack.GetUnoccupiedName(strName);
}

void YkModelGeode::ComputeBoundingBox()
{
	YkBoundingBox m_SkeletonBoundingBox;
	YkModelSkeletonMap::iterator itSkeleton = m_mapPack.m_mapSkeleton.begin();
	for(;itSkeleton!=m_mapPack.m_mapSkeleton.end();itSkeleton++)
	{
		YkModelSkeletonPtr pSkeleton = itSkeleton->second;
		if (pSkeleton.IsNull())
		{
			break;
		}
		m_SkeletonBoundingBox =ComputeSkeletonBoundingBox(pSkeleton);
		if(!m_SkeletonBoundingBox.IsNULL())
		{
			m_BoundingBox.Merge(m_SkeletonBoundingBox);
		}
	}
}

YkBoundingBox YkModelGeode::ComputeSkeletonBoundingBox(YkModelSkeletonPtr& pSkeleton)
{
	YkBoundingBox m_SkeletonBoundingBox;
	YkMatrix4d mat = m_matLocalView;
	YkDouble dMaxX =0.0,dMaxY =0.0,dMaxZ =0.0;
	YkDouble dMinX =0.0,dMinY =0.0,dMinZ =0.0;
	YkVector3d vMin = YkVector3d(1.0, 1.0, 1.0);
	YkVector3d vMax = YkVector3d(1.0, 1.0, 1.0);
	if (!pSkeleton.IsNull() && pSkeleton->GetVertexCount() > 0)
	{
		YkInt pSkeletonVertexCount = pSkeleton->GetVertexCount();
		YkVector3d tempYkVector3d = pSkeleton->GetVertex(0).MultiplyMatrix(mat);
		dMaxX = tempYkVector3d.x;
		dMaxY = tempYkVector3d.y;
		dMaxZ = tempYkVector3d.z;
		dMinX = tempYkVector3d.x;
		dMinY = tempYkVector3d.y;
		dMinZ = tempYkVector3d.z;

		for (YkInt i=1;i<pSkeletonVertexCount;i++)
		{
			YkVector3d iYkVector3d = pSkeleton->GetVertex(i).MultiplyMatrix(mat);

			if (iYkVector3d.x>dMaxX) dMaxX = iYkVector3d.x;
			if (iYkVector3d.y>dMaxY) dMaxY = iYkVector3d.y;
			if (iYkVector3d.z>dMaxZ) dMaxZ = iYkVector3d.z;

			if (iYkVector3d.x<dMinX) dMinX = iYkVector3d.x;
			if (iYkVector3d.y<dMinY) dMinY =  iYkVector3d.y;
			if (iYkVector3d.z<dMinZ) dMinZ = iYkVector3d.z;
		}
		vMin = YkVector3d(dMinX,dMinY,dMinZ);
		vMax = YkVector3d(dMaxX,dMaxY,dMaxZ);
		m_SkeletonBoundingBox = YkBoundingBox(vMin,vMax);

		
	}
	return m_SkeletonBoundingBox;
}

//! \brief 作用于各Geode的矩阵
void YkModelGeode::ReMultiplyMatrix(const YkMatrix4d& m)
{
	m_matLocalView = m_matLocalView * m_matSupplement.Invert();
	m_matLocalView = m_matLocalView * m;
	m_matSupplement = m;

	YkMatrix4d mTemp(m);
	m_BoundingBox.Transform(mTemp);
}

YkBoundingBox YkModelGeode::GetBoundingBox()
{
	if(m_BoundingBox.IsNULL())
	{
		ComputeBoundingBox();
	}
	return m_BoundingBox;
}

void YkModelGeode::SetBoundingBox(YkBoundingBox box)
{
	m_BoundingBox=box;
}

void YkModelGeode::SetPosition(YkMatrix4d mat)
{ 
	m_matLocalView = mat; 
	//范围需要重新计算
	m_BoundingBox.Transform(mat);
};

const YkMatrix4d& YkModelGeode::GetPosition() const
{ 
	return m_matLocalView; 
}

YkInt YkModelGeode::GetShellDataSize()
{
	YkInt nSize = 0;
	nSize += sizeof(YkDouble) * 16;
	return nSize;
}

YkBool YkModelGeode::SaveShell(YkStream& stream)
{
	stream.Save(m_matLocalView.GetPointer(), 16);

	//以壳儿（名字）为主存储
	std::vector<YkString> vecNames;
	vecNames = GetElementNames(YkModelElement::etSkeleton);
	YkInt nCount = vecNames.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		stream << vecNames[i];
	}

	vecNames.clear();
	vecNames = GetElementNames(YkModelElement::etMaterial);
	nCount = vecNames.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		stream << vecNames[i];
	}
	
	vecNames.clear();
	vecNames = GetElementNames(YkModelElement::etTexture);
	nCount = vecNames.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		stream << vecNames[i];
	}

	return TRUE;
}

YkBool YkModelGeode::LoadShell(YkStream& stream)
{
	stream.Load(m_matLocalView.GetPointer(), 16);

	YkInt nCount=0;
	YkString strName;
	stream >> nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		stream >> strName;
		m_mapPack.AddElementShell(YkModelElement::etSkeleton, strName);
	}

	stream >> nCount;	
	for (YkInt i=0; i<nCount; i++)
	{
		stream >> strName;
		m_mapPack.AddElementShell(YkModelElement::etMaterial, strName);
	}

	stream >> nCount;	
	for (YkInt i=0; i<nCount; i++)
	{
		stream >> strName;
		m_mapPack.AddElementShell(YkModelElement::etTexture, strName);
	}

	m_BoundingBox.SetNULL();

	return TRUE;
}

YkBool YkModelGeode::LoadElements(YkModelEM* pElementsManager,YkModelElement::METype mtype)
{
	YkModelSkeletonPtr pGeometry;
	YkModelMaterialPtr pMaterial3D;
	YkModelTexturePtr pTexture;
	YkBool bGeo = mtype == YkModelElement::etUnknown || mtype == YkModelElement::etSkeleton;
	YkBool bTexture = mtype == YkModelElement::etUnknown || mtype == YkModelElement::etTexture;
	YkBool bMaterial = mtype == YkModelElement::etUnknown || mtype == YkModelElement::etMaterial;

	if(bGeo)
	{
		std::vector<YkString> vecNames;
		vecNames = GetElementNames(YkModelElement::etSkeleton);
		if(vecNames.size() == 0)
		{
			return FALSE;
		}
		for (YkInt i=0; i<vecNames.size(); i++)
		{
			pGeometry = pElementsManager->GetLoadedSkeleton(vecNames[i]);
			YKASSERT(pGeometry.Get() != NULL);//外部保证
			if(pGeometry.Get() == NULL)
			{
				continue;
			}
			pGeometry->m_nRefCount ++;
			m_mapPack.SetSkeleton(pGeometry);
		}
	}

	if(bMaterial)
	{
		std::vector<YkString> vecNames;
		vecNames = GetElementNames(YkModelElement::etMaterial);
		for (YkInt i=0; i<vecNames.size(); i++)
		{
			pMaterial3D = pElementsManager->GetLoadedMaterial3D(vecNames[i]);
			YKASSERT(!pMaterial3D.IsNull());//外部保证
			if(pMaterial3D.IsNull())
			{
				continue;
			}
			m_mapPack.SetMaterial(pMaterial3D);
		}
	}

	if(bTexture)
	{
		std::vector<YkString> vecNames;
		vecNames = GetElementNames(YkModelElement::etTexture);
		for (YkInt i=0; i<vecNames.size(); i++)
		{
			pTexture = pElementsManager->GetLoadedTexture(vecNames[i]);
			YKASSERT(!pTexture.IsNull());//外部保证
			if(pTexture.IsNull())
			{
				continue;
			}
			m_mapPack.SetTexture(pTexture);
		}
	}
	m_BoundingBox.SetNULL();

	return TRUE;
}

YkBool YkModelGeode::RemoveSkeleton(const YkString strSkeleton, YkBool bSynElement)
{
	YkBool bFound = FALSE;
	if(!m_mapPack.RemoveSkeleton(strSkeleton).IsEmpty())
	{
		if(bSynElement)
		{
			m_mapPack.SynElement();
		}		
		bFound = TRUE;
	}

	m_BoundingBox.SetNULL();
	return bFound;
}

std::vector<YkString> YkModelGeode::GetElementNames(YkModelElement::METype metype)
{
	return m_mapPack.GetElementNames(metype);
}

//////////////////////////////////////////////////////////////////////////
YkModelPagedPatch::YkModelPagedPatch()
{
	m_dbLODDis = 0;
	m_pPagedLOD = NULL;
	m_nIndex = -1;
	m_pPatchParent = NULL;
	m_nPatchParentIndex = -1;
	m_vecChildren.clear();
	m_vecChildrenIndex.clear();
	m_vecGeode.clear();
	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	m_bElementLoaded = FALSE;
	m_nRenderID = -1;
	
}

YkModelPagedPatch::YkModelPagedPatch(YkModelPagedLOD* pPagedLOD, YkFloat range)
{
	m_dbLODDis = range;
	m_pPagedLOD = pPagedLOD;
	m_nIndex = -1;
	m_pPatchParent = NULL;
	m_nPatchParentIndex = -1;
	m_vecChildren.clear();
	m_vecChildrenIndex.clear();
	m_vecGeode.clear();
	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	m_bElementLoaded = FALSE;
	m_nRenderID = -1;
}

void YkModelPagedPatch::MakeFrom(YkModelPagedLOD* pPagedLOD, YkModelPagedPatch* pPatch)
{
	if(pPatch == NULL)
	{
		YKASSERT(FALSE);
		return;
	}

	//不是pPatch的PagedLOD
	m_pPagedLOD = pPagedLOD;

	m_nIndex = pPatch->GetIndex();

	m_dbLODDis = pPatch->GetLODDistance();
	m_BoundingSphere = pPatch->GetBoudingSphere();
	m_bElementLoaded = pPatch->IsElementLoaded();
	m_nRenderID = -1;
	m_strFileName = pPatch->GetFileName();

	//仅记录父子的Index,拷贝完成后在Node里重构父子关系
	m_pPatchParent = NULL;
	m_vecChildren.clear();
	m_nPatchParentIndex = pPatch->GetParentPatch()!=NULL ? pPatch->GetParentPatch()->GetIndex() : -1;
	for (YkInt i=0; i<pPatch->GetChildCount(); i++)
	{
		m_vecChildrenIndex.push_back(pPatch->GetChild(i)->GetIndex());
	}

	//m_vecGeode
	std::vector<YkModelGeode*> geodes = pPatch->GetGeodes();
	YkInt nSize = geodes.size();
	for (YkInt i=0; i<nSize; i++)
	{
		YkModelGeode *pGeode = new YkModelGeode(*(geodes[i]));
		m_vecGeode.push_back(pGeode);
	}
}



YkModelPagedPatch::~YkModelPagedPatch()
{
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		delete m_vecGeode[i];
		m_vecGeode[i] = NULL;
	}
	m_vecGeode.clear();
	m_vecChildren.clear();

}

YkFloat YkModelPagedPatch::GetLODDistance() const
{
	return m_dbLODDis;
}

void YkModelPagedPatch::SetLODDistance(YkFloat dbLODDis)
{
	m_dbLODDis = dbLODDis;
}

YkString YkModelPagedPatch::GetFileName()
{
	return m_strFileName;
}

void YkModelPagedPatch::SetFileName(YkString strName)
{
	m_strFileName = strName;
}

void YkModelPagedPatch::AddGeode(YkModelGeode *& pGeode)
{
	//根据Geode的矩阵判断是否应该合并成一个
	YkMatrix4d matGeodeSrc = pGeode->GetPosition();
	YkBool bAdded = FALSE;
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		YkMatrix4d matGeodeDes = m_vecGeode[i]->GetPosition();
		if(matGeodeSrc == matGeodeDes)
		{
			YkModelEMapPack mapPack = pGeode->GetEMapPack();
			m_vecGeode[i]->AddMapPack(mapPack);

			YkBoundingBox box= m_vecGeode[i]->GetBoundingBox();
			box.Merge(pGeode->GetBoundingBox());
			m_vecGeode[i]->SetBoundingBox(box);

			delete pGeode;
			pGeode = m_vecGeode[i];

			bAdded = TRUE;
			break;
		}
	}
	if(! bAdded)
	{
		m_vecGeode.push_back(pGeode);
	}
	
	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
}

std::vector<YkModelGeode *>& YkModelPagedPatch::GetGeodes()
{
	return m_vecGeode;
}

YkInt YkModelPagedPatch::GetGeodeCount() const
{
	return m_vecGeode.size();
}

YkModelGeode* YkModelPagedPatch::GetGeode(YkInt nIndex)
{
	if(nIndex < 0 || nIndex > m_vecGeode.size())
	{
		return NULL;
	}
	return m_vecGeode[nIndex];
}

void YkModelPagedPatch::RemoveAllGeodes()
{
	m_vecGeode.clear();
}

void YkModelPagedPatch::SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius )
{
	m_BoundingSphere.m_center = vecCenter;
	m_BoundingSphere.m_radius = dRadius;
}

void YkModelPagedPatch::SetBoundingBox(const YkBoundingBox& bbox)
{
	m_BoundingBox = bbox;
}

YkBoundingSphere YkModelPagedPatch::GetBoudingSphere()
{
	if(!m_BoundingSphere.valid())
	{
		ComputeBoundingSphere();
	}

	return m_BoundingSphere;
}

YkBoundingBox YkModelPagedPatch::GetBoundingBox()
{
	if(m_BoundingBox.IsNULL())
	{
		m_BoundingBox = ComputeBoudingBox();
	}

	return m_BoundingBox;
}

YkBoundingBox YkModelPagedPatch::ComputeBoudingBox()
{
	//开始计算
	YkBoundingBox boundingBox;
	YkBoundingBox bGeoBox;
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		YkModelGeode* pGeode = m_vecGeode[i];
		YKASSERT(pGeode != NULL);

		pGeode->ComputeBoundingBox();
		bGeoBox = pGeode->GetBoundingBox();
		if(!bGeoBox.IsNULL())
		{
			boundingBox.Merge(bGeoBox);
		}
	}
	return boundingBox;
}

//PagedLOD的Bounds是换算过矩阵的
void YkModelPagedPatch::ComputeBoundingSphere()
{
	m_BoundingSphere.m_radius = 0.0;

	//开始计算
	YkBoundingBox boundingBox;
	if (m_vecGeode.empty())
	{
		boundingBox = m_BoundingBox;
	}
	else
	{
		for (YkUint i = 0; i < m_vecGeode.size(); i++)
		{
			YkModelGeode* pGeode = m_vecGeode[i];
			YKASSERT(pGeode != NULL);

			pGeode->ComputeBoundingBox();
			const YkBoundingBox& bSubBox = pGeode->GetBoundingBox();
			if(!bSubBox.IsNULL())
			{
				boundingBox.Merge(bSubBox);
			}
		}
	}

	if(!boundingBox.IsNULL())
	{
		m_BoundingSphere.m_center = boundingBox.GetCenter();
		m_BoundingSphere.m_radius = (boundingBox.GetMax()-boundingBox.GetMin()).Length()/2;
	}
}

//! \brief 作用于各Geode的矩阵
void YkModelPagedPatch::ReMultiplyMatrix(const YkMatrix4d& m)
{
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		YkModelGeode* pGeode = m_vecGeode[i];
		YKASSERT(pGeode != NULL);
		pGeode->ReMultiplyMatrix(m);
	}
	m_BoundingSphere.m_radius = 0;
}

void YkModelPagedPatch::GetElementNames(std::vector<YkString>& vecNames, YkModelElement::METype metype)
{
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		std::vector<YkString> vectemp = m_vecGeode[i]->GetElementNames(metype);
		vecNames.insert(vecNames.end(), vectemp.begin(), vectemp.end());
	}
}

void YkModelPagedPatch::GetSkeletons(std::map<YkString, YkModelSkeletonPtr>& mapElement)
{
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		std::vector<YkString> vecNames = pGeode->GetElementNames(YkModelElement::etSkeleton);
		for (YkInt k=0; k<vecNames.size(); k++)
		{
			YkModelSkeletonPtr pElement = pGeode->GetSkeleton(vecNames[k]);
			mapElement[vecNames[k]] = pElement;
		}
	}
}

void YkModelPagedPatch::GetMaterials(std::map<YkString, YkModelMaterialPtr>& mapElement)
{
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		std::vector<YkString> vecNames = pGeode->GetElementNames(YkModelElement::etMaterial);
		for (YkInt k=0; k<vecNames.size(); k++)
		{
			YkModelMaterialPtr pElement = pGeode->GetMaterial(vecNames[k]);
			mapElement[vecNames[k]] = pElement;
		}
	}
}

void YkModelPagedPatch::GetTextures(std::map<YkString, YkModelTexturePtr>& mapElement)
{
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		std::vector<YkString> vecNames = pGeode->GetElementNames(YkModelElement::etTexture);
		for (YkInt k=0; k<vecNames.size(); k++)
		{
			YkModelTexturePtr pElement = pGeode->GetTexture(vecNames[k]);
			mapElement[vecNames[k]] = pElement;
		}
	}
}

YkModelSkeletonPtr YkModelPagedPatch::GetSkeleton(const YkString strName)
{
	YkModelSkeletonPtr pElement;
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		pElement = pGeode->GetSkeleton(strName);
		if(!pElement.IsNull())
		{
			break;;
		}
	}
	return pElement;
}

YkModelMaterialPtr YkModelPagedPatch::GetMaterial(const YkString strName)
{
	YkModelMaterialPtr pElement;
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		pElement = pGeode->GetMaterial(strName);
		if(!pElement.IsNull())
		{
			break;;
		}
	}
	return pElement;
}

YkModelTexturePtr YkModelPagedPatch::GetTexture(const YkString strName)
{
	YkModelTexturePtr pElement;
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		pElement = pGeode->GetTexture(strName);
		if(!pElement.IsNull())
		{
			break;;
		}
	}
	return pElement;
}

YkBool YkModelPagedPatch::IsNameExisted(const YkString strName)
{
	for (YkInt j=0; j<m_vecGeode.size(); j++)
	{
		YkModelGeode *pGeode = m_vecGeode[j];
		if(pGeode->IsNameExisted(strName))
		{
			return TRUE;
		}
	}
	return FALSE;
}

YkInt YkModelPagedPatch::GetShellDataSize()
{
	YkInt nSize = 0;
	nSize += sizeof(m_dbLODDis);
	nSize += sizeof(m_nIndex);
	nSize += sizeof(YkInt)*(m_vecChildrenIndex.size() + 1);

	YkInt nCount = m_vecGeode.size();
	nSize += sizeof(nCount);
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelGeode* pGeode = m_vecGeode[i];
		YKASSERT(pGeode != NULL);
		nSize += pGeode->GetShellDataSize();
	}
	return nSize;
}

YkBool YkModelPagedPatch::Save(YkStream& stream)
{
	stream << m_dbLODDis;
	stream << m_nIndex;

	//父节点索引号
	YkInt nParentIndex = (m_pPatchParent==NULL) ? -1 : m_pPatchParent->GetIndex();
	stream << nParentIndex;

	//子节点索引号
	YkInt nCount = m_vecChildren.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		stream << m_vecChildren[i]->GetIndex();
	}

	//数据包
	nCount = m_vecGeode.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelGeode* pGeode = m_vecGeode[i];
		YKASSERT(pGeode != NULL);
		pGeode->SaveShell(stream);
	}
	return TRUE;
}

YkBool YkModelPagedPatch::Load(YkStream& stream)
{
	stream >> m_dbLODDis;
	stream >> m_nIndex;

	//父节点索引号
	stream >> m_nPatchParentIndex;

	//子节点索引号
	YkInt nCount=0, nIndex=0;
	stream >> nCount;
	if(nCount == 0)
	{
		//精细层LOD=NULL
		YKASSERT(m_pPagedLOD == NULL);
	}
	for (YkInt i=0; i<nCount; i++)
	{
		stream >> nIndex;
		m_vecChildrenIndex.push_back(nIndex);
	}

	stream >> nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelGeode* pGeode = new YkModelGeode();
		pGeode->LoadShell(stream);
		m_vecGeode.push_back(pGeode);
	}
	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;

	return TRUE;
}

void YkModelPagedPatch::LoadPatchTree(YkModelNode* pNode)
{
	std::vector<YkModelPagedLOD*> vecPagedLODs = pNode->GetPagedLODs();
	YkInt nTotal = vecPagedLODs.size();
	YkInt nCurrentLOD = m_pPagedLOD!=NULL ? m_pPagedLOD->GetLODNum() : -1;
	YkModelPagedLOD* pChildLOD = nCurrentLOD-1>=0 ? vecPagedLODs[nCurrentLOD-1] : NULL;
	YkModelPagedLOD* pParentLOD = nCurrentLOD+1<nTotal ? vecPagedLODs[nCurrentLOD+1] : NULL;
	
	//如果是第0层，挂上精细层为子节点
	if(nCurrentLOD == 0)
	{
		for (YkInt i=0; i<m_vecChildrenIndex.size(); i++)
		{
			for (YkInt j=0; j<pNode->GetDataPatcheCount(); j++)
			{
				YkModelPagedPatch* pPatch = pNode->GetDataPatche(j);
				if(pPatch->GetIndex() == m_vecChildrenIndex[i])
				{
					m_vecChildren.push_back(pPatch);
					break;
				}
			}
		}
	}

	if(pParentLOD != NULL)
	{
		if(m_nPatchParentIndex >=0 )
		{
			m_pPatchParent = pParentLOD->GetPagedPatch(m_nPatchParentIndex);
		}
		else
		{
			for (YkInt i=0; i<pParentLOD->GetPagedPatchCount(); i++)
			{
				YkModelPagedPatch* pPatch = pParentLOD->GetPagedPatch(i);
				std::vector<YkInt> vecChild = pPatch->GetChildrenIndex();
				for (YkInt j=0; j<vecChild.size(); j++)
				{
					if(vecChild[j] == m_nIndex)
					{
						SetParentPatch(pPatch);
						break;
					}
				}
			}
		}
		YKASSERT(m_pPatchParent != NULL);
	}
	if(pChildLOD != NULL)
	{
		m_vecChildren.clear();

		for (YkInt i=0; i<m_vecChildrenIndex.size(); i++)
		{
			YkModelPagedPatch* pPatch = pChildLOD->GetPagedPatch(m_vecChildrenIndex[i]);
			if (pPatch != NULL)
			{
				m_vecChildren.push_back(pPatch);
			}
		}
	}
}

YkBool YkModelPagedPatch::LoadElements(YkModelEM* pEM, YkModelElement::METype mtype)
{
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		m_vecGeode[i]->LoadElements(pEM, mtype);
	}

	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	m_bElementLoaded = TRUE;
	return TRUE;
}


YkBool YkModelPagedPatch::IsElementLoaded()
{
	return m_bElementLoaded;
}

YkInt YkModelPagedPatch::AddChild(YkModelPagedPatch* pPatch)
{
	YKASSERT(pPatch != NULL);
	
	for (YkInt i=0; i<m_vecChildren.size(); i++)
	{
		if(m_vecChildren[i] == pPatch)
		{
			return pPatch->GetIndex();
		}
	}

	m_vecChildren.push_back(pPatch);
	return pPatch->GetIndex();
}


YkModelPagedPatch* YkModelPagedPatch::GetChild(YkInt nIndex)
{
	if(nIndex < 0 || nIndex >= m_vecChildren.size())
	{
		YKASSERT(FALSE);
		return NULL;
	}
	return m_vecChildren[nIndex];
}

std::vector<YkModelPagedPatch*>& YkModelPagedPatch::GetChildren()
{
	return m_vecChildren;
}

YkInt YkModelPagedPatch::GetChildCount()
{
	return  m_vecChildren.size();
}

void YkModelPagedPatch::RemoveChildren()
{
	m_vecChildren.clear();
}

void YkModelPagedPatch::RemoveChildren(YkModelPagedPatch* pChildPatch)
{
	for (YkInt i=0; i<m_vecChildren.size(); i++)
	{
		if(m_vecChildren[i] == pChildPatch)
		{
			m_vecChildren.erase(m_vecChildren.begin() + i);
			break;
		}
	}
}

YkInt YkModelPagedPatch::GetIndex()
{
	return m_nIndex;
}

YkModelPagedLOD* YkModelPagedPatch::GetPagedLOD()
{
	return m_pPagedLOD;
}

void YkModelPagedPatch::SetPagedLOD(YkModelPagedLOD* pPageLOD)
{
	m_pPagedLOD = pPageLOD;
}

void YkModelPagedPatch::SetIndex(YkInt nIndex)
{
	m_nIndex = nIndex;
}

void YkModelPagedPatch::SetParentPatch(YkModelPagedPatch* pPatch)
{
	m_pPatchParent = pPatch;
	m_nPatchParentIndex = (m_pPatchParent==NULL) ? -1 : m_pPatchParent->GetIndex();
}

YkModelPagedPatch* YkModelPagedPatch::GetParentPatch()
{
	return m_pPatchParent;
}

void YkModelPagedPatch::ReleaseGeode(YkModelGeode * pGeode)
{
	for (YkInt i=0; i<m_vecGeode.size(); i++)
	{
		if(m_vecGeode[i] == pGeode)
		{
			delete pGeode;
			pGeode = NULL;

			m_vecGeode.erase(m_vecGeode.begin() + i);
			break;
		}
	}
	m_BoundingSphere.m_radius = 0;
}


void YkModelPagedPatch::SetParentIndex(const YkInt nPatchParentIndex)
{
	m_nPatchParentIndex = nPatchParentIndex;
}

YkInt YkModelPagedPatch::GetParentIndex()
{
	if(m_pPatchParent != NULL)
	{
		m_nPatchParentIndex = m_pPatchParent->GetIndex();
	}
	return m_nPatchParentIndex;
}

void YkModelPagedPatch::SetChildrenIndex(const std::vector<YkInt> &vecChildrenIndex)
{
	m_vecChildrenIndex.clear();
	m_vecChildrenIndex.insert(m_vecChildrenIndex.begin(), vecChildrenIndex.begin(), vecChildrenIndex.end());
}

std::vector<YkInt>& YkModelPagedPatch::GetChildrenIndex()
{
	if(m_vecChildren.size() > 0)
	{
		m_vecChildrenIndex.clear();
		for (YkInt i=0; i<m_vecChildren.size(); i++)
		{
			m_vecChildrenIndex.push_back(m_vecChildren[i]->GetIndex());
		}
	}
	return m_vecChildrenIndex;
}

//////////////////////////////////////////////////////////////////////////
YkModelPagedLOD::YkModelPagedLOD()
{
	m_BoundingSphere.m_radius = 0.0;
	m_nLODNum = -1;
}

YkModelPagedLOD::YkModelPagedLOD(YkInt nLOD)
{
	m_nRangeMode = PIXEL_SIZE_ON_SCREEN;
	m_BoundingSphere.m_radius = 0.0;
	m_nLODNum = nLOD;
}

void YkModelPagedLOD::MakeFrom(YkModelPagedLOD *pPagedLOD)
{
	m_nRangeMode = pPagedLOD->GetRangeMode();
	m_BoundingSphere = pPagedLOD->GetBoudingSphere();
	m_nLODNum = pPagedLOD->GetLODNum();

	m_vecPagedPatches.clear();
	for (YkInt i=0; i<pPagedLOD->GetPagedPatchCount(); i++)
	{
		YkModelPagedPatch* pPatch = new YkModelPagedPatch();
		pPatch->MakeFrom(this, pPagedLOD->GetPagedPatch(i));
		m_vecPagedPatches.push_back(pPatch);
	}
}

YkModelPagedLOD::~YkModelPagedLOD()
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		YkModelPagedPatch* pPagedLod = m_vecPagedPatches[i];
		delete pPagedLod;
		pPagedLod = NULL;
	}
	m_vecPagedPatches.clear();	
	m_BoundingSphere.m_radius = 0;
}

YkModelPagedPatch* YkModelPagedLOD::CreatePagedPatch(YkFloat fDistance)
{
	YkModelPagedPatch* pPagedPatch = new YkModelPagedPatch(this, fDistance);
	pPagedPatch->SetIndex(m_vecPagedPatches.size());
	m_vecPagedPatches.push_back(pPagedPatch);
	return pPagedPatch;
}

void YkModelPagedLOD::AddPagedPatch(YkModelPagedPatch* pPagedPatch)
{
	pPagedPatch->SetIndex(m_vecPagedPatches.size());
	pPagedPatch->SetPagedLOD(this);
	m_vecPagedPatches.push_back(pPagedPatch);
}

YkBool YkModelPagedLOD::ReleasePagedPatch(YkModelPagedPatch *pPatch)
{
	YkInt nIndex = -1;
	for(YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		if(m_vecPagedPatches[i] == pPatch)
		{
			//父节点处理
			YkModelPagedPatch* pPatchParent = pPatch->GetParentPatch();
			if(pPatchParent != NULL)
			{
				pPatchParent->RemoveChildren(pPatch);
			}
			//子节点处理
			for (YkInt j=pPatch->GetChildCount()-1; j>=0; j--)
			{
				YkModelPagedPatch* pPatchChild = pPatch->GetChild(j);
				YkModelPagedLOD* pPagedLODNext = pPatchChild->GetPagedLOD();
				pPagedLODNext->ReleasePagedPatch(pPatchChild);
			}
			pPatch->RemoveChildren();
			delete pPatch;

			m_vecPagedPatches.erase(m_vecPagedPatches.begin() + i);
			nIndex = i;
			break;
		}
	}

	for(YkInt i=nIndex; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[nIndex]->SetIndex(m_vecPagedPatches[nIndex]->GetIndex()-1);
	}
	return nIndex > 0;
}

void YkModelPagedLOD::GetElementNames(std::vector<YkString>& vecNames, YkModelElement::METype metype)
{
	std::vector<YkString> vecNamesTemp;
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->GetElementNames(vecNames, metype);
	}
}

void YkModelPagedLOD::GetSkeletons(std::map<YkString, YkModelSkeletonPtr>& mapElement)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->GetSkeletons(mapElement);
	}
}

void YkModelPagedLOD::GetMaterials(std::map<YkString, YkModelMaterialPtr>& mapElement)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->GetMaterials(mapElement);
	}
}

void YkModelPagedLOD::GetTextures(std::map<YkString, YkModelTexturePtr>& mapElement)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->GetTextures(mapElement);
	}
}

YkModelSkeletonPtr YkModelPagedLOD::GetSkeleton(const YkString strName)
{
	YkModelSkeletonPtr pElement;
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		pElement = m_vecPagedPatches[i]->GetSkeleton(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	return pElement;
}

YkModelMaterialPtr YkModelPagedLOD::GetMaterial(const YkString strName)
{
	YkModelMaterialPtr pElement;
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		pElement = m_vecPagedPatches[i]->GetMaterial(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	return pElement;
}

YkModelTexturePtr YkModelPagedLOD::GetTexture(const YkString strName)
{
	YkModelTexturePtr pElement;
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		pElement = m_vecPagedPatches[i]->GetTexture(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	return pElement;
}

//! \brief 名字是否已经存在
YkBool YkModelPagedLOD::IsNameExisted(const YkString strName)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{	
		if(m_vecPagedPatches[i]->IsNameExisted(strName))
		{
			return TRUE;
		}
	}
	return FALSE;
}

void YkModelPagedLOD::GetLODRanges(std::vector<YkFloat>& vecDis)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		vecDis.push_back(m_vecPagedPatches[i]->GetLODDistance());
	}
}

YkInt YkModelPagedLOD::GetPagedPatchCount()
{
	return m_vecPagedPatches.size();
}

YkModelPagedPatch* YkModelPagedLOD::GetPagedPatch(YkInt nIndex)
{
	if(nIndex < 0 || nIndex >= m_vecPagedPatches.size())
	{
		return NULL;
	}
	return m_vecPagedPatches[nIndex];
}

std::vector<YkModelPagedPatch*>& YkModelPagedLOD::GetPagedPatchesRef()
{
	return m_vecPagedPatches;
}

YkFloat YkModelPagedLOD::GetLODDistance(YkInt nPatchIdex) const
{
	if(m_vecPagedPatches.size() == 0)
	{
		return 0.0;
	}

	if(nPatchIdex >= 0 && nPatchIdex < m_vecPagedPatches.size())
	{
		return m_vecPagedPatches[nPatchIdex]->GetLODDistance();
	}
	YkFloat fTotal = m_vecPagedPatches[0]->GetLODDistance();
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		fTotal = YKMAX(m_vecPagedPatches[i]->GetLODDistance(), fTotal);
	}
	return fTotal;
}

std::vector<YkModelGeode *> YkModelPagedLOD::GetGeodes()
{
	std::vector<YkModelGeode *> vecGeodes;
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		std::vector<YkModelGeode *> vecTemp = m_vecPagedPatches[i]->GetGeodes();
		vecGeodes.insert(vecGeodes.begin(), vecTemp.begin(), vecTemp.end());
	}
	return vecGeodes;
}

YkInt YkModelPagedLOD::GetShellDataSize()
{
	YkInt nSize = 0;
	YkInt nCount = m_vecPagedPatches.size();
	nSize += sizeof(nCount);
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelPagedPatch* pPatch = m_vecPagedPatches[i];
		YKASSERT(pPatch != NULL);
		nSize += pPatch->GetShellDataSize();
	}
	return nSize;
}



YkRangeMode YkModelPagedLOD::GetRangeMode()
{
	return m_nRangeMode;
}

YkBool YkModelPagedLOD::Save(YkStream& stream)
{
	YkShort nRM = m_nRangeMode; 
	stream << nRM;

	YKASSERT(m_nLODNum > -1);
	stream << m_nLODNum;

	YkInt nCount = m_vecPagedPatches.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelPagedPatch* pPagedPatch = m_vecPagedPatches[i];
		YKASSERT(pPagedPatch != NULL);
		pPagedPatch->Save(stream);
	}
	return TRUE;
}

YkBool YkModelPagedLOD::Load(YkStream& stream)
{
	YkShort nRM = 0; 
	stream >> nRM;
	m_nRangeMode = (YkRangeMode)nRM;

	stream >> m_nLODNum;
	YKASSERT(m_nLODNum > -1);

	YkInt nCount = 0;
	stream >> nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelPagedPatch* pPagedPatch = new YkModelPagedPatch(this, 0.0);
		pPagedPatch->Load(stream);
		m_vecPagedPatches.push_back(pPagedPatch);
	}
	return TRUE;
}

YkBool YkModelPagedLOD::LoadElements(YkModelEM* pEM, YkModelElement::METype mtype, YkInt nPatchIndex)
{
	if(nPatchIndex == -1)
	{
		for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
		{
			m_vecPagedPatches[i]->LoadElements(pEM, mtype);
		}
	}
	else if(nPatchIndex >= 0 && nPatchIndex < m_vecPagedPatches.size() )
	{
		m_vecPagedPatches[nPatchIndex]->LoadElements(pEM, mtype);
	}
	else
	{
		YKASSERT(FALSE);
	}

	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	return TRUE;
}

void YkModelPagedLOD::SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius )
{
	m_BoundingSphere.m_center = vecCenter;
	m_BoundingSphere.m_radius = dRadius;
}

YkBoundingSphere YkModelPagedLOD::GetBoudingSphere()
{
	if(!m_BoundingSphere.valid())
	{
		ComputeBoundingSphere();
	}

	return m_BoundingSphere;
}

void YkModelPagedLOD::ComputeBoundingSphere()
{
	m_BoundingSphere.m_radius = 0;

	//开始计算
	YkBoundingSphere bBoundsPatch;

	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->ComputeBoundingSphere();
		bBoundsPatch = m_vecPagedPatches[i]->GetBoudingSphere();
		if(bBoundsPatch.valid())
		{
			m_BoundingSphere.ExpandBy(bBoundsPatch);
		}
	}
}

//! \brief 作用于各Geode的矩阵
void YkModelPagedLOD::ReMultiplyMatrix(const YkMatrix4d& m)
{
	for (YkInt i=0; i<m_vecPagedPatches.size(); i++)
	{
		m_vecPagedPatches[i]->ReMultiplyMatrix(m);
	}
	m_BoundingSphere.m_radius = 0;
}

YkInt YkModelPagedLOD::GetLODNum() const
{
	return m_nLODNum;
}



}