/*
 *
 * YkStream.h
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

#ifndef AFX_YKSTREAM_H__756193E2_1352_4592_A350_0049AE0DF6E2__INCLUDED_
#define AFX_YKSTREAM_H__756193E2_1352_4592_A350_0049AE0DF6E2__INCLUDED_

#pragma once

#include "YkDefines.h"

namespace Yk {

//! \brief 流的操作模式
enum YkStreamDirection
{
	YkStreamDead=0,
	//! \brief 只写
	YkStreamSave=1,
	//! \brief 只读
	YkStreamLoad=2,
	//! \brief 读写
	YkStreamLoadSave = 3
};

//! \brief 流的状态
enum YkStreamStatus 
{
	//! \brief 状态良好
	YkStreamOK=0,
	//! \brief 读取时已到流的尾部
	YkStreamEnd=1,
	//! \brief 写入时流已经写满
	YkStreamFull=2,
	//! \brief 流不可写
	YkStreamNoWrite=3,
	//! \brief 流不可读
	YkStreamNoRead=4,
	//! \brief 内存分配错误
	YkStreamAlloc=5,
	//! \brief 流内部的数据格式错误
	YkStreamFormat=6,
	YkStreamFailure=7
};

//! \brief 流在设置位置（SetPosition）时的起始位置。
enum YkWhence 
{
	//! \brief 开始处。
	YkFromStart=0,
	//! \brief 当前位置。
	YkFromCurrent=1,
	//! \brief 结尾处。
	YkFromEnd=2
};

//! \brief 流的虚基类，一般作为存取函数的引用型参数，不可实例化。
//! \remarks 实例化时，内存流采用YkMemoryStream，文件流采用YkFileStream。
class STREAM_API YkStream
{
public:
	//! \brief 默认构造函数。
	STREAM_APIF YkStream();	

	//! \brief 析构函数，特意设置为纯虚函数，因为YkStream本身不可实例化。
	virtual STREAM_APIF  ~YkStream();	
	
	//! \brief 关闭流。
	virtual STREAM_APIF YkBool Close();
	
	//! \brief 流的刷新，仅对文件流有效，即把缓存中的内容写入到文件中。
	virtual STREAM_APIF YkBool Flush();  	

	//! \brief 得到流全部空间的大小，文件流时等同于内部缓存区的大小。
	virtual STREAM_APIF YkSizeT GetSpace() const;	

	//! \brief 设置流的空间大小，内存流时只能在空间由流所有时使用，文件流时则改变缓存区大小。
	//! \param space 设置的空间大小值[in]。
	//! \attention 对内存流，在内存空间不由流所有时调用，会发生Assert错误。
	STREAM_APIF void SetSpace(YkSizeT space);  		

	//! \brief 得到当前流的状态。
	STREAM_APIF YkStreamStatus  GetStatus() const;  	

	//! \brief 判断流是否已到结尾，主要供文件流使用
	virtual STREAM_APIF YkBool IsEOF() const;  

	//! \brief 设置流已经发生了某种错误。
	//! \param err 设置的错误类型[in]。
	//! \remarks 一般不需要使用。
	STREAM_APIF void  SetError(YkStreamStatus err);  

	//! \brief 得到流是只读、只写或者读写。
	STREAM_APIF YkStreamDirection GetDirection() const;  	
	
	//! \brief 得到当前指针的位置，以byte为单位。
	virtual STREAM_APIF YkUlong GetPosition() const;  
	
	//! \brief 设置当前指针的位置。
	//!  \remarks 对文件流，如果要设置的位置超过原来文件大小，会导致文件长度增加，
	//! 对内存流，如果要设置的位置超过内存空间大小，且内存由流所有，会导致内存空间重新分配，
    //!	内存空间不为流所有，则流会处于Full状态
	//! \param offset 偏移量[in]。
	//! \param whence 偏移量的起始位置[in]。
	virtual STREAM_APIF YkBool SetPosition(YkLong offset,YkWhence whence=YkFromStart);	
	
	//! \brief 对于文件流，得到文件长度；对于内存流，得到流中实际有效数据的长度（小于等于内存空间）。
	virtual STREAM_APIF YkUlong GetLength();
	
	STREAM_APIF void SetSwapBytes(YkBool s) {swap = s; }

	//! \brief 得到是否需要进行字节顺序的交换。
	//! \remarks 数据采用LittleEndian方式存储，在BigEndian的CPU中需要进行字节顺序交换。
	STREAM_APIF YkBool SwapBytes() const; 

	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkUchar& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkAchar& v);

	virtual STREAM_APIF YkStream& operator<<(const YkWchar& v){ *this<<(YkShort)v; return *this; }

	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkUshort& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkShort& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkUint& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkInt& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkFloat& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkDouble& v);
#ifdef Yk_LONG
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkLong& v);
	//! \brief operator <<
	virtual STREAM_APIF YkStream& operator<<(const YkUlong& v);
#endif

	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkUchar* p,YkSizeT n);	
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkAchar* p,YkSizeT n);

	virtual STREAM_APIF YkStream& Save(const YkWchar* p,YkSizeT n){ Save((YkShort*)p, n); return *this; }

	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkUshort* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkShort* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkUint* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkInt* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkFloat* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkDouble* p,YkSizeT n);
#ifdef Yk_LONG
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkLong* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Save(const YkUlong* p,YkSizeT n);
#endif

	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkUchar& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkAchar& v);

	virtual STREAM_APIF YkStream& operator>>(YkWchar& v){ *this>>(YkShort&)v; return *this; }

	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkUshort& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkShort& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkUint& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkInt& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkFloat& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkDouble& v);
#ifdef Yk_LONG
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkLong& v);
	//! \brief operator >>
	virtual STREAM_APIF YkStream& operator>>(YkUlong& v);
#endif
	
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkUchar* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkAchar* p,YkSizeT n);

	virtual STREAM_APIF YkStream& Load(YkWchar* p,YkSizeT n){ Load((YkShort*)p, n); return *this; }

	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkUshort* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkShort* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkUint* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkInt* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkFloat* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkDouble* p,YkSizeT n);
#ifdef Yk_LONG
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkLong* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	virtual STREAM_APIF YkStream& Load(YkUlong* p,YkSizeT n);
#endif


	//! \brief 把字符串保存到流中
	STREAM_APIF YkStream& operator<<(const YkString& s);
		
	//! \brief 从流中读出字符串
	STREAM_APIF YkStream& operator>>(YkString& s);

	STREAM_APIF void SetCharset(YkString::Charset eCharset)
	{
		m_eCharset = eCharset;
	}
	STREAM_APIF YkString::Charset GetCharset()
	{
		return m_eCharset;
	}

private:
	YkBool swap;

	// 考虑到Stream中要操作字符串，就需要考虑字符串的编码问题，增加该变量来控制对Stream
	// 对于多字节版本来说，只在读取YkString的时候有效，会把该YkString的字符集设计为m_eCharset，写入的时候不考虑该变量
	// 对于Unicode版本来说，在写入YkString的时候会先把字符集转换成m_eCharset，再写入，读取时，也会从m_eCharset转换成Unicode
	YkString::Charset m_eCharset;
protected:
	YkUchar           *begptr;
	YkUchar           *endptr;
	YkUchar           *wrptr;
	YkUchar           *rdptr;
	YkUlong			 pos;
	
	//文件的实际长度,主要用在内存文件中
	YkUlong m_lActualPos;
	
	YkStreamDirection  dir;
	YkStreamStatus     code;
	YkBool             owns;
	
	STREAM_APIF enum YkSignLoadSave 
	{
		YkSignStreamLoad =0,
		YkSignStreamSave =1
	};
	//YkStreamLoadSave模式下Load和Save和标识
	YkSignLoadSave     SignLoadSave; 

private:
	STREAM_APIF YkStream(const YkStream&);
	STREAM_APIF YkStream &operator=(const YkStream&);
	
protected:
	//这个函数被子类调用，外面不直接使用
	STREAM_APIF YkBool Open(YkStreamDirection save_or_load, YkSizeT size=8192,  YkUchar* data=NULL);  	
	
	virtual STREAM_APIF YkSizeT WriteBuffer(YkSizeT count);
	virtual STREAM_APIF YkSizeT ReadBuffer(YkSizeT count);
	
	
	//解决在YkStreamLoadSave模式下，Load和Save混乱的问题，这里主要是设置一些标志来控制Load和Save.
	virtual STREAM_APIF void SetSignLoadSave(YkInt nSign);
};


}

#endif



