/*
 *
 * YkMaterial3D.h
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

#ifndef AFX_YKMATERIAL3D_H__F59B3AEC_FB59_4998_9544_E036672AA5FF__INCLUDED_
#define AFX_YKMATERIAL3D_H__F59B3AEC_FB59_4998_9544_E036672AA5FF__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "Base3D/YkSharedPtr.h"
#include "Base3D/YkTechnique.h"

namespace Yk {
class YkTechnique;

//! \brief 三维材质类。
class BASE3D_API YkMaterial3D
{
public:
	/// distance list used to specify LOD
	typedef std::vector<YkDouble> LodValueList;
public:
	enum MaterialEffectType
	{
		NONE = 0,
		WATER = 1,
	};

	//! \brief 构造函数。
	YkMaterial3D();
	//! \brief 拷贝构造函数。
	YkMaterial3D(const YkMaterial3D& other);

	//! \brief 赋值函数
	YkMaterial3D& operator=(const YkMaterial3D& other);
	
	//! \brief 构造函数。
	~YkMaterial3D();

public:

	/** Creates a new Technique for this Material.*/
	YkTechnique* createTechnique(void);
	/** Gets the indexed technique. */
	YkTechnique* getTechnique(YkUshort index);
	/** searches for the named technique.
	/** Retrieves the number of techniques. */
	YkUint getNumTechniques(void) const;
	/** Removes all the techniques in this Material. */
	void removeAllTechniques(void);



	//! \brief 从流文件中加载
	//! \param fStream 文件流[in]。
	YkBool Load(YkStream& fStream);

	//! \brief 存入流文件中
	//! \param fStream 文件流[in]。
	void Save(YkStream& fStream);



public:
	//! \brief 材质名称。
	//! \remarks 备注信息。
	YkString m_strName;

	//! \brief 材质所在组的名称
	YkString m_strGroupName;

	typedef std::vector<YkTechnique*> Techniques;
	/// All techniques, supported and unsupported
	Techniques mTechniques;

	//! \brief 特效材质枚举
	//! \remarks 备注信息。
	MaterialEffectType m_nType;

	//! \brief 材质版本号
	YkDouble m_dVersion;
};
}

#endif 


