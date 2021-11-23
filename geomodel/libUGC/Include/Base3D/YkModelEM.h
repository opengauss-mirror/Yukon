/*
 *
 * YkModelEM.h
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

#if !defined(AFX_YKMODELEM_H__99D3488B_888D_4726_823D_613E7E313C48__INCLUDED_)
#define AFX_YKMODELEM_H__99D3488B_888D_4726_823D_613E7E313C48__INCLUDED_

#pragma once

#include "Base3D/YkSharedPtr.h"
#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkTextureData.h"
#include "Base3D/YkMaterial3D.h"
#include "Base3D/YkModelNodeElements.h"
#include "Base3D/YkModelElementPool.h"

namespace Yk {
class YkModelEM;

//! \brief 实体共用模式，管理器追加实体使用
enum ElementShared
{
	ES_None		= 0x00000000,
	ES_Skeleton		= 0x00000001,
	ES_Texture		= 0x00000002,
	ES_Material		= 0x00000004,
	ES_ALL			= 0x00000001|0x00000002|0x00000004,
};

class BASE3D_API YkModelEMapPack
{
public:
	YkModelEMapPack();

	YkModelEMapPack(YkModelEM* pEM);

	~YkModelEMapPack();

public:
	//! \brief 
	YkModelEMapPack& operator = (YkModelEMapPack& other);

	//! \brief 创建新的实体；重名返回空
	YkModelSkeletonPtr CreateSkeleton(const YkString strName);
	YkModelMaterialPtr CreateMaterial(const YkString strName);
	YkModelTexturePtr CreateTexture(const YkString strName);

	//! \brief 添加
	YkModelSkeletonPtr AddSkeleton(YkModelSkeleton* pElement);
	YkModelMaterialPtr AddMaterial(YkModelMaterial* pElement);
	YkModelTexturePtr AddTexture(YkModelTexture* pElement);

	YkBool AddSkeleton(YkModelSkeletonPtr pElement);
	YkBool AddMaterial(YkModelMaterialPtr pElement);
	YkBool AddTexture(YkModelTexturePtr pElement);

	//! \brief 添加空对象
	void AddElementShell(const YkModelElement::METype eType, const YkString strName);

	//! \brief 设置,从Em中Load实体使用,必须是智能指针
	YkModelSkeletonPtr SetSkeleton(YkModelSkeletonPtr pElement);
	YkModelMaterialPtr SetMaterial(YkModelMaterialPtr pElement);
	YkModelTexturePtr SetTexture(YkModelTexturePtr pElement);

	//! \brief 加载实体
	//! \brief AddElementShell 之后都是壳儿
	void LoadElements(YkModelEMapPack& mapPack);

	//! \brief 获取
	YkModelSkeletonPtr GetSkeleton(const YkString strName, const YkBool bTagName=TRUE);
	YkModelMaterialPtr GetMaterial(const YkString strName, const YkBool bTagName=TRUE);
	YkModelTexturePtr GetTexture(const YkString strName, const YkBool bTagName=TRUE);	

	//! \brief 给一个可用的名字
	YkString GetUnoccupiedName(YkString strName);

	//! \brief 获得实体名称
	std::vector<YkString> GetElementNames(YkModelElement::METype metype);

	//! \brief 获取属性
	void GetAtt(const YkString strName, const YkModelElement::METype eType, \
		std::vector<YkString>& vecAtt);

	//! \brief 包里数据同步，移除掉无用的对象
	void SynElement();

	//! \brief 骨架删除
	//! \brief strSkeleton 骨架名称
	//! \brief 返回骨架的材质名
	YkString RemoveSkeleton(const YkString strSkeleton);

	//! \brief 清空
	void Clear();

	//! \brief 名字是否已经存在
	YkBool IsExisted(const YkString strName);

public:
	YkModelSkeletonMap m_mapSkeleton;
	YkModelMaterialMap m_mapMaterial;
	YkModelTextureMap m_mapTexture;

private:
	YkModelEM* m_pEM;
};

class BASE3D_API YkModelEM
{
	friend class YkGeoModelElementSkeleton;
	friend class YkGeoModelElementMaterial3D;
	friend class YkGeoModelElementTexture;
	friend class YkModelNode;
	friend class YkModelGeode;

public:
	YkModelEM(YkBool bHash64 = FALSE);
	~YkModelEM();

public:
	//! \brief 是否已经初始化过了
	YkBool HasInited();


	//! \brief 清空
	void Clear();




	//! \brief 从内存获取实体
	YkModelSkeletonPtr GetLoadedSkeleton(YkString strName);
	YkModelMaterialPtr GetLoadedMaterial3D(YkString strName);
	YkModelTexturePtr GetLoadedTexture(YkString strName);
	
	std::map<YkString, YkModelSkeletonPtr>& GetLoadedSkeletonRef();
	std::map<YkString, YkModelMaterialPtr>& GetLoadedMaterialRef();
	std::map<YkString, YkModelTexturePtr>& GetLoadedTextureRef();

	//! \brief 清理掉UseCount=1(仅ElementsManager自己使用)的对象	
	void Release();


	//! \brief 获取属性
	//! \brief strName[in] 传入对象的名字
	//! \brief eType[in]传入对象的类型
	void GetAtt(const YkString strName, const YkModelElement::METype eType, \
		std::vector<YkString>& vecAtt);


	//////////////////////////////////////////////////////////////////////////
protected:
	//! \brief 外部实体托管

protected:
	//! \brief 骨架数据
	YkModelSkeletonPool m_SkeletonPool;
	
	//! \brief 材质数据
	YkModelMaterialPool m_MaterialPool;

	//! \brief 纹理数据
	YkModelTexturePool m_TexturePool;

	//! \brief 是否已经初始化
	YkBool m_bInited;
};

}

#endif
