/*
 *
 * YkFile.cpp
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

#include "Stream/YkFile.h"
#include "Stream/YkFileStream.h"
#include "Stream/YkPlatform.h"
#include "Toolkit/YkToolkit.h"
#if !defined(WIN32)
#include "Stream/YkNowin.h"
#endif

#if defined(_WIN32) || defined(WIN32)
#include <Windows.h>
#include <WinBase.h>
#else
#include <sys/types.h>
#include <unistd.h> 
#endif


namespace Yk {

// If folding case, make lower case
#define FOLD(c)          ((flags&FILEMATCH_CASEFOLD)?tolower(c):(c))

// Perform match
	
	
// 得到主程序所在的路径(不等于当前路径)
YkString YkFile::GetAppPath()
{
	return YkSystem::GetAppPath();
}

// 得到模块所在的路径(如果传入参数为NULL，就是主程序所在的路径)
YkString YkFile::GetModulePath(const YkString& strModultTitle)
{
	return YkSystem::GetModulePath(strModultTitle);
}

YkInt YkFile::DoMatch(const YkChar *pattern,const YkChar *str,YkUint flags)
{	
	const YkChar *p=pattern;
	const YkChar *q=str;
	const YkChar *s=NULL;
	YkChar c,cs,ce,cc,neg;
	int level=0;
	while((c=*p++)!=_U('\0'))
	{
		switch(c)
		{
		case _U('?'):
			if(*q==_U('\0'))
			{
				return 0;
			}
			if((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(*q))
			{
				return 0;
			}
			if((flags&FILEMATCH_PERIOD) && (*q==_U('.')) && ((q==str) || ((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(*(q-1)))))
			{
				return 0;
			}
			q++;
			break;
		case _U('*'):
			c=*p;
			while(c==_U('*'))
			{
				c=*++p;
			}
			if((flags&FILEMATCH_PERIOD) && (*q==_U('.')) && ((q==str) || ((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(*(q-1)))))
			{
				return 0;
			}
			if(c==_U('\0'))
			{    // Optimize for case of trailing '*'
				if(flags&FILEMATCH_FILE_NAME)
				{ 
					for(s=q; *s; s++)
					{ 
						if(ISPATHSEP(*s)) 
						{
							return 0;
						}
					} 
				}
				return 1;
			}
			while(!DoMatch(p,q,flags&~FILEMATCH_PERIOD))
			{
				if((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(*q))
				{
					return 0;
				}
				if(*q++==_U('\0'))
				{
					return 0;
				}
			}
			return 1;
		case _U('['):
			if(*q==_U('\0'))
			{
				return 0;
			}
			if((flags&FILEMATCH_PERIOD) && (*q==_U('.')) && ((q==str) || ((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(*(q-1)))))
			{
				return 0;
			}
			cc=FOLD(*q);
			if((neg=((*p==_U('!'))||(*p==_U('^')))), neg)
			{
				p++;
			}
			c=*p++;
			do
			{
				if(c==_U('\\') && !(flags&FILEMATCH_NOESCAPE))
				{
					c=*p++;
				}
				cs=ce=FOLD(c);
				if(c==_U('\0'))
				{
					return 0;
				}
				c=*p++;
				c=FOLD(c);
				if((flags&FILEMATCH_FILE_NAME) && ISPATHSEP(c))
				{
					return 0;
				}
				if(c==_U('-') && *p!=_U(']'))
				{
					ce = *p++;
					if(ce==_U('\\') && !(flags&FILEMATCH_NOESCAPE))
					{
						c=*p++;
					}
					if(c==_U('\0'))
					{
						return 0;
					}
					ce=FOLD(c);
					c=*p++;
				}
#if defined(_UGUNICODE)
				if(((YkUshort)cs)<=((YkUshort)cc) && ((YkUshort)cc)<=((YkUshort)ce))		
#else
				if(((YkByte)cs)<=((YkByte)cc) && ((YkByte)cc)<=((YkByte)ce))
#endif
				{
					goto match;
				}
			} while(c!=_U(']'));
			if(!neg)
			{
				return 0;
			}
			q++;
			break;
match:  while(c!=_U(']'))
		{
			if(c==_U('\0'))
			{
				return 0;
			}
			c=*p++;
			if(c==_U('\\') && !(flags&FILEMATCH_NOESCAPE))
			{
				p++;
			}
		}
        if(neg)
		{
			return 0;
		}
        q++;
        break;
		case _U('('):
nxt:    if(DoMatch(p,q,flags)) 
		{
			return 1;
		}
        for(level=0; *p && 0<=level; )
		{
			switch(*p++)
			{
            case _U('\\'):
				if(*p)
				{ 
					p++; 
				}
				break;
            case _U('('): 
				level++; 
				break;
            case _U(')'): 
				level--; 
				break;
            case _U('|'):
            case _U(','): 
				if (level==0) 
				{
					goto nxt;
				}
            }
		}
        return 0;
		case _U(')'):
			break;
		case _U('|'):
		case _U(','):
			for(level=0; *p && 0<=level; )
			{
				switch(*p++)
				{
				case _U('\\'): 
					if(*p)
					{
						p++; 
					}
					break;
				case _U('('): 
					level++; 
					break;
				case _U(')'): 
					level--; 
					break;
				}
			}
			break;
		case _U('\\'):
			if(*p && !(flags&FILEMATCH_NOESCAPE))
			{
				c=*p++;   // Trailing escape represents itself
			}
		default:
			if(FOLD(c)!=FOLD(*q))
			{
				return 0;
			}
			q++;
			break;
		}
    }
	return (*q==_U('\0')) || (ISPATHSEP(*q) && (flags&FILEMATCH_LEADING_DIR));
}


// Public API to matcher
YkInt YkFile::ugfilematch(const YkChar *pattern,const YkChar *str,YkUint flags)
{
	const YkChar *p=pattern;
	const YkChar *q=str;
	int level=0;
	if(p && q)
	{
nxt:	if(DoMatch(p,q,flags)) 
		{
			return 1;
		}
		for(level=0; *p && 0<=level; )
		{
			switch(*p++)
			{
			case _U('\\'): 
				if(*p)
				{
					p++; 
				}
				break;
			case _U('('): 
				level++; 
				break;
			case _U(')'): 
				level--; 
				break;
			case _U('|'):
			case _U(','): 
				if (level==0)
				{
					goto nxt;
				}
			}
		}
    }
	return 0;
}

YkBool YkFile::MkDirEx(const YkString& strDir)
{
#ifndef WIN32
	YkBool bAbsolutePath = FALSE; // [12/8/2009 林文玉]  判断linux下是否是绝对路径
	if (!strDir.IsEmpty() && strDir[0] == _U('/'))
	{
		bAbsolutePath = TRUE;
	}
#endif

	YkString strTempDir = strDir;
	strTempDir = PreDealPath(strTempDir, false);	

	YkStrings dirs;
	strTempDir.Split(dirs, _U("/"));	
	strTempDir.Empty(); 

#ifndef WIN32
	if(bAbsolutePath && dirs.GetSize()>=1 )
	{
		dirs.SetAt(0, _U("/")+dirs[0]);
	}
#endif

	YkBool bResult = true;	
	YkInt i = 0;
	for(i = 0; i < (YkInt)dirs.GetSize(); i++)
	{
		strTempDir += dirs.ElementAt(i);
		strTempDir += _U("/");
		bResult = MkDir(strTempDir);
	}	
	return bResult;
}

YkBool YkFile::RmDirEx(YkString strDir)
{
	//在最后加上路径分隔符
	if(!ISPATHSEP(strDir[strDir.GetLength() - 1]))
	{
#ifdef WIN32
		strDir += _U('\\');
#else
		strDir += _U('/');
#endif
	}
	YkArray<YkString> files;
	ListFiles(files ,strDir);
	YkInt iFile;
	YkString strTmpFile;
	YkUshort usMode;
	//删除文件夹里的所有文件和子文件夹
	for(iFile = 0 ;iFile != (YkInt)files.GetSize() ;++iFile)
	{
		strTmpFile = strDir + files[iFile];
		usMode = getMode(strTmpFile);
		if(usMode & YkDIRECTORY)//文件夹
		{
			if(!RmDirEx(strTmpFile))
			{
				return FALSE;
			}
		}
		else//文件
		{
			Delete(strTmpFile);
		}
	}
	//删除文件夹本身
	return RmDir(strDir);
}

YkBool YkFile::CopyDirEx(YkString strExistedDir, YkString strNewDir, YkBool bFailIfExists/* = FALSE*/)
{
	//在最后加上路径分隔符
	if(!ISPATHSEP(strExistedDir[strExistedDir.GetLength() - 1]))
	{
#ifdef WIN32
		strExistedDir += _U('\\');
#else
		strExistedDir += _U('/');
#endif
	}
	if (!IsExist(strExistedDir))
	{
		return FALSE;
	}

	if(!ISPATHSEP(strNewDir[strNewDir.GetLength() - 1]))
	{
#ifdef WIN32
		strNewDir += _U('\\');
#else
		strNewDir += _U('/');
#endif
	}
	if (!IsExist(strNewDir))
	{
		MkDirEx(strNewDir);
	}

	YkArray<YkString> files;
	ListFiles(files, strExistedDir);

	//拷贝文件夹里的所有文件和子文件夹
	for(YkUint iFile = 0; iFile != (YkUint)files.GetSize(); iFile++)
	{
		YkString strExistedFile = strExistedDir + files[iFile];
		YkString strNewFile = strNewDir + files[iFile];
		YkUshort usMode = getMode(strExistedFile);
		if(usMode & YkDIRECTORY)//文件夹
		{
			if (!IsExist(strNewFile))
			{
				MkDirEx(strNewFile);
			}
			if(!CopyDirEx(strExistedFile, strNewFile, bFailIfExists))
			{
				return FALSE;
			}
		}
		else//文件
		{
			Copy(strExistedFile, strNewFile, bFailIfExists);
		}
	}
	return TRUE;
}

