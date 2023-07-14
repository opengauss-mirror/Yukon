/*
 *
 * UGGeoHash.cpp
 *
 * Copyright (C) 2021-2023 SuperMap Software Co., Ltd.
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

#include "UGGeoHash.h"

namespace Yk
{

    UGGeoHash::UGGeoHash()
    {
    }

    UGGeoHash::~UGGeoHash()
    {
    }

    YkInt UGGeoHash::ComputerGeoHash(YkRect2D IndexBounds, YkArray<YkLong> &AryKeys, YkRect2D GeoBounds,
                                     YkInt nUpperLimitLevel, YkInt nMaxReturnKeyCount)
    {
        if (!IndexBounds.Contains(GeoBounds))
        {
            AryKeys.Add(-1);
            return -1;
        }

        //传入的geobounds接近indexbound情况；
        YkRect2D rcTemp(IndexBounds);
        rcTemp.IntersectRect(GeoBounds);

        YkArray<QueryParameter> aryResult;
        aryResult.SetGrowSize(nMaxReturnKeyCount + 4);

        QueryParameter qFirst;
        qFirst.lGeoKey = 0;
        qFirst.nLevel = 0;
        qFirst.rcGeoKey = IndexBounds;
        qFirst.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / IndexBounds.Width() / IndexBounds.Height();
        qFirst.rcQueryBounds = rcTemp;

        aryResult.Add(qFirst);

        YkArray<QueryParameter> aryGeoKeys;
        while ((YkInt)aryResult.GetSize() < nMaxReturnKeyCount)
        {
            aryGeoKeys.RemoveAll();
            YkInt n = GetMinPercentagePos(aryResult);
            if (aryResult[n].dPercentage < PERCENTAGE)
            {
                UGGeoHash::GetIntersectHash(aryResult[n], aryGeoKeys);
                aryResult.RemoveAt(n);
            }
            else
            {
                break;
            }

            YkBool bBreak = FALSE;
            for (YkUint i = 0; i < aryGeoKeys.GetSize(); i++)
            {
                if (aryGeoKeys[i].nLevel >= nUpperLimitLevel)
                {
                    bBreak = TRUE;
                }
                aryResult.Add(aryGeoKeys[i]);
            }

            if (bBreak)
            {
                break;
            }
        }

        for (YkInt i = aryResult.GetSize() - 1; i >= 0; i--)
        {
            YkLong lGeoKey = aryResult[i].lGeoKey;
            lGeoKey <<= (64 - aryResult[i].nLevel * 2 - 2);
            lGeoKey += aryResult[i].nLevel;
            AryKeys.Add(lGeoKey);
        }
        return 0;
    }

    YkInt UGGeoHash::ComputerGeoHash2(YkRect2D IndexBounds, YkArray<YkLong> &AryKeys, YkRect2D GeoBounds, YkInt nUpperLimitLevel, YkInt nMaxReturnKeyCount)
    {
        //找到level
        //确定4个还是1个

        YkInt nLevel = 0;
        YkDouble dWidth = GeoBounds.Width();
        YkDouble dHeight = GeoBounds.Height();
        YkDouble dIndexWidth = IndexBounds.Width();
        YkDouble dIndexHeight = IndexBounds.Height();
        if (dWidth / dHeight > dIndexWidth / dIndexHeight)
        {
            YkDouble dTemp = dIndexWidth;
            for (YkInt i = 0; i < MAXHASHLEVEL; i++)
            {
                if (dWidth >= dTemp)
                {
                    nLevel = i;
                    break;
                }
                dTemp /= 2;
            }
        }
        else
        {
            YkDouble dTemp = dIndexHeight;
            for (YkInt i = 0; i < MAXHASHLEVEL; i++)
            {
                if (dHeight >= dTemp)
                {
                    nLevel = i;
                    break;
                }
                dTemp /= 2;
            }
        }
        YkLong nHashKey;
        YkPoint2D pnt;
        pnt.x = GeoBounds.left;
        pnt.y = GeoBounds.bottom;
        ComputerPointGeoHash(IndexBounds, nHashKey, pnt, nLevel);
        AryKeys.Add(nHashKey);

        pnt.x = GeoBounds.left;
        pnt.y = GeoBounds.top;
        ComputerPointGeoHash(IndexBounds, nHashKey, pnt, nLevel);

        YkBool bFind = FALSE;
        for (YkUint i = 0; i < AryKeys.GetSize(); i++)
        {
            if (nHashKey == AryKeys[i])
            {
                bFind = TRUE;
                break;
            }
        }
        if (!bFind)
        {
            AryKeys.Add(nHashKey);
        }

        pnt.x = GeoBounds.right;
        pnt.y = GeoBounds.bottom;
        ComputerPointGeoHash(IndexBounds, nHashKey, pnt, nLevel);

        bFind = FALSE;
        for (YkUint i = 0; i < AryKeys.GetSize(); i++)
        {
            if (nHashKey == AryKeys[i])
            {
                bFind = TRUE;
                break;
            }
        }
        if (!bFind)
        {
            AryKeys.Add(nHashKey);
        }

        pnt.x = GeoBounds.right;
        pnt.y = GeoBounds.top;
        ComputerPointGeoHash(IndexBounds, nHashKey, pnt, nLevel);
        bFind = FALSE;
        for (YkUint i = 0; i < AryKeys.GetSize(); i++)
        {
            if (nHashKey == AryKeys[i])
            {
                bFind = TRUE;
                break;
            }
        }
        if (!bFind)
        {
            AryKeys.Add(nHashKey);
        }

        return 0;
    }

    YkInt UGGeoHash::ComputerPointGeoHash(YkRect2D IndexBounds, YkLong &GeoHashKey, YkPoint2D pnt, YkInt nUpperLimitLevel)
    {
        YkDouble dxStep = IndexBounds.Width() / pow(2, nUpperLimitLevel);
        YkDouble dyStep = IndexBounds.Height() / pow(2, nUpperLimitLevel);

        if (pnt.x >= IndexBounds.right)
        {
            pnt.x -= EP;
        }

        if (pnt.y >= IndexBounds.top)
        {
            pnt.y -= EP;
        }

        YkLong lXPos = YKFLOOR((pnt.x - IndexBounds.left) / dxStep);
        YkLong lYPos = YKFLOOR((pnt.y - IndexBounds.bottom) / dyStep);

        GeoHashKey = 0;
        YkLong lnTemp = 0;
        for (YkInt n = nUpperLimitLevel; n > 0; n--)
        {
            YkInt lxlast = lXPos % 2;
            YkInt lylast = lYPos % 2;
            lnTemp = lxlast * 2 + lylast;
            GeoHashKey |= lnTemp << (64 - n * 2 - 2);
            lXPos /= 2;
            lYPos /= 2;
        }
        GeoHashKey += nUpperLimitLevel;
        return 0;
    }

    YkString UGGeoHash::GetFilter2(YkRect2D IndexBounds, YkRect2D GeoBounds, YkInt nUpperLimitLevel)
    {
        if (!UGGeoHash::IsIntersect(IndexBounds, GeoBounds))
        {
            return _U("");
        }

        YkRect2D rcTemp(IndexBounds);
        rcTemp.IntersectRect(GeoBounds);
        QueryParameter qFirst;
        qFirst.lGeoKey = 0;
        qFirst.nLevel = 0;
        qFirst.rcGeoKey = IndexBounds;
        qFirst.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / IndexBounds.Width() / IndexBounds.Height();
        qFirst.rcQueryBounds = rcTemp;

        YkString strFilter;
        YkArray<QueryParameter> aryEquelKeys;
        aryEquelKeys.SetGrowSize(32);
        YkArray<QueryParameter> aryBetweenKeys;
        aryBetweenKeys.SetGrowSize(32);
        aryBetweenKeys.Add(qFirst);

        while (TRUE)
        {
            YkInt n = GetMinPercentagePos(aryBetweenKeys);
            aryEquelKeys.Add(aryBetweenKeys[n]);
            if (aryBetweenKeys[n].dPercentage < PERCENTAGE)
            {
                UGGeoHash::Query(aryBetweenKeys[n], aryEquelKeys, aryBetweenKeys);
                aryBetweenKeys.RemoveAt(n);
            }
            else
            {
                break;
            }

            if (aryEquelKeys.GetSize() + aryBetweenKeys.GetSize() > 30)
            {
                break;
            }
        }

        YkString strTemp;
        for (YkUint i = 0; i < aryEquelKeys.GetSize(); i++)
        {
            YkLong lGeoKey = aryEquelKeys[i].lGeoKey;
            lGeoKey <<= (64 - aryEquelKeys[i].nLevel * 2 - 2);
            lGeoKey += aryEquelKeys[i].nLevel;
            strTemp.Format(_U(" ht.SmGeoHashKey = %lld "), lGeoKey);
            strFilter += strTemp + _U(" or ");
        }

        for (YkUint i = 0; i < aryBetweenKeys.GetSize(); i++)
        {
            YkLong lGeoKey = aryBetweenKeys[i].lGeoKey;
            lGeoKey <<= (64 - aryBetweenKeys[i].nLevel * 2 - 2);
            lGeoKey += aryBetweenKeys[i].nLevel;
            strTemp = GetBetweenFilter2(lGeoKey);
            strFilter += strTemp + _U(" or ");
        }
        strFilter = strFilter.TrimRightString(_U("or "));
        return strFilter;
    }

    YkString UGGeoHash::GetFilter1(YkRect2D IndexBounds, YkRect2D GeoBounds, YkInt nUpperLimitLevel,YkInt nKeySizeLimit)
    {
        if (!UGGeoHash::IsIntersect(IndexBounds, GeoBounds))
        {
            return _U("");
        }

        YkRect2D rcTemp(IndexBounds);
        rcTemp.IntersectRect(GeoBounds);
        QueryParameter qFirst;
        qFirst.lGeoKey = 0;
        qFirst.nLevel = 0;
        qFirst.rcGeoKey = IndexBounds;
        qFirst.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / IndexBounds.Width() / IndexBounds.Height();
        qFirst.rcQueryBounds = rcTemp;

        YkString strFilter;
        YkArray<QueryParameter> aryEquelKeys;
        aryEquelKeys.SetGrowSize(32);
        YkArray<QueryParameter> aryBetweenKeys;
        aryBetweenKeys.SetGrowSize(32);
        aryBetweenKeys.Add(qFirst);

        while (TRUE)
        {
            // 这里修改为查询权重最小的下标 
            //YkInt n = GetMinPercentagePos(aryBetweenKeys);
            YkInt n = FindLowLevelAndLowPercentage(aryBetweenKeys);
			if (n == -1)
			{
				break;
			}
            aryEquelKeys.Add(aryBetweenKeys[n]);
            if (aryBetweenKeys[n].dPercentage < PERCENTAGE)
            {
                UGGeoHash::Query(aryBetweenKeys[n], aryEquelKeys, aryBetweenKeys);
                aryBetweenKeys.RemoveAt(n);
            }
            else
            {
                break;
            }

            if (aryBetweenKeys.GetSize() > nKeySizeLimit ||
                aryBetweenKeys[aryBetweenKeys.GetSize() - 1].nLevel > nUpperLimitLevel)
            {
                break;
            }
        }

        YkString strTemp;
        for (YkUint i = 0; i < aryBetweenKeys.GetSize(); i++)
        {
            YkLong lGeoKey = aryBetweenKeys[i].lGeoKey;
            lGeoKey <<= (64 - aryBetweenKeys[i].nLevel * 2 - 2);
            lGeoKey += aryBetweenKeys[i].nLevel;
            strTemp = GetBetweenFilter1(lGeoKey);
            strFilter += strTemp + _U(" or ");
        }
        strFilter = strFilter.TrimRightString(_U("or "));
        return strFilter;
    }

    YkBool UGGeoHash::GetIntersectHash(QueryParameter &QueryFetch, YkArray<QueryParameter> &aryRest)
    {
        if (QueryFetch.dPercentage >= 100)
        {
            return TRUE;
        }

        YkDouble dCentX = (QueryFetch.rcGeoKey.left + QueryFetch.rcGeoKey.right) / 2.0;
        YkDouble dCentY = (QueryFetch.rcGeoKey.top + QueryFetch.rcGeoKey.bottom) / 2.0;

        YkRect2D rcTemp;
        rcTemp.left = QueryFetch.rcGeoKey.left;
        rcTemp.bottom = QueryFetch.rcGeoKey.bottom;
        rcTemp.right = dCentX;
        rcTemp.top = dCentY;

        if (UGGeoHash::IsIntersect(QueryFetch.rcQueryBounds, rcTemp))
        {
            QueryParameter query;
            query.lGeoKey = QueryFetch.lGeoKey;
            query.lGeoKey <<= 2;

            query.nLevel = QueryFetch.nLevel + 1;

            query.rcGeoKey = rcTemp;
            rcTemp.IntersectRect(QueryFetch.rcQueryBounds);
            query.rcQueryBounds = rcTemp;
            query.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / query.rcGeoKey.Width() / query.rcGeoKey.Height();
            aryRest.Add(query);
        }

        rcTemp.left = dCentX;
        rcTemp.bottom = QueryFetch.rcGeoKey.bottom;
        rcTemp.right = QueryFetch.rcGeoKey.right;
        rcTemp.top = dCentY;
        if (UGGeoHash::IsIntersect(QueryFetch.rcQueryBounds, rcTemp))
        {
            QueryParameter query;
            query.lGeoKey = QueryFetch.lGeoKey;
            query.lGeoKey <<= 1;
            query.lGeoKey += 1;
            query.lGeoKey <<= 1;

            query.nLevel = QueryFetch.nLevel + 1;

            query.rcGeoKey = rcTemp;
            rcTemp.IntersectRect(QueryFetch.rcQueryBounds);
            query.rcQueryBounds = rcTemp;
            query.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / query.rcGeoKey.Width() / query.rcGeoKey.Height();
            aryRest.Add(query);
        }

        rcTemp.left = QueryFetch.rcGeoKey.left;
        rcTemp.bottom = dCentY;
        rcTemp.right = dCentX;
        rcTemp.top = QueryFetch.rcGeoKey.top;
        if (UGGeoHash::IsIntersect(QueryFetch.rcQueryBounds, rcTemp))
        {
            QueryParameter query;
            query.lGeoKey = QueryFetch.lGeoKey;
            query.lGeoKey <<= 1;
            query.lGeoKey <<= 1;
            query.lGeoKey += 1;

            query.nLevel = QueryFetch.nLevel + 1;

            query.rcGeoKey = rcTemp;
            rcTemp.IntersectRect(QueryFetch.rcQueryBounds);
            query.rcQueryBounds = rcTemp;
            query.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / query.rcGeoKey.Width() / query.rcGeoKey.Height();
            aryRest.Add(query);
        }

        rcTemp.left = dCentX;
        rcTemp.bottom = dCentY;
        rcTemp.right = QueryFetch.rcGeoKey.right;
        rcTemp.top = QueryFetch.rcGeoKey.top;
        if (UGGeoHash::IsIntersect(QueryFetch.rcQueryBounds, rcTemp))
        {
            QueryParameter query;
            query.lGeoKey = QueryFetch.lGeoKey;
            query.lGeoKey <<= 1;
            query.lGeoKey += 1;
            query.lGeoKey <<= 1;
            query.lGeoKey += 1;

            query.nLevel = QueryFetch.nLevel + 1;

            query.rcGeoKey = rcTemp;
            rcTemp.IntersectRect(QueryFetch.rcQueryBounds);
            query.rcQueryBounds = rcTemp;
            query.dPercentage = rcTemp.Width() * rcTemp.Height() * 100 / query.rcGeoKey.Width() / query.rcGeoKey.Height();
            aryRest.Add(query);
        }
        return TRUE;
    }

    YkBool UGGeoHash::GetIntersectHash2(QueryParameter &QueryFetch, YkArray<QueryParameter> &aryRest)
    {

        return TRUE;
    }

    YkBool UGGeoHash::Query(QueryParameter QueryFetch, YkArray<QueryParameter> &aryEquelRest, YkArray<QueryParameter> &aryBetweenRest)
    {
        YkArray<QueryParameter> aryGeoKeys;
        aryGeoKeys.SetGrowSize(32);
        while (TRUE)
        {
            aryGeoKeys.RemoveAll();
            if (QueryFetch.dPercentage < PERCENTAGE)
            {
                UGGeoHash::GetIntersectHash(QueryFetch, aryGeoKeys);
            }
            else
            {
                return TRUE;
            }

            if (aryGeoKeys.GetSize() == 1)
            {
                if (aryGeoKeys[0].dPercentage > PERCENTAGE)
                {
                    aryBetweenRest.Add(aryGeoKeys[0]);
                    break;
                }
                else
                {
                    QueryFetch = aryGeoKeys[0];
                    aryEquelRest.Add(aryGeoKeys[0]);
                }
            }
            else
            {
                for (YkUint i = 0; i < aryGeoKeys.GetSize(); i++)
                {
                    aryBetweenRest.Add(aryGeoKeys[i]);
                }
                break;
            }
        }
        return TRUE;
    }

    YkInt UGGeoHash::GetMinPercentagePos(YkArray<QueryParameter> &aryRest)
    {
        YkInt nPos = 0;
        for (YkUint i = 1; i < aryRest.GetSize(); i++)
        {
            if (aryRest[i].dPercentage < aryRest[nPos].dPercentage)
            {
                nPos = i;
            }
        }
        return nPos;
    }

    YkRect2D UGGeoHash::GetBoundsByKey(YkRect2D IndexBounds, YkLong nKey)
    {
        YkRect2D rc(IndexBounds);
        YkInt n = nKey & 0x3f;
        for (YkInt x = 0; x < n; x++)
        {
            YkDouble dCentX = (rc.left + rc.right) / 2.0;
            YkDouble dCentY = (rc.top + rc.bottom) / 2.0;
            YkLong nMode = nKey >> ((30 - x) * 2);
            YkInt nTemp = 3 & nMode;
            if (nTemp == 0)
            {
                rc.top = dCentY;
                rc.right = dCentX;
            }
            else if (nTemp == 1)
            {
                rc.bottom = dCentY;
                rc.right = dCentX;
            }
            else if (nTemp == 2)
            {
                rc.left = dCentX;
                rc.top = dCentY;
            }
            else
            {
                rc.left = dCentX;
                rc.bottom = dCentY;
            }
        }
        return rc;
    }

    YkBool UGGeoHash::IsIntersect(YkRect2D &rc1, YkRect2D &rc2)
    {
        if (!rc1.IsValid() || !rc2.IsValid())
        {
            return false;
        }
        return (rc1.right > rc2.left) && (rc1.left < rc2.right) && (rc1.top > rc2.bottom) && (rc1.bottom < rc2.top);
    }

    YkString UGGeoHash::GetBetweenFilter1(YkLong nGeoHashKey)
    {
        YkString strFilter;
        YkLong nUpperVal = nGeoHashKey;
        YkInt n = nGeoHashKey & 0x3f;
        YkLong lTemp = 1;
        for (YkInt x = 0; x <= (64 - n * 2 - 4); x++)
        {
            lTemp <<= 1;
            lTemp += 1;
        }
        nUpperVal = nUpperVal | lTemp;
        strFilter.Format(_U(" (geohash >= %lld and geohash <= %lld) "), nGeoHashKey, nUpperVal);
        return strFilter;
    }

    YkString UGGeoHash::GetBetweenFilter2(YkLong nGeoHashKey)
    {
        YkString strFilter;
        YkLong nUpperVal = nGeoHashKey;
        YkInt n = nGeoHashKey & 0x3f;
        YkLong lTemp = 1;
        for (YkInt x = 0; x <= (64 - n * 2 - 4); x++)
        {
            lTemp <<= 1;
            lTemp += 1;
        }
        nUpperVal = nUpperVal | lTemp;
        strFilter.Format(_U(" (ht.SmGeoHashKey >= %lld and ht.SmGeoHashKey <= %lld) "), nGeoHashKey, nUpperVal);
        return strFilter;
    }

    /**
	 * 返回查询条件中权重最小的下标.
	 *
	 * \param aryRest 查询条件数组
	 * \return -1 没有找到，其他值为下标
	 */
	YkInt UGGeoHash::FindLowLevelAndLowPercentage(YkArray<QueryParameter>& aryRest)
	{
		YkInt nPos = -1;
		for (YkUint i = 0; i < aryRest.GetSize(); i++)
		{
			//这里的计算公式为 level * weight * percentage
			if ((aryRest[i].nLevel * WEIGHTS[aryRest[i].nLevel] * aryRest[i].dPercentage) < PERCENTAGE)
			{
				nPos = i;
				break;
			}
		}
		return nPos;
	}

    // UGbool UGGeoHash::test_ComputerGeoHash()
    // {
    //     UGRect2D rcIndex(-180, 90, 180, -90);
    //     UGArray<UGlong> arykeys;
    //     UGRect2D rcgeobounds(1, 45, 45, 1);
    //     UGRect2D rcResult;
    //     UGGeoHash::ComputerGeoHash(rcIndex, arykeys, rcgeobounds);
    //     if (arykeys.GetSize() != 2)
    //     {
    //         return FALSE;
    //     }
    //     arykeys.RemoveAll();
    //     UGGeoHash::ComputerGeoHash(rcIndex, arykeys, rcIndex);
    //     if (arykeys.GetSize() != 1 || arykeys[0] != 0)
    //     {
    //         return FALSE;
    //     }

    //     return TRUE;
    // }

    // UGbool UGGeoHash::test_GetFilter()
    // {
    // 	UGRect2D rcIndex(-180,90,180,-90);
    // 	UGRect2D rcgeobounds(1,45,45,1);
    // 	UGString str = UGGeoHash::GetFilter2(rcIndex,rcgeobounds);
    // 	str = str.TrimLeft().TrimRight();
    // 	if( str != _U("ht.SmGeoHashKey = 0  or  ht.SmGeoHashKey = 3458764513820540929  or  ht.SmGeoHashKey = 3458764513820540930  or  ht.SmGeoHashKey = 3458764513820540931  or  (ht.SmGeoHashKey >= 3458764513820540931 and ht.SmGeoHashKey <= 3530822107858468863)  or  (ht.SmGeoHashKey >= 3530822107858468867 and ht.SmGeoHashKey <= 3602879701896396799)") )
    // 	{
    // 		return FALSE;
    // 	}

    // 	rcgeobounds.left = 0;
    // 	rcgeobounds.bottom = 0;
    // 	rcgeobounds.top = 90;
    // 	rcgeobounds.right = 180;
    // 	str = UGGeoHash::GetFilter2(rcIndex,rcgeobounds);
    // 	str = str.TrimLeft().TrimRight();
    // 	if( str != _U("ht.SmGeoHashKey = 0  or  ht.SmGeoHashKey = 3458764513820540929  or  (ht.SmGeoHashKey >= 3458764513820540929 and ht.SmGeoHashKey <= 4611686018427387903)") )
    // 	{
    // 		return FALSE;
    // 	}

    // 	rcgeobounds.left = 115.438728;
    // 	rcgeobounds.bottom = 39.4970131;
    // 	rcgeobounds.top = 40.1370544;
    // 	rcgeobounds.right = 116.632416;
    // 	str = UGGeoHash::GetFilter2(rcIndex,rcgeobounds);
    // 	str = str.TrimLeft().TrimRight();
    // 	if( str != _U("ht.SmGeoHashKey = 0  or  ht.SmGeoHashKey = 3458764513820540929  or  ht.SmGeoHashKey = 4035225266123964418  or  ht.SmGeoHashKey = 4107282860161892355  or  ht.SmGeoHashKey = 4161326055690338308  or  ht.SmGeoHashKey = 4165829655317708805  or  ht.SmGeoHashKey = 4165829655317708806  or  ht.SmGeoHashKey = 4166392605271130119  or  ht.SmGeoHashKey = 4166462974015307784  or  ht.SmGeoHashKey = 4166462974015307785  or  ht.SmGeoHashKey = 4166498158387396617  or  ht.SmGeoHashKey = 4166462974015307786  or  ht.SmGeoHashKey = 4166462974015307787  or  ht.SmGeoHashKey = 4166463798649028620  or  ht.SmGeoHashKey = 4166463798649028621  or  ht.SmGeoHashKey = 4166463850188636174  or  ht.SmGeoHashKey = 4166463858778570767  or  ht.SmGeoHashKey = 4166463859852312592  or  ht.SmGeoHashKey = 4166463860389183505  or  (ht.SmGeoHashKey >= 4166392605271130120 and ht.SmGeoHashKey <= 4166462974015307775)  or  (ht.SmGeoHashKey >= 4166471770108329994 and ht.SmGeoHashKey <= 4166476168154841087)  or  (ht.SmGeoHashKey >= 4166498158387396618 and ht.SmGeoHashKey <= 4166502556433907711)  or  (ht.SmGeoHashKey >= 4166506954480418826 and ht.SmGeoHashKey <= 4166511352526929919)  or  (ht.SmGeoHashKey >= 4166465173038563339 and ht.SmGeoHashKey <= 4166466272550191103)  or  (ht.SmGeoHashKey >= 4166463523771121676 and ht.SmGeoHashKey <= 4166463798649028607)  or  (ht.SmGeoHashKey >= 4166463936087982093 and ht.SmGeoHashKey <= 4166464004807458815)  or  (ht.SmGeoHashKey >= 4166463833008766990 and ht.SmGeoHashKey <= 4166463850188636159)  or  (ht.SmGeoHashKey >= 4166463858778570768 and ht.SmGeoHashKey <= 4166463859852312575)  or  (ht.SmGeoHashKey >= 4166463860926054416 and ht.SmGeoHashKey <= 4166463861999796223)  or  (ht.SmGeoHashKey >= 4166463861999796240 and ht.SmGeoHashKey <= 4166463863073538047)  or  (ht.SmGeoHashKey >= 4166463860523401234 and ht.SmGeoHashKey <= 4166463860590510079)  or  (ht.SmGeoHashKey >= 4166463860590510098 and ht.SmGeoHashKey <= 4166463860657618943)") )
    // 	{
    // 		return FALSE;
    // 	}

    // 	rcgeobounds.left = 116.438728;
    // 	rcgeobounds.bottom = 40.0970131;
    // 	rcgeobounds.top = 40.1370544;
    // 	rcgeobounds.right = 116.632416;
    // 	str = UGGeoHash::GetFilter2(rcIndex,rcgeobounds);
    // 	str = str.TrimLeft().TrimRight();
    // 	if( str != _U("ht.SmGeoHashKey = 0  or  ht.SmGeoHashKey = 3458764513820540929  or  ht.SmGeoHashKey = 4035225266123964418  or  ht.SmGeoHashKey = 4107282860161892355  or  ht.SmGeoHashKey = 4161326055690338308  or  ht.SmGeoHashKey = 4165829655317708805  or  ht.SmGeoHashKey = 4165829655317708806  or  ht.SmGeoHashKey = 4166392605271130119  or  ht.SmGeoHashKey = 4166462974015307784  or  ht.SmGeoHashKey = 4166498158387396617  or  ht.SmGeoHashKey = 4166506954480418826  or  ht.SmGeoHashKey = 4166509153503674379  or  ht.SmGeoHashKey = 4166509978137395212  or  ht.SmGeoHashKey = 4166509978137395213  or  ht.SmGeoHashKey = 4166509703259488268  or  ht.SmGeoHashKey = 4166509703259488269  or  ht.SmGeoHashKey = 4166509720439357454  or  ht.SmGeoHashKey = 4166509724734324751  or  ht.SmGeoHashKey = 4166509995317264398  or  ht.SmGeoHashKey = 4166509995317264399  or  ht.SmGeoHashKey = 4166509724734324752  or  ht.SmGeoHashKey = 4166509725002760209  or  (ht.SmGeoHashKey >= 4166506954480418827 and ht.SmGeoHashKey <= 4166508053992046591)  or  (ht.SmGeoHashKey >= 4166509153503674380 and ht.SmGeoHashKey <= 4166509428381581311)  or  (ht.SmGeoHashKey >= 4166509428381581324 and ht.SmGeoHashKey <= 4166509703259488255)  or  (ht.SmGeoHashKey >= 4166509978137395214 and ht.SmGeoHashKey <= 4166509995317264383)  or  (ht.SmGeoHashKey >= 4166509771978965005 and ht.SmGeoHashKey <= 4166509840698441727)  or  (ht.SmGeoHashKey >= 4166509725808066576 and ht.SmGeoHashKey <= 4166509726881808383)  or  (ht.SmGeoHashKey >= 4166509995317264400 and ht.SmGeoHashKey <= 4166509996391006207)  or  (ht.SmGeoHashKey >= 4166509996391006224 and ht.SmGeoHashKey <= 4166509997464748031)  or  (ht.SmGeoHashKey >= 4166509725069869074 and ht.SmGeoHashKey <= 4166509725136977919)  or  (ht.SmGeoHashKey >= 4166509725204086802 and ht.SmGeoHashKey <= 4166509725271195647)") )
    // 	{
    // 		return FALSE;
    // 	}
    // 	return TRUE;
    // }
    // UGbool UGGeoHash::test_GetBoundsByKey()
    // {
    // 	/*
    // 	if( GetInfo()->GetIndexType() == IdxGeoHash )
    // 	{
    // 		UGDatasetVector *pDT = (UGDatasetVector *)this->GetDataSource()->GetDataset(_U("New_Region"));
    // 		pDT->Open();
    // 		UGQueryDef query;
    // 		query.m_nCursorType = UGQueryDef::OpenDynamic;

    // 		UGRecordset *prs = pDT->Query(query);
    // 		UGString strSQL;
    // 		strSQL.Format(_U("Select * from Sm_GeoHash_smdtv_%d"),GetID());
    // 		UGODBCCICursor cursor(GetConnection());
    // 		try
    // 		{
    // 			cursor.Open();
    // 			UGlong nKey;
    // 			UGint nID;
    // 			UGRect2D rcIndexBounds(-180.0,90.0,180.0,-90.0);
    // 			UGRect2D rect;
    // 			cursor.Prepare(strSQL);
    // 			cursor.DefineByPos(1,&nKey, 8, NULL, odbcBigInt);
    // 			cursor.DefineByPos(2,&nID, 4, NULL, odbcLong);
    // 			cursor.Execute(__LINE__,__UGFILE__,_U(""));
    // 			SQLRETURN rc = cursor.Fetch();
    // 			while( SQL_SUCCEEDED(rc) )
    // 			{
    // 				rect = UGGeoHash::GetBoundsByKey(rcIndexBounds,nKey);
    // 				UGGeoRegion geo;
    // 				geo.Make(rect);
    // 				prs->AddNew(&geo);
    // 				prs->SetFieldValue(_U("SmUserID"),UGVariant(nID));
    // 				prs->Update();
    // 				rc = cursor.Fetch();
    // 			}
    // 			cursor.Close();
    // 		}
    // 		catch ( UGODBCCIException* e )
    // 		{
    // 			delete e;
    // 			e = NULL;
    // 			cursor.Close();
    // 		}
    // 	}
    // 	*/

    // 	UGRect2D rcIndex(-180,90,180,-90);
    // 	UGArray<UGlong> arykeys;
    // 	UGRect2D rcgeobounds(1,45,45,1);
    // 	UGRect2D rcResult;
    // 	UGGeoHash::ComputerGeoHash(rcIndex,arykeys,rcgeobounds);
    // 	UGRect2D rc = UGGeoHash::GetBoundsByKey(rcIndex,arykeys[0]);
    // 	return UGIS0(rc.left -0.0 );
    // }

    // UGbool UGGeoHash::test_ComputerPointGeoHash()
    // {

    // 	UGRect2D rcIndex(-180,90,180,-90);
    // 	UGRect2D rcgeobounds(45.02,45.02,45.02,45.02);
    // 	UGArray<UGlong> arykeys;
    // 	UGGeoHash::ComputerGeoHash(rcIndex,arykeys,rcgeobounds);

    // 	UGPoint2D pnt(45.02,45.02);
    // 	UGlong lPkey = 0;
    // 	UGGeoHash::ComputerPointGeoHash(rcIndex,lPkey,pnt);
    // 	return TRUE;
    // }

    // UGbool UGGeoHash::test_ComputerPointGeoHash1()
    // {

    // 	UGRect2D rcIndex(-180,90,180,-90);
    // 	UGRect2D rcgeobounds(45.02,45.02,45.02,45.02);
    // 	UGArray<UGlong> arykeys;

    // 	UGint nTimes = 10000;
    // 	for (int i = 0; i < nTimes; i++)
    // 	{
    // 		arykeys.RemoveAll();
    // 		UGGeoHash::ComputerGeoHash(rcIndex,arykeys,rcgeobounds);
    // 	}
    // 	UGPoint2D pnt(45.02,45.02);
    // 	UGlong lPkey = 0;

    // 	for (int i = 0; i < nTimes; i++)
    // 	{
    // 		UGGeoHash::ComputerPointGeoHash(rcIndex,lPkey,pnt);
    // 	}
    // 	return TRUE;
    // }

    // UGbool UGGeoHash::test_ComputerGeoHash2()
    // {
    // 	UGRect2D rcIndex(-180,90,180,-90);
    // 	UGArray<UGlong> arykeys;
    // 	//UGRect2D rcgeobounds(-89,44,89,-44);
    // 	UGRect2D rcgeobounds(-91,46,91,-46);
    // 	UGRect2D rcResult;
    // 	UGGeoHash::ComputerGeoHash(rcIndex,arykeys,rcgeobounds,25,4);
    // 	if ( arykeys.GetSize() != 4 )
    // 	{
    // 		return  FALSE;
    // 	}
    // 	UGArray<UGlong> arykeys2;
    // 	UGGeoHash::ComputerGeoHash2(rcIndex,arykeys2,rcgeobounds);

    // 	UGSort<UGlong >::QSort(arykeys.GetData(), 0, arykeys.GetSize()-1);
    // 	if( arykeys[0] != arykeys2[0] || arykeys[1] != arykeys2[1] )
    // 	{
    // 		return FALSE;
    // 	}

    // 	UGArray<UGlong> arykeys3;
    // 	UGGeoHash::ComputerGeoHash2(rcIndex,arykeys3,rcIndex);
    // 	if( arykeys3.GetSize() != 1 || arykeys3[0] != 0)
    // 	{
    // 		return FALSE;
    // 	}

    // 	return FALSE;
    // }

}
