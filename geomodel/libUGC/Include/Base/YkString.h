/*
 *
 * YkString.h
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

#if !defined(AFX_YKSTRING_H__2CA5E3F6_86E2_4EA7_A56F_CE716B8B1E05__INCLUDED_)
#define AFX_YKSTRING_H__2CA5E3F6_86E2_4EA7_A56F_CE716B8B1E05__INCLUDED_

#pragma once

#if defined(_WIN32) || defined(WIN32)
/* This needs to come before any includes for MSVC compiler */
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <string>
#include "YkArray.h"
#define WhiteBox_Ignore

#ifdef _UGUNICODE
namespace icu_3_8
{
	class UnicodeString;
	class NumberFormat;
}
using namespace icu_3_8;
class QString;
#endif

namespace Yk {
#ifdef _MSC_VER
#pragma warning(disable:4267)
#endif
class YkMBString;
class YkUnicodeString;

//! \brief YKPCTSTR为了不与微软的LPCTSTR冲突，因为指针可能是4位或者8位，去掉L，加上Yk前缀。
typedef const YkAchar* YKPCTSTR; 
typedef const YkWchar* YKPCWSTR;

typedef YkArray<YkUnicodeString> YkUnicodeStringArray; 
typedef YkArray<YkMBString> YkMBStringArray;

//! \brief 字符串字符集类
struct  YKCharset
{
public:
	enum Charset
	{   
		//! \brief ANSI编码
		ANSI = 0,
		//! \brief Default编码
		Default = 1,
		//! \brief Symbol编码
		Symbol = 2,
		//! \brief MAC编码
		MAC = 77,
		
		//! \brief ISO-2022-JP-2编码
		ISO2022JP2 = 127,
		//! \brief ShiftJIS编码
		ShiftJIS = 128,

		//! \brief Hangeul编码
		Hangeul = 129,
		//! \brief Johab编码
		Johab	= 130,
		
		//! \brief GB18030则是最新的国家标准
		GB18030 = 134,  
		//! \brief Big5编码
		ChineseBIG5 = 136, 
		//! \brief Greek编码
		Greek = 161,
		//! \brief Turkish编码
		Turkish = 162,
		//! \brief Vietnamese编码
		Vietnamese = 163,
		//! \brief Hebrew编码
		Hebrew = 177,
		//! \brief Arabic编码
		Arabic = 178,
		//! \brief Baltic编码
		Baltic = 186,
		//! \brief Russian编码
		Russian = 204,
		//! \brief Thai编码
		Thai = 222,
		//! \brief EastEurope编码
		EastEurope = 238,
		//! \brief UTF8编码
		UTF8 = 250,
		//! \brief Windows默认的Unicode编码	这里默认是按照UTF16LE处理的
		UCS2LE = 251,
		//! \brief UCS2BE编码 这里默认是按照UTF16BE处理的
		UCS2BE = 252,
		//! \brief UCS4LE编码 这里默认是按照UTF32LE处理的
		UCS4LE = 253,
		//! \brief UCS4BE编码 这里默认是按照UTF32BE处理的
		UCS4BE = 254,
		//! \brief Unicode编码
		Unicode = 132,
		//! \brief OEM编码
		OEM = 255,
		//! \brief Windows1252编码
		Windows1252 = 137,
		//! \brief Korean编码
		Korean = 131,
		//! \brief Cyrillic编码
		Cyrillic = 135,
        //! \brief xIA5编码
		xIA5 = 3,
		//! \brief xIA5German编码
		xIA5German = 4,
		//! \brief xIA5Swedish编码
		xIA5Swedish = 5,
		//! \brief xIA5Norwegian编码
		xIA5Norwegian = 6,
		//! \brief UTF7编码
		UTF7 = 7
	};                                                                                                                                                                      
};
//! \brief 字符串类
//! \attention YkMBString仅支持多字节编码的字符串。
class BASE_API YkMBString : public YKCharset 
{
public:
	//! \brief 默认构造函数。
	YkMBString();
    
	//! \brief 拷贝构造函数。
	//! \param str 用来拷贝的字符串(YkMBString类型)
	YkMBString(const YkMBString& str);	

	//! \brief 拷贝构造函数
	//! \param str 用来拷贝的字符串(std::string类型)
	YkMBString(const std::string& str);	
	
	//! \brief 拷贝构造函数。	
	//! \param pstr 用来拷贝的字符串(YkAchar*类型)
	YkMBString(const YkAchar* pStr);
	
	//! \brief 拷贝构造函数。
	//! \param pstr 用来拷贝的字符串(YkAchar*类型)
	//! \param nSize 指定使用的字节长度
	YkMBString(const YkAchar* pStr, YkInt nSize);
	
	//! \brief 拷贝构造函数，将ch重复拷贝nRepeat次。
	//! \param ch 用来拷贝的字符串(YkAchar类型)
	//! \param nRepeat 重复拷贝次数,默认为1
	explicit YkMBString(YkAchar ch, YkInt nRepeat=1);
	
	//! \brief 追加字符串
	//! \param pch 要追加的字符串
	//! \param nSize 要追加的字节个数
	void Append(const YkAchar* pStr, YkInt nSize);

	//! \brief 将YkMBString转换为const char*。
	operator YKPCTSTR() const;

	//! \brief 将YkMBString转换成std::string
	operator std::string() const;

	//! \brief 获取C字符串
	const YkAchar * Cstr() const;

	//! \brief 获取C++字符串
	const std::string& Str() const;

	//! \brief 获取指定索引的字符。
	//! \param nIndex 指定的索引
	//! \return 返回指定索引的字符。
	YkAchar GetAt(YkInt nIndex) const;		
	
