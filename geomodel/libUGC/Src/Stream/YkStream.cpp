/*
 *
 * YkStream.cpp
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

#include "Stream/YkStream.h"

namespace Yk {

static bool s_In = false;

YkStream::YkStream()
{
	begptr=NULL;
	endptr=NULL;
	wrptr=NULL;
	rdptr=NULL;
	pos=0;
	dir=YkStreamDead;
	code=YkStreamOK;
	swap = YK_ISBIGENDIAN;
	owns=FALSE;  
	m_lActualPos = 0;
	m_eCharset = YkString::Default;
}

YkStream::~YkStream()
{
	if(owns)
	{
		YKFREE(&begptr);
	}
	begptr=(YkUchar*)-1;
	endptr=(YkUchar*)-1;
	wrptr=(YkUchar*)-1;
	rdptr=(YkUchar*)-1;
}

YkBool YkStream::SwapBytes() const
{ 
	return swap; 
}	

YkSizeT YkStream::WriteBuffer(YkSizeT)
{
	rdptr=begptr;
	wrptr=begptr;
	return (endptr-wrptr);
}

YkSizeT YkStream::ReadBuffer(YkSizeT)
{
	rdptr=begptr;
	wrptr=endptr;
	return (wrptr-rdptr);
}

void YkStream::SetSignLoadSave(YkInt /*nSign*/)
{

}


void YkStream::SetError(YkStreamStatus err)
{
	code=err;
}

YkSizeT YkStream::GetSpace() const 
{
	return (YkSizeT)(endptr-begptr);
}

void YkStream::SetSpace(YkSizeT size)
{
	if(code==YkStreamOK)
	{
		if(begptr+size!=endptr)
		{      
			YkUchar *oldbegptr=begptr;
			YKASSERT(owns);            
			if(!YKRESIZE(&begptr,YkUchar,size))
			{ 
				code=YkStreamAlloc; 
				return; 
			}
			endptr=begptr+size;
			wrptr=begptr+(wrptr-oldbegptr);
			rdptr=begptr+(rdptr-oldbegptr);
			if(wrptr>endptr) 
			{
				wrptr=endptr;
			}
			if(rdptr>endptr) 
			{
				rdptr=endptr;
			}
		}
    }
}


YkBool YkStream::Open(YkStreamDirection save_or_load, YkSizeT size, YkUchar* data)
{
	YKASSERT(save_or_load ==YkStreamSave 
		|| save_or_load==YkStreamLoad 
		|| save_or_load == YkStreamLoadSave);
	if(!dir)
	{    
		if(data)
		{
			begptr=data;
			if(size==UINT_MAX)
			{
				endptr=((YkUchar*)NULL)-1;  //NOSONAR
			}
			else
			{
				endptr=begptr+size;
			}
			wrptr=begptr;
			rdptr=begptr;
			m_lActualPos = size;
			owns=FALSE;
		}
		else
		{
			if(!YKCALLOC(&begptr,YkUchar,size))
			{ 
				code=YkStreamAlloc; 
				return FALSE; 
			}
			endptr=begptr+size;
			wrptr=begptr;
			rdptr=begptr;
			owns=TRUE;
		}
		dir=save_or_load;
		pos=0;    
		m_lActualPos = 0;
		code=YkStreamOK;
		return TRUE;
    }
	return FALSE;
}

YkBool YkStream::Close()
{
	m_lActualPos = 0; 
	if(dir)
	{
		dir=YkStreamDead;
		if(owns)
		{
			YKFREE(&begptr);
		}
		begptr=NULL;
		wrptr=NULL;
		rdptr=NULL;
		endptr=NULL;
		owns=FALSE;
		return code==YkStreamOK;
    }
	return FALSE;
}


YkBool YkStream::Flush()
{
  WriteBuffer(0);
  return code==YkStreamOK;
}

YkBool YkStream::SetPosition(YkLong offset,YkWhence whence)
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
			offset=offset+endptr-begptr;
		}
		pos=offset;
		return TRUE;
    }
	return FALSE;
}

