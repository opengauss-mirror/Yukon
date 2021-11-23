/*
 *
 * YkMarkup.h
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

#ifndef AFX_YKMARKUP_H__30D0E25C_A9F9_4FF8_BBB6_7C1324189289__INCLUDED_
#define AFX_YKMARKUP_H__30D0E25C_A9F9_4FF8_BBB6_7C1324189289__INCLUDED_

#pragma once

#include "Stream/YkDefines.h"
#include "Stream/YkPlatform.h"
#include "Toolkit/YkToolkit.h"

#define EOL _U("\r\n") 
#ifdef _UGUNICODE
#define EOLLEN Ykstrlen(EOL)
#else
#define EOLLEN (sizeof(EOL)-1); 
#endif

namespace Yk {

#define _DS(i) (i?&(m_strDoc.Cstr())[m_pos[i].nStartL]:0)
#define MARKUP_SETDEBUGSTATE m_pMainDS=_DS(m_nPos); m_pChildDS=_DS(m_nPosChild)

//XML文件头的格式。
#define XML_FILE_HEADER  _U("<?xml version=\"%VERSION\" encoding=\"%ENCODING\"?>")

class TOOLKIT_API YkMarkup
{
public:
	YkMarkup();

	YkMarkup( const YkChar* szDoc ); 

	YkMarkup( const YkMarkup& markup ) { *this = markup; };

	void operator=( const YkMarkup& markup );

	~YkMarkup();

	//加载的时候会将XML文件中的编码方式，转换到当前的默认编码方式
	YkBool Load( const YkChar* szFileName );

	YkBool SetDoc( const YkChar* szDoc );

	YkBool IsWellFormed();

	YkBool FindElem( const YkChar* szName=NULL );

	YkBool FindChildElem( const YkChar* szName=NULL );

	YkBool IntoElem();

	YkBool OutOfElem();

	void ResetChildPos() { SetPos(m_posParent,m_nPos,0); };

	void ResetMainPos() { SetPos(m_posParent,0,0); };

	void ResetPos() { SetPos(0,0,0); };

	YkString GetTagName() const;

	YkString GetChildTagName() const { return GetTagName(m_nPosChild); };

	YkString GetData() const { return GetData(m_nPos); };

	YkString GetChildData() const { return GetData(m_nPosChild); };

	YkString GetAttrib( const YkChar* szAttrib ) const { return GetAttrib(m_nPos,szAttrib); };

	YkString GetChildAttrib( const YkChar* szAttrib ) const { return GetAttrib(m_nPosChild,szAttrib); };

	YkString GetAttribName( YkInt n ) const;

	YkString GetChildAttribName( YkInt n ) const;

	YkBool SavePos( const YkChar* szPosName=_U("") );

	YkBool RestorePos( const YkChar* szPosName=_U("") );

	const YkString& GetError() const { return m_strError; };

	YkBool MoveNextElem();

	YkBool MoveNextChildElem();

	YkBool HasChild() const;

	YkInt GetAttribCount() const;

	YkInt GetChildAttribCount() const;

	enum MarkupNodeType
	{
		MNT_ELEMENT					= 1,  // 0x01

		MNT_TEXT					= 2,  // 0x02

		MNT_WHITESPACE				= 4,  // 0x04

		MNT_CDATA_SECTION			= 8,  // 0x08

		MNT_PROCESSING_INSTRUCTION	= 16, // 0x10

		MNT_COMMENT					= 32, // 0x20

		MNT_DOCUMENT_TYPE			= 64, // 0x40

		MNT_EXCLUDE_WHITESPACE		= 123// 0x7b
	};

	// Create
	//保存的时候会根据你设置的编码方式保存当前的XML的文件
	YkBool Save( const YkChar* szFileName );

	const YkString& GetDoc() const { return m_strDoc; };

	YkBool AddElem( const YkChar* szName, const YkChar* szData=NULL ) { return AddElem(szName,szData,false,false); };

	YkBool InsertElem( const YkChar* szName, const YkChar* szData=NULL ) { return AddElem(szName,szData,true,false); };

	YkBool AddChildElem( const YkChar* szName, const YkChar* szData=NULL ) { return AddElem(szName,szData,false,true); };

	YkBool InsertChildElem( const YkChar* szName, const YkChar* szData=NULL ) { return AddElem(szName,szData,true,true); };

	YkBool AddAttrib( const YkChar* szAttrib, const YkChar* szValue ) { return SetAttrib(m_nPos,szAttrib,szValue); };

	YkBool AddChildAttrib( const YkChar* szAttrib, const YkChar* szValue ) { return SetAttrib(m_nPosChild,szAttrib,szValue); };

	YkBool AddAttrib( const YkChar* szAttrib, YkInt nValue ) { return SetAttrib(m_nPos,szAttrib,nValue); };

	YkBool AddChildAttrib( const YkChar* szAttrib, YkInt nValue ) { return SetAttrib(m_nPosChild,szAttrib,nValue); };

	YkBool AddSubDoc( const YkChar* szSubDoc ) { return AddSubDoc(szSubDoc,false,false); };

	YkBool InsertSubDoc( const YkChar* szSubDoc ) { return AddSubDoc(szSubDoc,true,false); };

	YkString GetSubDoc() const { return GetSubDoc(m_nPos); };

	YkBool AddChildSubDoc( const YkChar* szSubDoc ) { return AddSubDoc(szSubDoc,false,true); };

	YkBool InsertChildSubDoc( const YkChar* szSubDoc ) { return AddSubDoc(szSubDoc,true,true); };

	YkString GetChildSubDoc() const { return GetSubDoc(m_nPosChild); };

	// Modify
	YkBool RemoveElem();

	YkBool RemoveChildElem();

	YkBool SetAttrib( const YkChar* szAttrib, const YkChar* szValue ) { return SetAttrib(m_nPos,szAttrib,szValue); };

	YkBool SetChildAttrib( const YkChar* szAttrib, const YkChar* szValue ) { return SetAttrib(m_nPosChild,szAttrib,szValue); };

	YkBool SetAttrib( const YkChar* szAttrib, YkInt nValue ) { return SetAttrib(m_nPos,szAttrib,nValue); };

	YkBool SetChildAttrib( const YkChar* szAttrib, YkInt nValue ) { return SetAttrib(m_nPosChild,szAttrib,nValue); };

	YkBool SetData( const YkChar* szData, YkInt nCDATA=0 ) { return SetData(m_nPos,szData,nCDATA); };

	YkBool SetRealData( const YkChar* szData ) { return SetRealData(m_nPos,szData); };

	YkBool SetChildData( const YkChar* szData, YkInt nCDATA=0 ) { return SetData(m_nPosChild,szData,nCDATA); };

	//! \brief 根据当前的编码方式获取的XML文件头
	YkString GetXMLFileHeader();
	//! \brief 获取当前XML的编码方式
	YkMBString::Charset GetEncoding();
	//! \brief 设置当前的编码方式，则XML保存的时候会将当前系统默认的编码方式保存到这个编码方式

	void SetEncoding(YkMBString::Charset nEncoding);


protected:
	const YkChar* m_pMainDS;
	const YkChar* m_pChildDS;

	YkMBString::Charset m_nEncoding;
	YkString m_strDoc;
	YkString m_strError;

	struct ElemPos
	{
		ElemPos() { Clear(); };
		ElemPos( const ElemPos& pos ) { *this = pos; };
		YkBool IsEmptyElement() const { return (nStartR == nEndL + 1); };
		void Clear() { memset( this, 0, sizeof(ElemPos) ); };
		void AdjustStart( YkInt n ) { nStartL+=n; nStartR+=n; };
		void AdjustEnd( YkInt n ) { nEndL+=n; nEndR+=n; };
		YkInt nStartL;
		YkInt nStartR;
		YkInt nEndL;
		YkInt nEndR;
		YkInt nReserved;
		YkInt iElemParent;
		YkInt iElemChild;
		YkInt iElemNext;
	};

	YkInt m_posParent;
	YkInt m_nPos;
	YkInt m_nPosChild;
	YkInt m_nPosFree;
	YkInt m_nNodeType;

	struct TokenPos
	{
		TokenPos( const YkChar* sz ) { Clear(); szDoc = sz; };
		void Clear() { nL=0; nR=-1; nNext=0; bIsString=false; };
		YkBool Match( const YkChar* szName )
		{
			YkInt nLen = nR - nL + 1;
		#ifdef MARKUP_IGNORECASE
			return ( (Ykstrnicmp( &szDoc[nL], szName, nLen ) == 0)
		#else
			return ( (Ykstrncmp( &szDoc[nL], szName, nLen ) == 0)
		#endif
				&& ( szName[nLen] == _U('\0') || Ykstrchr(_U(" =//["),szName[nLen]) ) );
		};
		YkInt nL;
		YkInt nR;
		YkInt nNext;
		const YkChar* szDoc;
		YkBool bIsString;
	};

	struct SavedPos
	{
		YkInt iPosParent;
		YkInt iPos;
		YkInt iPosChild;
	};
	typedef std::map<YkString,SavedPos> mapSavedPosT;
	mapSavedPosT m_mapSavedPos;

	void SetPos( YkInt iPosParent, YkInt iPos, YkInt iPosChild )
	{
		m_posParent = iPosParent;
		m_nPos = iPos;
		m_nPosChild = iPosChild;
		m_nNodeType = iPos?MNT_ELEMENT:0;
		MARKUP_SETDEBUGSTATE;
	};

	struct PosArray
	{
		PosArray() { Clear(); };

		~PosArray() { Release(); };

		enum { PA_SEGBITS = 16, PA_SEGMASK = 0xffff };

		void RemoveAll() { Release(); Clear(); };

		void Release() { 
			for (YkInt n=0;n<SegsUsed();++n) 
				delete[] (YkChar*)pSegs[n]; 
			if (pSegs) 
				delete[] (YkChar*)pSegs; };

		void Clear() { nSegs=0; nSize=0; pSegs=NULL; };

		YkInt GetSize() const { return nSize; };

		YkInt SegsUsed() const { return ((nSize-1)>>PA_SEGBITS) + 1; };

		ElemPos& operator[](YkInt n) const { return pSegs[n>>PA_SEGBITS][n&PA_SEGMASK]; };

		ElemPos** pSegs;

		YkInt nSize;

		YkInt nSegs;
	};
	PosArray m_pos;

	YkInt GetFreePos() { if (m_nPosFree==m_pos.GetSize()) AllocPosArray(); return m_nPosFree++; };

	YkInt ReleasePos() { --m_nPosFree; return 0; };

	YkBool AllocPosArray( YkInt nNewSize = 0 );

	YkBool ParseDoc();

	YkInt ParseElem( YkInt iPos );

	YkInt ParseError( const YkChar* szError, const YkChar* szName = NULL );

	static YkBool FindChar( const YkChar* szDoc, YkInt& nChar, YkChar c );

	static YkBool FindAny( const YkChar* szDoc, YkInt& nChar );

	static YkBool FindToken( TokenPos& token );

	YkString GetToken( const TokenPos& token ) const;

	YkInt FindElem( YkInt iPosParent, YkInt iPos, const YkChar* szPath );

	YkString GetTagName( YkInt iPos ) const;

	YkString GetData( YkInt iPos ) const;

	YkString GetAttrib( YkInt iPos, const YkChar* szAttrib ) const;

	YkBool AddElem( const YkChar* szName, const YkChar* szValue, YkBool bInsert, YkBool bAddChild );

	YkString GetSubDoc( YkInt iPos ) const;

	YkBool AddSubDoc( const YkChar* szSubDoc, YkBool bInsert, YkBool bAddChild );

	YkBool FindAttrib( TokenPos& token, const YkChar* szAttrib=NULL ) const;

	YkBool SetAttrib( YkInt iPos, const YkChar* szAttrib, const YkChar* szValue );

	YkBool SetAttrib( YkInt iPos, const YkChar* szAttrib, YkInt nValue );

	YkBool CreateNode( YkString& strNode, YkInt nNodeType, const YkChar* szText );

	void LocateNew( YkInt iPosParent, YkInt& iPosRel, YkInt& nOffset, YkInt nLength, YkInt nFlags );

	YkInt ParseNode( TokenPos& token );

	YkBool SetData( YkInt iPos, const YkChar* szData, YkInt nCDATA );

	YkBool SetRealData( YkInt iPos, const YkChar* szData );

	YkInt RemoveElem( YkInt iPos );

	void DocChange( YkInt nLeft, YkInt nReplace, const YkString& strInsert );

	void PosInsert( YkInt iPos, YkInt nInsertLength );

	void Adjust( YkInt iPos, YkInt nShift, YkBool bAfterPos = false );

	YkString TextToDoc( const YkChar* szText, YkBool bAttrib = false ) const;

	YkString TextFromDoc( YkInt nLeft, YkInt nRight ) const;

};

}

#endif