YkString YkFile::GetDir(const YkString& file)
{
	return YkSystem::GetDir(file);
}

YkString YkFile::GetName(const YkString& file)
{
	YkInt f=0,n=0;
	if(!file.IsEmpty())
	{
		n=0;
#ifdef WIN32
		if(isalpha((YkUchar)file[0]) && file[1]==_U(':'))
		{
			n=2;
		}
#endif
		f=n;
		int nLen = file.GetLength();
		while(n < nLen)
		{
			if(ISPATHSEP(file[n]))
			{
				f=n+1;
			}
			n++;
		}
		return YkString(file.Cstr()+f,n-f);
    }
	return _U("");
}

YkString YkFile::GetTitle(const YkString& file)
{
	YkInt f=0,e=0,b=0,i=0;
	if(!file.IsEmpty())
	{
		i=0;
#ifdef WIN32
		if(isalpha((YkUchar)file[0]) && file[1]==_U(':'))
		{
			i=2;
		}
#endif
		f=i;
		int nLen = file.GetLength();
		while(i < nLen)
		{
			if(ISPATHSEP(file[i]))
			{
				f=i+1;
			}
			i++;
		}
		b=f;

		if (b >= nLen)
		{
			return _U("");
		}

		if(file[b]==_U('.'))
		{
			b++;     // Leading _U(".")
		}
		e=i;
		while(b<i)
		{
			if(file[--i]==_U('.'))
			{ 
				e=i; 
				break; 
			}
		}
		return YkString(file.Cstr()+f,e-f);
    }
	return _U("");
}