YkStream& YkStream::operator<<(const YkUchar& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(wrptr+1>endptr && WriteBuffer(1)<1)
		{ 
			code=YkStreamFull; 
			return *this; 
		}
		YKASSERT(wrptr+1<=endptr);
		*wrptr++ = v;
		pos++;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator<<(const YkUshort& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if( (wrptr+2>endptr)
			&& WriteBuffer((YkSizeT)(wrptr-endptr)+2)<2 )
		{ 
			code=YkStreamFull; 
			return *this; 
		}
		YKASSERT(wrptr+2<=endptr);
		if(swap)
		{
			wrptr[0]=((const YkUchar*)&v)[1];
			wrptr[1]=((const YkUchar*)&v)[0];
		}
		else
		{
			wrptr[0]=((const YkUchar*)&v)[0];
			wrptr[1]=((const YkUchar*)&v)[1];
		}
		wrptr+=2;
		pos+=2;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator<<(const YkUint& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(wrptr+4>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+4)<4)
		{ 
			code=YkStreamFull; 
			return *this; 
		}
		YKASSERT(wrptr+4<=endptr);
		if(swap)
		{
			wrptr[0]=((const YkUchar*)&v)[3];
			wrptr[1]=((const YkUchar*)&v)[2];
			wrptr[2]=((const YkUchar*)&v)[1];
			wrptr[3]=((const YkUchar*)&v)[0];
		}
		else
		{
			wrptr[0]=((const YkUchar*)&v)[0];
			wrptr[1]=((const YkUchar*)&v)[1];
			wrptr[2]=((const YkUchar*)&v)[2];
			wrptr[3]=((const YkUchar*)&v)[3];
		}
		wrptr+=4;
		pos+=4;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator<<(const YkDouble& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(wrptr+8>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+8)<8)
		{ 
			code=YkStreamFull; 
			return *this; 
		}
		YKASSERT(wrptr+8<=endptr);
		if(swap)
		{
			wrptr[0]=((const YkUchar*)&v)[7];
			wrptr[1]=((const YkUchar*)&v)[6];
			wrptr[2]=((const YkUchar*)&v)[5];
			wrptr[3]=((const YkUchar*)&v)[4];
			wrptr[4]=((const YkUchar*)&v)[3];
			wrptr[5]=((const YkUchar*)&v)[2];
			wrptr[6]=((const YkUchar*)&v)[1];
			wrptr[7]=((const YkUchar*)&v)[0];
		}
		else
		{
			wrptr[0]=((const YkUchar*)&v)[0];
			wrptr[1]=((const YkUchar*)&v)[1];
			wrptr[2]=((const YkUchar*)&v)[2];
			wrptr[3]=((const YkUchar*)&v)[3];
			wrptr[4]=((const YkUchar*)&v)[4];
			wrptr[5]=((const YkUchar*)&v)[5];
			wrptr[6]=((const YkUchar*)&v)[6];
			wrptr[7]=((const YkUchar*)&v)[7];
		}
		wrptr+=8;
		pos+=8;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Save(const YkUchar* p,YkSizeT n)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		while(0<n)
		{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<1)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr<endptr);
				do{
					*wrptr++=*p++;
					pos++;
					n--;
				}
				while(0<n && wrptr<endptr);
			}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Save(const YkUshort* p,YkSizeT n)
{
	const YkUchar *q=(const YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		n<<=1;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<2)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+2<=endptr);
				do{
					wrptr[0]=q[1];
					wrptr[1]=q[0];
					wrptr+=2;
					pos+=2;
					q+=2;
					n-=2;
				}
				while(0<n && wrptr+2<=endptr);
			}
		}
		else
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<2)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+2<=endptr);
				do{
					wrptr[0]=q[0];
					wrptr[1]=q[1];
					wrptr+=2;
					pos+=2;
					q+=2;
					n-=2;
				}
				while(0<n && wrptr+2<=endptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Save(const YkUint* p,YkSizeT n)
{
	const YkUchar *q=(const YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		n<<=2;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<4)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+4<=endptr);
				do{
					wrptr[0]=q[3];
					wrptr[1]=q[2];
					wrptr[2]=q[1];
					wrptr[3]=q[0];
					wrptr+=4;
					pos+=4;
					q+=4;
					n-=4;
				}
				while(0<n && wrptr+4<=endptr);
			}
		}
		else
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<4)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+4<=endptr);
				do{
					wrptr[0]=q[0];
					wrptr[1]=q[1];
					wrptr[2]=q[2];
					wrptr[3]=q[3];
					wrptr+=4;
					pos+=4;
					q+=4;
					n-=4;
				}
				while(0<n && wrptr+4<=endptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Save(const YkDouble* p,YkSizeT n)
{
	const YkUchar *q=(const YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamSave);
	}
	if(code==YkStreamOK)
	{
		n<<=3;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<8)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+8<=endptr);
				do{
					wrptr[0]=q[7];
					wrptr[1]=q[6];
					wrptr[2]=q[5];
					wrptr[3]=q[4];
					wrptr[4]=q[3];
					wrptr[5]=q[2];
					wrptr[6]=q[1];
					wrptr[7]=q[0];
					wrptr+=8;
					pos+=8;
					q+=8;
					n-=8;
				}
				while(0<n && wrptr+8<=endptr);
			}
		}
		else
		{
			while(0<n)
			{
				if(wrptr+n>endptr && WriteBuffer((YkSizeT)(wrptr-endptr)+n)<8)
				{ 
					code=YkStreamFull; 
					return *this; 
				}
				YKASSERT(wrptr+8<=endptr);
				do{
					wrptr[0]=q[0];
					wrptr[1]=q[1];
					wrptr[2]=q[2];
					wrptr[3]=q[3];
					wrptr[4]=q[4];
					wrptr[5]=q[5];
					wrptr[6]=q[6];
					wrptr[7]=q[7];
					wrptr+=8;
					pos+=8;
					q+=8;
					n-=8;
				}
				while(0<n && wrptr+8<=endptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Save( const YkAchar* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkUchar*>(p),n);
}

YkStream& YkStream::Save( const YkShort* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkUshort*>(p),n);
}

YkStream& YkStream::Save( const YkInt* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkUint*>(p),n);
}

