/*
 *
 * YkFileStdio.h
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

#if !defined(AFX_YKFILESTDIO_H__93B98C9C_498A_43C5_8D3D_F4D4D3FD5FC5__INCLUDED_)
#define AFX_YKFILESTDIO_H__93B98C9C_498A_43C5_8D3D_F4D4D3FD5FC5__INCLUDED_

#pragma once

#include "Stream/YkFileStream.h"

namespace Yk{

class STREAM_API YkFileStdio  
{
public:
	
	//! \brief 文件换行符类型
	enum EnterType
	{
		Enter_Adapt = 1,   //当前的根据系统
		Enter_Windows = 2, //Windows上的换行符
		Enter_Unix = 3    //Linux和Unix上的换行符
	};


	YkFileStdio();
	~YkFileStdio();

public:

	//! \brief 		打开文本文件
	//! \return 	YkBool
	//! \param 		filename
	//! \param 		save_or_load
	//! \param 		size=8192
	//! \param      在Unicode版本中，按照指定编码读写文件，如果文件头为Unicode，则指定无效
	//! \remarks 
	YkBool Open(const YkChar* filename,YkStreamDirection save_or_load,YkUint size=8192,YkString::Charset charset = YkString::Default);

	//! \brief 是否在文件未
	YkBool IsEOF() const;
	//! \brief 设置操作系统换行符
	void SetCurEnterType(EnterType eEnterType);
	//! \brief 得到文件当前的位置
	YkUlong GetPosition() const;
	//! \brief 设置文件的位置
	YkBool SetPosition(YkLong offset,YkWhence whence=YkFromStart);
	//! \brief 得到文件的长度
	YkLong GetLength();

	//! \brief 写一行文本到文件
	//! \remarks 写入一行数据时会自动添加行未的换行符
	YkBool WriteLine(const YkString& strSource);
    
	//! \brief 直接写入到文件中
    void Write(const YkString& strSource);
	
	//! \brief 把数据即时写入到文件中
	YkBool Flush();

	//! \brief 关闭文件
	YkBool Close();
	YkBool WriteLine(const YkAchar *pSrc,YkInt nLength);
	void Write(const YkAchar *pSrc,YkInt nLength);
	//! \brief  从文件中读取一行文本
	//! \remarks 读取一行数据时会自动去掉行未的换行符，这里自动识别行未的换行符
	YkInt ReadLine(YkMBString& strSource);
#ifdef _UGUNICODE
	YkString::Charset GetCharset(){return m_eCharset;};
#else
	YkString::Charset GetCharset(){return YkString::Default;};
#endif
#ifdef _UGUNICODE
	YkInt ReadLine(YkString& strSourceIn);
private:
	YkMBString::Charset m_eCharset;
	YkInt m_nStartPos;
#endif
private:
	YkFileStream m_FileStream;  //文件流对象
	EnterType m_eEnterType;		//回车符类型,各个系统都不一样
};


}
#endif

