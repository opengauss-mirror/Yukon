/*
 *
 * YkTechnique.h
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

#ifndef AFX_YKTECHNIQUE_H__32CC8874_43C6_4310_AB71_93579B0735D9__INCLUDED_
#define AFX_YKTECHNIQUE_H__32CC8874_43C6_4310_AB71_93579B0735D9__INCLUDED_

#pragma once

#include "Base3D/YkPrerequisites3D.h"
#include "YkPass.h"

namespace Yk {
//! \brief 三维材质类。
class BASE3D_API YkTechnique 
{
public:
	/// Constructor
	YkTechnique();
	/// Copy constructor
	YkTechnique(const YkTechnique& oth);
	~YkTechnique();

public:

	/** Creates a new Pass for this Technique.*/
	YkPass* createPass(void);
	/** Retrieves the Pass with the given index. */
	YkPass* getPass(YkUshort index);
	/** Retrieves the number of passes. */
	YkUshort getNumPasses(void) const;
	/** Removes all Passes from this Technique. */
	void removeAllPasses(void);
	//! \brief 从流文件中加载
	//! \param fStream 文件流[in]。
	YkBool Load(YkStream& fStream, YkDouble dVersion);

	//! \brief 存入流文件中
	//! \param fStream 文件流[in]。
	void Save(YkStream& fStream, YkDouble dVersion);

	//! \brief 获取数据存储时的大小
	//! \return 返回存储大小
	//! \remark 暂未考虑编码情况，与Save()一起维护
	//! \attention 字符串的长度要+4
	YkInt GetDataSize() const;

protected:
	typedef std::vector<YkPass*> Passes;
	/// List of primary passes
	Passes mPasses;

	YkString mName; // optional name for the technique
	YkString mSchemeName;
	YkUshort mLodIndex;
	YkString mShadowCasterMaterialName;
	YkString mShadowReceiverMaterialName;
} ;
}

#endif 