YkStream& YkStream::Save( const YkFloat* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkUint*>(p),n);
}

#ifdef Yk_LONG
YkStream& YkStream::Save( const YkLong* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkDouble*>(p),n);
}

YkStream& YkStream::Save( const YkUlong* p,YkSizeT n )
{
	return Save(reinterpret_cast<const YkDouble*>(p),n);
}
#endif

YkStream& YkStream::operator>>(YkUchar& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(rdptr+1>=wrptr && ReadBuffer(1)<1)
		{
			code=YkStreamEnd; 
			return *this; 
		}
		YKASSERT(rdptr+1<=wrptr);
		v=*rdptr++;
		pos++;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator>>(YkUshort& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(rdptr+2>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+2)<2)
		{
			code=YkStreamEnd; 
			return *this; 
		}
		
		YKASSERT(rdptr+2<=wrptr);
		if(swap)
		{
			((YkUchar*)&v)[1]=rdptr[0];
			((YkUchar*)&v)[0]=rdptr[1];
		}
		else
		{
			((YkUchar*)&v)[0]=rdptr[0];
			((YkUchar*)&v)[1]=rdptr[1];
		}
		rdptr+=2;
		pos+=2;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator>>(YkUint& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(rdptr+4>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+4)<4)
		{ 
			code=YkStreamEnd; 
			return *this;
		}
		YKASSERT(rdptr+4<=wrptr);
		if(swap)
		{
			((YkUchar*)&v)[3]=rdptr[0];
			((YkUchar*)&v)[2]=rdptr[1];
			((YkUchar*)&v)[1]=rdptr[2];
			((YkUchar*)&v)[0]=rdptr[3];
		}
		else
		{
			((YkUchar*)&v)[0]=rdptr[0];
			((YkUchar*)&v)[1]=rdptr[1];
			((YkUchar*)&v)[2]=rdptr[2];
			((YkUchar*)&v)[3]=rdptr[3];
		}
		rdptr+=4;
		pos+=4;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator>>(YkDouble& v)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(rdptr+8>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+8)<8)
		{ 
			code=YkStreamEnd; 
			return *this;
		}
		YKASSERT(rdptr+8<=wrptr);
		if(swap)
		{
			((YkUchar*)&v)[7]=rdptr[0];
			((YkUchar*)&v)[6]=rdptr[1];
			((YkUchar*)&v)[5]=rdptr[2];
			((YkUchar*)&v)[4]=rdptr[3];
			((YkUchar*)&v)[3]=rdptr[4];
			((YkUchar*)&v)[2]=rdptr[5];
			((YkUchar*)&v)[1]=rdptr[6];
			((YkUchar*)&v)[0]=rdptr[7];
		}
		else
		{
			((YkUchar*)&v)[0]=rdptr[0];
			((YkUchar*)&v)[1]=rdptr[1];
			((YkUchar*)&v)[2]=rdptr[2];
			((YkUchar*)&v)[3]=rdptr[3];
			((YkUchar*)&v)[4]=rdptr[4];
			((YkUchar*)&v)[5]=rdptr[5];
			((YkUchar*)&v)[6]=rdptr[6];
			((YkUchar*)&v)[7]=rdptr[7];
		}
		rdptr+=8;
		pos+=8;
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::operator>>( YkAchar& v )
{
	return *this >> reinterpret_cast<YkUchar&>(v);
}

