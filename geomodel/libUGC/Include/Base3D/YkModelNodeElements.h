/*
 *
 * YkModelNodeElements.h
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

#if !defined(AFX_YKMODELNODEELEMENTS_H__2B684F7C_5AD9_467C_8095_30B4F46E674A__INCLUDED_)
#define AFX_YKMODELNODEELEMENTS_H__2B684F7C_5AD9_467C_8095_30B4F46E674A__INCLUDED_

#pragma once

#include "Base3D/YkMatrix4d.h"
#include "Base3D/YkTextureData.h"
#include "Base3D/YkMaterial3D.h"
#include "Base3D/YkRenderOperationGroup.h"
#include "Base3D/YkModelSkeletonDataPackTemplate.h"

namespace Yk {
#define Yk_MATERTIALNAME_DEFAULT	_U("matdefault")
#define Yk_TEXTURENAME_DEFAULT	_U("texdefault")

class YkModelEM;

typedef YkModelSkeletonDataPackTemplate<YkVertexDataPackageExact> YkModelSkeletonExactPack;
typedef YkModelSkeletonDataPackTemplate<YkVertexDataPackage>	  YkModelSkeletonRenderPack;

class BASE3D_API YkModelElement
{
public:
	enum METype
	{
		etUnknown		= 0,
		etSkeleton	= 1,
		etTexture		= 2,
		etMaterial	= 3
	};

public:
	YkModelElement(){};
	~YkModelElement(){};

public:
	virtual YkModelElement::METype GetType() = 0;
	virtual std::vector<YkString> GetAtt() = 0;
	virtual YkString GetName() = 0;
	virtual YkBool HasBoundingBox() = 0;

	//! \brief 骨架重载之
	virtual YkBoundingBox GetBoundingBox()
	{
		return YkBoundingBox();
	}
};

class BASE3D_API YkModelTexture : public YkModelElement
{
public:
	YkModelTexture();

	YkModelTexture(const YkModelTexture& other);

	YkModelTexture(YkString strName);

public:
	~YkModelTexture();

public:
	YkModelElement::METype GetType()
	{
		return YkModelElement::etTexture;
	}
	std::vector<YkString> GetAtt()
	{
		return std::vector<YkString>();
	}

	YkString GetName()
	{
		return m_strName;
	}

	YkBool HasBoundingBox()
	{
		return FALSE;
	}

	YkModelTexture& operator = (const YkModelTexture& other);

	//! \brief 写入
	YkBool Save(YkStream& stream);

	//! \brief 加载
	YkBool Load(YkStream& stream);

	//! \brief 创建mipmap
	void UpdateTextureTier();


	//! \brief 对纹理数据进行缩放
	//! \brief nMaxW nMaxH 宽高限制； =0 表示不限制
	//! \brief 返回 comp
	static YkInt ScaleTextureData(YkTextureData* pTextureData, const YkInt nMaxW=0, const YkInt nMaxH=0);

public:
	YkString m_strName;

	YkTextureData* m_pTextureData;

	YkBool m_bMipmap;

	YkInt m_nLevel;
};
typedef YkSharedPtr<YkModelTexture> YkModelTexturePtr;

class BASE3D_API YkModelMaterial : public YkMaterial3D, public YkModelElement
{
public:
	YkModelMaterial(YkString strName=Yk_MATERTIALNAME_DEFAULT);
	
	YkModelMaterial(const YkModelMaterial & other);

	YkModelMaterial & operator=(const YkModelMaterial & other);

	~YkModelMaterial();
public:
	YkModelElement::METype GetType()
	{
		return YkModelElement::etMaterial;
	}

	//! \brief 获取属性
	std::vector<YkString> GetAtt();

	YkString GetName()
	{
		return m_strName;
	}

	YkBool HasBoundingBox()
	{
		return FALSE;
	}

	//! \brief 获取材质中的纹理名
	void GetTextureNames(std::vector<YkString> &vecNames);

	//! \brief 设置纹理名，没有则新建
	//! \brief vecTexNames个数为0，则清空纹理
	void SetTextureName(std::vector<YkString>& );

	//! \brief 初始化默认内容
	void MakeDefault();
};
typedef YkSharedPtr<YkModelMaterial> YkModelMaterialPtr;

class BASE3D_API YkModelSkeleton : public YkModelElement
{
public:
	YkModelSkeleton();
	YkModelSkeleton(YkString strName);
	YkModelSkeleton(const YkModelSkeleton& other);

public:
	~YkModelSkeleton();

public:
	YkModelElement::METype GetType()
	{
		return YkModelElement::etSkeleton;
	}

	std::vector<YkString> GetAtt();

	YkString GetName()
	{
		return m_strName;
	}

	YkBool HasBoundingBox()
	{
		return TRUE;
	}

	YkModelSkeleton& operator = (const YkModelSkeleton& other);

	//! \brief 写入
	YkBool Save(YkStream& stream);

	//! \brief 加载
	YkBool Load(YkStream& stream);

	//! \brief 是否使用高精度数据
	void SetExactDataTag(YkBool bExactTag = TRUE);

	//! \brief 是否使用高精度数据
	YkBool GetExactDataTag() const;

	//! \brief 清理
	void Clear();

	//! \brief 判断存储的数据是否有效
	YkBool IsValid();

	//! \brief 设置数据包：高精度
	void SetExactDataPackRef(YkVertexDataPackageExact* pVertexPack, 
		YkArray<YkIndexPackage*> &arrIndexPack);
	//! \brief 设置数据包：高精度
	void SetExactDataPackRef(YkVertexDataPackageExact* pVertexPack, 
		YkIndexPackage* pIndexPack);

	//! \brief 设置数据包：低精度
	void SetDataPackRef(YkVertexDataPackage* pVertexPack, \
		YkArray<YkIndexPackage*> &arrIndexPack);
	//! \brief 设置数据包：低精度
	void SetDataPackRef(YkVertexDataPackage* pVertexPack, \
		YkIndexPackage* pIndexPack);

	//! \brief 重新设置IndexPack
	void SetIndexPackage(YkArray<YkIndexPackage*> &arrIndexPack);

	//! \brief 获取高精度数据包，可能为空
	YkModelSkeletonExactPack* GetExactDataPackRef();

	//! \brief 获取低精度数据包，可能为空
	YkModelSkeletonRenderPack* GetInExactDataPackRef();

	//! \brief 获取骨架PassName
	YkArray<YkString> GetPassNames();

	//! \brief 获取Indexpack个数：高精度、低精度、渲染包共用
	YkInt GetIndexPackageCount();

	//! \brief 获取IndexPackage
	YkIndexPackage* GetIndexPackage(const YkInt i=0);

	//! \brief 获取顶点个数
	YkInt GetVertexCount();

	//! \brief 获取第i个顶点
	YkVector3d GetVertex(const YkInt i);

	//! \brief 设置材质名：骨架属性和Indexpack的pass名
	void SetMaterialName(YkString strMaterialName);

private:
	//! \brief 清理
	void ClearRenderPack();

	//! \brief 清理
	void ClearExactPack();

	//! \brief 清理
	void ClearInExactPack();

public:
	//! \brief 名字
	YkString m_strName;

	//! \brief 材质名
	YkString m_strMaterialName;

	//! \brief 位置矩阵
	YkMatrix4d m_matLocalView;

	//! \brief 在ModelNode中的引用次数
	YkInt m_nRefCount;

private:	
	//! \brief 数据包标记：
	//! \brief 为TRUE，则m_pDataExactPack!=NULL
	//! \brief 为FALSE，则m_pDataInExactPack!=NULL
	YkBool m_bExactTag;

	//! \brief 模型顶点信息：渲染包。用的时候构造
	YkModelSkeletonRenderPack* m_pDataRenderPack;
	YkBool m_bRenderPackShared;

	//! \brief 模型顶点信息：低精度数据
	YkModelSkeletonRenderPack* m_pDataInExactPack;

	//! \brief 模型顶点信息：高精度数据
	YkModelSkeletonExactPack* m_pDataExactPack;

	//! \brief 包围盒
	YkBoundingBox m_BoundingBox;
};

typedef YkSharedPtr<YkModelSkeleton> YkModelSkeletonPtr;

}

#endif
