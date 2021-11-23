/*
 *
 * YkDataCodec.h
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

#ifndef AFX_YKDATACODEC_H__5AC9D6F2_1591_4DDC_BD08_190AEF63A1DE__INCLUDED_
#define AFX_YKDATACODEC_H__5AC9D6F2_1591_4DDC_BD08_190AEF63A1DE__INCLUDED_

#pragma once

#include "Stream/YkMemoryStream.h"

namespace Yk {

#define YKMAXWORD 0xffff

//! 空间数据编码类
class TOOLKIT_API YkDataCodec  
{
public:
	//! 编码类型
	enum CodecType
	{
		//! 不使用编码方式
 		encNONE		= 0,

		//! \brief s3t dxtn纹理压缩
		enrS3TCDXTN	= 14,
	};
};
}

#endif