	//! \brief 设置指定索引的字符
	//! \param nIndex 指定的索引
	//! \param ch 要设置的字符
	void SetAt(YkInt nIndex, YkAchar ch);

	//! \brief 获取指定索引的字符。
	//! \param  nIndex 指定的索引
	//! \return 返回指定索引的字符
	YkAchar operator[](YkInt nIndex) const;
	
	//! \brief 设置指定索引的字符的引用，支持str[i]='a';
	//! \param nIndex 指定的索引
	//! \return 返回指定索引的字符的引用
	YkAchar& operator[](YkInt nIndex);

	//! \brief 保留指定字节个数的内存空间
	//! \param nCapacity 可容纳的字节个数
	//! \remarks 如果原有空间不够, 内存空间会增加。
	//! 如果原有空间比指定的大, 也不会释放空间。
	void SetCapacity(YkInt nCapacity);
	
	//! \brief 获取目前所有空间(包括已经使用的和保留的)所占用的字节个数
	YkInt GetCapacity() const; 

	//! \brief 获取字符串长度
	YkInt GetLength() const;	
	
	//! \brief 设置字符串长度
	//! \param nLength 字节个数
	//! \remarks 如果长度变小,会自动截断; 
	//!  如果长度变大,会填充0x00。
	void SetLength(YkInt nLength);
	

	//! \brief 把字符串内容转换为大写，不会修改非ASCII字符
	//! \return 返回自身的引用,支持连写
	YkMBString& MakeUpper();
	
	//! \brief 把字符串内容转换为小写，不会修改非ASCII字符
	//! \return 返回自身的引用,支持连写
	YkMBString& MakeLower();
	
	//! \brief 把字符串逆转
	//! \return 返回自身的引用,支持连写
	//! \remarks 支持中间带有中文（MBCS）的转换
	YkMBString& MakeReverse();
	
	//! \brief 取出字符串对象左边的指定长度的子字符串
	//! \param nCount 字节个数
	//! \return 返回左边指定长度的子字符串
	YkMBString Left(YkInt nCount) const;
	
	//! \brief 取出字符串对象最右边的指定长度的子字符串
	//! \param nCount 字节个数
	//! \return 返回右边指定长度的子字符串
	YkMBString Right(YkInt nCount) const;
	
	//! \brief 从字符串对象中取出从指定索引起的子字符串对象
	//! \param nFirst 指定的中间的索引序号
	//! \return 返回从中间指定的索引开始,到最后的子字符串
	YkMBString Mid(YkInt nFirst) const;
	
	//! \brief 从字符串对象中获取从指定索引起、指定长度的子字符串对象
	//! \param nFirst nFirst 索引值，大于等于零
	//! \param nCount 要取出的字节个数
	YkMBString Mid(YkInt nFirst,YkInt nCount) const;

	//! \brief 删除从指定索引起的指定长度的字符串
	//! \param nIndex 开始删除的索引值
	//! \param nCount 要删除的字节个数
	//! \return 返回删除字符串后的结果字符串对象的长度
	YkInt Delete(YkInt nIndex,YkInt nCount=1);
	
	//! \brief 在指定的索引之前插入字符串
	//! \param nIndex 指定的索引
	//! \param pstr 要插入的字符串
	//! \return 插入后,结果字符串的长度
	YkInt Insert(YkInt nIndex,const YkAchar * pStr);	
	
	//! \brief 在指定的索引之前插入若干数目的字符
	//! \param nIndex 指定的索引
	//! \param ch 指定的字符
	//! \param nCount 字符重复的次数
	//! \return 插入后,结果字符串的长度
	YkInt Insert(YkInt nIndex,YkAchar ch,YkInt nCount=1);
	
	//! \brief 删除字符串对象中等于指定字符的字符
	//! \param ch 要删除的字符
	//! \return 返回删除的字符个数
	YkInt Remove(YkAchar ch);		

	//! \brief 删除字符串对象左边的空格（\\t\\r\\n）
	//! \return 删除后,字符串对象的引用
	YkMBString& TrimLeft();
	
	//! \brief 删除字符串对象左边的指定字符
	//! \param ch 要删除的字符
	//! \return 删除后,字符串对象的引用
	YkMBString& TrimLeft(YkAchar ch);
	
	//! \brief 删除字符串对象左边属于指定字符集合的字符
	//! \param pstr 指定的字符串集合
	//! \return 删除后,字符串对象的引用,以支持连写
	//! \remarks 对于字符集合,是要删除左边的存在于字符集合中的每一个字符
	YkMBString& TrimLeft(const YkAchar* pStr);
	
	//! \brief 删除字符串对象右边的空格（\\t\\r\\n）
	//! \return 删除后,字符串对象的引用
	YkMBString& TrimRight();
	
	//! \brief 删除字符串对象右边的指定字符
	//! \param ch 指定的字符
	//! \return 删除后,字符串对象的引用
	YkMBString& TrimRight(YkAchar ch);
	
	//! \brief 删除字符串对象右边属于指定字符集合的字符
	//! \param pstr 指定的字符串集合
	//! \return 删除后,字符串对象的引用,以支持连写
	//! \remarks 对于字符集合,是要删除右边的存在于字符集合中的每一个字符
	YkMBString& TrimRight(const YkAchar* pStr);

	//! \brief 删除字符串对象右边等于指定字符串的字符
	//! \param &str 指定的字符串
	//! \return 删除后,字符串对象的引用,以支持连写
	YkMBString& TrimRightString(const YkMBString &str);
	