YkBool YkFile::IsFileNameValid(const YkString& strFileName)
{
	if (strFileName.Find(_U("/")) != -1
		|| strFileName.Find(_U("*")) != -1
		|| strFileName.Find(_U("?")) != -1
		|| strFileName.Find(_U("<")) != -1
		|| strFileName.Find(_U(">")) != -1
		|| strFileName.Find(_U("\"")) != -1
		|| strFileName.Find(_U("|")) != -1
		|| strFileName.Find(_U("\\")) != -1
		|| strFileName.Find(_U("\\\\")) != -1)
	{
		return FALSE;
	}

	return TRUE;

}

YkString YkFile::GetExt(const YkString& file)
{
	return YkSystem::GetExtName(file);
}

YkBool YkFile::FindFileExtPaths(const YkString& strPath, const YkString& strExtName, 
								  YkArray<YkString>& strFileNames, YkBool bAddPath)
{
	return YkSystem::FindFileExtPaths(strPath,strExtName,strFileNames,bAddPath);
}
YkString YkFile::StripExt(const YkString& file)
{
	YkInt f=0,e=0,n=0;
	if(!file.IsEmpty())
	{
		n=0;
#ifdef WIN32
		if(isalpha((YkUchar)file[0]) && file[1]==_U(':'))
		{
			n=2;
		}
#endif
		f=n;
		int nLen = file.GetLength();
		while(n < nLen)
		{
			if(ISPATHSEP(file[n]))
			{
				f=n+1;
			}
			n++;
		}
		if(file[f]==_U('.'))
		{
			f++;     // Leading '.'
		}
		e=n;
		while(f<n)
		{
			if(file[--n]==_U('.'))
			{ 
				e=n; 
				break; 
			}
		}
		return YkString(file.Cstr(),e);
    }
	return _U("");
}

