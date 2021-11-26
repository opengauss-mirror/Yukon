/*
 *
 * YkNowin.cpp
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

// 在没有定义win32宏的情况下，才能使用本文件
#ifndef WIN32
#include <dirent.h> // for opendir(), readdir(), closedir() 
#include <sys/types.h> // for lstat
#include <sys/stat.h> // for lstat

#include "Stream/YkNowin.h"
#include "Stream/YkFile.h"
#include "Base/YkPlatform.h"

#include <sys/stat.h>
#include <sys/time.h>
#include <assert.h>

// #include <X11/Xlib.h>
// #include <X11/Xutil.h>
// #include <X11/cursorfont.h>

#include <pthread.h>
#include <semaphore.h>

#include <unistd.h> // for getwcd()
#include <dlfcn.h> // for dlopen() & dlsym()

#include <cstring>
#include <time.h>
#include <fstream>
#include "Stream/prefix.h"

namespace Yk {
    
// YkFile  {{========================================================================
// List all the files in directory
YkInt YkFile::ListFiles(YkArray<YkString>& filelist,const YkString& path,const YkString& pattern,YkUint flags) 
{
	YkUint matchmode= FILEMATCH_FILE_NAME | FILEMATCH_NOESCAPE;
	struct dirent *dp = NULL;	
	YkInt count = 0;
	YkString strPathName;
	YkString strName;
	DIR *pDir =NULL;
	struct stat statInf;

	memset(&statInf, 0, sizeof(statInf));
	
		
	// Folding case
	if(flags&LIST_CASEFOLD) matchmode|=FILEMATCH_CASEFOLD;
	
	// Get directory stream pointer
	YkInt nLength = YKstrlen(path.Cstr());
	YkAchar *pathachar = new YkAchar[nLength* 3 + 1];
	YkUnicodeString::WcharToAchar(path, pathachar, nLength * 3 + 1);
	pDir=opendir(pathachar);
	delete[] pathachar;
	if(pDir)
	{
		while((dp=readdir(pDir))!=NULL)
		{
			// 赋值strName
#ifdef _UGUNICODE
			YkInt nWcharLength = 3 * strlen(dp->d_name) + 1;
			YkWchar *pWchar = new YkWchar[nWcharLength];
			YkUnicodeString::AcharToWchar(dp->d_name, pWchar, nWcharLength);
			strName = pWchar;
#else
			strName=dp->d_name; 
#endif
			// 赋值strPathName
			strPathName = path;	  

			if(!ISPATHSEP(strPathName[strPathName.GetLength()-1]))
			{
				strPathName += PATHSEPSTRING;
			}

			strPathName += strName;      
			
			if( !S_ISDIR(statInf.st_mode) 
				&& ((flags&LIST_NO_FILES) 
				   || (strName[0]==_U('.') && !(flags&LIST_HIDDEN_FILES)) 
				   || (!(flags&LIST_ALL_FILES) && !Match(pattern,strName,matchmode))))
			{
				delete[] pWchar;
				pWchar = NULL;
				continue;
			}
			
			if(S_ISDIR(statInf.st_mode) 
				&& ((flags&LIST_NO_DIRS) 
					|| strName.GetLength()<=2
					|| (strName[0]==_U('.') && (strName[1]==0 || (strName[1]==_U('.') && strName[2]==0 && (flags&LIST_NO_PARENT)) || (strName[1]!=_U('.') && !(flags&LIST_HIDDEN_DIRS))))
					|| (!(flags&LIST_ALL_DIRS) && !Match(pattern,strName,matchmode)))) 
			{
				delete[] pWchar;
				pWchar = NULL;
				continue;
			}
			
			filelist.Add(strName);
			count++;
			delete[] pWchar;
			pWchar = NULL;
		}
		closedir(pDir);
    }
	return count;
}

YkBool YkFile::GetStatus(const YkString& strFileName, YkFileStatus& status)
{
	struct stat statFile;
	YkMBString strMB;
	UNICODE2MBSTRING(strFileName, strMB);
	if (lstat(strMB.Cstr(), &statFile) >= 0)
	{
		status.m_tmCreate = statFile.st_ctime;
		status.m_tmUpdate = statFile.st_mtime;
		status.m_tmAccess = statFile.st_atime;
		status.m_nSize	  = statFile.st_size;
		status.m_nMode    = statFile.st_mode;
		return true;
	}
	return false;
}

YkBool YkFile::GetStatus64(const YkString& strFileName, YkFileStatus& status)
{
	struct stat64 statFile;

	YkMBString strMB;
	UNICODE2MBSTRING(strFileName, strMB);

	if (lstat64(strMB.Cstr(), &statFile) >= 0)
	{
		status.m_tmCreate = statFile.st_ctime;
		status.m_tmUpdate = statFile.st_mtime;
		status.m_tmAccess = statFile.st_atime;
		status.m_nSize	  = statFile.st_size;
		status.m_nMode    = statFile.st_mode;
		return true;
	}

	return false;
}

//! \brief 创建文件夹
YkBool YkFile::MkDir(const YkString& strDir)
{
	YkMBString strMB;
	UNICODE2MBSTRING(strDir, strMB);	
	return mkdir(strMB.Cstr(), 0777) == 0;
}

YkBool YkFile::RmDir(const YkString& strDir)
{
	YkMBString strMB;
	UNICODE2MBSTRING(strDir, strMB);	
	return rmdir(strMB.Cstr()) == 0;
}

YkUint YkFile::GetTemporaryFileName(const YkString &strPathName, const YkString &strPrefixString, 
									   YkUint uUnique, YkString &strTempFileName)
{
	if(strPathName == _U("")) 
	{
		return 0;
	}

	YkString pathname(strPathName);
	pathname.TrimLeft();
	pathname.TrimRight();
	pathname.Replace(_U('\\'), _U('/'));
	pathname.TrimRight(_U('/'));
	pathname += _U('/');
	YkInt i = Ykaccess(pathname.Cstr(), F_OK);
	if(i==-1) 
	{ // directory not exist !
		return 0;
	}
	YkString prefix(strPrefixString);
	if(prefix.Find(_U('/'))!=-1 || prefix.Find(_U('\\'))!=-1) 
	{	// invalid prefix
		return 0;
	}
	prefix.TrimLeft();
	prefix.TrimRight();
	if(uUnique) 
	{
		char tmp[8 + 1];
		sprintf(tmp, "%x", uUnique);
		tmp[8] = 0;
		YkMBString strMB(tmp);
		strMB+=".tmp";
		YkString strU;
		MBSTRING2UNICODE(strMB, strU);
		strTempFileName += pathname;
		strTempFileName += prefix;
		strTempFileName += strU;
		
		return uUnique;
	}
	else 
	{
		srand((unsigned)time(NULL));
	x:	i = rand();
		char tmp[8 + 1];
		sprintf(tmp, "%x", i);
		tmp[8] = 0;
		YkMBString strMB(tmp);
		strMB+=".tmp";
		YkString strU;
		MBSTRING2UNICODE(strMB, strU);
		strTempFileName += pathname;
		strTempFileName += prefix;
		strTempFileName += strU;
		if(!Ykaccess(strTempFileName.Cstr(), F_OK)) 
		{
            strTempFileName = _U("");

			goto x;
		}
		return i;
	}	
}

YkString YkFile::GetTemporaryPath()
{
	if(!Ykaccess(_U("/tmp/"), F_OK)) 
	{			// In linux, return "/tmp" directly, not defined variable TMP & TEMP.
		return _U("/tmp/");
	}
	else 
	{		// if /tmp doesn't exists, return current work directory.
		YkAchar buf[PATH_MAX];
		if(!getcwd(buf, PATH_MAX)) 
		{
			return _U("");
		}
		else 
		{
			YkString strU;
			YkMBString strMB(buf);
			MBSTRING2UNICODE(strMB, strU);
			return strU;
		}
	}
}

YkBool YkFile::InnerIsExist(const YkString& strFileName)
{
	YkBool bReturn = false;
	bReturn = (-1 != Ykaccess(strFileName.Cstr(), F_OK)); 		// file doesn't exist.
	return bReturn;
}

YkBool YkFile::Copy(const YkString& strExistedFileName, const YkString& strNewFileName, YkBool bFailIfExists)
{
	YkBool bReturn = false;
	//解决Linux带中文路径的文件拷贝失败问题
	YkInt nLengthSrc = YKstrlen(strExistedFileName.Cstr());
	YkAchar *src = new YkAchar[nLengthSrc * 3 + 1];
	YkUnicodeString::WcharToAchar(strExistedFileName, src, nLengthSrc * 3 + 1);
	YkInt nLengthDes = YKstrlen(strNewFileName.Cstr());
	YkAchar *dst = new YkAchar[nLengthDes * 3 + 1];
	YkUnicodeString::WcharToAchar(strNewFileName, dst, nLengthDes * 3 + 1);

	if(!(dst&&src))
	{
		delete[]  src;
		delete[]  dst;
		return FALSE;
	}
	if(access(src, F_OK))
	{
		delete[]  src;
		delete[]  dst;
		return FALSE;
	}
		
	if(bFailIfExists)
	{
		YkBool bExisted = FALSE;
		if(!access(dst, F_OK))
		{
			bExisted = TRUE;
		}
		if(bExisted)
		{
			bReturn = TRUE;
		}
		else
		{
			std::ifstream in_file(src, std::ios::in|std::ios::binary);
			if(!in_file)
			{
				delete[]  src;
				delete[]  dst;
				return FALSE;
			}
			std::ofstream out_file(dst, std::ios::out|std::ios::binary);
			if(!out_file)
			{
				delete[]  src;
				delete[]  dst;
				return FALSE;
			}
			out_file<<in_file.rdbuf();
			bReturn = TRUE;
		}
	}
	else 
	{
		std::ifstream in_file(src, std::ios::in|std::ios::binary);
		if(!in_file)
		{
			delete[]  src;
			delete[]  dst;
			return FALSE;
		}
		std::ofstream out_file(dst, std::ios::out|std::ios::binary);
		if(!out_file)
		{
			delete[]  src;
			delete[]  dst;
			return FALSE;
		}
		out_file<<in_file.rdbuf();
		bReturn = TRUE;
	}
	delete[]  src;
	delete[]  dst;
	return bReturn;
}

YkUint  YkFile::getAttribute(const YkString & strFileName)
{
	return 0;
}

YkString YkFile::GetCurrentPath()
{
	YkAchar strBuffer[LENGTH_MAX_PATH];
	
	if (!getcwd(strBuffer, LENGTH_MAX_PATH))
	{
		return _U("");
	}
	else
	{
#ifdef _UGUNICODE
		YkString strPath;
		strPath.FromMBString(strBuffer, LENGTH_MAX_PATH);
		return strPath;
#else
		return YkString(strBuffer, LENGTH_MAX_PATH);
#endif	
	}
}

// YkFile  }}========================================================================

}

#ifdef _UGUNICODE
YkInt Yk_open(const YkChar* pFileName, YkInt nOpenFlag, YkInt nPermissionMode)
{
	YkInt nLength = YKstrlen(pFileName);
	YkAchar *pFileNameAchar = new YkAchar[nLength * 3 + 1];
	YkUnicodeString::WcharToAchar(pFileName, pFileNameAchar, nLength * 3 + 1);
	YkInt nResult = open(pFileNameAchar, nOpenFlag, nPermissionMode);
	delete []pFileNameAchar;
	return nResult;
}

EXTERN_C STREAM_API YkInt Yk_access( const YkChar *path, YkInt mode )
{
	YkInt nLength = YKstrlen(path);
	YkAchar *pPathAchar = new YkAchar[nLength * 3 + 1];
	YkUnicodeString::WcharToAchar(path, pPathAchar, nLength * 3 + 1);
	YkInt nResult = access(pPathAchar, mode);
	delete []pPathAchar;
	return nResult;
}

EXTERN_C STREAM_API YkInt Yk_remove( const YkChar *path )
{
	YkInt nLength = YKstrlen(path);
	YkAchar *pPathAchar = new YkAchar[nLength * 3 + 1];
	YkUnicodeString::WcharToAchar(path, pPathAchar, nLength * 3 + 1);
	YkInt nResult = remove(pPathAchar);
	delete []pPathAchar;
	return nResult;
}

EXTERN_C STREAM_API YkInt Yk_rename( const YkChar *oldname, const YkChar *newname )
{
	YkInt nOldLength = YKstrlen(oldname);
	YkAchar *pOldAchar = new YkAchar[nOldLength * 3 + 1];
	YkUnicodeString::WcharToAchar(oldname, pOldAchar, nOldLength * 3 + 1);
	YkInt nNewLength = YKstrlen(newname);
	YkAchar *pNewAchar = new YkAchar[nNewLength * 3 + 1];
	YkUnicodeString::WcharToAchar(newname, pNewAchar, nNewLength * 3 + 1);
	YkInt nResult = rename(pOldAchar, pNewAchar);
	delete []pOldAchar;
	delete []pNewAchar;
	return nResult;
}

EXTERN_C STREAM_API FILE* Yk_fopen( const YkChar *path, const YkChar *mode )
{
	YkInt nPathLength = YKstrlen(path);
	YkAchar *pPathAchar = new YkAchar[nPathLength * 3 + 1];
	YkUnicodeString::WcharToAchar(path, pPathAchar, nPathLength * 3 + 1);
	YkInt nModeLength = YKstrlen(mode);
	YkAchar *pModeAchar = new YkAchar[nModeLength * 3 + 1];
	YkUnicodeString::WcharToAchar(mode, pModeAchar, nModeLength * 3 + 1);
	FILE *pFile = fopen(pPathAchar, pModeAchar);
	delete []pModeAchar;
	delete []pPathAchar;
	return pFile;
}

EXTERN_C STREAM_API FILE* Yk_fopen64( const YkChar *path, const YkChar *mode )
{
        YkInt nPathLength = YKstrlen(path);
        YkAchar *pPathAchar = new YkAchar[nPathLength * 3 + 1];
        YkUnicodeString::WcharToAchar(path, pPathAchar, nPathLength * 3 + 1);
        YkInt nModeLength = YKstrlen(mode);
        YkAchar *pModeAchar = new YkAchar[nModeLength * 3 + 1];
        YkUnicodeString::WcharToAchar(mode, pModeAchar, nModeLength * 3 + 1);

		FILE *pFile = fopen64(pPathAchar, pModeAchar);

        delete []pModeAchar;
        delete []pPathAchar;
        return pFile;
}

#endif
#endif