	//! \brief 删除字符串对象左边等于指定字符串的字符
	//! \param &str 指定的字符串
	//! \return 删除后,字符串对象的引用,以支持连写
	YkMBString& TrimLeftString(const YkMBString &str);
	
	//! \brief 判断字符串是否为空
	//! \return 为空,返回true;不为空,返回false
	YkBool IsEmpty() const;	
	
	//! \brief 清空字符串
	void Empty();

	//! \brief 比较两个字符串的大小
	//! \param lpsz 要比较的字符串指针
	//! \return 小于返回负数；等于返回0；大于返回正数
	//! \remarks 区分大小写，按照内部值依顺序比较
	YkInt Compare(const YkAchar* pStr) const;
	
	//! \brief 比较两个字符串的大小
	//! \param lpsz 要比较的字符串指针
	//! \return 小于返回负数；等于返回0；大于返回正数
	//! \remarks 不区分大小写，按照内部值依顺序比较
 	YkInt CompareNoCase(const YkAchar* pStr) const;

	//! \brief 比较两个字符串的大小
	//! \remarks 不区分大小写，按照内部值依顺序比较
	//! \param str 要比较的字符串
	//! \return 小于返回负数；等于返回0；大于返回正数。
	YkInt CompareNoCase(const YkMBString& str) const;
	
	//! \brief 从指定位置开始，查找指定的字符
	//!	\param ch 要查找的字符[in]。
	//!	\param nStart 查找起始位置，对应的字符会被查找[in]。
	//!	\return 返回找到的字符的索引值，从0算起。找不到返回-1
	YkInt Find(YkAchar ch,YkInt nStart=0) const;		

	//! \brief 从指定位置开始，查找完全相等的子字符串
	//!	\param lpszSub 要查找的子字符串[in]。
	//!	\param nStart 查找起始位置，对应的子字符串会被查找[in]。
	//!	\return 返回找到的子字符串的索引值，从0算起。找不到返回-1
	YkInt Find(const YkAchar* pStrSub, YkInt nStart=0) const;

	//! \brief 从后向前查找指定的字符
	//!	\param ch 要查找的字符[in]。
	//!	\param nStart 查找起始位置，对应的子字符会被查找[in] -1代表从末尾查找。
	//!	\return 返回找到的字符的索引值，从0算起;找不到返回-1
	YkInt ReverseFind(YkAchar ch,YkInt nStart=-1) const;	

	//! \brief 从后向前查找指定的字符串。
	//!	\param ch 要查找的字符串[in]。
	//!	\param nStart 查找起始位置，对应的子字符串会被查找[in] -1代表从末尾查找。
	//!	\return 返回找到的字符串的索引值，从0算起;找不到返回-1
	YkInt ReverseFind(const YkAchar* pStrSub,YkInt nStart=-1) const;
	
	//! \brief 查找第一个“既在字符串对象中，又在指定的字符集合中”的字符的位置。
	//!	\param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回找到的字符的索引，从0算起。找不到返回-1
	YkInt FindOneOf(const YkAchar* pStrCharSet) const;	

	//! \brief 得到字符串对象中，从第0个字符起，到不符合指定字符集合中的字符为止的子字符串。
	//! \param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回符合条件的子字符串。
	YkMBString SpanIncluding(const YkAchar* pStrCharSet) const;		

	//! \brief 得到字符串对象中，从第0个字符起，到符合指定字符集合中的字符为止的子字符串。
	//! \param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回符合条件的子字符串。
	YkMBString SpanExcluding(const YkAchar* pStrCharSet) const;
		
	//! \brief 替换函数，把字符串对象中指定的字符替换为新的字符。
	//! \param chOld 指定要被替换的字符[in]。
	//!	\param chNew 新的字符[in]。
	//!	\return 返回替换的次数。
	//! \remarks 比较时区分大小写。
	YkInt Replace(YkAchar chOld,YkAchar chNew);

	//! \brief 替换函数，把字符串对象中指定的子字符串替换为新的字符串
	//! \param lpszOld 指定要被替换的子字符串[in]。
	//!	\param lpszNew 新的字符串[in]。
	//!	\return 返回替换的次数。
	//!	\remarks 比较时区分大小写。
	YkInt Replace(const YkAchar* pStrOld,const YkAchar* pStrNew);

	//! \brief 格式化函数，类似于sprintf。
	//!	\param fmt 用来表达格式的字符串[in]。
	//!	\param ... 可选参数[in]。
	//!	\remarks 不能将YkMBString作为参数传进该函数，而必须调用YkMBString的Cstr函数得到C字符串。
	//!			 不能将YkLong、YkUlong等64位整数作为参数传入该函数,而应该先强制转化为32位整数
	void Format(const YkAchar* pStr,...);

	//! \brief 字符串赋值函数。
	const YkMBString& operator=(const YkMBString& str);	
	
	//! \brief 用std::string来赋值函数。
	//! \param str std::string类型的字符串,主要用于和Markup等进行匹配
	const YkMBString& operator=(const std::string& str);	
	
	//! \brief 用C 字符串赋值函数。	
	//! \param pstr C字符串参数
	const YkMBString& operator=(const YkAchar* pStr);
	//! \brief 重载 +
	YkMBString operator+(const YkAchar ch) const;
	//! \brief 重载 +
	YkMBString operator+(const YkAchar* pStr) const;
	//! \brief 重载 +
	YkMBString operator+(const YkMBString& str) const;
	
	//! \brief 重载 +=
	const YkMBString& operator+=(const YkMBString& str);
	//! \brief 重载 +=
	const YkMBString& operator+=(const YkAchar ch);
	//! \brief 重载 +=
	const YkMBString& operator+=(const YkAchar* pStr);
	