void YkFile::SplitPath(const YkString& strPathFile, YkString& strDir, YkString& strTitle, YkString& strExt)
{
	strDir = YkFile::GetDir(strPathFile);
	strTitle = YkFile::GetTitle(strPathFile);
	strExt = YkFile::GetExt(strPathFile);
	return ;
}

YkString YkFile::ChangeExt(const YkString &strFileName, const YkString &strNewExt)
{
	YkString strDir, strTitle, strExt;
	SplitPath(strFileName, strDir, strTitle, strExt);
	strExt = strNewExt;
	strExt.TrimLeft(_U('.'));
	strExt = _U(".") + strExt;
	YkString strRet = strDir + strTitle + strExt;
	return strRet;
}

//! \brief 得到单元测试数据所在的目录
//! \param strUnitTestName 单元测试工程的文件夹名
//! \return 返回单元测试数据所在的目录
YkString YkFile::GetUnitTestDataPath(const YkString& strUnitTestName)
{
	YkString strBinPath = GetModulePath(_U("SuBase"));
	YkString strRefPath = _U("../../../../02_TestCode/03_YkC/01_UnitTest/TestUnit/Data/") + strUnitTestName + _U("/");
	YkString strDataPath = GetAbsolutePath(strBinPath, strRefPath);
	return strDataPath;
}