YkStream& YkStream::operator>>( YkShort& v )
{
	return *this >> reinterpret_cast<YkUshort&>(v);
}

YkStream& YkStream::operator>>( YkInt& v )
{
	return *this >> reinterpret_cast<YkUint&>(v);
}

YkStream& YkStream::operator>>( YkFloat& v )
{
	return *this >> reinterpret_cast<YkUint&>(v);
}

#ifdef Yk_LONG
YkStream& YkStream::operator>>( YkLong& v )
{
	return *this >> reinterpret_cast<YkDouble&>(v);
}

YkStream& YkStream::operator>>( YkUlong& v )
{
	return *this >> reinterpret_cast<YkDouble&>(v);
}
#endif

YkStream& YkStream::operator<<(const YkString& s)
{
#ifdef _UGUNICODE
	if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
	{
		YkInt len = s.GetLength();
		YkInt nByteLen = len * sizeof(YkChar);
		*this<<nByteLen;
		Save((YkUshort*)s.Cstr(), len);
	}
	else
	{
		YkMBString strMB;
		strMB.SetCharset(m_eCharset);
		s.ToMBString(strMB);
		YkInt len = strMB.GetLength();
		*this<<len;
		Save(strMB.Cstr(), len);
	}
#else
	YkInt len=s.GetLength();
	*this<<len;
	Save(s.Cstr(),len);
#endif
	return *this;
}

YkStream& YkStream::operator>>(YkString& s)
{
#ifdef _UGUNICODE
	if(m_eCharset == YkString::Unicode || m_eCharset == YkString::UCS2LE)
	{
		YkInt len = 0;
		*this>>len;
		s.SetLength(len/sizeof(YkChar));
		Load((YkUshort*)s.Cstr(), len/sizeof(YkChar));
	}
	else
	{
		YkMBString strMB;
		strMB.SetCharset(m_eCharset);
		YkInt len = 0;
		*this>>len;
		strMB.SetLength(len);
		Load((YkAchar*)strMB.Cstr(), len);
		s.FromMBString(strMB);
		s.SetCharset(m_eCharset);
	}
#else
	YkInt len=0;
	*this>>len;
	s.SetLength(len);
	Load((YkAchar*)s.Cstr(),len);
	s.SetCharset(m_eCharset);
#endif
	return *this;
}

