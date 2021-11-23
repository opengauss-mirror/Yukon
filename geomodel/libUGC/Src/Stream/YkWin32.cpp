/*
 *
 * YkWin32.cpp
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

// 在定义win32宏的情况下，才能使用本文件
#ifdef WIN32 

#include "Stream/YkDefines.h"
#include "Stream/YkFile.h"
#include "Stream/YkWin32.h"

#include <Windows.h>
#include <assert.h>
#include <process.h>
#include <direct.h>

#define MAX_PRO_STRING_LENGTH 10240

namespace Yk {

// YkFile {{=========================================================================
// List all the files in directory
YkInt YkFile::ListFiles(YkArray<YkString>& filelist,const YkString& path,const YkString& pattern,YkUint flags) 
{	
	// Init
	YkInt count = 0;
	YkUint matchmode = FILEMATCH_FILE_NAME|FILEMATCH_NOESCAPE;
	YkString strPathName;
	YkString strName;			

	if(flags&LIST_CASEFOLD)
	{
		matchmode|=FILEMATCH_CASEFOLD;
	}
	
	// 复制文件夹名称
	strPathName=path;
	if(!ISPATHSEP(strPathName[strPathName.GetLength()-1])) 
	{
		strPathName+=PATHSEPSTRING;
	}
	strPathName+=_U("*");
	
	YkLong lIsFindFile=-1;	
	struct Yk_finddata_t c_file;
	// 打开文件夹
	lIsFindFile=Yk_findfirst(strPathName.Cstr(),&c_file);
	if(lIsFindFile!=-1)
	{
		// Loop over directory entries
		do
		{
			// 获取名称
			strName=c_file.name;

			if(!(c_file.attrib&_A_SUBDIR) 
				&& (   (flags&LIST_NO_FILES) 
					|| ((c_file.attrib&_A_HIDDEN) && !(flags&LIST_HIDDEN_FILES)) 
					|| (!(flags&LIST_ALL_FILES) && !Match(pattern,strName,matchmode))))
			{
				continue;
			}
			
			// Filter out directories; even more tricky!
			if((c_file.attrib&_A_SUBDIR) 
				&& (	(flags&LIST_NO_DIRS) 
					|| ((c_file.attrib&_A_HIDDEN) && !(flags&LIST_HIDDEN_DIRS)) 
					|| (strName==_U(".") || strName==_U(".."))
					|| (strName[0]==_U('.') && (strName[1]==0 || (strName[1]==_U('.') && strName[2]==0 && (flags&LIST_NO_PARENT)))) 
					|| (!(flags&LIST_ALL_DIRS) && !Match(pattern,strName,matchmode))))
			{
				continue;
			}
			
			filelist.Add(strName);
			count++;
		}while(Yk_findnext(lIsFindFile,&c_file) == 0);
		
		Yk_findclose(lIsFindFile);
    }
	return count;
}

YkBool YkFile::GetStatus(const YkString& strFileName, YkFileStatus& status)
{
	struct _stat statFile;
	if (Yk_stat(strFileName.Cstr(), &statFile) == 0)
	{
		status.m_tmCreate = statFile.st_ctime;
		status.m_tmUpdate = statFile.st_mtime;
		status.m_tmAccess = statFile.st_atime;
		status.m_nSize = statFile.st_size;
		status.m_nMode = statFile.st_mode;
		return true;
	}
	return false;
}

YkBool YkFile::GetStatus64(const YkString& strFileName, YkFileStatus& status)
{
	struct _stati64 statFile;
	if (Yk_stati64(strFileName.Cstr(), &statFile) == 0)
	{
		status.m_tmCreate = statFile.st_ctime;
		status.m_tmUpdate = statFile.st_mtime;
		status.m_tmAccess = statFile.st_atime;
		status.m_nSize = statFile.st_size;
		status.m_nMode = statFile.st_mode;
		return true;
	}
	return false;
}

//! \brief 创建文件夹
YkBool YkFile::MkDir(const YkString& strDir)
{
	return Yk_mkdir(strDir.Cstr()) == 0;
}

YkBool YkFile::RmDir(const YkString& strDir)
{
	return Yk_rmdir(strDir.Cstr()) == 0;
}

YkUint YkFile::GetTemporaryFileName(const YkString &strPathName, const YkString &strPrefixString, 
									   YkUint uUnique, YkString &strTempFileName)
{
	YkChar cpBuffer[MAX_PRO_STRING_LENGTH];
	YkUint nResult = ::GetTempFileName(strPathName.Cstr(), strPrefixString.Cstr(), uUnique, cpBuffer);
	strTempFileName = YkString(cpBuffer);
	return nResult;
}

YkString YkFile::GetTemporaryPath()
{
	YkChar cpBuffer[MAX_PRO_STRING_LENGTH];
	YkChar cpLongBuffer[MAX_PRO_STRING_LENGTH];
	::GetTempPath(MAX_PRO_STRING_LENGTH, cpBuffer);

	::GetLongPathName(cpBuffer,cpLongBuffer,MAX_PRO_STRING_LENGTH);

	YkString strResult(cpLongBuffer);
	return strResult;
}

YkBool YkFile::InnerIsExist(const YkString& strFileName)
{
	YkString strDrive(strFileName);
	strDrive.TrimRight(_U("./\\"));
	if(strDrive.GetLength() == 2 && strDrive.GetAt(1) == ':' && 
		isalpha((YkUchar)strDrive.GetAt(0)) )
	{
		YkInt nDisLength = GetLogicalDriveStrings(0,NULL);
		YkChar* pDstr = new YkChar[nDisLength];
		GetLogicalDriveStrings(nDisLength, (LPTSTR)pDstr);
		YkUchar nDrive = strDrive.GetAt(0);
		for(YkInt i=0; i<nDisLength;i++)
		{
			if((YkUchar)pDstr[i] == nDrive)
			{
				delete[] pDstr;
				pDstr = NULL;
				return TRUE;
			}
		}
		delete[] pDstr;
		pDstr = NULL;
	}

	YkBool bReturn = false;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile(strFileName, &FindFileData);
	if(hFind != INVALID_HANDLE_VALUE)
	{ 
		bReturn = true;
		FindClose(hFind);
	}
	
	return bReturn;
}

YkBool YkFile::Copy(const YkString& strExistedFileName, const YkString& strNewFileName, YkBool bFailIfExists)
{
	YkBool bReturn = false;
	bReturn = CopyFile(strExistedFileName, strNewFileName, bFailIfExists);
	return bReturn;
}

YkUint  YkFile::getAttribute(const YkString & strFileName)
{
	struct Yk_finddata_t c_file;
    YkLong hFile;
    if( (hFile = Yk_findfirst(strFileName.Cstr(), &c_file )) == -1L )
		return 0;
	Yk_findclose( hFile );
	return c_file.attrib;
}

YkString YkFile::GetCurrentPath()
{
	YkChar strBuffer[MAX_PATH];
	Yk_getcwd(strBuffer,MAX_PATH);
	return YkString(strBuffer,MAX_PATH);
}

// YkFile }}===================================================================================

}

#endif
