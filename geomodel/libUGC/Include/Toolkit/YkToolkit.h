/*
 *
 * YkToolkit.h
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

#ifndef AFX_YKTOOLKIT_H__F8FE3447_7D93_4278_978F_942832101D97__INCLUDED_
#define AFX_YKTOOLKIT_H__F8FE3447_7D93_4278_978F_942832101D97__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include "Toolkit/YkTextCodec.h"
#include "Stream/YkPlatform.h"
#include "Toolkit/YkThread.h"

namespace Yk{

// 全局锁，用来控制应用层把主线程拆分到多个子线程里面操作的情况导致的乱图和死锁
extern TOOLKIT_API YkMutex g_MutexToolkit;

class TOOLKIT_API YkToolkit  
{
private:
	YkToolkit();
	~YkToolkit();

public:
	static YkInt RoundLong(YkDouble dValue)
	{
		if(dValue>0) return YkInt(dValue+0.5);
		else return YkInt(dValue-0.5);
	};

	//! \brief 得到当前进程的id
	static YkInt GetCurProcessId();

	//{{ 动态库处理函数======================================================
	static void* LoadDll(const YkString& strLibName);
	// 通过库的句柄和函数名字,得到函数指针
	static void* GetProcAddress(void* hModule, const YkString& strProName);
	// 释放库
	static YkBool FreeDll(void* hModule);
	//}} 动态库处理函数======================================================

	//! \brief 得到当前系统默认的字符集
	static YkMBString::Charset GetCurCharset();

	// 创建guid
	static YkString CreateGuid();

	//! \brief 内存信息结构
	struct MemoryStatus
	{
		MemoryStatus(){memset(this, 0, sizeof(MemoryStatus));}
		//! \brief 可用物理内存
		YkUlong m_nAvailablePhysical;
		//! \brief 所有物理内存
		YkUlong m_nTotalPhysical;
		//! \brief 可用虚拟内存
		YkUlong m_nAvailableVirtual;
		//! \brief 所有虚拟内存
		YkUlong m_nTotalVirtual;
		//! \brief 内存的占有率
		YkUlong m_nMemoryLoad;
	};

	struct ProcesMemoryStatus 
	{
		//!\brief 当前物理内存分配大小, 单位Kb
		YkUlong m_nWorkingSetSize;
		//!\brief 当前虚拟内存分配大小,单位Kb
		YkUlong m_nQuotaPagedPoolUsage;
	};

	//! \brief 得到系统内存状态, 单位均为KB
	//! \param memoryStatus 通过引用返回内存的各种信息[out]
	//! \return 返回查询得到的内存状态
	static YkBool GetMemoryStatus(MemoryStatus& memoryStatus);

	//! \brief 得到系统中空闲(可用)物理内存的大小(KB为单位)
	//! \return 返回系统中空闲(可用)物理内存的大小(KB为单位)
	//! \remarks 分平台实现,Windows上有API:GlobalMemoryStatus;Linux/Unix上用sysconf函数
	//! \attention 本函数已经被GetMemoryStatus替代, 等价于 GetMemoryStatus(AvailablePhysical); 
	//! 为了保持以前的程序可用,暂时保存;新的代码请直接使用 GetMemoryStatus
	static YkUlong GetAvailablePhysicalMemory();
	
#ifdef WIN32
	//!\brief 获取当前进程的内存使用情况
	static YkBool GetCurrentProcMemoryStatus(ProcesMemoryStatus& procesMemory);
#endif

	static YkUint GetCPUCount();

public:
	//! \brief 得到XML文件头
	//! \remarks 在简体中文环境下返回 <?xml version="1.0" encoding="GB18030"?> 
 	static YkString GetXmlFileHeader(YkMBString::Charset charset = GetCurCharset());

public:
	//判断字符串是否utf-8，有缺陷，例如改gb2312的“食品”。但还可以用一下
 	static YkBool IsUTF8(YkMBString &strSrc);

public:
	//! \brief 指定目录的可用磁盘空间
	//! \param  strPath		指定目录[in]
	//! \return 磁盘可用字节
	static YkUlong GetDiskSpace(const YkString& strPath);
};

//得到最(较)大值
template<typename Type>
const Type& YkMax(const Type& tValue1, const Type& tValue2)
{
	return ( (tValue1<tValue2)? tValue2 : tValue1);
}
//在nCount个数中得到最大值
template<typename Type>
Type YkMax(const Type* ptValue, YkInt nCount)
{
	assert(ptValue != NULL); //ptValue不能为空
	Type tResult(ptValue[0]); 
	for(YkInt i=1; i<nCount; i++)
	{
		tResult = ((ptValue[i]<tResult)? tResult : ptValue[i]);
	}
	return tResult;
}

//得到最(较)大值,采用比较仿函数(小于比较)
template<typename Type, typename Compare>
const Type& YkMax(const Type& tValue1, const Type& tValue2, Compare compare)
{
	return (compare(tValue1, tValue2)? tValue2 : tValue1);
}

//在nCount个数中得到最大值,采用比较仿函数(小于比较)
template<typename Type, typename Compare>
Type YkMax(const Type* ptValue, YkInt nCount, Compare compare)
{
	assert(ptValue != NULL); //ptValue不能为空
	Type tResult(ptValue[0]); 
	for(YkInt i=1; i<nCount; i++)
	{
		tResult = (compare(ptValue[i], tResult)? tResult : ptValue[i]);
	}
	return tResult;
}

template<typename Type>
const Type& YkMin(const Type& tValue1, const Type& tValue2)
{
	return ( (tValue2<tValue1)? tValue2 : tValue1);
}

//在nCount个数中得到最小值
template<typename Type>
Type YkMin(const Type* ptValue, YkInt nCount)
{
	assert(ptValue != NULL); //ptValue不能为空
	Type tResult(ptValue[0]); 
	for(YkInt i=1; i<nCount; i++)
	{
		tResult = ((tResult<ptValue[i])? tResult : ptValue[i]);
	}
	return tResult;
}

//得到最(较)小值,采用比较仿函数(小于比较)
template<typename Type, typename Compare>
const Type& YkMin(const Type& tValue1, const Type& tValue2, Compare compare)
{
	return (compare(tValue2,tValue1)? tValue2 : tValue1);
}

//在nCount个数中得到最小值,采用比较仿函数(小于比较)
template<typename Type, typename Compare>
Type YkMin(const Type* ptValue, YkInt nCount, Compare compare)
{
	assert(ptValue != NULL); //ptValue不能为空
	Type tResult(ptValue[0]); 
	for(YkInt i=1; i<nCount; i++)
	{
		tResult = (compare(tResult, ptValue[i])? tResult : ptValue[i]);
	}
	return tResult;
}
template<typename Type>
void Sort(Type* pBegin, Type* pEnd)
{
	std::sort(pBegin,pEnd);
}
template<typename Type, typename Compare>
void Sort(Type* pBegin, Type* pEnd, Compare compare)
{
	std::sort(pBegin,pEnd,compare);
}
// StableSort, 基本同Sort, 区别在于StableSort保证相等元素的原本次序在排序后保持不变
template<typename Type>
void StableSort(const Type* pBegin, const Type* pEnd)
{
	std::stable_sort(pBegin, pEnd);
}

template<typename Type, typename Compare>
void StableSort(const Type* pBegin, const Type* pEnd, Compare compare)
{
	std::stable_sort(pBegin, pEnd, compare);
}

//! \brief 求两个集合之间的差
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type>
void Difference(const Type* pBegin1, const Type* pEnd1, 
				const Type* pBegin2, const Type* pEnd2, YkArray<Type>&arrResult)
{
	std::set_difference(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()));
}

//! \brief 求两个集合之间的差
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type, typename Compare>
void Difference(const Type* pBegin1, const Type* pEnd1, 
				const Type* pBegin2, const Type* pEnd2, 
				YkArray<Type>&arrResult, Compare compare)
{
	std::set_difference(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()), compare);
}

//! \brief 求两个集合之间的交
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type>
void Intersection(const Type* pBegin1, const Type* pEnd1, 
				  const Type* pBegin2, const Type* pEnd2, YkArray<Type>&arrResult)
{
	std::set_intersection(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()));
}

//! \brief 求两个集合之间的交
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type, typename Compare>
void Intersection(const Type* pBegin1, const Type* pEnd1, 
				  const Type* pBegin2, const Type* pEnd2, 
				  YkArray<Type>&arrResult, Compare compare)
{
	std::set_intersection(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()), compare);
}

//! \brief 求两个集合之间的对称差
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type>
void SymmetricDifference(const Type* pBegin1, const Type* pEnd1, 
						 const Type* pBegin2, const Type* pEnd2, YkArray<Type>&arrResult)
{
	std::set_symmetric_difference(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()));
}

//! \brief 求两个集合之间的对称差
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type, typename Compare>
void SymmetricDifference(const Type* pBegin1, const Type* pEnd1, 
						 const Type* pBegin2, const Type* pEnd2, 
						 YkArray<Type>&arrResult, Compare compare)
{
	std::set_symmetric_difference(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()), compare);
}

//! \brief 求两个集合之间的并
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type>
void Union(const Type* pBegin1, const Type* pEnd1, 
		   const Type* pBegin2, const Type* pEnd2, YkArray<Type>&arrResult)
{
	std::set_union(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()));
}

//! \brief 求两个集合之间的并
//! \remarks arrResult原有数据会被覆盖,空间不够会自动增长
template<typename Type, typename Compare>
void Union(const Type* pBegin1, const Type* pEnd1, 
		   const Type* pBegin2, const Type* pEnd2, 
		   YkArray<Type>&arrResult, Compare compare)
{
	std::set_union(pBegin1, pEnd1, pBegin2, pEnd2, 
		std::inserter(arrResult.Inner(), arrResult.Inner().begin()), compare);
}

//! \brief 让集合中的元素不重复
//! \param pBegin 集合开始指针
//! \param pEnd 集合结束指针
//! \remarks 在调用Unique之前,集合必须先通过排序,即必须是有序集合
//!			 处理后,保证前面若干个元素不重复
//!			 内部采用stl中的unique算法函数实现
//! \return 返回不重复元素的个数
template<typename Type>
YkUint Unique(Type* pBegin, Type* pEnd)
{
	return std::unique(pBegin, pEnd) - pBegin;
}

//! \brief 让集合中的元素不重复
//! \param pBegin 集合开始指针
//! \param pEnd 集合结束指针
//! \param Compare 元素比较对象
//! \remarks 在调用Unique之前,集合必须先通过排序,即必须是有序集合
//!			 处理后,保证前面若干个元素不重复
//!			 内部采用stl中的unique算法函数实现
//! \return 返回不重复元素的个数
template<typename Type, typename Compare>
YkUint Unique(Type* pBegin, Type* pEnd, Compare compare)
{
	return std::unique(pBegin, pEnd, compare) - pBegin;
}

//! \brief 让集合中的元素不重复
//! \param pBegin 集合开始指针
//! \param pEnd 集合结束指针
//! \param pOut 输出的集合指针
//! \remarks 在调用UniqueCopy之前,集合必须先通过排序,即必须是有序集合;且pOut有足够的空间(不小于返回值)
//!			 处理后,保证pOut前面若干个元素不重复
//!			 内部采用stl中的unique算法函数实现
//! \return 返回不重复元素的个数
template<typename Type>
YkUint UniqueCopy(Type* pBegin, Type* pEnd, Type* pOut)
{
	return std::unique_copy(pBegin, pEnd, pOut) - pOut;
}

//! \brief 让集合中的元素不重复
//! \param pBegin 集合开始指针
//! \param pEnd 集合结束指针
//! \param pOut 输出的集合指针
//! \param Compare 元素比较对象
//! \remarks 在调用UniqueCopy之前,集合必须先通过排序,即必须是有序集合;且pOut有足够的空间(不小于返回值)
//!			 处理后,保证pOut前面若干个元素不重复
//!			 内部采用stl中的unique算法函数实现
//! \return 返回不重复元素的个数
template<typename Type, typename Compare>
YkUint UniqueCopy(Type* pBegin, Type* pEnd, Type* pOut, Compare compare)
{
	return std::unique_copy(pBegin, pEnd, pOut, compare) - pOut;
}

}

#endif

