/*
 *
 * YkFileStream.cpp
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

#include "Stream/YkFileStream.h"
#include "Stream/YkPlatform.h"
#include <sys/types.h>
#include <sys/stat.h>

namespace Yk {


YkFileStream::YkFileStream():YkStream()
{
	file=-1;
	SignLoadSave = YkSignStreamLoad;
}

YkSizeT YkFileStream::WriteBuffer(YkSizeT)
{
	YkInt m=0,n=0;
	YKASSERT(dir==YkStreamSave || dir == YkStreamLoadSave);
	YKASSERT(begptr<=rdptr);
	YKASSERT(rdptr<=wrptr);
	YKASSERT(wrptr<=endptr);
	if(code==YkStreamOK)
	{
		m=wrptr-rdptr;
		n=Ykwrite(file,rdptr,m);
		if(n<0)
		{
			code=YkStreamFull;
			return endptr-wrptr;
		}
		m-=n;
		if(m)
		{
			memmove(begptr,rdptr+n,m);
		}
		rdptr=begptr;
		wrptr=begptr+m;
		return endptr-wrptr;
    }
	return 0;
}

YkSizeT YkFileStream::ReadBuffer(YkSizeT)
{
	YkInt m=0,n=0;
	YKASSERT(dir==YkStreamLoad || dir == YkStreamLoadSave);
	YKASSERT(begptr<=rdptr);
	YKASSERT(rdptr<=wrptr);
	YKASSERT(wrptr<=endptr);
	if(code==YkStreamOK)
	{
		m=wrptr-rdptr;
		if(m)
		{
			memmove(begptr,rdptr,m);
		}
		rdptr=begptr;
		wrptr=begptr+m;
		n=Ykread(file,wrptr,endptr-wrptr);

		if(n<=0)
		{ 
			code=YkStreamEnd;
			return wrptr-rdptr;
		}
		wrptr+=n;
		return wrptr-rdptr;
    }
	return 0;
}

void YkFileStream::SetSignLoadSave(YkInt nSign)
{
	if (SignLoadSave != nSign)
	{
		if (SignLoadSave == YkSignStreamSave && nSign == YkSignStreamLoad)
		{//如果上次是Save操作，这次是Load的操作，则需要把Buffer中的数据保存在文件中
			WriteBuffer(0);
		}
		//这里需要设置当前操作的标志
		SignLoadSave = (YkSignLoadSave)nSign;
		//把wrptr和rdptr指针设置在开始的位置
		rdptr = wrptr = begptr;
	}
	//这种情况比较特殊，如果当打开一个文件后load数据，使的code == YkStreamEnd，
	//此时又要在此基础上添加数据，这时就需要这样设置，而不是直接返回
	if (code == YkStreamEnd && nSign == YkSignStreamSave) 
	{
		code = YkStreamOK;
		rdptr = wrptr = begptr;
	}
}

YkUlong YkFileStream::GetLength() 
{
	//得到当前的位置
	YkLong nCurrentPos = GetPosition();
	//把文件位置设置在末尾
	SetPosition(0, YkFromEnd);
	//得到末尾的位置，也就是文件的长度
	YkLong lLength = GetPosition();
	//把文件位置设置回去
	SetPosition(nCurrentPos, YkFromStart);
	return lLength;
}

YkBool YkFileStream::Open(const YkChar* filename,YkStreamDirection save_or_load,YkUint size){
	YKASSERT(save_or_load ==YkStreamSave || save_or_load==YkStreamLoad || save_or_load == YkStreamLoadSave);
	YKASSERT(filename != NULL);

	//如果给的是一个文件路径返回FALSE
	YkSizeT nLength = Ykstrlen(filename);
	if (nLength == 0 || ISPATHSEP(filename[nLength -1]))
	{
		return FALSE;
	}

	if(!dir)
	{
		if(save_or_load==YkStreamLoad)
		{
			file=Ykopen(filename,O_RDONLY|YkBINARY);
			if(file<0)
			{ 
				code=YkStreamNoRead; 
				return FALSE; 
			}
		}
		else if(save_or_load==YkStreamSave)
		{
			file=Ykopen(filename,O_RDWR|O_CREAT|O_TRUNC|YkBINARY,YkREADWRITE);
			if(file<0)
			{
				code=YkStreamNoWrite; 
				return FALSE; 
			}
		}
		else if(save_or_load == YkStreamLoadSave)
		{
			file=Ykopen(filename,O_RDWR|O_CREAT|YkBINARY,YkREADWRITE);
			if(file<0)
			{
				code=YkStreamNoWrite;
				return FALSE; 
			}
		}
		return YkStream::Open(save_or_load, size);
    }
	return FALSE;
}

YkBool YkFileStream::IsOpen() const
{
	return file != -1;
}

YkBool YkFileStream::Close()
{
	if(dir)
	{
		if(dir==YkStreamSave)
		{
			Flush();
		}

		if (dir == YkStreamLoadSave && SignLoadSave  == YkSignStreamSave) 
		{
			Flush();
		}
		Ykclose(file);
		file = -1;
		return YkStream::Close();
    }
	return FALSE;
}

YkBool YkFileStream::SetPosition(YkLong offset,YkWhence whence){
	YkLong p=0;
	YKASSERT(dir!=YkStreamDead);
	if(code==YkStreamOK || code == YkStreamEnd) 
	{
		YKASSERT(YkFromStart==SEEK_SET);
		YKASSERT(YkFromCurrent==SEEK_CUR);
		YKASSERT(YkFromEnd==SEEK_END);
		if(dir==YkStreamSave || dir == YkStreamLoadSave)
		{
			WriteBuffer(0);			
			if(whence==YkFromCurrent)
			{
				offset+=wrptr-rdptr;
			}
			if((p=Ykseek(file,offset,whence))<0)
			{ 
				code=YkStreamFull; 
				return FALSE; 
			}
			wrptr=begptr;
			rdptr=begptr;
		}
		else
		{			
			if(whence==YkFromCurrent)
			{
				offset-=wrptr-rdptr;
			}
			if((p=Ykseek(file,offset,whence))<0)
			{
				code=YkStreamEnd; 
				return FALSE; 
			}
			wrptr=begptr;
			rdptr=begptr;
		}
		pos=p;
		code=YkStreamOK;
		return TRUE;
    }
	return FALSE;
}

YkBool YkFileStream::IsEOF() const 
{ 
	if (code!=YkStreamOK) 
	{
		return TRUE;
	}
	return FALSE;
}

YkFileStream::~YkFileStream()
{
	Close();
}
YkBool YkFileStream::LoadByteArray(YkByteArray& byteArray)
{
	if( this->IsOpen() == FALSE )
	{
		return FALSE;
	}
	YkSizeT nLen = GetLength();
	byteArray.SetSize(nLen);
	byteArray.SetPosition(0);
	YkByte* pByte =(YkByte*)byteArray.GetData();
	Load(pByte,nLen);
	return TRUE;
}
YkBool YkFileStream::SaveByteArray(const YkByteArray& byteArray)
{
	if( IsOpen() == FALSE )
	{
		return FALSE;
	}
	YkSizeT nSize = byteArray.GetSize();
	YkByte* pByte =(YkByte*)byteArray.GetData();
	Save(pByte,nSize*sizeof(YkByte));
	return TRUE;
}

YkStream& YkFileStream::operator<<( const YkString& s )
{
	YkStream::operator <<(s);
	return *this;
}

YkStream& YkFileStream::operator>>( YkString& s )
{
	YkStream::operator >>(s);
	return *this;
}

}