	//! \brief 重载 ==
	bool operator==(const YkAchar* pStr) const;
	//! \brief 重载 ==
	bool operator==(const YkMBString& str) const;

	//! \brief 重载 !=
	bool operator!=(const YkAchar* pStr) const;
	//! \brief 重载 !=
	bool operator!=(const YkMBString& str) const;

	//! \brief 重载 <
	bool operator<(const YkMBString& str) const;

	//! \brief 重载 +
	friend BASE_API YkMBString operator+(YkAchar ch, const YkMBString &str);
	//! \brief 重载 +
	friend BASE_API YkMBString operator+(const YkAchar* pStr, const YkMBString &str);	

	
	//! \brief 把YkLong型的数据转化为YkMBString型字符串
	//! \param lVal 要转化的YkLong型数据
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	//! \return 返回转化后的字符串
	//! \remarks 当base为16进制时,转化后的结果前面会加0X,字母都为大写。
	//! 用这种方式间接支持long型数据的Format,即先转化为YkMBString,再用%s进行格式化。
	//! 采用静态函数，返回YkMBString的方式，以便于调用的代码连写。
	static YkMBString From(YkLong lVal, YkUint base=10);
	//! \brief 把YkUlong型的数据转化为YkMBString型字符串
	static YkMBString From(YkUlong lVal, YkUint base=10);
	//! \brief 把YkInt型的数据转化为YkMBString型字符串
	static YkMBString From(YkInt nVal, YkUint base=10);
	//! \brief 把YkUint型的数据转化为YkMBString型字符串
	static YkMBString From(YkUint nVal, YkUint base=10);
	//! \brief 考虑到YkArray.GetSize(),返回了YkSizeT,在Linux上该类型为unsigned long,特为此添加转换函数
	static YkMBString From(unsigned long nVal, YkUint base=10);	WhiteBox_Ignore

	//! \brief 把字符串转换为YkInt型数值
	YkInt ToInt(YkUint base=10) const;

	//! \brief 把字符串转换为无符号YkUint型数值
	YkUint ToUInt(YkUint base=10) const;
	
	//! \brief 把字符串转换为YkLong型数值
	//! \param s 要转化的字符串
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	YkLong ToLong(YkUint base=10) const;

	//! \brief 把字符串转换为无符号YkUlong型数值
	//! \param s 要转化的字符串
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	YkUlong ToULong(YkUint base=10) const;
	
	//! \brief 把字符串转换为YkFloat型数值
	YkFloat ToFloat() const;

	//! \brief 把字符串转换为YkDouble型数值	
	YkDouble ToDouble() const;
	
	//! \brief 把字符串按照指定的分隔符分割为一个字符串数组。
	//! \param dest 生成的字符串数组[out]。
	//!	\param delims 指定的分隔符，默认为回车换行[in]。
	//!	\param bKeepEmptyString 是否保留分割后 空的字符串, 如字符串";"通过';'分割后,dest数组的size
	//! 在保留空字符串的时候为2, 不保留的时候为0; 
	//!	\return 分割后字符串数组的长度。
	YkInt Split(YkMBStringArray &strDests,const YkMBString& strDelims="\r\n", YkBool bKeepEmptyString=FALSE) const;
	
	//! \brief 设置字符串里面的字符的字符集	 
	//是不是YkInt 要变成YkMBString::Charset
	void SetCharset(YkInt nCharset);
	//! \brief 获取字符串里面的字符的字符集	 
	YkMBString::Charset GetCharset() const;

	//! \brief 替换函数，把字符串对象中指定的子字符串替换为新的字符串		
	//!	\remarks 比较时区分大小写。
	void Replace(YkInt startIndex, YkInt length, const YkAchar* pStrNew);

	//! \brief 计算两个字符串之间的相似程度
	//! \param str 要比较的字符串
	//! \return 返回相似比例，完全相同返回1，不相同返回小于1的小数，值域 0~1 之间
	YkDouble CalculateDistance(const YkMBString& str);

public:
	//! \brief 将当前String转换成std::string
	//! \param str 目标std::string
	//!  \return str的引用,内部不涉及到编码转换，简单的转换，这个和unicodestring的概念不太一致 
	std::string& ToStd(std::string& str) const;

	//! \brief 将std::string 转换成当前的String
	//! \param str 源std::string
	//!  \return 当前String的引用 这个和unicodestring的概念不太一致 
	YkMBString& FromStd(const std::string& str);

public:	
	//! \brief std的字符串
	std::string m_string; // 内部采用std的string实现
	//! \brief 字符集
	YkMBString::Charset m_nCharset;
};

#ifdef _UGUNICODE
const YkWchar g_strNull[]={_U('\r'),_U('\n'),_U('\t'),_U(' '),_U('\0')};
class BASE_API YkUnicodeString : public YKCharset 
{
public:
	//! \brief 默认构造函数
	YkUnicodeString();

	//! \breif 析构函数
	~YkUnicodeString();

	//! \brief 拷贝构造函数
	//! \param 用来拷贝的字符串YkUnicodeString
	YkUnicodeString(const YkUnicodeString& str);

	//! \brief 拷贝构造函数。	
	//! \param pstr 用来拷贝的字符串(YkWchar*类型)
	YkUnicodeString(const YkWchar* pStr);

	//! \brief 拷贝构造函数。
	//! \param pstr 用来拷贝的字符串(YkWchar*类型)
	//! \param nSize 指定使用的字节长度
	YkUnicodeString(const YkWchar* pStr, YkInt nSize);

