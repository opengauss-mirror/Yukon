/*
 *
 * YkFileStdio.cpp
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

#include "Stream/YkFileStdio.h"
#include "Stream/YkPlatform.h"

namespace Yk {

#ifdef WIN32
const YkAchar m_strEnterAdapt[] = "\r\n";
#else
const YkAchar m_strEnterAdapt[] = "\n";
#endif
const YkAchar m_strEnterWindows[] = "\r\n";
const YkAchar m_strEnterUnix[] = "\n";

#if defined(_UGUNICODE)
#ifdef WIN32
const YkChar m_wstrEnterAdapt[] = _U("\r\n");
#else
const YkChar m_wstrEnterAdapt[] = _U("\n");
#endif
const YkChar m_wstrEnterWindows[] = _U("\r\n");
const YkChar m_wstrEnterUnix[] = _U("\n");
#endif

YkFileStdio::YkFileStdio()
{
	//默认值是根据当前的操作系统
	m_eEnterType = Enter_Adapt;
#ifdef _UGUNICODE
	m_eCharset = YkMBString::Default;
	m_nStartPos = 0;
#endif

}

YkFileStdio::~YkFileStdio()
{

}


YkBool YkFileStdio::Open(const YkChar* filename,YkStreamDirection save_or_load,YkUint size,YkString::Charset charset )
{
#ifdef _UGUNICODE
	YkBool bOpen = m_FileStream.Open(filename, save_or_load, size);
	if(bOpen && m_FileStream.GetLength()>2)
	{
		YkByte BOM[3];
		m_FileStream>>BOM[0];
		m_FileStream>>BOM[1];
		m_FileStream>>BOM[2];
		if(BOM[0]==0xFF && BOM[1]==0xFE)
		{
			m_eCharset = YkMBString::UCS2LE;
			m_FileStream.SetPosition(2);
			m_nStartPos = 2;
		}
		else if(BOM[1]==0xFF && BOM[0]==0xFE)
		{
			m_eCharset = YkMBString::UCS2BE;
			m_FileStream.SetPosition(2);
			m_nStartPos = 2;
		}
		else if(BOM[0]==0xEF && BOM[1]==0xBB && BOM[2] ==0xBF)
		{
			m_eCharset = YkMBString::UTF8;
			m_nStartPos = 3;
		}
		else
		{
			m_eCharset = charset;
			m_nStartPos = 0;
			m_FileStream.SetPosition(0);
		}
	}
	else if(bOpen)
	{
			m_eCharset = charset;
			m_nStartPos = 0;
	}

	if((m_eCharset == YkMBString::UCS2LE && YK_ISBIGENDIAN) ||
		(m_eCharset == YkMBString::UCS2BE && !YK_ISBIGENDIAN))
	{
		m_FileStream.SetSwapBytes(TRUE);
	}
	else
	{
		m_FileStream.SetSwapBytes(FALSE);
	}
	return bOpen;

#else
	return m_FileStream.Open(filename, save_or_load, size);
#endif
	
}

YkBool YkFileStdio::IsEOF() const
{
	return m_FileStream.IsEOF();
}


void YkFileStdio::SetCurEnterType(EnterType eEnterType)
{
	m_eEnterType =eEnterType;
}

YkUlong YkFileStdio::GetPosition() const
{
#ifdef _UGUNICODE
	return m_FileStream.GetPosition() - m_nStartPos;
#else
	return m_FileStream.GetPosition();
#endif
	
}


YkBool YkFileStdio::SetPosition(YkLong offset,YkWhence whence)
{
#ifdef _UGUNICODE
	return m_FileStream.SetPosition(offset+m_nStartPos, whence);
#else
	return m_FileStream.SetPosition(offset, whence);
#endif
	
}


YkLong YkFileStdio::GetLength() 
{
#ifdef _UGUNICODE
	return m_FileStream.GetLength() - m_nStartPos;
#else
	return m_FileStream.GetLength();
#endif
	
}
YkInt YkFileStdio::ReadLine(YkMBString& strSource)
{
	strSource.Empty();			 //设置为空
	strSource.SetCapacity(1024); //设置缓存的大小
	YkInt nIndex = 0;
	YkAchar ch1, ch2;
	while (!m_FileStream.IsEOF())
	{
		m_FileStream >> ch1;
		if (!m_FileStream.IsEOF())
		{

			if (ch1 == '\r')
			{
				//继续读取下一个字符，如果是\n，则不需要重新设置文件的位置
				//否则就需要重新设置文件的位置，使文件退回到原来的位置
				m_FileStream >> ch2;
				if (ch2 != '\n')
				{
					m_FileStream.SetPosition(-1, YkFromCurrent);
				}
				break;
			}
			else if (ch1 == '\n')
			{
				break;
			}
			else
			{	
				strSource.Insert(nIndex++, ch1);
			}
		}
		else
		{
			// 注：为能够把Linux上的文件在Windows上也能正确的读取出一行 故意思就是凡是windows上存在'\r'、'\n'都认为是换行的标志
			if(ch1 != '\n' && ch1 != '\r')
			{
				strSource.Insert(nIndex++, ch1);
			}
		}
	}
#ifdef _UGUNICODE
	strSource.SetCharset(m_eCharset);
#endif
	return strSource.GetLength();
}
#ifdef _UGUNICODE
//现在换行符有三种：1、\r; 2、\r\n; 3、\n.
YkInt YkFileStdio::ReadLine(YkString& strSourceIn)
{
	strSourceIn.Empty();			 //设置为空
	strSourceIn.SetCapacity(1024); //设置缓存的大小
	if(m_eCharset == YkMBString::UCS2LE || m_eCharset == YkMBString::UCS2BE)
	{
		YkInt nIndex = 0;
		YkChar ch1, ch2;
		while (!m_FileStream.IsEOF())
		{
#ifdef _UGUNICODE
			m_FileStream >> (YkUshort&)ch1;
#else
			m_FileStream >> ch1;
#endif
			if (!m_FileStream.IsEOF())
			{

				if (ch1 == _U('\r'))
				{
					//继续读取下一个字符，如果是\n，则不需要重新设置文件的位置
					//否则就需要重新设置文件的位置，使文件退回到原来的位置
#ifdef _UGUNICODE
					m_FileStream >> (YkUshort&)ch2;
#else
					m_FileStream >> ch2;
#endif			
					
					if (ch2 != _U('\n'))
					{
						m_FileStream.SetPosition(-1*(YkInt)sizeof(YkChar), YkFromCurrent);
					}
					break;
				}
				else if (ch1 == _U('\n'))
				{
					break;
				}
				else
				{	
					strSourceIn.Insert(nIndex++, ch1);
				}
			}
			else
			{
				// 注：为能够把Linux上的文件在Windows上也能正确的读取出一行 故意思就是凡是windows上存在'\r'、'\n'都认为是换行的标志
				if(ch1 != _U('\n') && ch1 != _U('\r'))
				{
					strSourceIn.Insert(nIndex++, ch1);
				}
			}
		}
		return strSourceIn.GetLength();
	}
	else
	{
		YkMBString strSource;
		ReadLine(strSource);
		if(m_eCharset == YkMBString::UTF8)
		{
				strSourceIn.FromUTF8(strSource.Cstr(), strSource.GetLength());
		}
		else
		{
				strSourceIn.FromMBString(strSource.Cstr(), strSource.GetLength(),m_eCharset);
		}
	
		return strSourceIn.GetLength();
	}

}

#endif


YkBool YkFileStdio::WriteLine(const YkString& strSource)
{
#ifdef _UGUNICODE
	YkMBString strMB;
	if(m_eCharset!= YkMBString::UTF8 && m_eCharset != YkMBString::UCS2BE && m_eCharset != YkMBString::UCS2LE)
	{
		strMB.SetCharset(m_eCharset);
		strSource.ToMBString(strMB);
		return WriteLine(strMB.Cstr(), strMB.GetLength());
	}
	else if (m_eCharset == YkMBString::UTF8)
	{
		strSource.ToUTF8(strMB);
		return WriteLine(strMB.Cstr(), strMB.GetLength());
	}
	else
	{
#ifdef _UGUNICODE
		m_FileStream.Save((YkUshort*)strSource.Cstr(), strSource.GetLength());
		if (m_eEnterType == Enter_Adapt) 
		{
			m_FileStream.Save((YkUshort*)m_wstrEnterAdapt, Ykstrlen(m_wstrEnterAdapt));
		}
		else if (m_eEnterType == Enter_Windows)
		{
			m_FileStream.Save((YkUshort*)m_wstrEnterWindows, Ykstrlen(m_wstrEnterWindows));		
		}
		else if (m_eEnterType == Enter_Unix)
		{
			m_FileStream.Save((YkUshort*)m_wstrEnterUnix, Ykstrlen(m_wstrEnterUnix));		
		}
		else
		{
			return FALSE;
		}
#else
		m_FileStream.Save(strSource.Cstr(), strSource.GetLength());
		if (m_eEnterType == Enter_Adapt) 
		{
			m_FileStream.Save(m_wstrEnterAdapt, Ykstrlen(m_wstrEnterAdapt));
		}
		else if (m_eEnterType == Enter_Windows)
		{
			m_FileStream.Save(m_wstrEnterWindows, Ykstrlen(m_wstrEnterWindows));		
		}
		else if (m_eEnterType == Enter_Unix)
		{
			m_FileStream.Save(m_wstrEnterUnix, Ykstrlen(m_wstrEnterUnix));		
		}
		else
		{
			return FALSE;
		}
#endif
		
		return TRUE;

	}
#else
	return WriteLine(strSource.Cstr(),strSource.GetLength());
#endif
}
YkBool YkFileStdio::WriteLine(const YkAchar *pSrc,YkInt nLength)
{
	m_FileStream.Save((YkByte*)pSrc, nLength);

	if (m_eEnterType == Enter_Adapt) 
	{
		m_FileStream.Save((YkByte*)m_strEnterAdapt, strlen(m_strEnterAdapt));
	}
	else if (m_eEnterType == Enter_Windows)
	{
		m_FileStream.Save((YkByte*)m_strEnterWindows, strlen(m_strEnterWindows));		
	}
	else if (m_eEnterType == Enter_Unix)
	{
		m_FileStream.Save((YkByte*)m_strEnterUnix, strlen(m_strEnterUnix));		
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void YkFileStdio::Write(const YkAchar *pSrc,YkInt nLength)
{
	m_FileStream.Save(pSrc,nLength);
}

void YkFileStdio::Write(const YkString& strSource)
{
#ifdef _UGUNICODE
	YkMBString strMB;
	if(m_eCharset == YkMBString::Default)
	{
		strSource.ToMBString(strMB); 
		m_FileStream.Save((YkByte*)strMB.Cstr(), strMB.GetLength());
	}
	else if (m_eCharset == YkMBString::UTF8)
	{
		strSource.ToUTF8(strMB); 
		m_FileStream.Save((YkByte*)strMB.Cstr(), strMB.GetLength());
	}
	else
	{
		m_FileStream.Save((YkUshort*)strSource.Cstr(), strSource.GetLength());
	}

#else
	m_FileStream.Save(strSource.Cstr(),strSource.GetLength());
#endif
	
}


YkBool YkFileStdio::Flush()
{
	return m_FileStream.Flush();
}

YkBool YkFileStdio::Close()
{
	return m_FileStream.Close();
}

}
