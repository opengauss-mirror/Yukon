/*
 *
 * YkTextureInfo.cpp
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

#include "Toolkit/YkTextureInfo.h"

using namespace Yk;

YkTextureInfo::~YkTextureInfo()
{
	delete []m_pBuffer;
	m_pBuffer=NULL;
	m_enFormat=IPF_UNKNOWN;
	m_nSize = 0;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nMipLev = 0;
	m_nCompress = 0;
	m_bLoadImage = TRUE;
}

const YkTextureInfo& YkTextureInfo::operator=( const YkTextureInfo& Info )
{
	if (&Info == this)
	{
		return *this;
	}

	delete []m_pBuffer;
	m_pBuffer = NULL;

	m_enFormat = Info.m_enFormat;
	m_nWidth = Info.m_nWidth;
	m_nHeight = Info.m_nHeight;
	m_nSize = Info.m_nSize;
	m_strPath = Info.m_strPath;
	m_nMipLev = Info.m_nMipLev; 
	m_strName = Info.m_strName;
	m_nCompress = Info.m_nCompress;

	if (Info.m_pBuffer != NULL && Info.m_nSize > 0)
	{
		m_pBuffer = (YkUchar*)malloc(Info.m_nSize);
		memcpy(m_pBuffer,Info.m_pBuffer,Info.m_nSize);
	}

	m_bLoadImage = Info.m_bLoadImage;
	return *this;
}

YkTextureInfo::YkTextureInfo( const YkTextureInfo& Info )
{
	m_enFormat = Info.m_enFormat;
	m_nWidth = Info.m_nWidth;
	m_nHeight = Info.m_nHeight;
	m_nSize = Info.m_nSize;
	m_pBuffer = NULL;
	if (m_nSize > 0)
	{
		m_pBuffer = (YkUchar*)malloc(m_nSize);
		memcpy(m_pBuffer,Info.m_pBuffer,m_nSize);
	}
	m_strPath = Info.m_strPath;
	m_nMipLev = Info.m_nMipLev; 
	m_strName = Info.m_strName;
	m_nCompress = Info.m_nCompress;
	m_bLoadImage = Info.m_bLoadImage;
}

YkTextureInfo::YkTextureInfo()
{
	m_enFormat = IPF_UNKNOWN;
	m_nWidth = 0;
	m_nHeight = 0;
	m_nSize = 0;
	m_pBuffer = NULL;
	m_nMipLev = 0;
	m_nCompress = 0;
	m_bLoadImage = TRUE;
}

YkTextureInfo::YkTextureInfo( PixelFormat enFormat,YkUint Width,YkUint Height,
						 void* data,YkUint nSize, const YkString& path)
{
	m_enFormat = enFormat;
	m_nWidth = Width;
	m_nHeight = Height;
	m_nSize = nSize;
	m_pBuffer = (YkUchar*)data;
	m_strPath = path;
	m_nCompress = 0;
	m_nMipLev = 0;
	m_bLoadImage = TRUE;
}

YkInt YkTextureInfo::GetDataSize()
{
	YkInt nSize = 0;
	nSize += m_strPath.GetLength() + sizeof(YkInt);
	nSize += m_strName.GetLength() + sizeof(YkInt);
	nSize = sizeof(m_enFormat);
	nSize += sizeof(m_nCompress);
	nSize += sizeof(m_nHeight);
	nSize += sizeof(m_nWidth);
	nSize += sizeof(m_nMipLev);
	nSize += sizeof(m_nSize);
	nSize += sizeof(YkByte) * m_nSize;
	return nSize;
}

void YkTextureInfo::Save(YkStream& fStream)
{
	fStream<<m_strPath;
	fStream<<m_strName;
	fStream<<m_enFormat;
	fStream<<m_nCompress;
	fStream<<m_nHeight;
	fStream<<m_nWidth;
	fStream<<m_nMipLev;
	fStream<<m_nSize;
	if(m_nSize > 0 && m_pBuffer != NULL)
	{
		fStream.Save(m_pBuffer, m_nSize);
	}
}

void YkTextureInfo::Load(YkStream& fStream)
{
	fStream>>m_strPath;
	fStream>>m_strName;
	fStream>>(YkInt&)m_enFormat;
	fStream>>m_nCompress;
	fStream>>m_nHeight;
	fStream>>m_nWidth;
	fStream>>m_nMipLev;
	fStream>>m_nSize;
	if(m_nSize > 0)
	{
		m_pBuffer = (YkUchar*)malloc(m_nSize);
		fStream.Load(m_pBuffer, m_nSize);
	}
}