	//! \brief 拷贝构造函数，将ch重复拷贝nRepeat次。
	//! \param ch 用来拷贝的字符串(YkWchar类型)
	//! \param nRepeat 重复拷贝次数,默认为1
	//! \remarks explicit用于防止隐形的转化
	explicit YkUnicodeString(YkWchar ch, YkInt nRepeat=1);

	//! \brief 追加字符串
	//! \param pch 要追加的字符串
	//! \param nSize 要追加的字节个数
	void Append(const YkWchar* pStr, YkInt nSize);

	//! \brief 将YkUnicodeString转换为const char*。
	operator YKPCWSTR() const;

	//! \brief 将YkUnicodeString转换为UnicodeString
	operator UnicodeString() const;

	//! \brief 获取Wchar字符串
	const YkWchar * Cstr() const;

	//! \brief 获取指定索引的字符。
	//! \param nIndex 指定的索引
	//! \return 返回指定索引的字符。
	YkWchar GetAt(YkInt nIndex) const;		
	YkWchar32 GetWchar32At(YkInt nIndex) const;

	//! \brief 设置指定索引的字符
	//! \param nIndex 指定的索引
	//! \param ch 要设置的字符
	void SetAt(YkInt nIndex, YkWchar ch);

	//! \brief 获取指定索引的字符。
	//! \param  nIndex 指定的索引
	//! \return 返回指定索引的字符
	YkWchar operator[](YkInt nIndex) const;

	//! \brief 设置指定索引的字符的引用，支持str[i]='a';
	//! \param nIndex 指定的索引
	//! \return 返回指定索引的字符的引用
	YkWchar& operator[](YkInt nIndex);

	//! \brief 保留指定字节个数的内存空间
	//! \param nCapacity 可容纳的字节个数
	//! \remarks 如果原有空间不够, 内存空间会增加。
	//! 如果原有空间比指定的大, 也不会释放空间。
	void SetCapacity(YkInt nCapacity);

	//! \brief 获取目前所有空间(包括已经使用的和保留的)所占用的字节个数
	YkInt GetCapacity() const; 

	//! \brief 获取字符串长度
	YkInt GetLength() const;	

	//! \brief 获取字符串长度
	//! \breif 这里提供Char32长度，如果有大于2byte大小长度的字符，这个长度
	//  会和GetLength所获取的长度不等，
	YkInt GetWchar32Length() const;	

	//! \brief 设置字符串长度
	//! \param nLength 字节个数
	//! \remarks 如果长度变小,会自动截断; 
	//!  如果长度变大,会填充0x00。
	void SetLength(YkInt nLength);

	//! \brief 把字符串内容转换为大写，不会修改非ASCII字符
	//! \return 返回自身的引用,支持连写
	YkUnicodeString& MakeUpper();

	//! \brief 把字符串内容转换为小写，不会修改非ASCII字符
	//! \return 返回自身的引用,支持连写
	YkUnicodeString& MakeLower();

	//! \brief 把字符串逆转
	//! \return 返回自身的引用,支持连写
	//! \remarks 支持中间带有中文（MBCS）的转换
	YkUnicodeString& MakeReverse();

	//! \brief 取出字符串对象左边的指定长度的子字符串
	//! \param nCount 字节个数
	//! \return 返回左边指定长度的子字符串
	YkUnicodeString Left(YkInt nCount) const;

	//! \brief 取出字符串对象最右边的指定长度的子字符串
	//! \param nCount 字节个数
	//! \return 返回右边指定长度的子字符串
	YkUnicodeString Right(YkInt nCount) const;

	//! \brief 从字符串对象中取出从指定索引起的子字符串对象
	//! \param nFirst 指定的中间的索引序号
	//! \return 返回从中间指定的索引开始,到最后的子字符串
	YkUnicodeString Mid(YkInt nFirst) const;

	//! \brief 从字符串对象中获取从指定索引起、指定长度的子字符串对象
	//! \param nFirst nFirst 索引值，大于等于零
	//! \param nCount 要取出的字节个数
	YkUnicodeString Mid(YkInt nFirst,YkInt nCount) const;

	//! \brief 删除从指定索引起的指定长度的字符串
	//! \param nIndex 开始删除的索引值
	//! \param nCount 要删除的字节个数
	//! \return 返回删除字符串后的结果字符串对象的长度
	YkInt Delete(YkInt nIndex,YkInt nCount=1);

	//! \brief 在指定的索引之前插入字符串
	//! \param nIndex 指定的索引
	//! \param pstr 要插入的字符串
	//! \return 插入后,结果字符串的长度
	YkInt Insert(YkInt nIndex,const YkWchar * pStr);	

	//! \brief 在指定的索引之前插入若干数目的字符
	//! \param nIndex 指定的索引
	//! \param ch 指定的字符
	//! \param nCount 字符重复的次数
	//! \return 插入后,结果字符串的长度
	YkInt Insert(YkInt nIndex,YkWchar ch,YkInt nCount=1);
    
	//! \brief 删除字符串对象中等于指定字符的字符
	//! \param ch 要删除的字符
	//! \return 返回删除的字符个数
	YkInt Remove(YkWchar ch);

	//! \brief 删除字符串对象左边的空格（\\t\\r\\n）
	//! \return 删除后,字符串对象的引用
	YkUnicodeString& TrimLeft()
	{
		return TrimLeft(g_strNull);
	};
	
	//! \brief 删除字符串对象左边的指定字符
	//! \param ch 要删除的字符
	//! \return 删除后,字符串对象的引用
	YkUnicodeString& TrimLeft(YkWchar ch);
	
