/*
 *
 * YkTechnique.cpp
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

#include "Base3D/YkTechnique.h"
#include "Toolkit/YkMarkup.h"

namespace Yk
{
	/// Constructor
	YkTechnique::YkTechnique()
	{
		mLodIndex = 0;
	}

	/// Copy constructor
	YkTechnique::YkTechnique(const YkTechnique& oth)
	{
		mLodIndex = oth.mLodIndex;
		mName = oth.mName;
		mSchemeName = oth.mSchemeName;
		mShadowCasterMaterialName = oth.mShadowCasterMaterialName;
		mShadowReceiverMaterialName = oth.mShadowReceiverMaterialName;
		removeAllPasses();
		YkInt i = 0;
		for(i = 0; i < oth.mPasses.size(); i++)
		{
			YkPass *pPass = new YkPass(*oth.mPasses.at(i));
			if(pPass != NULL)
			{
				mPasses.push_back(pPass);
			}
		}
	}

	YkTechnique::~YkTechnique()
	{
		removeAllPasses();
	}

	//-----------------------------------------------------------------------------
	YkPass* YkTechnique::createPass(void)
	{
		YkPass* newPass = new YkPass();
		mPasses.push_back(newPass);
		return newPass;
	}
	//-----------------------------------------------------------------------------
	YkPass* YkTechnique::getPass(YkUshort index)
	{
		return mPasses[index];
	}
	//-----------------------------------------------------------------------------
	YkUshort YkTechnique::getNumPasses(void) const
	{
		return static_cast<YkUshort>(mPasses.size());
	}
	//-----------------------------------------------------------------------------
	void YkTechnique::removeAllPasses(void)
	{
		Passes::iterator i, iend;
		iend = mPasses.end();
		for (i = mPasses.begin(); i != iend; ++i)
		{
			delete (*i);
		}
		mPasses.clear();
	}
	void YkTechnique::Save(YkStream& fStream, YkDouble dVersion)
	{
		fStream<<mName;
		fStream<<mSchemeName;
		fStream<<mLodIndex;
		fStream<<mShadowCasterMaterialName;
		fStream<<mShadowReceiverMaterialName;
		
		YkInt i = 0;
		YkInt nSize = mPasses.size();
		fStream<<nSize;
		for(i = 0; i < nSize; i++)
		{
			YkPass *pPass = mPasses.at(i);
			if(pPass != NULL)
			{
				pPass->Save(fStream, dVersion);
			}
		}
	}

	YkBool YkTechnique::Load(YkStream& fStream, YkDouble dVersion)
	{
		fStream>>mName;
		fStream>>mSchemeName;
		fStream>>mLodIndex;
		fStream>>mShadowCasterMaterialName;
		fStream>>mShadowReceiverMaterialName;

		YkInt i = 0;
		YkInt nSize = 0;
		fStream>>nSize;
		for(i = 0; i < nSize; i++)
		{
			YkPass *pPass = new(std::nothrow) YkPass;
			if(pPass == NULL)
			{
				return FALSE;
			}

			if(!pPass->Load(fStream, dVersion))
			{
				delete pPass;
				pPass = NULL;
				return FALSE;
			}
			mPasses.push_back(pPass);
		}

		return TRUE;
	}

	YkInt YkTechnique::GetDataSize() const
	{
		YkInt nSize = 0;
		nSize += mName.GetLength() + sizeof(YkInt);
		nSize += mSchemeName.GetLength() + sizeof(YkInt);
		nSize += sizeof(mLodIndex);
		nSize += mShadowCasterMaterialName.GetLength() + sizeof(YkInt);
		nSize += mShadowReceiverMaterialName.GetLength() + sizeof(YkInt);
		nSize += sizeof(YkInt);

		YkInt i = 0;
		for(i = 0; i < mPasses.size(); i++)
		{
			YkPass *pPass = mPasses.at(i);
			if(pPass != NULL)
			{
				nSize += pPass->GetDataSize();
			}
		}

		return nSize;
	}
}