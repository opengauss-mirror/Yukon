/*
 *
 * YkModelNode.cpp
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

#include "Base3D/YkModelNode.h"
#include "Toolkit/YkHashCode.h"

namespace Yk
{
struct vectorLess
{
	bool operator()(const YkVector4d v1, const YkVector4d v2) const
	{
		if (v1.x < v2.x) return true;
		if (v1.x == v2.x && v1.y < v2.y) return true;
		if (v1.x == v2.x && v1.y == v2.y && v1.z < v2.z) return true;

		return false;
	}
};

YkModelNode::YkModelNode()
{
	m_bElementLoaded = FALSE;
}

YkModelNode::~YkModelNode()
{
	Clear();
}

void YkModelNode::Clear(YkBool bRelease)
{
	if(!bRelease)
	{
		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			m_vecDataPatches[i]->RemoveAllGeodes();
		}
	}

	for (YkInt i=0; i<m_vecDataPatches.size(); i++)
	{
		delete m_vecDataPatches[i];
		m_vecDataPatches[i] = NULL;
	}
	m_vecDataPatches.clear();

	for (YkInt i=0; i<(YkInt)m_vecPagedLODs.size(); i++)
	{
		YkModelPagedLOD* pPagedLod = m_vecPagedLODs[i];
		delete pPagedLod;
		pPagedLod = NULL;
	}
	m_vecPagedLODs.clear();
	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	m_bElementLoaded = FALSE;
}

YkModelNode& YkModelNode::operator = (YkModelNode& modelNode)
{
	Clear();

	YkModelEMapPack mapDes;

	//载Shell
	YkInt nLods = modelNode.GetPagedLODs().size();
	for (YkInt i=0; i<nLods; i++)
	{
		YkModelPagedLOD* pPagedLodSrc = modelNode.GetPagedLOD(i);	
		YkModelPagedLOD* pPagedLodDes = AddPagedLOD();
		pPagedLodDes->MakeFrom(pPagedLodSrc);
	}

	for (YkInt j=0; j<modelNode.GetDataPatcheCount(); j++)
	{
		YkModelPagedPatch* pPatchSrc = modelNode.GetDataPatche(j);
		YkModelPagedPatch* pPatchDes = CreatePatch();
		pPatchDes->MakeFrom(NULL, pPatchSrc);
	}

	MakeTree();

	m_BoundingSphere = modelNode.GetBoudingSphere();

	m_bElementLoaded = modelNode.m_bElementLoaded;
	return *this;
}

YkBool YkModelNode::SortPagedLOD(const YkModelPagedLOD* p1,const YkModelPagedLOD* p2)
{
	return p1->GetLODNum() < p2->GetLODNum();
}

void YkModelNode::SortPagedLODs()
{
	if(m_vecPagedLODs.size() == 0)
	{
		return;
	}
	std::sort(m_vecPagedLODs.begin(), m_vecPagedLODs.end(), YkModelNode::SortPagedLOD);
}

YkModelPagedLOD* YkModelNode::GetPagedLOD(YkInt nLODIndex)
{
	if(nLODIndex >= 0 && nLODIndex < m_vecPagedLODs.size())
	{
		return m_vecPagedLODs[nLODIndex];
	}
	return NULL;
}

YkModelPagedLOD* YkModelNode::AddPagedLOD()
{	
	YkModelPagedLOD* pPagedLOD = new YkModelPagedLOD(m_vecPagedLODs.size());
	m_vecPagedLODs.push_back(pPagedLOD);
	return pPagedLOD;
}

YkBoundingBox YkModelNode::ComputeBoudingBox()
{
	YkBoundingBox bbox;
	YkInt nCount = m_vecDataPatches.size();
	for (YkInt i=0; i<nCount; i++)
	{
		YkBoundingBox bboxPatch = m_vecDataPatches[i]->ComputeBoudingBox();
		if(!bboxPatch.IsNULL())
		{
			bbox.Merge(bboxPatch);
		}
	}
	return bbox;
}

YkBoundingSphere YkModelNode::ComputeBoudingSphere()
{
	YkBoundingSphere boundSphere;

	YkInt nCount = m_vecPagedLODs.size();
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecPagedLODs[i]->ComputeBoundingSphere();
		YkBoundingSphere b = m_vecPagedLODs[i]->GetBoudingSphere();
		//内部会判断合法性
		boundSphere.ExpandBy(b);
	}

	nCount = m_vecDataPatches.size();
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecDataPatches[i]->ComputeBoundingSphere();
		YkBoundingSphere b = m_vecDataPatches[i]->GetBoudingSphere();
		boundSphere.ExpandBy(b);
	}

	return boundSphere;
}

void YkModelNode::GetElementNames(YkInt nLOD, \
								 std::vector<YkString>& vecNamesResult, YkModelElement::METype metype)
{
	if(nLOD == -1)
	{
		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			std::vector<YkString> vecNames;
			m_vecDataPatches[i]->GetElementNames(vecNames, metype);

			vecNamesResult.insert(vecNamesResult.end(),
				vecNames.begin(), vecNames.end());
		}
	}
	else if(nLOD >= 0 && nLOD < m_vecPagedLODs.size())
	{
		YkModelPagedLOD* pPaged = GetPagedLOD(nLOD);
		if(pPaged == NULL)
		{
			return ;
		}
		std::vector<YkString> vecNames;
		pPaged->GetElementNames(vecNames , metype);

		vecNamesResult.insert(vecNamesResult.end(),
			vecNames.begin(), vecNames.end());
	}
	else if(nLOD == -3)//所有LOD层
	{
		YkInt nPagedCount = m_vecPagedLODs.size();
		for(YkInt i=0; i<nPagedCount; i++)
		{
			YkModelPagedLOD* pPaged = m_vecPagedLODs[i];
			YKASSERT(pPaged != NULL);
			std::vector<YkString> vecNames;
			pPaged->GetElementNames(vecNames, metype);

			vecNamesResult.insert(vecNamesResult.end(),
				vecNames.begin(), vecNames.end());
		}
	}
	else
	{
		YkInt nPagedCount = m_vecPagedLODs.size();
		for(YkInt i=0; i<nPagedCount; i++)
		{
			YkModelPagedLOD* pPaged = m_vecPagedLODs[i];
			YKASSERT(pPaged != NULL);

			std::vector<YkString> vecNames;
			pPaged->GetElementNames(vecNames, metype);

			vecNamesResult.insert(vecNamesResult.end(),
				vecNames.begin(), vecNames.end());
		}

		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			std::vector<YkString> vecNames;
			m_vecDataPatches[i]->GetElementNames(vecNames, metype);

			vecNamesResult.insert(vecNamesResult.end(),
				vecNames.begin(), vecNames.end());
		}
	}
}

void YkModelNode::GetElementHashCodes(const YkInt nLOD, std::vector<YkLong>& vecHashCodes, \
						YkBool bHash64, YkModelElement::METype metype)
{
	std::vector<YkString> vecNames;
	GetElementNames(nLOD, vecNames, metype);

	YkLong lHashCode = 0;
	for (YkInt i=0; i<vecNames.size(); i++)
	{
		if(!bHash64)
		{
			lHashCode = YkHashCode::FastStringToHashCode(vecNames[i]);
		}
		else
		{
			lHashCode = YkHashCode::StringToHashCode64(vecNames[i]);
		}
		vecHashCodes.push_back(lHashCode);
	}
}

void YkModelNode::ReMultiplyMatrix(const YkMatrix4d& m)
{
	YkInt nCount = m_vecPagedLODs.size();
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecPagedLODs[i]->ReMultiplyMatrix(m);
	}

	nCount = m_vecDataPatches.size();
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecDataPatches[i]->ReMultiplyMatrix(m);
	}
	m_BoundingSphere.m_radius = 0;
}

YkModelPagedPatch* YkModelNode::CreatePatch()
{
	YkModelPagedPatch* pPatch = new YkModelPagedPatch();
	pPatch->SetIndex(m_vecDataPatches.size());
	m_vecDataPatches.push_back(pPatch);
	return pPatch;
}

YkInt YkModelNode::GetDataPatcheCount() const
{
	return m_vecDataPatches.size();
}

YkModelPagedPatch* YkModelNode::GetDataPatche(YkInt nIndex)
{
	if(nIndex < 0 || nIndex >= m_vecDataPatches.size())
	{
		return NULL;
	}
	return m_vecDataPatches[nIndex];
}

std::vector<YkModelPagedLOD*>& YkModelNode::GetPagedLODs() 
{
	return m_vecPagedLODs;
}

YkInt YkModelNode::GetShellDataSize()
{
	YkInt nSize = 0;
	YkInt nCount = m_vecPagedLODs.size();
	//nCount 
	nSize += sizeof(YkInt);
	for (YkInt i=0; i<nCount; i++)
	{
		nSize += m_vecPagedLODs[i]->GetShellDataSize();
	}

	nCount = m_vecDataPatches.size();
	nSize += sizeof(YkInt);
	for (YkInt i=0; i<nCount; i++)
	{
		nSize += m_vecDataPatches[i]->GetShellDataSize();
	}
	return nSize;
}

YkBool YkModelNode::SaveShells(YkStream& stream)
{
	SortPagedLODs();

	YkInt nCount = m_vecPagedLODs.size();
	stream << nCount;
	//倒序存储
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecPagedLODs[i]->Save(stream);
	}

	nCount = m_vecDataPatches.size();
	stream << nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		m_vecDataPatches[i]->Save(stream);
	}
	return TRUE;
}

YkBool YkModelNode::LoadShells(YkStream& stream)
{
	Clear();

	YkInt nCount = 0;
	stream >> nCount;
	for (YkInt i=0; i<nCount; i++)
	{
		YkModelPagedLOD *pPagedLOD = new YkModelPagedLOD();
		pPagedLOD->Load(stream);

		m_vecPagedLODs.push_back(pPagedLOD);
	}

	stream >> nCount;
	YkModelPagedPatch *pPatch = NULL;
	for (YkInt i=0; i<nCount; i++)
	{
		pPatch = new YkModelPagedPatch();
		pPatch->Load(stream);

		m_vecDataPatches.push_back(pPatch);
	}

	MakeTree();

	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;

	return TRUE;
}

void YkModelNode::MakeTree()
{
	for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
	{
		YkModelPagedLOD *pPagedLOD = m_vecPagedLODs[i];
		for (YkInt j=0; j<pPagedLOD->GetPagedPatchCount(); j++)
		{
			YkModelPagedPatch* pPatch = pPagedLOD->GetPagedPatch(j);
			pPatch->LoadPatchTree(this);
		}
	}
	for (YkInt j=0; j<m_vecDataPatches.size(); j++)
	{
		m_vecDataPatches[j]->LoadPatchTree(this);
	}
}

YkBool YkModelNode::LoadElements(YkModelEM* pEM,YkInt nLOD, YkModelElement::METype mtype)
{
	YKASSERT(pEM != NULL);

	if(nLOD == -1)
	{
		YkInt nCount = m_vecDataPatches.size();
		for (YkInt i=0; i<nCount; i++)
		{
			m_vecDataPatches[i]->LoadElements(pEM, mtype);
		}
		m_bElementLoaded = TRUE;
	}
	else if(nLOD >= 0 && nLOD < m_vecPagedLODs.size())
	{
		m_vecPagedLODs[nLOD]->LoadElements(pEM, mtype);
	}
	else 
	{
		YkInt nCount = m_vecPagedLODs.size();
		for (YkInt i=0; i<nCount; i++)
		{
			m_vecPagedLODs[i]->LoadElements(pEM, mtype);
		}

		nCount = m_vecDataPatches.size();
		for (YkInt i=0; i<nCount; i++)
		{
			m_vecDataPatches[i]->LoadElements(pEM, mtype);
		}
		m_bElementLoaded = TRUE;
	}

	//获取范围时，重新计算
	m_BoundingSphere.m_radius = 0;
	m_BoundingSphere = ComputeBoudingSphere();
	return TRUE;
}

YkModelMaterialPtr YkModelNode::GetMaterial(const YkString strName)
{	
	YkModelMaterialPtr pElement;
	for (YkInt i=0; i<m_vecDataPatches.size(); i++)
	{
		YkModelPagedPatch *pPatch = m_vecDataPatches[i];
		pElement = pPatch->GetMaterial(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	if(pElement.IsNull())
	{
		for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
		{
			pElement = m_vecPagedLODs[i]->GetMaterial(strName);
			if(!pElement.IsNull())
			{
				break;
			}
		}
	}
	return pElement;
}

YkModelTexturePtr YkModelNode::GetTexture(const YkString strName)
{
	YkModelTexturePtr pElement;
	for (YkInt i=0; i<m_vecDataPatches.size(); i++)
	{
		YkModelPagedPatch *pPatch = m_vecDataPatches[i];
		pElement = pPatch->GetTexture(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	if(pElement.IsNull())
	{
		for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
		{
			pElement = m_vecPagedLODs[i]->GetTexture(strName);
			if(!pElement.IsNull())
			{
				break;
			}
		}
	}
	return pElement;
}

YkModelSkeletonPtr YkModelNode::GetSkeleton(const YkString strName)
{
	YkModelSkeletonPtr pElement;
	for (YkInt i=0; i<m_vecDataPatches.size(); i++)
	{
		YkModelPagedPatch *pPatch = m_vecDataPatches[i];
		pElement = pPatch->GetSkeleton(strName);
		if(!pElement.IsNull())
		{
			break;
		}
	}
	if(pElement.IsNull())
	{
		for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
		{
			pElement = m_vecPagedLODs[i]->GetSkeleton(strName);
			if(!pElement.IsNull())
			{
				break;
			}
		}
	}
	return pElement;
}

YkBool YkModelNode::IsNameExisted(const YkString strName)
{
	for (YkInt i=0; i<m_vecDataPatches.size(); i++)
	{
		YkModelPagedPatch *pPatch = m_vecDataPatches[i];
		if(pPatch->IsNameExisted(strName))
		{
			return TRUE;
		}
	}
	for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
	{
		if(m_vecPagedLODs[i]->IsNameExisted(strName))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//! \brief 给一个可用的名字
YkString YkModelNode::GetUnoccupiedName(YkString strName)
{
	YkString strResult = strName;
	YkInt nRecur = 0;
	while(IsNameExisted(strResult))
	{
		strResult.Format(_U("%s_%d"), strName.Cstr(), nRecur++);
	}
	return strResult;
}

void YkModelNode::GetMapMaterial3D(std::map<YkString, YkModelMaterialPtr>& mapMaterial3D)
{
	GetMapMaterial3DByLOD(-1, mapMaterial3D);
	for (YkInt i=0; i< m_vecPagedLODs.size(); i++)
	{
		GetMapMaterial3DByLOD(i, mapMaterial3D);
	}
}

void YkModelNode::GetMapTextureData(std::map<YkString, YkModelTexturePtr>& mapTexutreData)
{
	GetMapTextureDataByLOD(-1, mapTexutreData);
	for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
	{
		GetMapTextureDataByLOD(i, mapTexutreData);
	}
}

void YkModelNode::GetMapSkeleton(std::map<YkString, YkModelSkeletonPtr>& mapSkeleton)
{
	GetMapSkeletonByLOD(-1, mapSkeleton);
	for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
	{
		GetMapSkeletonByLOD(i, mapSkeleton);
	}
}

void YkModelNode::GetEMapPack(YkModelEMapPack &mapPack)
{
	GetMapSkeleton(mapPack.m_mapSkeleton);
	GetMapMaterial3D(mapPack.m_mapMaterial);
	GetMapTextureData(mapPack.m_mapTexture);
}

void YkModelNode::RemoveLODModel()
{
	for (YkInt i=0; i<m_vecPagedLODs.size(); i++)
	{
		YkModelPagedLOD* pPagedLod = m_vecPagedLODs[i];
		delete pPagedLod;
		pPagedLod = NULL;
	}
	m_vecPagedLODs.clear();

	for (YkInt i=0; i<m_vecDataPatches.size();i++)
	{
		YkModelPagedPatch* pPatch = m_vecDataPatches[i];
		pPatch->SetParentPatch(NULL);
	}
}

YkBool YkModelNode::GetSkeleton(const YkCSkeletonID id, 
				   YkModelSkeleton*& pSkeleton, YkMatrix4d& matGeode)
{
	if(id.IsValid() && pSkeleton != NULL)
	{
		YKASSERT(FALSE);
		return FALSE;
	}
	YkModelPagedPatch* pPatch = NULL;
	if(id.m_nIdx_LOD == -1)
	{
		pPatch = GetDataPatche(id.m_nIdx_Patch);
	}
	else
	{
		YKASSERT(id.m_nIdx_LOD >= 0);
		YkModelPagedLOD* pPageLOD = GetPagedLOD(id.m_nIdx_LOD);
		YKASSERT(pPageLOD != NULL);

		pPatch = pPageLOD->GetPagedPatch(id.m_nIdx_Patch);
	}
	YKASSERT(pPatch != NULL);

	YkModelGeode* pGeode = pPatch->GetGeode(id.m_nIdx_Geode);
	YKASSERT(pGeode != NULL);

	matGeode = pGeode->GetPosition();
	std::vector<YkString> vecNames = pGeode->GetElementNames(YkModelElement::etSkeleton);
	pSkeleton = pGeode->GetSkeleton(vecNames[id.m_nIdx_Skeleton]).Get();
	YKASSERT(pSkeleton != NULL);

	return pSkeleton != NULL;
}

YkBool YkModelNode::RemoveSkeleton(YkCSkeletonID id)
{
	if(!id.IsValid())
	{
		YKASSERT(FALSE);
		return FALSE;
	}
	YkModelPagedPatch* pPatch = NULL;
	if(id.m_nIdx_LOD == -1)
	{
		pPatch = GetDataPatche(id.m_nIdx_Patch);
	}
	else
	{
		YKASSERT(id.m_nIdx_LOD >= 0);
		YkModelPagedLOD* pPageLOD = GetPagedLOD(id.m_nIdx_LOD);
		YKASSERT(pPageLOD != NULL);

		pPatch = pPageLOD->GetPagedPatch(id.m_nIdx_Patch);
	}
	YKASSERT(pPatch != NULL);
	if(pPatch == NULL)
	{
		return FALSE;
	}
	YkModelGeode* pGeode = pPatch->GetGeode(id.m_nIdx_Geode);
	YKASSERT(pGeode != NULL);
	if(pGeode == NULL)
	{
		return FALSE;
	}
	std::vector<YkString> vecNames = pGeode->GetElementNames(YkModelElement::etSkeleton);
	if(pGeode->RemoveSkeleton(vecNames[id.m_nIdx_Skeleton]))
	{
		if(pGeode->GetElementNum(YkModelElement::etUnknown) == 0)
		{
			pPatch->ReleaseGeode(pGeode);
		}
		return TRUE;
	}
	return FALSE;
}

void YkModelNode::GetMapMaterial3DByLOD(YkInt nLOD, std::map<YkString, YkModelMaterialPtr>& mapMaterial3D)
{
	if (nLOD < 0)
	{
		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			YkModelPagedPatch *pPatch = m_vecDataPatches[i];
			pPatch->GetMaterials(mapMaterial3D);
		}
	}
	else
	{
		YkInt nPageLODSize = m_vecPagedLODs.size();
		if (nLOD >= nPageLODSize || m_vecPagedLODs[nLOD] == NULL)
		{
			return;
		}
		YkModelPagedLOD* pPagedLOD = m_vecPagedLODs[nLOD];
		pPagedLOD->GetMaterials(mapMaterial3D);
	}
}

void YkModelNode::GetMapTextureDataByLOD(YkInt nLOD, std::map<YkString, YkModelTexturePtr>& mapTexutreData)
{
	if (nLOD < 0)
	{
		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			YkModelPagedPatch *pPatch = m_vecDataPatches[i];
			pPatch->GetTextures(mapTexutreData);
		}
	}
	else
	{
		YkInt nPageLODSize = m_vecPagedLODs.size();
		if (nLOD >= nPageLODSize || m_vecPagedLODs[nLOD] == NULL)
		{
			return;
		}
		YkModelPagedLOD* pPagedLOD = m_vecPagedLODs[nLOD];
		pPagedLOD->GetTextures(mapTexutreData);
	}
}

void YkModelNode::GetMapSkeletonByLOD(YkInt nLOD, std::map<YkString, YkModelSkeletonPtr>& mapSkeleton)
{
	if (nLOD < 0)
	{
		for (YkInt i=0; i<m_vecDataPatches.size(); i++)
		{
			YkModelPagedPatch *pPatch = m_vecDataPatches[i];
			pPatch->GetSkeletons(mapSkeleton);
		}
	}
	else
	{
		YkInt nPageLODSize = m_vecPagedLODs.size();
		if (nLOD >= nPageLODSize || m_vecPagedLODs[nLOD] == NULL)
		{
			return;
		}
		YkModelPagedLOD* pPagedLOD = m_vecPagedLODs[nLOD];
		pPagedLOD->GetSkeletons(mapSkeleton);
	}
}

void YkModelNode::SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius )
{
	m_BoundingSphere.m_center = vecCenter;
	m_BoundingSphere.m_radius = dRadius;
}

YkBoundingSphere YkModelNode::GetBoudingSphere()
{
	if(!m_BoundingSphere.valid())
	{
		YkBoundingSphere boundSphere = ComputeBoudingSphere();
		if(boundSphere.valid())
		{
			SetBoudingSphere(boundSphere.GetCenter(), boundSphere.GetRadius());
		}
	}
	return m_BoundingSphere;
}


void YkModelNode::AppendByPatch(YkModelNode* pModelNode, YkBool bIgnoreLOD)
{
	//LOD合并使用
	YkInt nLODNum = m_vecPagedLODs.size();
	std::map<YkInt, YkInt> mapOrgLODPatchCount;
	mapOrgLODPatchCount[-1] = m_vecDataPatches.size();
	for (YkInt i=0; i<nLODNum; i++)
	{
		mapOrgLODPatchCount[i] = m_vecPagedLODs[i]->GetPagedPatchCount();
	}

	//精细层
	for (YkInt i=0; i<pModelNode->GetDataPatcheCount(); i++)
	{
		YkModelPagedPatch* pPatchSrc = pModelNode->GetDataPatche(i);
		YkModelPagedPatch* pPatchDes = CreatePatch();
		pPatchDes->SetFileName(pPatchSrc->GetFileName());
		//数据追加
		for (YkInt k=0; k<pPatchSrc->GetGeodeCount(); k++)
		{
			YkModelGeode* pGeodeSrc = pPatchSrc->GetGeode(k);
			YkModelGeode* pGeodeDes = new YkModelGeode(*pGeodeSrc);
			pPatchDes->AddGeode(pGeodeDes);
		}
	}

	//如果LOD层数一样，则合并
	if(bIgnoreLOD || pModelNode->GetPagedLODs().size() != nLODNum)
	{
		return;
	}
	//LOD对应关系
	for (YkInt i=0; i<nLODNum; i++)
	{
		YkModelPagedLOD* pPagedLODDes = m_vecPagedLODs[i];
		YkModelPagedLOD* pPagedLODSrc = pModelNode->GetPagedLOD(i);
		for (YkInt j=0; j<pPagedLODSrc->GetPagedPatchCount(); j++)
		{
			YkModelPagedPatch* pPatchSrc = pPagedLODSrc->GetPagedPatch(j);
			YkModelPagedPatch* pPatchDes = pPagedLODDes->CreatePagedPatch(pPatchSrc->GetLODDistance());
			//数据追加
			for (YkInt k=0; k<pPatchSrc->GetGeodeCount(); k++)
			{
				YkModelGeode* pGeodeSrc = pPatchSrc->GetGeode(k);
				YkModelGeode* pGeodeDes = new YkModelGeode(*pGeodeSrc);
				pPatchDes->AddGeode(pGeodeDes);
			}

			//父子关系维护
			std::vector<YkInt> vecChildIndexSrc = pPatchSrc->GetChildrenIndex();
			std::vector<YkInt> vecChildIndexDes;
			for (YkInt k=0; k<vecChildIndexSrc.size(); k++)
			{
				vecChildIndexDes.push_back(mapOrgLODPatchCount[i-1] + vecChildIndexSrc[k]);
			}
			pPatchDes->SetChildrenIndex(vecChildIndexDes);
			if(i != nLODNum-1)//非顶层
			{
				//当前层的父层
				pPatchDes->SetParentIndex(mapOrgLODPatchCount[i+1] + \
					pPatchSrc->GetParentIndex());
			}
		}			
	}
	MakeTree();
}


}