	//! \brief 删除字符串对象左边属于指定字符集合的字符
	//! \param pstr 指定的字符串集合
	//! \return 删除后,字符串对象的引用,以支持连写
	//! \remarks 对于字符集合,是要删除左边的存在于字符集合中的每一个字符
	YkUnicodeString& TrimLeft(const YkWchar* pStr);
	
	//! \brief 删除字符串对象右边的空格（\\t\\r\\n）
	//! \return 删除后,字符串对象的引用
	YkUnicodeString& TrimRight()
	{
		return TrimRight(g_strNull);
	};
	
	//! \brief 删除字符串对象右边的指定字符
	//! \param ch 指定的字符
	//! \return 删除后,字符串对象的引用
	YkUnicodeString& TrimRight(YkWchar ch);
	
	//! \brief 删除字符串对象右边属于指定字符集合的字符
	//! \param pstr 指定的字符串集合
	//! \return 删除后,字符串对象的引用,以支持连写
	//! \remarks 对于字符集合,是要删除右边的存在于字符集合中的每一个字符
	YkUnicodeString& TrimRight(const YkWchar* pStr);

	//! \brief 删除字符串对象右边等于指定字符串的字符
	//! \param &str 指定的字符串
	//! \return 删除后,字符串对象的引用,以支持连写
	YkUnicodeString& TrimRightString(const YkUnicodeString &str);
	
	//! \brief 删除字符串对象左边等于指定字符串的字符
	//! \param &str 指定的字符串
	//! \return 删除后,字符串对象的引用,以支持连写
	YkUnicodeString& TrimLeftString(const YkUnicodeString &str);
	
	//! \brief 判断字符串是否为空
	//! \return 为空,返回true;不为空,返回false
	YkBool IsEmpty() const;	
	
	//! \brief 清空字符串
	void Empty();

	//! \brief 比较两个字符串的大小
	//! \param lpsz 要比较的字符串指针
	//! \return 小于返回负数；等于返回0；大于返回正数
	//! \remarks 区分大小写，按照内部值依顺序比较
	YkInt Compare(const YkWchar* pStr) const;
	
	//! \brief 比较两个字符串的大小
	//! \param lpsz 要比较的字符串指针
	//! \return 小于返回负数；等于返回0；大于返回正数
	//! \remarks 不区分大小写，按照内部值依顺序比较
 	YkInt CompareNoCase(const YkWchar* pStr) const;

	//! \brief 比较两个字符串的大小
	//! \remarks 不区分大小写，按照内部值依顺序比较
	//! \param str 要比较的字符串
	//! \return 小于返回负数；等于返回0；大于返回正数。
	YkInt CompareNoCase(const YkUnicodeString& str) const;

	//! \brief 计算两个字符串之间的相似程度
	//! \param str 要比较的字符串
	//! \return 返回相似比例，完全相同返回1，不相同返回小于1的小数，值域 0~1 之间
	YkDouble CalculateDistance(const YkUnicodeString& str) const;
	
	//! \brief 从指定位置开始，查找指定的字符
	//!	\param ch 要查找的字符[in]。
	//!	\param nStart 查找起始位置，对应的字符会被查找[in]。
	//!	\return 返回找到的字符的索引值，从0算起。找不到返回-1
	YkInt Find(YkWchar ch,YkInt nStart=0) const;		

	//! \brief 从指定位置开始，查找完全相等的子字符串
	//!	\param lpszSub 要查找的子字符串[in]。
	//!	\param nStart 查找起始位置，对应的子字符串会被查找[in]。
	//!	\return 返回找到的子字符串的索引值，从0算起。找不到返回-1
	YkInt Find(const YkWchar* pStrSub, YkInt nStart=0) const;

	//! \brief 从后向前查找指定的字符
	//!	\param ch 要查找的字符[in]。
	//!	\param nStart 查找起始位置，对应的子字符会被查找[in] -1代表从末尾查找。
	//!	\return 返回找到的字符的索引值，从0算起;找不到返回-1
	YkInt ReverseFind(YkWchar ch,YkInt nStart=-1) const;	

	//! \brief 从后向前查找指定的字符串。
	//!	\param ch 要查找的字符串[in]。
	//!	\param nStart 查找起始位置，对应的子字符串会被查找[in] -1代表从末尾查找。
	//!	\return 返回找到的字符串的索引值，从0算起;找不到返回-1
	YkInt ReverseFind(const YkWchar* pStrSub,YkInt nStart=-1) const;
	
	//! \brief 查找第一个“既在字符串对象中，又在指定的字符集合中”的字符的位置。
	//!	\param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回找到的字符的索引，从0算起。找不到返回-1
	YkInt FindOneOf(const YkWchar* pStrCharSet) const;	

	//! \brief 得到字符串对象中，从第0个字符起，到不符合指定字符集合中的字符为止的子字符串。
	//! \param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回符合条件的子字符串。
	YkUnicodeString SpanIncluding(const YkWchar* pStrCharSet) const;		

	//! \brief 得到字符串对象中，从第0个字符起，到符合指定字符集合中的字符为止的子字符串。
	//! \param lpszCharSet 指定的字符集合[in]。
	//!	\return 返回符合条件的子字符串。
	YkUnicodeString SpanExcluding(const YkWchar* pStrCharSet) const;
		
	//! \brief 替换函数，把字符串对象中指定的字符替换为新的字符。
	//! \param chOld 指定要被替换的字符[in]。
	//!	\param chNew 新的字符[in]。
	//!	\return 返回替换的次数。
	//! \remarks 比较时区分大小写。
	YkInt Replace(YkWchar chOld,YkWchar chNew);