YkStream& YkStream::Load(YkUchar* p,YkSizeT n)
{
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		while(0<n)
		{
			if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<1)
			{ 
				code=YkStreamEnd; 
				return *this; 
			}
			YKASSERT(rdptr<wrptr);
			do{
				*p++=*rdptr++;
				pos++;
				n--;
			}
			while(0<n && rdptr<wrptr);
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Load(YkUshort* p,YkSizeT n)
{
	YkUchar *q=(YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		n<<=1;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<2)
				{
					code=YkStreamEnd; 
					return *this; 
				}
				YKASSERT(rdptr+2<=wrptr);
				do{
					q[1]=rdptr[0];
					q[0]=rdptr[1];
					rdptr+=2;
					pos+=2;
					q+=2;
					n-=2;
				}
				while(0<n && rdptr+2<=wrptr);
			}
		}
		else{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<2)
				{ 
					code=YkStreamEnd; 
					return *this; 
				}
				YKASSERT(rdptr+2<=wrptr);
				do{
					q[0]=rdptr[0];
					q[1]=rdptr[1];
					rdptr+=2;
					pos+=2;
					q+=2;
					n-=2;
				}
				while(0<n && rdptr+2<=wrptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Load(YkUint* p,YkSizeT n)
{
	YkUchar *q=(YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		n<<=2;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<4)
				{ 
					code=YkStreamEnd; 
					return *this; 
				}
				YKASSERT(rdptr+4<=wrptr);
				do{
					q[3]=rdptr[0];
					q[2]=rdptr[1];
					q[1]=rdptr[2];
					q[0]=rdptr[3];
					rdptr+=4;
					pos+=4;
					q+=4;
					n-=4;
				}
				while(0<n && rdptr+4<=wrptr);
			}
		}
		else{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<4)
				{
					code=YkStreamEnd; 
					return *this; 
				}
				YKASSERT(rdptr+4<=wrptr);
				do{
					q[0]=rdptr[0];
					q[1]=rdptr[1];
					q[2]=rdptr[2];
					q[3]=rdptr[3];
					rdptr+=4;
					pos+=4;
					q+=4;
					n-=4;
				}
				while(0<n && rdptr+4<=wrptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Load(YkDouble* p,YkSizeT n)
{
	YkUchar *q=(YkUchar*)p;
	if (dir == YkStreamLoadSave )  
	{
		SetSignLoadSave(YkSignStreamLoad);
	}
	if(code==YkStreamOK)
	{
		n<<=3;
		YKASSERT(begptr<=rdptr);
		YKASSERT(rdptr<=wrptr);
		YKASSERT(wrptr<=endptr);
		if(swap)
		{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<8)
				{
					code=YkStreamEnd; 
					return *this; 
				}
				YKASSERT(rdptr+4<=wrptr);
				do{
					q[7]=rdptr[0];
					q[6]=rdptr[1];
					q[5]=rdptr[2];
					q[4]=rdptr[3];
					q[3]=rdptr[4];
					q[2]=rdptr[5];
					q[1]=rdptr[6];
					q[0]=rdptr[7];
					rdptr+=8;
					pos+=8;
					q+=8;
					n-=8;
				}
				while(0<n && rdptr+8<=wrptr);
			}
		}
		else{
			while(0<n)
			{
				if(rdptr+n>=wrptr && ReadBuffer((YkSizeT)(rdptr-wrptr)+n)<8)
				{
					code=YkStreamEnd; 
					return *this;
				}
				YKASSERT(rdptr+4<=wrptr);
				do{
					q[0]=rdptr[0];
					q[1]=rdptr[1];
					q[2]=rdptr[2];
					q[3]=rdptr[3];
					q[4]=rdptr[4];
					q[5]=rdptr[5];
					q[6]=rdptr[6];
					q[7]=rdptr[7];
					rdptr+=8;
					pos+=8;
					q+=8;
					n-=8;
				}
				while(0<n && rdptr+8<=wrptr);
			}
		}
    }

	m_lActualPos = YKMAX(m_lActualPos, pos);

	return *this;
}

YkStream& YkStream::Load( YkAchar* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkUchar*>(p),n);
}

YkStream& YkStream::Load( YkShort* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkUshort*>(p),n);
}

YkStream& YkStream::Load( YkInt* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkUint*>(p),n);
}

YkStream& YkStream::Load( YkFloat* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkUint*>(p),n);
}

#ifdef Yk_LONG
YkStream& YkStream::Load( YkLong* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkDouble*>(p),n);
}

YkStream& YkStream::Load( YkUlong* p,YkSizeT n )
{
	return Load(reinterpret_cast<YkDouble*>(p),n);
}
#endif

YkStreamStatus YkStream::GetStatus() const
{
	return code;
}

YkBool YkStream::IsEOF() const
{
	return code!=YkStreamOK;
}

YkStreamDirection YkStream::GetDirection() const
{
	return dir;
}

YkUlong YkStream::GetPosition() const
{
	return pos;
}

YkUlong YkStream::GetLength()
{
	return m_lActualPos;
}

YkStream& YkStream::operator<<( const YkAchar& v )
{
	return *this << reinterpret_cast<const YkUchar&>(v);
}

YkStream& YkStream::operator<<( const YkShort& v )
{
	return *this << reinterpret_cast<const YkUshort&>(v);
}

YkStream& YkStream::operator<<( const YkInt& v )
{
	return *this << reinterpret_cast<const YkUint&>(v);
}

YkStream& YkStream::operator<<( const YkFloat& v )
{
	return *this << reinterpret_cast<const YkUint&>(v);
}

#ifdef Yk_LONG
YkStream& YkStream::operator<<( const YkLong& v )
{
	return *this << reinterpret_cast<const YkDouble&>(v);
}

YkStream& YkStream::operator<<( const YkUlong& v )
{
	return *this << reinterpret_cast<const YkDouble&>(v);
}
#endif

}


