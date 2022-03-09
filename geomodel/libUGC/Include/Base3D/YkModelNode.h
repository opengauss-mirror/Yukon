/*
 *
 * YkModelNode.h
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

#if !defined(AFX_YKMODELNODE_H__79FA0D5A_DADB_4A32_A36A_1BAB9F1342F2__INCLUDED_)
#define AFX_YKMODELNODE_H__79FA0D5A_DADB_4A32_A36A_1BAB9F1342F2__INCLUDED_

#pragma once

#include "Base3D/YkSharedPtr.h"
#include "Base3D/YkModelNodeElements.h"
#include "Base3D/YkModelNodeShells.h"
#include "Base3D/YkModelEM.h"

namespace Yk {
class YkModelPagedLOD;
class YkModelNode;

class BASE3D_API YkModelNode
{
	friend class YkModelNodeTools;

public:
	//! \brief 构造函数
	YkModelNode();

	~YkModelNode();

public:
	YkModelNode& operator = (YkModelNode& other);
	
	//! \brief 按Patch追加。对象合并使用
	void AppendByPatch(YkModelNode* pModelNode, YkBool bIgnoreLOD);

	//! \brief 创建精细层Patch
	YkModelPagedPatch* CreatePatch();

	//! \brief 清空数据
	void Clear(YkBool bRelease = TRUE);

	//! \brief 得到LOD
	YkModelPagedLOD* GetPagedLOD(YkInt nLODNum);

	YkModelPagedLOD* AddPagedLOD();

	//! \brief 最精细层
	YkInt GetDataPatcheCount() const;

	//! \brief 最精细层
	YkModelPagedPatch* GetDataPatche(YkInt nIndex);

	std::vector<YkModelPagedLOD*>& GetPagedLODs();

	YkInt GetShellDataSize();

	YkBool SaveShells(YkStream& stream);

	YkBool LoadShells(YkStream& stream);

	//! \brief 包围盒
	void SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius );

	//! \brief 包围盒
	YkBoundingSphere GetBoudingSphere();

	YkBoundingSphere ComputeBoudingSphere();

	//! \brief 实时计算出准确的脚点
	YkBoundingBox ComputeBoudingBox();

	//! \brief 获取实体对象名称
	//! \brief nLOD=-2-->LOD+Geodes; nLOD=-1-->Geodes; nLOD=0-n -->LOD(i);
	void GetElementHashCodes(const YkInt nLOD,
		std::vector<YkLong>& vecHashCodes, YkBool bHash64, YkModelElement::METype metype);

	//! \brief 获取实体对象名称
	//! \brief nLOD=-2-->LOD+Geodes; nLOD=-1-->Geodes; nLOD=0-n -->LOD(i);
	void GetElementNames(YkInt nLOD, std::vector<YkString>& vecNames, YkModelElement::METype metype);

	//! \brief 作用于各Geode的矩阵
	void ReMultiplyMatrix(const YkMatrix4d& m);

	//////////////////////////////////////////////////////////////////////////

	//! \brief 仅精细层：nLOD=-1; 第n层：nLOD=n; 全部加载nLOD=-2
	YkBool LoadElements(YkModelEM* pEM, YkInt nLOD = -1,YkModelElement::METype mtype = YkModelElement::etUnknown);

	//! \brief 取实体
	YkModelMaterialPtr GetMaterial(const YkString strName);
	YkModelTexturePtr GetTexture(const YkString strName);
	YkModelSkeletonPtr GetSkeleton(const YkString strName);

	//! \brief 名字是否已经存在
	YkBool IsNameExisted(const YkString strName);

	//! \brief 给一个可用的名字
	YkString GetUnoccupiedName(YkString strName);

	//! \brief 取出不重复的材质指针
	void GetMapMaterial3D(std::map<YkString, YkModelMaterialPtr>& mapMaterial3D);
	//! \brief 取出不重复的材质指针
	void GetMapTextureData(std::map<YkString, YkModelTexturePtr>& mapTexutreData);
	//! \brief 取出不重复的骨架指针
	void GetMapSkeleton(std::map<YkString, YkModelSkeletonPtr>& mapSkeleton);

	//! \brief 获取数据包
	void GetEMapPack(YkModelEMapPack &mapPack);

	//! \brief 取出特定LOD层的不重复的材质指针
	//! \param nLOD [in] LOD层号，-1为最精细层。
	void GetMapMaterial3DByLOD(YkInt nLOD, std::map<YkString, YkModelMaterialPtr>& mapMaterial3D);
	//! \brief 取出特定LOD层的不重复的材质指针
	//! \param nLOD [in] LOD层号，-1为最精细层。
	void GetMapTextureDataByLOD(YkInt nLOD, std::map<YkString, YkModelTexturePtr>& mapTexutreData);
	//! \brief 取出特定LOD层的不重复的骨架指针
	//! \param nLOD [in] LOD层号，-1为最精细层。
	void GetMapSkeletonByLOD(YkInt nLOD, std::map<YkString, YkModelSkeletonPtr>& mapSkeleton);

	//! \brief 移除LODs
	void RemoveLODModel();

	//! \brief 从ModelNode中提取指定的骨架
	YkBool GetSkeleton(const YkCSkeletonID id, 
		YkModelSkeleton*& pSkeleton, YkMatrix4d& matGeode);

	//! \brief 移除指定的骨架
	YkBool RemoveSkeleton(YkCSkeletonID id);

protected:
	//! \brief 构建父子关系：据Patch中的父子Index值
	void MakeTree();

private:
	void SortPagedLODs();
	static YkBool SortPagedLOD(const YkModelPagedLOD *p1,const YkModelPagedLOD *p2);

private:
	//! \brief LOD 层
	std::vector<YkModelPagedLOD*> m_vecPagedLODs;

	//! \brief 叶子节点 ,精细层，所属LOD=NULL
	std::vector<YkModelPagedPatch *> m_vecDataPatches;

	//! \brief 包围球
	YkBoundingSphere m_BoundingSphere;

	//! \brief 最精细层的Element是否Load过
	YkBool m_bElementLoaded;
};

}

#endif