	//! \brief 替换函数，把字符串对象中指定的子字符串替换为新的字符串
	//! \param lpszOld 指定要被替换的子字符串[in]。
	//!	\param lpszNew 新的字符串[in]。
	//!	\return 返回替换的次数。
	//!	\remarks 比较时区分大小写。
	YkInt Replace(const YkWchar* pStrOld,const YkWchar* pStrNew);

	//! \brief 格式化函数，类似于sprintf。
	//!	\param fmt 用来表达格式的字符串[in]。
	//!	\param ... 可选参数[in]。
	//!	\remarks 不能将YkUnicodeString作为参数传进该函数，而必须调用YkUnicodeString的Cstr函数得到C字符串。
	void Format(const YkWchar* pStr,...);

	//! \brief 字符串赋值函数。
	const YkUnicodeString& operator=(const YkUnicodeString& str);
	
	//! \brief 用C 字符串赋值函数。	
	//! \param pstr C字符串参数
	const YkUnicodeString& operator=(const YkWchar* pStr);

	//! \brief 重载 +
	YkUnicodeString operator+(const YkWchar ch) const;
	//! \brief 重载 +
	YkUnicodeString operator+(const YkWchar* pStr) const;
	//! \brief 重载 +
	YkUnicodeString operator+(const YkUnicodeString& str) const;
	
	//! \brief 重载 +=
	const YkUnicodeString& operator+=(const YkUnicodeString& str);
	//! \brief 重载 +=
	const YkUnicodeString& operator+=(const YkWchar ch);
	//! \brief 重载 +=
	const YkUnicodeString& operator+=(const YkWchar* pStr);
	
	//! \brief 重载 ==
	bool operator==(const YkWchar* pStr) const;
	//! \brief 重载 ==
	bool operator==(const YkUnicodeString& str) const;

	//! \brief 重载 !=
	bool operator!=(const YkWchar* pStr) const;
	//! \brief 重载 !=
	bool operator!=(const YkUnicodeString& str) const;

	//! \brief 重载 <
	bool operator<(const YkUnicodeString& str) const;

	//! \brief 重载 +
	friend BASE_API YkUnicodeString operator+(YkWchar ch, const YkUnicodeString &str);
	//! \brief 重载 +
	friend BASE_API YkUnicodeString operator+(const YkWchar* pStr, const YkUnicodeString &str);	

	//! \brief 重载 ==
	friend BASE_API bool operator==(const YkWchar* pStr, const YkUnicodeString& str);
	
	//! \brief 把YkLong型的数据转化为YkUnicodeString型字符串
	//! \param lVal 要转化的YkLong型数据
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	//! \return 返回转化后的字符串
	//! \remarks 当base为16进制时,转化后的结果前面会加0X,字母都为大写。
	//! 用这种方式间接支持long型数据的Format,即先转化为YkUnicodeString,再用%s进行格式化。
	static YkUnicodeString From(YkLong lVal, YkUint base=10);
	//! \brief 把YkUlong型的数据转化为YkUnicodeString型字符串
	static YkUnicodeString From(YkUlong lVal, YkUint base=10);
	//! \brief 把YkInt型的数据转化为YkUnicodeString型字符串
	static YkUnicodeString From(YkInt nVal, YkUint base=10);
	//! \brief 把YkUint型的数据转化为YkUnicodeString型字符串
	static YkUnicodeString From(YkUint nVal, YkUint base=10);
	//! \brief 考虑到YkArray.GetSize(),返回了YkSizeT,在Linux上该类型为unsigned long,特为此添加转换函数
	static YkUnicodeString From(unsigned long nVal, YkUint base=10);	WhiteBox_Ignore

	//! \brief 把字符串转换为YkInt型数值
	YkInt ToInt(YkUint base=10) const;
	YkBool ToInt(YkInt& value, YkUint base) const;

	//! \brief 把字符串转换为无符号YkUint型数值
	YkUint ToUInt(YkUint base=10) const;
	YkBool ToUInt(YkUint& value, YkUint base) const;
	
	//! \brief 把字符串转换为YkLong型数值
	//! \param s 要转化的字符串
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	YkLong ToLong(YkUint base=10) const;
	YkBool ToLong(YkLong& value, YkUint base) const;

	//! \brief 把字符串转换为无符号YkUlong型数值
	//! \param s 要转化的字符串
	//! \param base 进制,默认为10进制,还支持16进制;其它暂时不支持
	YkUlong ToULong(YkUint base=10) const;
	YkBool ToULong(YkUlong& value, YkUint base) const;
	
	//! \brief 把字符串转换为YkFloat型数值
	YkFloat ToFloat() const;
	YkBool ToFloat(YkFloat& value) const;

	//! \brief 把字符串转换为YkDouble型数值	
	YkDouble ToDouble() const;
	YkBool ToDouble(YkDouble& value) const;
	
	//! \brief 把字符串按照指定的分隔符分割为一个字符串数组。
	//! \param dest 生成的字符串数组[out]。
	//!	\param delims 指定的分隔符，默认为回车换行[in]。
	//!	\param bKeepEmptyString 是否保留分割后 空的字符串, 如字符串";"通过';'分割后,dest数组的size
	//! 在保留空字符串的时候为2, 不保留的时候为0; 
	//!	\return 分割后字符串数组的长度。
	YkInt Split(YkUnicodeStringArray &strDests,const YkWchar* pStrDelims=_U("\r\n"), YkBool bKeepEmptyString=FALSE) const;
	