YkString YkFile::PreDealPath(YkString strPath, YkBool bIsFile)
{
	// make both of strings end with '/'
	strPath.TrimLeft();
	strPath.TrimRight();
	strPath.Replace(_U('\\'), _U('/'));

	if(strPath.GetLength()>=2)
	{
		if(strPath.Left(2).Compare(_U("//"))==0)
		{
			strPath.SetAt(0, _U('\\'));
			strPath.SetAt(1, _U('\\'));
		}		
		strPath.Replace(_U("//"), _U("/"));
	}
	if (!bIsFile)
	{
		strPath.TrimRight(_U('/'));
		strPath += _U("/");
	}
	return strPath;
}
YkString YkFile::GetAbsolutePath(const YkString &strSrcPath, const YkString &strFile)
{
	YkString strSourcePath = strSrcPath, strRefPath = strFile;
	
	if(strSourcePath == _U("")) 
	{
		if( (strRefPath.Left(2) == _U("./")) || (strRefPath.Left(2) == _U(".\\")) )
		{
			return strRefPath.Mid(2);
		}
		if( (strRefPath.Left(3) == _U("../")) || (strRefPath.Left(3) == _U("..\\")) )
		{
			return YkString(_U(""));
		}
	}
	
	if(strRefPath == _U(""))
	{
		return strSourcePath;
	}
	// for network. haven't decided.
	if(strRefPath.Left(2)==_U("\\\\")) {
		return strRefPath;
	}
	strSourcePath = PreDealPath(strSourcePath, FALSE);
	strRefPath = PreDealPath(strRefPath, TRUE);

	// if strRefPath like this : ./abc
	if(strRefPath.Left(2)==_U("./")) {
		strRefPath = strRefPath.Mid(2);
		
		return strSourcePath + strRefPath;
	}
	// if strRefPath like this: c:/a.sdb or c:/a/b/
	else if (strRefPath.GetLength()>=2 && strRefPath[1]==_U(':'))
	{
		return strRefPath;
	}
	// if strRefPath like this : abc/def/
	else if(strRefPath[0]!='/' && strRefPath.Left(3)!=_U("../")) 
	{
		return strSourcePath + strRefPath;
	}
	// if strRefPath like this : /abc;
	else if(strRefPath[0]==_U('/')) 
	{
		return strRefPath;
	}
	// if strRefPath like this : ../../abc/def
	else if(strRefPath.Left(3)==_U("../")) 
	{
		YkInt nIndex=0;
		strSourcePath.SetLength(strSourcePath.GetLength()-1);
		do {
			nIndex = strSourcePath.ReverseFind(_U('/'));
			strSourcePath = strSourcePath.Left(nIndex);
			strRefPath = strRefPath.Mid(3);
		}while(strRefPath.Left(3)==_U("../"));
		return strSourcePath + _U("/") + strRefPath;
	}

	return strSrcPath;
}

