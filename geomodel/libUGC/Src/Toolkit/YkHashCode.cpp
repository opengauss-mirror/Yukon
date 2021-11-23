/*
 *
 * YkHashCode.cpp
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

#include "Toolkit/YkHashCode.h"

namespace Yk{
YkHashCode::YkHashCode()
{

}

YkHashCode::~YkHashCode()
{

}

YkInt YkHashCode::StringToHashCode(const YkString& strValue)
{	
#ifdef _UGUNICODE
	YkInt nWCharCount = strValue.GetLength();
	const YkUshort* chPtr = (const YkUshort* )strValue.Cstr();
#else
	YkArray<YkUshort> aryChars ;
	if(YkC_ISBIGENDIAN)
	{
		//最终目的是转换成Windows上标准的2个字节, LE 的String 
		YkInt nSize = strValue.GetLength();
		YkInt i =0;
		YkString strResult;
		YkChar* charData = (YkChar*)strValue.Cstr();
		YkTextCodec decode(YkString::UCS2BE, strValue.GetCharset());
		decode.Convert(strResult, (const YkChar *)charData, nSize);
		for(i=0;i<strResult.GetLength()/2;i++)
		{
			aryChars.Add(((YkUshort*)strResult.Cstr())[i]);
		}
		aryChars.Add(0);
	}
	else
	{
		YkStringUCS2LE strTemp(strValue);
		aryChars = strTemp.GetChars();
	}

	YkInt nWCharCount = (YkInt)aryChars.GetSize();
	const YkUshort* chPtr = aryChars.GetData();
#endif
	YkInt num = 0x15051505;
	YkInt num2 = num;
	const YkUshort *numPtr = chPtr;
	for (YkInt i = nWCharCount; i > 0; --i)
	{
		num = (((num << 5) + num) + (num >> 0x1b)) ^ numPtr[0];
		if (i < 2)
		{
			break;
		}
		num2 = (((num2 << 5) + num2) + (num2 >> 0x1b)) ^ numPtr[1];
		numPtr += 1;
	}
	return (num + (num2 * 0x5d588b65));
}

YkInt YkHashCode::BoolToHashCode(YkBool bValue)
{
	if(bValue)
	{
		return 1;
	}
	return 0;
}

YkInt YkHashCode::ShortToHashCode(YkShort nValue)
{
	return (nValue | (nValue << 0x10));
}

YkInt YkHashCode::FastStringToHashCode(const YkString& strValue)
{	
#ifdef _UGUNICODE
	YkMBString strTemp;
	strValue.ToMBString(strTemp);
	YkInt nSize = strTemp.GetLength();
	const YkAchar* pch = strTemp.Cstr();
#else
	YkInt nSize = strValue.GetLength();
	const YkAchar* pch = strValue.Cstr();
#endif
	YkInt num = 0x15051505;
	YkInt num2 = num;
	for (YkInt i = nSize; i > 0; i -= 2)
	{
		num = (((num << 5) + num) + (num >> 0x1b)) ^ pch[0];
		if (i <= 1)
		{
			break;
		}
		num2 = (((num2 << 5) + num2) + (num2 >> 0x1b)) ^ pch[1];
		pch += 2;
	}
	return (num + (num2 * 0x5d588b65));
}

YkLong YkHashCode::StringToHashCode64(const YkString& strValue)
{
	const YkUint m=0x5bd1e995;
	const YkInt r=24;
	YkUint nSeed=0xEE6B27EB;

	YkMBString strTempKey;
	strTempKey.SetCharset(YKCharset::GB18030);
	Unicode2MBString(strValue,strTempKey);
	YkInt nLen= strTempKey.GetLength();
	YkUint h1=nSeed^nLen;
	YkUint h2=0;

	const YkUint* pData=(const YkUint*)(strTempKey.Cstr());
	while(nLen>=8)
	{
		YkUint k1=*pData++;
		k1*=m;k1^=k1>>r;k1*=m;
		h1*=m;h1^=k1;
		nLen-=4;

		YkUint k2=*pData++;
		k2*=m;k2^=k2>>r;k2*=m;
		h2*=m;h2^=k2;
		nLen-=4;
	}
	if(nLen>=4)
	{
		YkUint k1=*pData++;
		k1*=m;k1^=k1>>r;k1*=m;
		h1*=m;h1^=k1;
		nLen-=4;
	}
	switch (nLen)
	{
	case 3:	h2^=((YkUchar*)pData)[2]<<16;
	case 2:	h2^=((YkUchar*)pData)[1]<<8;
	case 1:	h2^=((YkUchar*)pData)[0];
		h2*=m;	
	}
	h1^=h2>>18;h1*=m;
	h2^=h1>>22;h2*=m;
	h1^=h2>>17;h1*=m;
	h2^=h1>>19;h2*=m;
	
	YkUlong nResult=h1;
	nResult=(nResult<<32)|h2;

	YkLong lResult = nResult - 9223372036854775808;
	return lResult;
}

}

