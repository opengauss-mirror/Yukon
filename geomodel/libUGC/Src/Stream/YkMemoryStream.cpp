/*
 *
 * YkMemoryStream.cpp
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

#include "Stream/YkMemoryStream.h"

namespace Yk {

YkMemoryStream::YkMemoryStream():YkStream()
{
	SignLoadSave = YkSignStreamLoad;
	m_nBufferSize = 1024;
}

YkSizeT YkMemoryStream::WriteBuffer(YkSizeT count)
{
	if(owns)
	{
		SetSpace((YkSizeT)GetSpace()+YKMAX(count, (YkSizeT)m_nBufferSize));
	}
	return (YkSizeT)(endptr-wrptr);
}

YkSizeT YkMemoryStream::ReadBuffer(YkSizeT count)
{
	if (dir == YkStreamLoadSave )
	{
		if (rdptr+count > endptr)
		{
			return 0;
		}
		else
		{
			wrptr = rdptr + count;
			return count;
		}
	}
	else
	{
		return (YkSizeT)(wrptr-rdptr);
	}
}


void YkMemoryStream::SetSignLoadSave(YkInt nSign)
{
	//内存文件操作相对要简单一些，分俩种情况：
	//（1）、先Load，再保存，因每次Load的时候rdptr 和 wrptr赋为相等
	//（2）、如先Save,再保存，那么w指针在后面，rdptr和begptr相等，当在Load时，
	//已经执行rdptr = wrptr;操作了，因而也不会有问题
	if (SignLoadSave != nSign)
	{
		rdptr = wrptr;
		SignLoadSave = (YkSignLoadSave)nSign;
	}
	if (code == YkStreamEnd && nSign == YkSignStreamSave) 
	{
		code = YkStreamOK;
	}
}

//得到文件长度
YkUlong YkMemoryStream::GetLength() 
{//这里直接返回长度就可以了
	return m_lActualPos;
}

YkUchar* YkMemoryStream::GetData() const
{
	return begptr;
}

YkBool YkMemoryStream::Open(YkStreamDirection save_or_load,YkSizeT size,YkUchar* data)
{
	if(YkStream::Open(save_or_load,size,data))
	{
		m_nBufferSize = size;
		if(save_or_load==YkStreamSave || save_or_load==YkStreamLoadSave)
		{
			wrptr=begptr;
			rdptr=begptr;
		}
		else 
		{
			wrptr=endptr;
			rdptr=begptr;
		}

		return TRUE;
    }
	return FALSE;
}

void YkMemoryStream::TakeBuffer(YkUchar*& data,YkUint& size)
{
	// 受参数类型影响，TakeBuffer暂时不支持超过4G的情况
	if((YkUlong)(endptr - begptr)> 4294967295uL)
	{
		data = NULL;
		size = 0;
		return;
	}
	data=begptr;
	size=(YkUint)(endptr-begptr);
	begptr=NULL;
	wrptr=NULL;
	rdptr=NULL;
	endptr=NULL;
	owns=FALSE;
}

void YkMemoryStream::GiveBuffer(YkUchar *data,YkUint size)
{
	YKASSERT(data);
	if(owns)
	{
		YKFREE(&begptr);
	}
	begptr=data;
	endptr=data+size;
	if(dir==YkStreamSave || dir == YkStreamLoadSave) 
	{
		wrptr=begptr;
		rdptr=begptr;
    }
	else
	{
		wrptr=endptr;
		rdptr=begptr;
    }
	owns=TRUE;
}


YkBool YkMemoryStream::Close()
{
	m_nBufferSize = 1024;
	return YkStream::Close();
}

YkBool YkMemoryStream::SetPosition(YkLong offset,YkWhence whence)
{
	YKASSERT(dir!=YkStreamDead);
	if(code==YkStreamOK)
	{
		if(whence==YkFromCurrent) 
		{
			offset=offset+pos;
		}
		else if(whence==YkFromEnd) 
		{
			offset=offset+m_lActualPos;
		}

		if(dir==YkStreamSave)
		{
			if(begptr+offset>=endptr)
			{
				if(!owns)
				{ 
					SetError(YkStreamFull); 
					return FALSE; 
				}
				SetSpace((YkSizeT)offset);
				if(begptr+offset>endptr)
				{
					return FALSE;
				}
			}
			wrptr=begptr+offset;
		}
		else if (dir==YkStreamLoad)
		{
			if(begptr+offset>=endptr)
			{ 
				SetError(YkStreamEnd);
				return FALSE; 
			}
			rdptr=begptr+offset;
		}
		else
		{
			if(begptr+offset>endptr)
			{
				if(!owns)
				{ 
					SetError(YkStreamFull); 
					return FALSE; 
				}
				SetSpace((YkSizeT)offset);
				if(begptr+offset>=endptr) 
				{
					return FALSE;
				}
			}
			//这里让wrptr和rdptr相等，那么在调用load的时候就会调用ReadBuffer函数
			wrptr=begptr+offset;  
			rdptr=begptr+offset;
		}
		pos=offset;
		return TRUE;
    }
	return FALSE;
}

YkMemoryStream::~YkMemoryStream()
{
	Close();
}

YkStream& YkMemoryStream::operator<<( const YkUchar& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkAchar& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkUshort& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkShort& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkUint& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkInt& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkFloat& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkDouble& v )
{
	YkStream::operator<<(v);  return *this;
}

#ifdef Yk_LONG
YkStream& YkMemoryStream::operator<<( const YkLong& v )
{
	YkStream::operator<<(v);  return *this;
}

YkStream& YkMemoryStream::operator<<( const YkUlong& v )
{
	YkStream::operator<<(v);  return *this;
}
#endif

YkStream& YkMemoryStream::Save( const YkUchar* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkAchar* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkUshort* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkShort* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkUint* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkInt* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkFloat* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkDouble* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

#ifdef Yk_LONG
YkStream& YkMemoryStream::Save( const YkLong* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}

YkStream& YkMemoryStream::Save( const YkUlong* p,YkSizeT n )
{
	YkStream::Save(p,n);  return *this;
}
#endif

YkStream& YkMemoryStream::operator>>( YkUchar& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkAchar& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkUshort& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkShort& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkUint& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkInt& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkFloat& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkDouble& v )
{
	YkStream::operator>>(v); return *this;
}

#ifdef Yk_LONG
YkStream& YkMemoryStream::operator>>( YkLong& v )
{
	YkStream::operator>>(v); return *this;
}

YkStream& YkMemoryStream::operator>>( YkUlong& v )
{
	YkStream::operator>>(v); return *this;
}
#endif

YkStream& YkMemoryStream::Load( YkUchar* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkAchar* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkUshort* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkShort* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkUint* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkInt* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkFloat* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkDouble* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

#ifdef Yk_LONG
YkStream& YkMemoryStream::Load( YkLong* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}

YkStream& YkMemoryStream::Load( YkUlong* p,YkSizeT n )
{
	YkStream::Load(p,n); return *this;
}
#endif

YkStream& YkMemoryStream::operator<<(const YkString& s)
{	
	YkStream::operator <<(s);
	return *this;
}

YkStream& YkMemoryStream::operator>>(YkString& s)
{
	YkStream::operator >>(s);
	return *this;
}

}


