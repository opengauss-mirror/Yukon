/*
 *
 * YkModelNodeShells.h
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

#if !defined(AFX_YKMODELNODESHELLS_H__16DC8D36_2B80_4B48_BCC9_F7257704511C__INCLUDED_)
#define AFX_YKMODELNODESHELLS_H__16DC8D36_2B80_4B48_BCC9_F7257704511C__INCLUDED_

#pragma once

#include "Base3D/YkModelNodeElements.h"
#include "Base3D/YkModelEM.h"

namespace Yk {
class BASE3D_API YkCSkeletonID
{
public:
	//! \brief 初始化值为非法值
	YkCSkeletonID();
	YkCSkeletonID(YkInt nL,YkInt nP, YkInt nG, YkInt nS);

	//! \brief 初始化值为非法值
	~YkCSkeletonID();

	YkCSkeletonID& operator = (const YkCSkeletonID& other);

	YkBool operator == (const YkCSkeletonID &id);

	void Reset();

	YkBool IsValid() const;

	//! \brief nLOD=-1则为精细层
	YkInt m_nIdx_LOD;

	//! \brief >=0
	YkInt m_nIdx_Patch;

	//! \brief >=0 
	YkInt m_nIdx_Geode;

	//! \brief >=0 
	YkInt m_nIdx_Skeleton;
};

class BASE3D_API YkModelGeode
{
	friend class YkModelNode;

public:
	YkModelGeode();

	//! \brief 完全拷贝.ModelNode operator = 使用
	YkModelGeode(YkModelGeode& geode);
	
	~YkModelGeode();

public:
	//! \brief 构造模型对象。pSkeletonSrc 会被释放掉，外部不能再使用
	//! \brief 会默认新建一个材质对象
	void MakeFrom(YkModelSkeleton* pSkeletonSrc, YkMatrix4d matGeode);
	
	//! \brief 把mapPack里的骨架全部加入
	void AddMapPack(YkModelEMapPack& mapPack);

	//! \brief 添加Geometry
	YkBool AddSkeleton(YkModelSkeletonPtr pModelSkeleton);
	YkBool AddMaterial(YkModelMaterialPtr pMaterial3D);
	YkBool AddTexture(YkModelTexturePtr pTexData);

	//! \brief 添加对象
	void AddElement(YkModelElement* pElement);
	void AddTexture(std::vector<YkModelTexture*>& vecElement);

	//! \brief 添加壳儿 Geometry
	void AddElementShell(const YkModelElement::METype eType, \
		const YkString strSkeleton);

	//! \brief 创建实体，不改名字，创建失败返回空
	YkModelSkeletonPtr CreateSkeleton(const YkString strName);
	YkModelMaterialPtr CreateMaterial(const YkString strName);
	YkModelTexturePtr CreateTexture(const YkString strName);

	//! \brief 获取数据包引用
	YkModelEMapPack& GetEMapPack();

	//! \brief 获得Element总数
	YkInt GetElementNum(YkModelElement::METype metype) const;

	//! \brief 获得实体名称
	std::vector<YkString> GetElementNames(YkModelElement::METype metype);

	YkModelSkeletonPtr GetSkeleton(const YkString strName);
	YkModelMaterialPtr GetMaterial(const YkString strName);
	YkModelTexturePtr GetTexture(const YkString strName);

	//! \brief 名字是否已经存在
	YkBool IsNameExisted(const YkString strName);

	//! \brief 给一个可用的名字
	YkString GetUnoccupiedName(YkString strName);

	//! \brief 包内所有骨架的包围盒，包含骨架的矩阵和包的矩阵
	void ComputeBoundingBox();

	//! \brief 包内所有骨架的包围盒，包含骨架的矩阵和包的矩阵
	YkBoundingBox GetBoundingBox();

	//! \brief 设置包围盒,包内所有骨架的包围盒，包含骨架的矩阵和包的矩阵
	void SetBoundingBox(YkBoundingBox box);

	//! \brief 单个骨架的包围盒
	YkBoundingBox ComputeSkeletonBoundingBox(YkModelSkeletonPtr& pSkeleton);

	//! \brief 作用于各Geode的矩阵
	void ReMultiplyMatrix(const YkMatrix4d& m);

	//! \brief 设置位置
	void SetPosition(YkMatrix4d mat);

	//! \brief 获得位置
	const YkMatrix4d& GetPosition() const ;

	YkInt GetShellDataSize();

	//! \brief 写入，不包含实体数据
	YkBool SaveShell(YkStream& stream);

	//! \brief 加载
	YkBool LoadShell(YkStream& stream);

	//! \brief 从加载EM填充vector
	YkBool LoadElements(YkModelEM* pElementsManager, YkModelElement::METype mtype = YkModelElement::etUnknown);

	//! \brief 从内存移除骨架对象及其包含的材质纹理（如果没人在用的话）
	//! \brief strSkeleton 骨架名称
	//! \brief bSynElement 是否同步清理材质纹理对象
	YkBool RemoveSkeleton(const YkString strSkeleton, YkBool bSynElement=TRUE);

	//! \brief 清理
	void Clear();

private:
	//! \brief 加载实体
	void LoadElements(YkModelEMapPack& mapPack);

public:
	//! \brief 名字
	YkString m_strName;
	//! \brief FBX 属性信息
	YkString m_strAttTableName;
	YkInt m_nAttID;

private:
	//! \brief 位置矩阵
	YkMatrix4d m_matLocalView;

	//! \brief 重新放置插入点时计算出的修正矩阵
	YkMatrix4d m_matSupplement;

	//! \brief 包围盒
	YkBoundingBox m_BoundingBox;

	//! \brief 数据包
	YkModelEMapPack m_mapPack;
};

class YkModelPagedLOD;
class YkModelNode;

class BASE3D_API YkModelPagedPatch
{
friend class YkModelPagedLOD;
friend class YkModelNode;
friend class YkModelNodeTools;
friend class YkFileParserOSGTile;
friend class YkFileParserS3MTile;
friend class YkFileParserS3MBTile;
friend class YkS3MBWriter;
friend class YkS3MBModelTools;
friend class YkTileStuff;

protected:
	YkModelPagedPatch();

	YkModelPagedPatch(YkModelPagedLOD* pPagedLOD, YkFloat range);

	~YkModelPagedPatch();

public:
		//! \brief 完全拷贝.ModelNode operator = 使用
	void MakeFrom(YkModelPagedLOD* pPagedLOD, YkModelPagedPatch* pPatch);


	//! \brief 获取距离
	YkFloat GetLODDistance() const;
	void SetLODDistance(YkFloat dbLODDis);

	//! \brief 获取对象引用
	//! \brief pGeode 有可能被替换
	void AddGeode(YkModelGeode *& pGeode);

	//! \brief 释放Geode
	void ReleaseGeode(YkModelGeode * pGeode);

	//! \brief 获取个数
	 YkInt GetGeodeCount() const;
	 
	 //! \brief 获取
	 YkModelGeode* GetGeode(YkInt nIndex);

	std::vector<YkModelGeode *>& GetGeodes();

	//! \brief 包围球
	void SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius );

	//! \brief 包围盒
	void SetBoundingBox(const YkBoundingBox& bbox);

	//! \brief 包围球
	YkBoundingSphere GetBoudingSphere();

	//! \brief 包围盒
	YkBoundingBox GetBoundingBox();

	//! \brief 计算包围球
	void ComputeBoundingSphere();

	//! \brief 实时计算准确的脚点
	YkBoundingBox ComputeBoudingBox();

	//! \brief 作用于各Geode的矩阵
	void ReMultiplyMatrix(const YkMatrix4d& m);

	//! \brief 获取当前层的实体对象名称
	void GetElementNames(std::vector<YkString>& vecNames, YkModelElement::METype metype);

	void GetSkeletons(std::map<YkString, YkModelSkeletonPtr>& mapElement);
	void GetMaterials(std::map<YkString, YkModelMaterialPtr>& mapElement);
	void GetTextures(std::map<YkString, YkModelTexturePtr>& mapElement);

	YkModelSkeletonPtr GetSkeleton(const YkString strName);
	YkModelMaterialPtr GetMaterial(const YkString strName);
	YkModelTexturePtr GetTexture(const YkString strName);

	//! \brief 名字是否已经存在
	YkBool IsNameExisted(const YkString strName);

	//! \brief 写入
	YkBool Save(YkStream& stream);

	//! \brief 加载
	YkBool Load(YkStream& stream);

	//! \brief 获取存储空间大小
	YkInt GetShellDataSize();

	//! \brief 从加载EM填充vector
	YkBool LoadElements(YkModelEM* pEM,YkModelElement::METype mtype = YkModelElement::etUnknown);

	//! \brief 返回Element是否Load过
	YkBool IsElementLoaded();

	//! \brief 添加子节点，更精细层(LODNum-1)
	YkInt AddChild(YkModelPagedPatch* pPatch);

	//! \brief 获取子节点
	YkModelPagedPatch* GetChild(YkInt nIndex);

	//! \brief 获取所有子节点
	std::vector<YkModelPagedPatch*>& GetChildren();

	//! \brief 获取子节点个数
	YkInt GetChildCount();

	//! \brief 父节点
	void SetParentPatch(YkModelPagedPatch* pPatch);

	//! \brief 父节点，更粗糙层(LODNum+1)
	YkModelPagedPatch* GetParentPatch();
	
	//! \brief 索引号
	YkInt GetIndex();

	//! \brief 所属LOD
	void SetPagedLOD(YkModelPagedLOD* pPageLOD);

	//! \brief 所属LOD
	YkModelPagedLOD* GetPagedLOD();
	
	//! \brief 移除所有Geode指针，不删除指针内容
	void RemoveAllGeodes();

	//! \brief Patch对应的文件名，不存储
	YkString GetFileName();
	void SetFileName(YkString strName);

protected:
	//! \brief 移除所有子节点
	void RemoveChildren();

	//! \brief 移除指定子节点
	void RemoveChildren(YkModelPagedPatch* pChildPatch);

	//! \brief 索引号
	void SetIndex(YkInt nIndex);

	//! \brief 构造父子节点关系，ModelNode LoadShells专用
	void LoadPatchTree(YkModelNode* pNode);

	//! \brief 慎用，仅Model的Adjust使用
	void ReplaceChild(YkModelPagedPatch* pPatch);

	//! \brief 慎用，仅Reset使用
	void ReplaceGeode(YkInt nIndex, YkModelGeode* pGeode);



	//! \brief 设置/获取父 索引号,ModelNode MakeShell使用
	void SetParentIndex(const YkInt nPatchParentIndex);
	YkInt GetParentIndex();
	//! \brief 设置/获取子 索引号,ModelNode MakeShell使用
	void SetChildrenIndex(const std::vector<YkInt>& vecChildrenIndex);
	std::vector<YkInt>& GetChildrenIndex();

private:
	//! \brief 距离范围
	YkFloat m_dbLODDis;

	//! \brief 所属LOD层
	YkModelPagedLOD * m_pPagedLOD;

	//! \brief 在当前LOD层Patches中的索引号
	YkInt m_nIndex;

	//! \brief 父节点
	YkModelPagedPatch* m_pPatchParent;
	//! \brief 父节点Index，仅加载使用
	YkInt m_nPatchParentIndex;

	//! \brief 子节点
	std::vector<YkModelPagedPatch*> m_vecChildren;
	//! \brief 子节点Index，仅加载使用
	std::vector<YkInt> m_vecChildrenIndex;

	//! \brief 数据包 
	std::vector<YkModelGeode *> m_vecGeode;

	//! \brief 包围球
	YkBoundingSphere m_BoundingSphere;

	//! \brief 包围盒
	YkBoundingBox m_BoundingBox;

	//! \brief Element是否Load过
	YkBool m_bElementLoaded;

	//! \brief 渲染使用的ID
	YkInt m_nRenderID;

	//! \brief Patch对应的文件名，不存储
	YkString m_strFileName;
};

class BASE3D_API YkModelPagedLOD
{
	friend class YkModelNode;
	friend class YkModelNodeLODBuilder;

protected:
	YkModelPagedLOD();

	YkModelPagedLOD(YkInt nLOD);

	~YkModelPagedLOD();

public:
	//! \brief 完全拷贝.ModelNode operator = 使用
	void MakeFrom(YkModelPagedLOD *pPagedLOD);


	//! \brief 获得切换范围模式
	YkRangeMode GetRangeMode();

	//! \brief 得到Patch，不存在则创建
	YkModelPagedPatch* CreatePagedPatch(YkFloat fDistance);

	//! \brief 添加一个已有的Patch，并设置好Index
	void AddPagedPatch(YkModelPagedPatch* pPagedPatch);

	//! \brief 释放
	YkBool ReleasePagedPatch(YkModelPagedPatch *pPatch);

	//! \brief 获取当前层的实体对象名称
	void GetElementNames(std::vector<YkString>& vecNames,YkModelElement::METype metype);

	void GetSkeletons(std::map<YkString, YkModelSkeletonPtr>& mapElement);
	void GetMaterials(std::map<YkString, YkModelMaterialPtr>& mapElement);
	void GetTextures(std::map<YkString, YkModelTexturePtr>& mapElement);

	YkModelSkeletonPtr GetSkeleton(const YkString strName);
	YkModelMaterialPtr GetMaterial(const YkString strName);
	YkModelTexturePtr GetTexture(const YkString strName);

	//! \brief 名字是否已经存在
	YkBool IsNameExisted(const YkString strName);

	//! \brief 获得所有的距离范围
	void GetLODRanges(std::vector<YkFloat>& vecDis);

	//! \brief 得到Patches
	YkInt GetPagedPatchCount();

	//! \brief 根据索引号得到Patch，不会创建新的Patch
	YkModelPagedPatch* GetPagedPatch(YkInt nIndex);

	//! \brief 获取Patches
	std::vector<YkModelPagedPatch*>& GetPagedPatchesRef();

	//! \brief 获取距离
	YkFloat GetLODDistance(YkInt nPatchIdex=-1) const;

	//! \brief 获取所有Patch的Geodes
	std::vector<YkModelGeode *> GetGeodes();

	//! \brief 获取存储空间大小
	YkInt GetShellDataSize();

	//! \brief 写入
	YkBool Save(YkStream& stream);

	//! \brief 加载
	YkBool Load(YkStream& stream);

	//! \brief 从加载YkModelEM填充vector
	YkBool LoadElements(YkModelEM* pEM, 
		YkModelElement::METype mtype =YkModelElement::etUnknown, YkInt nPatchIndex = -1);

	//! \brief 包围球
	void SetBoudingSphere(YkVector3d vecCenter, YkDouble dRadius );

	//! \brief 包围球
	YkBoundingSphere GetBoudingSphere();

	//! \brief 计算包围球
	void ComputeBoundingSphere();

	//! \brief 作用于各Geode的矩阵
	void ReMultiplyMatrix(const YkMatrix4d& m);

	//! \brief 获取层号
	YkInt GetLODNum() const;


protected:

private:
	//! \brief 切换范围模式
	YkRangeMode m_nRangeMode;

	//! \brief LOD 标识号
	YkInt m_nLODNum;

	//! \brief 包围球
	YkBoundingSphere m_BoundingSphere;

	//! \brief 本层的PageLODs
	std::vector<YkModelPagedPatch*> m_vecPagedPatches;
};

}

#endif