	//! \brief 设置字符串里面的字符的字符集	
	void SetCharset(YkInt nCharset);
	//! \brief 获取字符串里面的字符的字符集	 
	YkMBString::Charset GetCharset() const;

	//! \brief 替换函数，把字符串对象中指定的子字符串替换为新的字符串		
	//!	\remarks 比较时区分大小写。
	void Replace(YkInt startIndex, YkInt length, const YkWchar* pStrNew);

	YkUnicodeString& FromUTF8(const YkAchar *pSrc, YkInt nSrcLength);
	YkMBString& ToUTF8(YkMBString& strResult) const;

public:
	//! \brief 将当前String转换成std::string
	//! \param str 目标std::string
	//! \param charset 目标std::string的编码
	//!  \return str的引用,
	std::string& ToStd(std::string& str,YKCharset::Charset charset = YKCharset::UTF8) const;

	//! \brief 转换成指定编码的MBString,编码为str的编码
	YkMBString& ToMBString(YkMBString& str) const
	{
		ToStd(str.m_string,str.m_nCharset);
		return str;
	}

	//! \brief 将std::string 转换成当前的String
	//! \param str 源std::string
	//! \param charset 源std::string 编码
	//!  \return 当前String的引用 
	YkUnicodeString& FromStd(const std::string& str,YKCharset::Charset charset = YKCharset::UTF8)
	{
		return FromMBString(str.c_str(),str.length(),charset);
	}

	//! \brief 从str转换成当前UnicodeString,str可以是任意编码
	YkUnicodeString& FromMBString(const YkMBString& str)
	{
		return  FromStd(str.m_string,str.m_nCharset);
	}
	YkUnicodeString& FromMBString(const YkAchar *pSrc, YkInt nSrcLength,YKCharset::Charset charset = YKCharset::Default);

public:
	static YkInt u_str32len(const YkWchar32* pStr)
	{
		const YkWchar32 *eos = pStr;
		while( *eos++ ) ;
		return( (YkInt)(eos - pStr - 1) );
	};
	static YkInt u_str16len(const YkWchar* pStr)
	{
		const YkWchar *eos = pStr;
		while( *eos++ ) ;
		return( (YkInt)(eos - pStr - 1) );
	};

	static YkInt WcharToAchar(const YkWchar* pWchar, YkAchar *pAchar, YkInt nAcharLength);
	static YkInt AcharToWchar(const YkAchar* pAchar, YkWchar *pWchar, YkInt nWcharLength);
	static YkInt WcharToWchar32(const YkWchar* pWchar, YkWchar32* pWchar32, YkInt nWchar32Length);
	static YkInt Wchar32ToWchar( const YkWchar32* pWchar32, YkWchar* pWchar, YkInt nWcharLength );
	
	struct ShowUnicodeString
	{
		void		*pString;
		void		*pVftable;
		YkInt		fLength;
		YkInt		fCapacity;     
		YkWchar	*fArray; 
		YkShort	fFlags;
		YkWchar	fStackBuffer[7];
	};

	struct ShowQString {
		int ref;
		int size;
		unsigned int alloc : 31;
		unsigned int capacityReserved : 1;
		YkPtrType<sizeof(void*)>::Signed offset;
#ifdef WIN32
		#pragma warning(disable:4200)
#endif
		YkWchar data[];
	};
	
private:
	YkDouble CalculateJaroDistance(const YkUnicodeString& str) const;

	YkInt FindNotOneOf(const YkWchar* pStrCharSet) const;
    YkInt GetStrLength(const YkWchar* pStr) const ;
	inline YkWchar* YK_memchr(const YkWchar *s, YkWchar c, YkInt count) 
	{
		if(count>0) 
		{
			const YkWchar *limit=s+count;
			do 
			{
				if(*s==c) 
				{
					return (YkWchar *)s;
				}
			} while(++s!=limit);
			 return NULL;
		} 
		else 
		{
			return NULL;
		}
	}

	inline YkInt YK_memcmp(const YkWchar *buf1, const YkWchar *buf2, YkInt count) 
	{
		if(count > 0) 
		{
			const YkWchar *limit = buf1 + count;
			YkInt result;

			while (buf1 < limit) {
				result = (YkInt)*buf1 - (YkInt)*buf2;
				if (result != 0) {
					return result;
				}
				buf1++;
				buf2++;
			}
		}
		return 0;
	}
	inline YkWchar* YK_memcpy(YkWchar  *dst,const YkWchar  *src, YkInt n) 
	{
		YkWchar *anchor = dst;
		while(n > 0 )  
		{
			*dst= *src;
			dst++;
			src++;
			--n;
		}

		return anchor;
	}

private:
	union
	{
		QString* m_qString;
		struct ShowQString** m_innerQString;
	};
	
public:
	//! \brief 字符集
	YkMBString::Charset m_nCharset;

private:
	inline void InitCharset()
	{
		if (YK_ISBIGENDIAN)
		{
			m_nCharset = YkMBString::UCS2BE;
		}
		else
		{
			m_nCharset = YkMBString::UCS2LE;
		}
	}
};
#endif

#ifdef _UGUNICODE
typedef YkUnicodeString YkString;
#else
typedef YkMBString YkString;
#endif

EXTERN_C BASE_API  void Unicode2MBString(const YkString& strUnicode, YkMBString& strMB);
#define UNICODE2MBSTRING(strU,strMB) Unicode2MBString(strU,strMB)

EXTERN_C BASE_API  void MBString2Unicode(const YkMBString& strMB, YkString& strUnicode);
#define MBSTRING2UNICODE(strMB,strU) MBString2Unicode(strMB,strU)

typedef YkArray<YkString> YkStringArray;
}

#endif