YkString YkFile::GetRelativePath(YkString strSourcePath, YkString strAbsPath)
{
	if(strSourcePath.IsEmpty())
	{
		return strAbsPath;
	}

	strSourcePath = PreDealPath(strSourcePath, FALSE);
	strAbsPath = PreDealPath(strAbsPath, FALSE);
	//如果src是网络地址，Abs是本地的绝对地址，则直接返回本地地址
	if (strSourcePath.Left(2).CompareNoCase(_U("\\\\"))==0 && strAbsPath.Find(_U(":/"))==1 )
	{
		return strAbsPath;
	}
	
#ifdef WIN32
	YkStringArray strArraySrcPathTmp;
	strArraySrcPathTmp.SetSize(2);
	YkStringArray strArrayAbsPathTmp;
	strArrayAbsPathTmp.SetSize(2);

	YkInt nFlagSrc = strSourcePath.Find(_U(":"));
	YkInt nFlagAbs = strAbsPath.Find(_U(":"));
	if(nFlagSrc != -1 && nFlagAbs != -1)
	{
		strSourcePath.Split(strArraySrcPathTmp, _U(":"));
		strAbsPath.Split(strArrayAbsPathTmp, _U(":"));
		strArrayAbsPathTmp[0].MakeLower();
		strArraySrcPathTmp[0].MakeLower();

		if (strArrayAbsPathTmp[0] == strArraySrcPathTmp[0])
		{		
			strAbsPath = strArrayAbsPathTmp[1];
			strSourcePath = strArraySrcPathTmp[1];
		}
	}
#endif
	// src = /a/b/c/, abs = /a/b/c/,     => relative=./
	if(strAbsPath==strSourcePath) 
	{
		return _U("./");
	}
	// src = /a/b/c/, abs = /a/b/c/d/e/, => relative=./d/e/
	else if(strAbsPath.Find(strSourcePath.Cstr())!=-1) 
	{
		YkInt lenSrc = strSourcePath.GetLength();
		if (strAbsPath.Left(2)==_U("\\\\") && (strSourcePath.GetLength()<2 || strSourcePath.Left(2)!=_U("\\\\")) )
		{
			return strAbsPath;
		}
		else
		{
			return _U("./") + strAbsPath.Mid(lenSrc);
		}
	}
	// src = /a/b/c/d/e/, abs = /a/b/c/, => relative=../../
	else if(strSourcePath.Find(strAbsPath.Cstr())!=-1)
	{
		YkInt lenAbs = strAbsPath.GetLength();
		YkString strRest = strSourcePath.Mid(lenAbs);
		strRest.TrimLeft(_U('\\'));

		YkInt i = strRest.Remove(_U('/'));	// count '/' numbers.
		strRest=_U("");
		while(i--) 
		{
			strRest += _U("../");
		}
		return strRest;
	}
	// src = /a/b/c/d/e/, abs = /f/g/, => relative=../../../../../f/g/
	else 
	{
		if (strAbsPath.GetLength()>=2 && strAbsPath.Left(2)==_U("\\\\"))
		{
			return strAbsPath;
		}
		else
		{		
			YkInt nIndex, nSrcLength = strSourcePath.GetLength();
			for(nIndex = 0; nIndex < nSrcLength; ++nIndex)
			{
				if(strSourcePath[nIndex] != strAbsPath[nIndex]) 
				{			
					break;
				}
			}
			YkString strSamePart = strSourcePath.Left(nIndex);
			YkInt nPos = strSamePart.ReverseFind(_U('/'));

			if(nPos >= 0)
			{
				YkString srcRest = strSourcePath.Mid(nPos+1);
				YkString absRest = strAbsPath.Mid(nPos+1);

				YkInt nCount = srcRest.Remove(_U('/'));
				srcRest = _U("");
				while(nCount--)
				{
					srcRest += _U("../");
				}
				return srcRest + absRest;
			}
			else
			{
				return strAbsPath;
			}
		}
	}
}

YkBool YkFile::Match(const YkString& pattern,const YkString& file,YkUint flags)
{
	return ugfilematch(pattern.Cstr(),file.Cstr(),flags);
}

YkBool YkFile::IsExist(const YkString& strFileDir)
{
	int nLen = strFileDir.GetLength();
	if(nLen > 0)
	{
		YkChar chLast = strFileDir.GetAt(nLen-1);
		// 如果最后一个字符是 目录分隔符, 则表明要进行 目录是否存在的判断, 要加一个".",然后再进行判断
		if(ISPATHSEP(chLast))
		{
			return InnerIsExist(strFileDir+_U("."));
		}
	}
	return InnerIsExist(strFileDir);
}

//! \brief 得到文件创建时间
YkTime YkFile::GetCreateTime(const YkString& strFileName)
{
	YkFileStatus status;
	GetStatus(strFileName, status);
	return status.m_tmCreate;
}

//! \brief 得到文件访问时间
YkTime YkFile::GetAccessTime(const YkString& strFileName)
{
	YkFileStatus status;
	GetStatus(strFileName, status);
	return status.m_tmAccess;
}

//! \brief 得到文件最后更新时间
YkTime YkFile::GetUpdateTime(const YkString& strFileName)
{
	YkFileStatus status;
	GetStatus(strFileName, status);
	return status.m_tmUpdate;
}

YkUshort YkFile::getMode(const YkString & strFileName)
{
	YkFileStatus status;
	GetStatus(strFileName, status);
	return status.m_nMode;
}

