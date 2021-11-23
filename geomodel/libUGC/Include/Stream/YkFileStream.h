/*
 *
 * YkFileStream.h
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

#ifndef AFX_YKFILESTREAM_H__4333B6A9_E370_424C_BA82_C44F452386D0__INCLUDED_
#define AFX_YKFILESTREAM_H__4333B6A9_E370_424C_BA82_C44F452386D0__INCLUDED_

#pragma once

#include "Stream/YkStream.h"
#include "Stream/YkByteArray.h"

namespace Yk {

class STREAM_API YkFileStream : public YkStream
{
public:
	//!\brief 默认构造函数
	STREAM_APIF YkFileStream();
	//!\brief 默认析构函数
	virtual STREAM_APIF ~YkFileStream();	
	
	//! \brief 打开文件流。
	//! \param filename 指定文件名[in]。
	//! \param save_or_load 指定打开的方式，只读、只写或读写[in]。
	//! \param size 缓存的大小[in]。
	STREAM_APIF YkBool Open(const YkChar* filename, YkStreamDirection save_or_load, YkUint size=8192);			

	//! \brief 关闭文件流。
	virtual STREAM_APIF YkBool Close();
	
	//! \brief 设置文件指针的位置，如果超过文件大小，会导致文件增长。
	//! \param offset 偏移量[in]。
	//! \param whence 起始位置[in]。
	virtual STREAM_APIF YkBool SetPosition(YkLong offset,YkWhence whence=YkFromStart);	
	
	//! \brief 得到文件长度
	virtual STREAM_APIF YkUlong GetLength();

	//! \brief 判断是否已到文件尾
	virtual STREAM_APIF YkBool IsEOF() const;


	//! \brief 判断文件流是否处于打开状态。
	//! \return 打开状态返回true,关闭状态返回false。
	STREAM_APIF YkBool IsOpen() const;

	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkUchar& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkAchar& v){ YkStream::operator<<(v); return *this; }

	STREAM_APIF YkStream& operator<<(const YkWchar& v){ YkStream::operator<<(v); return *this; }

	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkUshort& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkShort& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkUint& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkInt& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkFloat& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkDouble& v){ YkStream::operator<<(v); return *this; }
#ifdef Yk_LONG
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkLong& v){ YkStream::operator<<(v); return *this; }
	//! \brief operator <<
	STREAM_APIF YkStream& operator<<(const YkUlong& v){ YkStream::operator<<(v); return *this; }
#endif
	
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUchar* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkAchar* p,YkSizeT n){ YkStream::Save(p,n); return *this; }

	STREAM_APIF YkStream& Save(const YkWchar* p,YkSizeT n){ YkStream::Save(p,n); return *this; }

	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUshort* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkShort* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUint* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkInt* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkFloat* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkDouble* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
#ifdef Yk_LONG
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkLong* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUlong* p,YkSizeT n){ YkStream::Save(p,n); return *this; }
#endif
	
	//! \brief  Load single items from stream
	STREAM_APIF YkStream& operator>>(YkUchar& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkAchar& v){ YkStream::operator>>(v); return *this; }

	STREAM_APIF YkStream& operator>>(YkWchar& v){ YkStream::operator>>(v); return *this; }

	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkUshort& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkShort& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkUint& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkInt& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkFloat& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkDouble& v){ YkStream::operator>>(v); return *this; }
#ifdef Yk_LONG
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkLong& v){ YkStream::operator>>(v); return *this; }
	//! \brief operator >>
	STREAM_APIF YkStream& operator>>(YkUlong& v){ YkStream::operator>>(v); return *this; }
#endif
	
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUchar* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkAchar* p,YkSizeT n){ YkStream::Load(p,n); return *this; }

	STREAM_APIF YkStream& Load(YkWchar* p,YkSizeT n){ YkStream::Load(p,n); return *this; }

	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUshort* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkShort* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUint* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkInt* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkFloat* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkDouble* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
#ifdef Yk_LONG
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkLong* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUlong* p,YkSizeT n){ YkStream::Load(p,n); return *this; }
#endif
	//! \brief 读文件到字节数组中
	//! \param byteArray [in]。
	//! \return 。
	//! \remarks 。
	//! \attention 。
	STREAM_APIF YkBool LoadByteArray(YkByteArray& byteArray);

	//! \brief 写字节数组到文件中
	//! \param byteArray [in]。
	//! \return 。
	//! \remarks 。
	//! \attention 。
	STREAM_APIF YkBool SaveByteArray(const YkByteArray& byteArray);

	STREAM_APIF YkStream& operator<<(const YkString& s);

	//! \brief 从流中读出字符串
	STREAM_APIF YkStream& operator>>(YkString& s);

private:
	YkInt file;
protected:
	virtual STREAM_APIF YkSizeT WriteBuffer(YkSizeT count);
	virtual STREAM_APIF YkSizeT ReadBuffer(YkSizeT count);
	//{{解决在YkStreamLoadSave模式下，Load和Save混乱的问题，这里主要是设置一些标志来控制Load和Save.
	virtual STREAM_APIF void SetSignLoadSave(YkInt nSign);
	//}}
};

}

#endif


