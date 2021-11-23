/*
 *
 * YkMemoryStream.h
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

#ifndef AFX_YKMEMORYSTREAM_H__7C9F623B_99D6_41A3_AF0F_440843D63DF5__INCLUDED_
#define AFX_YKMEMORYSTREAM_H__7C9F623B_99D6_41A3_AF0F_440843D63DF5__INCLUDED_

#pragma once

#include "YkStream.h"

namespace Yk {

class STREAM_API YkMemoryStream : public YkStream
{
private:
	STREAM_APIF YkMemoryStream(const YkMemoryStream& ms);
	//! \brief operator =
	STREAM_APIF YkMemoryStream& operator=(const YkMemoryStream& ms);

public:	
	//! \brief 构造函数
	STREAM_APIF YkMemoryStream();	
	virtual STREAM_APIF ~YkMemoryStream();

	//========================================================
	//! \brief 		打开内存流，可以指定每次分配buffer的size，也可以用户传入一个内存
	//! \return 	YkBool    正确返回True，错误返回False
	//! \param 		save_or_load 打开的模式，现在支持三种模式，only save, only load and saveOrload
	//! \param 		size  分配buffer大小，这个对提高性能很重要，内存流的大小每次以size增长
	//! \param 		data  这个参数也很重要，用户可以外面传进来一快内存，用内存流来操作这块内存，
	//!                       如果用户传进来一块内存，那么就要和size协同工作，size就是外面传进来的大小
	//! \remarks 	打开内存流，可以指定每次buffer增长的size，也可以用户传入一个内存
	STREAM_APIF YkBool Open(YkStreamDirection save_or_load, YkSizeT size=1024, YkUchar* data=NULL);	


	//========================================================
	//! \brief 		得到内存流的指针data和整个buffer的长度
	//!              同时要警告路人的是:Take出的内存外面必须负责释放,否则会内存泄露
	//! \return 	void  没有返回值
	//! \param 		data  返回内存流的数据指针
	//! \param 		size  返回整个buffer的size(这里需要注意:是整个buffer的大小,不是实际数据的长度)
	//!					  实际数据长度可调用GetLength得到；
	//!					  如果是Save模式，还可以调用GetPosition来获得已经存储的数据的大小
	//! \remarks 	得到内存流的指针data和整个buffer的长度	
	//! \attention	TakeBuffer暂时不支持超过4G的情况
	STREAM_APIF void TakeBuffer(YkUchar*& data, YkUint& size);	


	//========================================================
	//! \brief 		外面给内存流一块内存，这时内存流会 把原来内存流中的数据释放
	//! \return 	void
	//! \param 		*data  外面给出的一块内存不能为空
	//! \param 		size   给出这块内存的大小，
	//! \remarks 	要注意的是：外面这块内存交给了内存流，内存流就负责维护它
	STREAM_APIF void GiveBuffer(YkUchar *data,YkUint size);	


	//========================================================
	//! \brief 		关闭流
	//! \return 	YkBool 成功返回True,失败返回False
	virtual STREAM_APIF YkBool Close();	


	//========================================================
	//! \brief 		设置当前指针的位置。
	//! \remarks		如果要设置的位置超过内存空间大小，且内存由流所有，会导致内存空间重新分配，
	//! 内存空间不为流所有，则流会处于Full状态
	//! \param offset 偏移量[in]。
	//! \param whence 偏移量的起始位置[in]。
	virtual STREAM_APIF YkBool SetPosition(YkLong offset,YkWhence whence=YkFromStart);


	//========================================================
	//! \brief 		得到流中实际有效数据的长度（小于等于内存空间）
	virtual STREAM_APIF YkUlong GetLength();


	//========================================================
	//! \brief 		得到内存流中的数据指针, 这个路人要注意的是: 外面可以用此数据,可以通过GetLength来得到数据的长度 
	//!               外面不需要释放, 当关闭流会自动释放这块内存
	//! \return 	YkUchar*  返回内存流的首地址
	//! \remarks 	得到内存流中的数据指针,如果实际数据长度为0, 返回NULL指针	
	virtual STREAM_APIF YkUchar* GetData() const;
		
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkUchar& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkAchar& v);

	 STREAM_APIF YkStream& operator<<(const YkWchar& v){return *this<<(YkUshort)(v);}

	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkUshort& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkShort& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkUint& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkInt& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkFloat& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkDouble& v);
#ifdef Yk_LONG
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkLong& v);
	//! \brief operator <<
	 STREAM_APIF YkStream& operator<<(const YkUlong& v);
#endif
	
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUchar* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkAchar* p,YkSizeT n);

	STREAM_APIF YkStream& Save(const YkWchar* p,YkSizeT n){return Save((YkUshort*)p, n);}

	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUshort* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkShort* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUint* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkInt* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkFloat* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkDouble* p,YkSizeT n);
#ifdef Yk_LONG
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkLong* p,YkSizeT n);
	//! \brief 保存 p 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Save(const YkUlong* p,YkSizeT n);
#endif
	
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkUchar& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkAchar& v);

	 STREAM_APIF YkStream& operator>>(YkWchar& v){return *this>>(YkUshort&)(v);}

	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkUshort& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkShort& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkUint& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkInt& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkFloat& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkDouble& v);
#ifdef Yk_LONG
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkLong& v);
	//! \brief operator >>
	 STREAM_APIF YkStream& operator>>(YkUlong& v);
#endif
	
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUchar* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkAchar* p,YkSizeT n);

	STREAM_APIF YkStream& Load(YkWchar* p,YkSizeT n){return Load((YkUshort*)p, n);}

	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUshort* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkShort* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUint* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkInt* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkFloat* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkDouble* p,YkSizeT n);
#ifdef Yk_LONG
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkLong* p,YkSizeT n);
	//! \brief 获取 p 指针 
	//! \param p 字节指针 [in]。
	//! \param n 个数[in]。
	STREAM_APIF YkStream& Load(YkUlong* p,YkSizeT n);
#endif

	//! \brief 把字符串保存到流中
	STREAM_APIF YkStream& operator<<(const YkString& s);

	//! \brief 从流中读出字符串
	STREAM_APIF YkStream& operator>>(YkString& s);
	
protected:
	virtual STREAM_APIF YkSizeT WriteBuffer(YkSizeT count);
	virtual STREAM_APIF YkSizeT ReadBuffer(YkSizeT count);
	
	//{{解决在YkStreamLoadSave模式下，Load和Save混乱的问题，这里主要是设置一些标志来控制Load和Save.
	virtual STREAM_APIF void SetSignLoadSave(YkInt nSign);
	//}}

	YkSizeT m_nBufferSize;
};

}

#endif


