/*
 *
 * YkColorValue3DEx.h
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

#if !defined(AFX_YKCOLORVALUE3DEX_H__77F37090_FF08_46BD_9EEB_312314BBBD47__INCLUDED_)
#define AFX_YKCOLORVALUE3DEX_H__77F37090_FF08_46BD_9EEB_312314BBBD47__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"

namespace  Yk 
{

#define YkGetAValue(rgb)      ((YkByte)((rgb)>>24))
#define YkGetRValue(rgb)      ((YkByte)(rgb))
#define YkGetGValue(rgb)      ((YkByte)(((YkUshort)(rgb)) >> 8))
#define YkGetBValue(rgb)      ((YkByte)((rgb)>>16))

typedef YkUint RGBA;
typedef YkUint ARGB;
typedef YkUint ABGR;

//! \brief 三维颜色类。
class TOOLKIT_API YkColorValue3D  
{
public:
	//! \brief 静态变量黑色。
	static YkColorValue3D Black;
	//! \brief 静态变量黑色
	static YkColorValue3D White;
	//! \brief 静态变量黑色
	static YkColorValue3D Red;
	//! \brief 静态变量黑色
	static YkColorValue3D Green;
	//! \brief 静态变量黑色
	static YkColorValue3D Blue;
	//! \brief 静态变量黑色
	static YkColorValue3D Yellow;
	//! \brief 三维颜色类的构造函数。
	//! \param color 32位的颜色变量[in]。
	YkColorValue3D(YkUint color);	
	//! \brief 三维颜色类的构造函数。
	//! \param red 红色分量，范围0-1[in]。
	//! \param green 红色分量，范围0-1[in]。
	//! \param blue  蓝色分量，范围0-1[in]。
	//! \param alpha 不透明分量，范围0-1[in]。
	YkColorValue3D( YkDouble red = 1.0f,
				    YkDouble green = 1.0f,
					YkDouble blue = 1.0f,
					YkDouble alpha = 1.0f )	: r(red), g(green), b(blue), a(alpha)
	{ }		
	//! \brief 重载等值判断运算符。
	//! \param rhs [in]。
	//! \return 是否相等。
	YkBool operator==(const YkColorValue3D& rhs) const;	
	//! \brief 重载不等值判断运算符。
	//! \param rhs [in]。
	//! \return 是否不等。
	YkBool operator!=(const YkColorValue3D& rhs) const;	
	//! \brief 红色分量，范围0-1。
	YkDouble r;
	//! \brief 绿色分量，范围0-1。
	YkDouble g;
	//! \brief 蓝色分量，范围0-1。
	YkDouble b;
	//! \brief 不透明分量，范围0-1。
	YkDouble a;
	//! \brief 转换为以RGBA32位整型表示的颜色。
	//! \return RGBA，32位整型表示的颜色。
	RGBA GetAsLongRGBA(void) const;	
	//! \brief 得到现有颜色的反色。
	//! \return 现有颜色的反色。
	YkColorValue3D GetReverseColor() const;
	//! \brief 传入R、G、B、A分量构建三维颜色 。
	//! \param nRed 红色分量，范围0-255[in]。
	//! \param nGreen 绿色分量，范围0-255[in]。
	//! \param nBlue 蓝色分量，范围0-255[in]。
	//! \param nAlpha 不透明度分量，范围0-255[in]。
	void SetValue(YkInt nRed,YkInt nGreen,YkInt nBlue,YkInt nAlpha);	
	//! \brief 传入R、G、B、A分量构建三维颜色。
	//! \param dRed 红色分量，范围0-1.0[in]。
	//! \param dGreen 绿色分量，范围0-1[in]。
	//! \param dBlue 蓝色分量，范围0-1[in]。
	//! \param dAlpha 不透明度分量，范围0-1[in]。
	void SetValue(YkDouble dRed,YkDouble dGreen,YkDouble dBlue,YkDouble dAlpha);	
	//! \brief  将YkColor转换为YkColorValue3D
	void SetValue(YkColor color);
	//! \brief  将YkColorValue3D转换为YkColor
	YkColor GetValue() const;
	//! \brief 得到R、G、B、A分量颜色 。
	//! \param nRed 红色分量，范围0-255[out]。
	//! \param nGreen 绿色分量，范围0-255[out]。
	//! \param nBlue 蓝色分量，范围0-255[out]。
	//! \param nAlpha 不透明度分量，范围0-255[out]。
	void GetRGBA(YkInt& nRed,YkInt& nGreen,YkInt& nBlue,YkInt& nAlpha) const; 
	//! \brief 转换为以ARGB 32位整型表示的颜色。
	//! \return ARGB，32位整型表示的颜色。
	ARGB GetAsLongARGB(void) const;
	//! \brief 转换为以ABGR 32位整型表示的颜色。
	//! \return ABGR，32位整型表示的颜色。
	ABGR GetAsLongABGR(void) const;
	//! \brief 转换为以ABGR 32位整型表示的颜色。
	//! \return ABGR，32位整型表示的颜色。
	ABGR GetAsCOLORREF(void)const;
	//! \brief 转换为以ABGR 32位整型表示的颜色。
	//! \return ABGR，32位整型表示的颜色。
	void FromAsABGR(YkInt color);
	//! \brief 转换为以YkColor 32位整型表示的颜色。
	//! \return YkColor，32位整型表示的颜色。
	static YkColor ABGRToColor(YkInt color);
};

}
#endif