YkBool YkFile::Delete(const YkString& strFileName)
{
	return Ykremove(strFileName.Cstr()) == 0;
}

YkBool YkFile::Rename(const YkString& strOldName, const YkString& strNewName)
{
	return (Ykrename(strOldName.Cstr(), strNewName.Cstr()) == 0); // ==0 表示成功
}

YkBool YkFile::JudgeSamePath(const YkString& strPath1, const YkString& strPath2)
{
	YkString strTempPath1 = PreDealPath(strPath1,FALSE);
	YkString strTempPath2 = PreDealPath(strPath2,FALSE);
	return strTempPath1.CompareNoCase(strTempPath2)==0;
}

YkBool YkFile::FileTruncate(void * pFileHandle, YkUlong nPositon)
{
#if defined(_WIN32) || defined(WIN32)
	BOOL rc;
	long upperBits = (nPositon>>32) & 0x7fffffff;
	long lowerBits = nPositon & 0xffffffff;

	HANDLE FileHandle = (HANDLE)_get_osfhandle((YkInt)pFileHandle);
	rc = SetFilePointer((HANDLE)FileHandle, lowerBits, &upperBits, FILE_BEGIN);
	if( INVALID_SET_FILE_POINTER != rc )
	{
		/* SetEndOfFile will fail if nByte is negative */
		if( SetEndOfFile((HANDLE)FileHandle))
		{
			return TRUE;
		}
	}
	return FALSE;
#else
	YkBool rc;
	rc = ftruncate((YkSizeT)pFileHandle, (off_t)nPositon);
	if( rc )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
#endif
}

YkBool YkFile::JudgeSameFile(const YkString& strName1, const YkString& strName2)
{
	YkString strdir=_U("");
	YkString strNewName1 = strName1;
	YkString strNewName2 = strName2;
    
	if (strNewName1.Find(_U("\\")) )
	{
	   strNewName1.Replace(_U("\\"),_U("/"));	    
	}
	if (strName2.Find(_U("\\")))
	{
		strNewName2.Replace(_U("\\"),_U("/"));
	}
	strdir = YkFile::GetAppPath();
    strNewName1 =YkFile::GetAbsolutePath(strdir,strNewName1);
	strNewName2 =YkFile::GetAbsolutePath(strdir,strNewName2);
	return strNewName1.CompareNoCase(strNewName2) == 0;
}

Yk::YkFile::YkFileType YkFile::GetFileType( const YkString& strFile )
{
	YkFile::YkFileType nFileType = YkFile::Yk_FILE_UNKNOWN;
	
	YkFileStream filestream;
	if (!filestream.Open(strFile.Cstr(), YkStreamLoad))
	{
		return YkFile::Yk_FILE_UNKNOWN;
	}
	
	// 测试文件规则:读出文件的前16字节，看看他们的文件头标记
	const YkUint nMarkLen = 16;
	YkAchar byteMarker[nMarkLen] = {0};
	filestream.Load(byteMarker, nMarkLen);
	
	if (byteMarker[0] == 'P' && 
		byteMarker[1] == 'K' &&
		byteMarker[2] == 3   &&
		byteMarker[3] == 4   )
	{
		nFileType = YkFile::Yk_FILE_ZIP;
	}
	
	filestream.Close();

	return nFileType;
}

YkBool YkFile::IsReadOnly(YkString strFile)
{
#if defined (__linux__)
		return FALSE;
#else
#if _UNICODE
	return _waccess(strFile.Cstr(), 2) == -1;
#else
	return access(strFile.Cstr(), 2) == -1;
#endif
#endif
}

YkBool YkFile::changeMode(YkString strFile,YkFileMod mod)
{
#if defined (__linux__)
	return FALSE;
#else
#if _UNICODE
	return _waccess(strFile.Cstr(), 2) == -1;
#else
	return access(strFile.Cstr(), 2) == -1;
#endif
#endif
}
}
