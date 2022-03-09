/*
 *
 * YkMarkup.cpp
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

#include "Toolkit/YkMarkup.h"
#include "Stream/YkFileStdio.h"

namespace Yk {

#include <stdio.h>

using namespace std; 

#define ATTRIBQUOTE _U("\"") 

YkMarkup::YkMarkup()
{ 
	m_pos.pSegs = NULL;
	m_pos.nSegs = 0;
	m_nEncoding = YkToolkit::GetCurCharset();
	SetDoc( NULL ); 
}

YkMarkup::YkMarkup( const YkChar* szDoc )
{ 
	m_pos.pSegs = NULL;
	m_pos.nSegs = 0;

#ifdef _UGUNICODE
	m_nEncoding = YkString::UTF8;
#else
	m_nEncoding = YkToolkit::GetCurCharset();
#endif
	SetDoc( szDoc ); 
}

YkMarkup::~YkMarkup()
{
}

YkBool YkMarkup::FindChildElem( const YkChar* szName )
{
	if ( ! m_nPos )
		FindElem();
	
	YkInt iPosChild = FindElem( m_nPos, m_nPosChild, szName );
	if ( iPosChild )
	{
		YkInt nPos = m_pos[iPosChild].iElemParent;
		SetPos( m_pos[nPos].iElemParent, nPos, iPosChild );
		return true;
	}
	
	return false;
}

YkBool YkMarkup::FindElem( const YkChar* szName )
{
	if ( m_pos.GetSize() )
	{
		YkInt nPos = FindElem( m_posParent, m_nPos, szName );
		if ( nPos )
		{
			SetPos( m_pos[nPos].iElemParent, nPos, 0 );
			return true;
		}
	}
	return false;
}

YkString YkMarkup::GetAttribName( YkInt n ) const
{
	YkString strReturn;

	TokenPos token( m_strDoc.Cstr() );
	if ( m_nPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_pos[m_nPos].nStartL + 1;
	else
		return strReturn;
	
	for ( YkInt nAttrib=0; nAttrib<=n; ++nAttrib )
		if ( ! FindAttrib(token) )
			return strReturn;
		
		strReturn =  GetToken( token );
		return strReturn;
}

YkString YkMarkup::GetTagName() const
{
	YkString strTagName;
	strTagName.SetCapacity(16);
	
	if ( m_nPos )
		strTagName = GetTagName( m_nPos );
	return strTagName;
}

YkBool YkMarkup::IntoElem()
{
	if ( m_nPos && m_nNodeType == MNT_ELEMENT )
	{
		SetPos( m_nPos, m_nPosChild, 0 );
		return true;
	}
	return false;
}

YkBool YkMarkup::IsWellFormed()
{
	if ( m_pos.GetSize() && m_pos[0].iElemChild )
		return true;
	return false;
}

YkBool YkMarkup::Load( const YkChar* szFileName )
{
	FILE* fp = (FILE*)Ykfopen( szFileName, _U("rb") );
	if ( ! fp )
	{
#if defined WIN32 && !defined _UGUNICODE
		m_strError = strerror(errno);
#else
		m_strError = _U("YkMarkup Can not open the xml file. ")+m_strError;
#endif

		YkString strError;
		strError.Format(_U("Open file %s Error: %s"), szFileName, m_strError.Cstr());
		return false;
	}
	
	fseek( fp, 0, SEEK_END );
	YkInt nFileByteLen = ftell(fp);
	fseek( fp, 0, SEEK_SET );

	YkChar szResult[100];	

	YkByte* pszBuffer = new YkByte[nFileByteLen];
	fread( pszBuffer, nFileByteLen, 1, fp );
	fclose( fp );

#ifdef _UGUNICODE
	YkMBString strSwap;
	strSwap.Append((YkAchar*)pszBuffer, nFileByteLen);
	Yksprintf( szResult, 100, _U("%d bytes"), nFileByteLen );
	m_strError = szResult;

	YkMBString Encoding="encoding=\"";
	YkInt nBegin=strSwap.Find(Encoding.Cstr(),0);
	YkInt bEnd =strSwap.Find("?>",0);
	if(nBegin > 0)
	{
		nBegin =nBegin +Encoding.GetLength();
		YkMBString strEncoding =strSwap.Mid(nBegin,bEnd-2);
		strEncoding.TrimRight().TrimLeft();
		m_nEncoding = YkTextCodec::GetCharset(strEncoding);
	}
	else
	{
		if( YkToolkit::IsUTF8(strSwap) )
		{
			m_nEncoding = YkMBString::UTF8;
		}
		else
		{
			m_nEncoding =YkToolkit::GetCurCharset();
		}
	}

	if(m_nEncoding != m_strDoc.GetCharset())
	{
		YkTextCodec textcodes((YkMBString::Charset)m_strDoc.GetCharset(),m_nEncoding);
		YkInt nDestSize = 0;
		m_strDoc.SetCapacity(nFileByteLen * 3);
		if (textcodes.Convert((YkByte*)m_strDoc.Cstr(), m_strDoc.GetCapacity() * 2, nDestSize, pszBuffer,nFileByteLen))
		{
			m_strDoc.SetLength(nDestSize/2);
		}
	}
	else
	{
		m_strDoc = (YkWchar *)pszBuffer;
	}
	delete [] pszBuffer;
	pszBuffer=NULL;
#else
	m_strDoc.Empty();
	m_strDoc.Append( (YkAchar*)pszBuffer, nFileByteLen );
	delete [] pszBuffer;
	pszBuffer=NULL;
	Yksprintf( szResult, 100, _U("%d bytes"), nFileByteLen );
	m_strError = szResult;

    YkString Encoding=_U("encoding=\"");
	YkInt nBegin=m_strDoc.Find(Encoding.Cstr(),0);
	YkInt bEnd =m_strDoc.Find(_U("?>"),0);
	if(nBegin > 0)
	{
		nBegin =nBegin +Encoding.GetLength();
		YkString strEncoding =m_strDoc.Mid(nBegin,bEnd-2);
		strEncoding.TrimRight().TrimLeft();
		m_nEncoding = YkTextCodec::GetCharset(strEncoding);
	}
	else
	{
		m_nEncoding =YkToolkit::GetCurCharset();
	}

	YkString strTarget;

	if(m_nEncoding != YkToolkit::GetCurCharset())
	{
		YkTextCodec textcodes(YkToolkit::GetCurCharset(),m_nEncoding);

		if (textcodes.Convert(strTarget,YkString(m_strDoc)))
		{
			m_strDoc.Empty();
			m_strDoc=strTarget.Cstr();
		}
	}
#endif
	return ParseDoc();
}

void YkMarkup::operator=( const YkMarkup& markup )
{
	m_nEncoding = markup.m_nEncoding;
	m_posParent = markup.m_posParent;
	m_nPos = markup.m_nPos;
	m_nPosChild = markup.m_nPosChild;
	m_nPosFree = markup.m_nPosFree;
	m_nNodeType = markup.m_nNodeType;
	
	m_pos.RemoveAll();
	m_pos.nSize = m_nPosFree;
	m_pos.nSegs = m_pos.SegsUsed();
	if ( m_pos.nSegs )
	{
		m_pos.pSegs = (ElemPos**)(new YkByte[m_pos.nSegs*sizeof(YkChar*)]);
		memset(m_pos.pSegs,0,m_pos.nSegs*sizeof(YkChar*));
		YkInt nSegSize = 1 << m_pos.PA_SEGBITS;
		for ( YkInt nSeg=0; nSeg < m_pos.nSegs; ++nSeg )
		{
			if ( nSeg + 1 == m_pos.nSegs )
				nSegSize = m_pos.GetSize() - (nSeg << m_pos.PA_SEGBITS);
			m_pos.pSegs[nSeg] = (ElemPos*)(new YkByte[nSegSize*sizeof(ElemPos)]);
			memset(m_pos.pSegs[nSeg],0,nSegSize*sizeof(ElemPos));
			memcpy( m_pos.pSegs[nSeg], markup.m_pos.pSegs[nSeg], nSegSize*sizeof(ElemPos) );
		}
	}
	
	m_strDoc = markup.m_strDoc;
	MARKUP_SETDEBUGSTATE;
}

YkBool YkMarkup::OutOfElem()
{
	if ( m_posParent )
	{
		SetPos( m_pos[m_posParent].iElemParent, m_posParent, m_nPos );
		return true;
	}
	return false;
}

YkBool YkMarkup::RemoveChildElem()
{
	if ( m_nPosChild )
	{
		YkInt iPosChild = RemoveElem( m_nPosChild );
		SetPos( m_posParent, m_nPos, iPosChild );
		return true;
	}
	return false;
}

YkBool YkMarkup::RemoveElem()
{
	if ( m_nPos && m_nNodeType == MNT_ELEMENT )
	{
		YkInt nPos = RemoveElem( m_nPos );
		SetPos( m_posParent, nPos, 0 );
		return true;
	}
	return false;
}

YkBool YkMarkup::RestorePos( const YkChar* szPosName )
{
	if ( szPosName )
	{
		YkString strPosName = szPosName;
		mapSavedPosT::const_iterator iterSavePos = m_mapSavedPos.find( strPosName );
		if ( iterSavePos != m_mapSavedPos.end() )
		{
			SavedPos savedpos = (*iterSavePos).second;
			SetPos( savedpos.iPosParent, savedpos.iPos, savedpos.iPosChild );
			return true;
		}
	}
	return false;
}

YkBool YkMarkup::Save( const YkChar* szFileName )
{
	YkInt nBeginHeader;
	YkInt nEndHeader;
	
	nBeginHeader = m_strDoc.Find(_U("<?"),0);
	nEndHeader = m_strDoc.Find(_U("?>"),0);
	if(nEndHeader>0)
	{
		if(GetEncoding()!= YkToolkit::GetCurCharset())
		{			
			m_strDoc.Replace(nBeginHeader,nEndHeader+2-nBeginHeader,GetXMLFileHeader().Cstr());
		}
	}
	else
	{
		YkString strTemp = GetXMLFileHeader()+EOLSTRING;
		m_strDoc =YkString(strTemp.Cstr()+m_strDoc);
	}
#ifdef _UGUNICODE
	YkMBString strTarget;
	if(GetEncoding()!= (YkMBString::Charset)m_strDoc.GetCharset())
	{
	    strTarget.Empty();
		strTarget.SetCharset(GetEncoding());
		m_strDoc.ToMBString(strTarget);
	}
#else
    YkString strTarget(m_strDoc);
	if(GetEncoding()!= YkToolkit::GetCurCharset())
	{
		strTarget = "";
		YkTextCodec textcode(GetEncoding(),YkToolkit::GetCurCharset());
		textcode.Convert(strTarget,YkString(m_strDoc));
	}
#endif
	
	YkBool bSuccess = false;
	FILE* fp = (FILE*)Ykfopen( szFileName, _U("wb") );
	if ( ! fp )
	{
		m_strError = _U("Cannot open file .\n");
		return false;
	}
	
	YkInt nLength = (YkInt)strTarget.GetLength();
	if ( ! nLength )
	{
		Ykfclose(fp);
		return true;
	}
	
	YkChar szResult[100];
	
	
	bSuccess = ( Ykfwrite( strTarget.Cstr(), nLength, 1, fp ) == 1 );
	Yksprintf( szResult, 100, _U("%d bytes"), nLength );
	m_strError = szResult;
	
	if ( ! bSuccess )
		m_strError = _U("Cannot write file .\n");
	Ykfclose(fp);
	return bSuccess;
}

YkBool YkMarkup::SavePos( const YkChar* szPosName )
{
	if ( szPosName )
	{
		SavedPos savedpos;
		savedpos.iPosParent = m_posParent;
		savedpos.iPos = m_nPos;
		savedpos.iPosChild = m_nPosChild;
		YkString strPosName = szPosName;
		m_mapSavedPos[strPosName] = savedpos;
		return true;
	}
	return false;
}

YkBool YkMarkup::SetDoc( const YkChar* szDoc )
{
	if ( szDoc )
		m_strDoc = szDoc;
	else
		m_strDoc.Empty();
	
	m_strError.Empty();
	return ParseDoc();
}

YkBool YkMarkup::AddElem( const YkChar* szName, const YkChar* szValue, YkBool bInsert, YkBool bAddChild )
{
	if ( bAddChild )
	{
		if ( ! m_nPos )
			return false;
	}
	else if ( m_posParent == 0 )
	{
		if ( IsWellFormed() )
			return false;


		m_pos[0].nEndL = (YkInt)m_strDoc.GetLength();
	}

	YkInt iPosParent, iPosBefore, nOffset = 0, nLength = 0;
	if ( bAddChild )
	{
		iPosParent = m_nPos;
		iPosBefore = m_nPosChild;
	}
	else
	{
		iPosParent = m_posParent;
		iPosBefore = m_nPos;
	}
	YkInt nFlags = bInsert?1:0;
	LocateNew( iPosParent, iPosBefore, nOffset, nLength, nFlags );
	YkBool isEmptyParent = m_pos[iPosParent].IsEmptyElement();
	if ( isEmptyParent || m_pos[iPosParent].nStartR + 1 == m_pos[iPosParent].nEndL )
		nOffset += EOLLEN;

	YkInt nPos = GetFreePos();
	m_pos[nPos].nStartL = nOffset;

	m_pos[nPos].iElemParent = iPosParent;
	m_pos[nPos].iElemChild = 0;
	m_pos[nPos].iElemNext = 0;
	if ( iPosBefore )
	{
		m_pos[nPos].iElemNext = m_pos[iPosBefore].iElemNext;
		m_pos[iPosBefore].iElemNext = nPos;
	}
	else
	{
		m_pos[nPos].iElemNext = m_pos[iPosParent].iElemChild;
		m_pos[iPosParent].iElemChild = nPos;
	}

	YkString strInsert;
	YkInt nLenName = (YkInt)Ykstrlen(szName);
	YkInt nLenValue = szValue? (YkInt)Ykstrlen(szValue) : 0;
	if ( ! nLenValue )
	{
		strInsert = _U("<");
		strInsert += szName;
		strInsert += _U("/>");
		strInsert +=  EOL;
		m_pos[nPos].nStartR = m_pos[nPos].nStartL + nLenName + 2;
		m_pos[nPos].nEndL = m_pos[nPos].nStartR - 1;
		m_pos[nPos].nEndR = m_pos[nPos].nEndL + 1;
	}
	else
	{
		YkString strValue = TextToDoc( szValue );
		nLenValue = (YkInt)strValue.GetLength();
		strInsert = _U("<");
		strInsert += szName;
		strInsert += _U(">");
		strInsert += strValue;
		strInsert += _U("</");
		strInsert += szName;
		strInsert += _U(">");
		strInsert +=  EOL;
		m_pos[nPos].nStartR = m_pos[nPos].nStartL + nLenName + 1;
		m_pos[nPos].nEndL = m_pos[nPos].nStartR + nLenValue + 1;
		m_pos[nPos].nEndR = m_pos[nPos].nEndL + nLenName + 2;
	}

	YkInt nReplace = 0, nLeft = m_pos[nPos].nStartL;
	if ( isEmptyParent )
	{
		YkString strParentTagName = GetTagName(iPosParent);
		YkString formatString;
		formatString = _U(">");
		formatString += EOL;
		formatString += strInsert;
		formatString += _U("</");
		formatString += strParentTagName;
		strInsert = formatString;
		nLeft = m_pos[iPosParent].nStartR - 1;
		nReplace = 1;
		m_pos[iPosParent].nStartR -= 1;
		m_pos[iPosParent].nEndL -= ((YkInt)strParentTagName.GetLength() + 1);
	}
	else if ( m_pos[iPosParent].nStartR + 1 == m_pos[iPosParent].nEndL )
	{
		strInsert = EOL + strInsert;
		nLeft = m_pos[iPosParent].nStartR + 1;
	}
	DocChange( nLeft, nReplace, strInsert );
	Adjust( nPos, (YkInt)strInsert.GetLength() - nReplace );

	if ( bAddChild )
		SetPos( m_posParent, iPosParent, nPos );
	else
		SetPos( iPosParent, nPos, 0 );
	return true;
}

YkBool YkMarkup::AddSubDoc( const YkChar* szSubDoc, YkBool bInsert, YkBool bAddChild )
{
	YkInt nOffset = 0, iPosParent, iPosBefore;
	if ( bAddChild )
	{
		if ( ! m_nPos )
			return false;
		iPosParent = m_nPos;
		iPosBefore = m_nPosChild;
	}
	else
	{
		if ( ! m_posParent )
			return false;
		iPosParent = m_posParent;
		iPosBefore = m_nPos;
	}

	YkInt flags = bInsert?1:0;
	LocateNew( iPosParent, iPosBefore, nOffset, 0, flags );

	YkBool isEmptyParent = m_pos[iPosParent].IsEmptyElement();

	if ( isEmptyParent )
	{
		nOffset += EOLLEN;
	}

	YkInt nParentEndLBeforeAdd = m_pos[iPosParent].nEndL;
	YkInt nPosFreeBeforeAdd = m_nPosFree;

	TokenPos token( szSubDoc );
	YkInt nNodeType = ParseNode( token );

	while ( nNodeType && nNodeType != MNT_ELEMENT )
	{
		token.szDoc = &token.szDoc[token.nNext];
		token.nNext = 0;
		nNodeType = ParseNode( token );
	}

	YkString insertString = token.szDoc;

	m_pos[iPosParent].nEndL = nOffset;

	YkInt nReplace = 0, nLeft = nOffset;
	YkString strParentTagName;
	if ( isEmptyParent )
	{
		strParentTagName = GetTagName(iPosParent);
		YkString formatString(_U(">"));		
		formatString += EOL;
		formatString += insertString;
		formatString += _U("</");
		formatString += strParentTagName;
		insertString = formatString;
		nLeft = m_pos[iPosParent].nStartR - 1;
		nReplace = 1;
		m_pos[iPosParent].nEndL = m_pos[iPosParent].nStartR + EOLLEN;
	}

	DocChange( nLeft, nReplace, insertString );

	YkInt nPos = ParseElem(iPosParent);
	m_pos[iPosParent].nEndL = nParentEndLBeforeAdd;
	if ( nPos <= 0 )
	{
		YkString revertString = isEmptyParent ? _U("/"): _U("");

		DocChange( nLeft, (YkInt)insertString.GetLength(), revertString );

		m_nPosFree = nPosFreeBeforeAdd;
		return false;
	}
	else
	{
		m_pos[nPos].iElemParent = iPosParent;
		if ( iPosBefore )
		{
			m_pos[nPos].iElemNext = m_pos[iPosBefore].iElemNext;
			m_pos[iPosBefore].iElemNext = nPos;
		}
		else
		{
			m_pos[nPos].iElemNext = m_pos[iPosParent].iElemChild;
			m_pos[iPosParent].iElemChild = nPos;
		}

		if ( isEmptyParent )
		{
			m_pos[iPosParent].nStartR -= 1;
			m_pos[iPosParent].nEndL -= ((YkInt)strParentTagName.GetLength() + 1);
		}

		Adjust( nPos, (YkInt)insertString.GetLength() - nReplace, true );
	}

	if ( bAddChild )
	{
		SetPos( m_posParent, iPosParent, nPos );
	} else { 
		SetPos( m_posParent, nPos, 0 );
	}

	return true;
}

void YkMarkup::Adjust( YkInt nPos, YkInt nShift, YkBool bAfterPos )
{
	YkInt iPosTop = m_pos[nPos].iElemParent;
	YkBool bPosFirst = bAfterPos; 
	while ( nPos )
	{
		YkBool bPosTop = false;
		if ( nPos == iPosTop )
		{
			iPosTop = m_pos[nPos].iElemParent;
			bPosTop = true;
		}
		
		if ( ! bPosTop && ! bPosFirst && m_pos[nPos].iElemChild )
		{
			nPos = m_pos[nPos].iElemChild;
		}
		else if ( m_pos[nPos].iElemNext )
		{
			nPos = m_pos[nPos].iElemNext;
		}
		else
		{
			while ( (nPos=m_pos[nPos].iElemParent) != 0 && nPos != iPosTop )
				if ( m_pos[nPos].iElemNext )
				{
					nPos = m_pos[nPos].iElemNext;
					break;
				}
		}
		bPosFirst = false;
		
		if ( nPos != iPosTop )
			m_pos[nPos].AdjustStart( nShift );
		m_pos[nPos].AdjustEnd( nShift );
	}
}

YkBool YkMarkup::AllocPosArray( YkInt nNewSize  )
{
	if ( ! nNewSize )
		nNewSize = m_nPosFree + (m_nPosFree>>1); 
	if ( m_pos.GetSize() < nNewSize )
	{
		YkInt nSeg = (m_pos.GetSize()?m_pos.GetSize()-1:0) >> m_pos.PA_SEGBITS;
		YkInt nNewSeg = (nNewSize-1) >> m_pos.PA_SEGBITS;
		if ( nNewSeg > nSeg + 1 )
		{
			nNewSeg = nSeg + 1;
			nNewSize = (nNewSeg+1) << m_pos.PA_SEGBITS;
		}
		
		if ( m_pos.nSegs <= nNewSeg )
		{
			YkInt nNewSegments = 4 + nNewSeg * 2;
			YkInt nAllocSize=nNewSegments*sizeof(YkChar*);
			YkByte* pNewSegments = new YkByte[nAllocSize];
			memset(pNewSegments,0,nAllocSize);
			if ( m_pos.SegsUsed() )
				memcpy( pNewSegments, m_pos.pSegs, m_pos.SegsUsed()*sizeof(YkChar*) );
			if ( m_pos.pSegs )
				delete[] (YkChar*)m_pos.pSegs;
			m_pos.pSegs=NULL;
			m_pos.pSegs = (ElemPos**)pNewSegments;
			m_pos.nSegs = nNewSegments;
		}
		
		YkInt nSegSize = m_pos.GetSize() - (nSeg << m_pos.PA_SEGBITS);
		YkInt nNewSegSize = nNewSize - (nNewSeg << m_pos.PA_SEGBITS);
		
		YkInt nFullSegSize = 1 << m_pos.PA_SEGBITS;
		if ( nSeg < nNewSeg && nSegSize < nFullSegSize )
		{
			YkByte* pNewFirstSeg = new YkByte[ nFullSegSize * sizeof(ElemPos) ];
			memset(pNewFirstSeg,0,nFullSegSize * sizeof(ElemPos));
			if ( nSegSize )
			{
				memcpy( pNewFirstSeg, m_pos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
				delete[] (YkChar*)m_pos.pSegs[nSeg];
				m_pos.pSegs[nSeg] = NULL;
			}
			if(m_pos.pSegs[nSeg] != NULL)
			{
				delete m_pos.pSegs[nSeg];
				m_pos.pSegs[nSeg] = NULL;
			}
			m_pos.pSegs[nSeg] = (ElemPos*)pNewFirstSeg;
		}
		YkInt nAllocSize =nNewSegSize * sizeof(ElemPos); 
		YkByte* pNewSeg = new YkByte[nAllocSize];
		memset(pNewSeg,0,nNewSegSize * sizeof(ElemPos));
		if ( nNewSeg == nSeg && nSegSize )
		{
			memcpy( pNewSeg, m_pos.pSegs[nSeg], nSegSize * sizeof(ElemPos) );
			delete[] (YkChar*)m_pos.pSegs[nSeg];
			m_pos.pSegs[nSeg]=NULL;
		}
		if(m_pos.pSegs[nNewSeg] != NULL)
		{
			delete m_pos.pSegs[nNewSeg];
			m_pos.pSegs[nNewSeg] = NULL;
		}
		m_pos.pSegs[nNewSeg] = (ElemPos*)pNewSeg;
		m_pos.nSize = nNewSize;
	}
	return true;
}

YkBool YkMarkup::CreateNode( YkString& strNode, YkInt nNodeType, const YkChar* szText )
{
	switch ( nNodeType )
	{
	case MNT_CDATA_SECTION:
		if ( Ykstrstr(szText,_U("]]>")) != NULL )
			return false;
		strNode = _U("<![CDATA[");
		strNode += szText;
		strNode += _U("]]>");
		break;
	}
	return true;
}

void YkMarkup::DocChange( YkInt nLeft, YkInt nReplace, const YkString& strInsert )
{
	m_strDoc.Replace( nLeft, nReplace, strInsert);
}

YkBool YkMarkup::FindAny( const YkChar* szDoc, YkInt& nChar )
{
	while ( szDoc[nChar] && Ykstrchr(_U(" \t\n\r"),szDoc[nChar]) )
		++nChar;
	return szDoc[nChar] != _U('\0');
}

YkBool YkMarkup::FindAttrib( YkMarkup::TokenPos& token, const YkChar* szAttrib ) const
{
	YkInt nAttrib = 0;
	for ( YkInt nCount = 0; FindToken(token); ++nCount )
	{
		if ( ! token.bIsString )
		{
			YkChar cChar = m_strDoc[token.nL];
			if ( cChar == _U('>') || cChar == _U('/') || cChar == _U('?') )
				break; 
			
			if ( cChar == _U('=') )
				continue;
			
			if ( ! nAttrib && nCount )
			{
				if ( ! szAttrib || ! szAttrib[0] )
					return true; 
				
				if ( token.Match(szAttrib) )
					nAttrib = nCount;
			}
		}
		else if ( nAttrib && nCount == nAttrib + 2 )
		{
			return true;
		}
	}
	
	return false;
}

YkBool YkMarkup::FindChar( const YkChar* szDoc, YkInt& nChar, YkChar c )
{
	const YkChar* pChar = &szDoc[nChar];
	while ( *pChar && *pChar != c )
		pChar += 1;
	nChar = pChar - szDoc;
	if ( ! *pChar )
		return false;
	return true;
}

YkInt YkMarkup::FindElem( YkInt iPosParent, YkInt nPos, const YkChar* szPath )
{
	if ( nPos )
		nPos = m_pos[nPos].iElemNext;
	else
		nPos = m_pos[iPosParent].iElemChild;
	
	if ( szPath == NULL || !szPath[0] )
		return nPos;
	
	TokenPos token( m_strDoc.Cstr() );
	while ( nPos )
	{
		token.nNext = m_pos[nPos].nStartL + 1;
		FindToken( token ); 
		if ( token.Match(szPath) )
			return nPos;
		nPos = m_pos[nPos].iElemNext;
	}
	return 0;
	
}

YkBool YkMarkup::FindToken( YkMarkup::TokenPos& token )
{
	const YkChar* szDoc = token.szDoc;
	YkInt nChar = token.nNext;
	token.bIsString = false;
	
	if ( ! FindAny(szDoc,nChar) )
	{
		token.nL = nChar;
		token.nR = nChar;
		token.nNext = nChar;
		return false;
	}
	
	YkChar cFirstChar = szDoc[nChar];
	if ( cFirstChar == _U('\"') || cFirstChar == _U('\'') )
	{
		token.bIsString = true;
		
		++nChar;
		token.nL = nChar;
		
		FindChar( token.szDoc, nChar, cFirstChar );
		
		token.nR = nChar - 1;
		
		if ( szDoc[nChar] )
			++nChar;
	}
	else
	{
		token.nL = nChar;
		while ( szDoc[nChar] && ! Ykstrchr(_U(" \t\n\r<>=\\/?!"),szDoc[nChar]) )
			nChar += 1;
		
		if ( nChar == token.nL )
			++nChar; 
		token.nR = nChar - 1;
	}
	
	token.nNext = nChar;
	return true;
}

YkString YkMarkup::GetAttrib( YkInt nPos, const YkChar* szAttrib ) const
{
	YkString strReturn;

	TokenPos token( m_strDoc.Cstr() );
	if ( nPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_pos[nPos].nStartL + 1;
	else
		return strReturn;
	
	if ( szAttrib && FindAttrib( token, szAttrib ) )
		return TextFromDoc( token.nL, token.nR - ((token.nR<(YkInt)m_strDoc.GetLength())?0:1) );
	return strReturn;
}

YkString YkMarkup::GetData( YkInt nPos ) const
{
	YkString strReturn;

	if ( ! m_pos[nPos].iElemChild && ! m_pos[nPos].IsEmptyElement() )
	{
		const YkChar* szDoc = m_strDoc.Cstr();
		YkInt nChar = m_pos[nPos].nStartR + 1;
		if ( FindAny( szDoc, nChar ) && szDoc[nChar] == _U('<')
			&& nChar + 11 < m_pos[nPos].nEndL
			&& Ykstrncmp( &szDoc[nChar], _U("<![CDATA["), 9 ) == 0 )
		{
			nChar += 9;
			YkInt nEndCDATA = (YkInt)m_strDoc.Find( _U("]]>"), nChar );
			if ( nEndCDATA != (YkInt)std::string::npos && nEndCDATA < m_pos[nPos].nEndL )
			{
				strReturn = m_strDoc.Mid( nChar, nEndCDATA - nChar );
				return strReturn;
			}
		}

		strReturn = TextFromDoc( m_pos[nPos].nStartR+1, m_pos[nPos].nEndL-1 );
	}
	return strReturn;
}

YkString YkMarkup::GetSubDoc( YkInt nPos ) const
{
	YkString strReturn;

	if ( nPos )
	{
		YkInt nL = m_pos[nPos].nStartL;
		YkInt nR = m_pos[nPos].nEndR + 1;
		const YkChar* szDoc = m_strDoc.Cstr();
		YkInt nChar = nR;
		if ( ! FindAny(szDoc,nChar) || szDoc[nChar] == _U('<') )
			nR = nChar;
		strReturn = m_strDoc.Mid( nL, nR - nL );
	}
	return strReturn;
}

YkString YkMarkup::GetTagName( YkInt nPos ) const
{
	YkString strReturn;

	TokenPos token( m_strDoc.Cstr() );
	token.nNext = m_pos[nPos].nStartL + 1;
	if ( ! nPos || ! FindToken( token ) )
		return strReturn;
	
	strReturn =  GetToken( token );
	return strReturn;
}

YkString YkMarkup::GetToken( const YkMarkup::TokenPos& token ) const
{
	YkString strReturn;

	if ( token.nL > token.nR )
		return strReturn;
	strReturn =  m_strDoc.Mid( token.nL, token.nR - token.nL + ((token.nR<(YkInt)m_strDoc.GetLength())? 1:0) );
	return strReturn;
}

void YkMarkup::LocateNew( YkInt iPosParent, YkInt& iPosRel, YkInt& nOffset, YkInt nLength, YkInt nFlags )
{
	YkBool bInsert = (nFlags&1)?true:false;
	YkBool bHonorWhitespace = (nFlags&2)?true:false;
	
	YkInt nStartL=0;
	if ( nLength )
	{
		if ( bInsert )
			nStartL = nOffset;
		else
			nStartL = nOffset + nLength;
	}
	else if ( iPosRel )
	{
		if ( bInsert ) 
			nStartL = m_pos[iPosRel].nStartL;
		else 
			nStartL = m_pos[iPosRel].nEndR + 1;
	}
	else if ( ! iPosParent )
	{
		if ( bInsert )
			nStartL = 0;
		else
			nStartL = (YkInt)m_strDoc.GetLength();
	}
	else if ( m_pos[iPosParent].IsEmptyElement() )
	{
		nStartL = m_pos[iPosParent].nStartR;
	}
	else
	{
		if ( bInsert ) 
			nStartL = m_pos[iPosParent].nStartR + 1;
		else 
			nStartL = m_pos[iPosParent].nEndL;
	}
	
	if ( ! bHonorWhitespace && ! m_pos[iPosParent].IsEmptyElement() )
	{
		const YkChar* szDoc = m_strDoc.Cstr();
		YkInt nChar = nStartL;
		if ( ! FindAny(szDoc,nChar) || szDoc[nChar] == _U('<') )
			nStartL = nChar;
	}
	
	YkInt iPosBefore = 0;
	if ( iPosRel )
	{
		if ( bInsert )
		{
			YkInt iPosPrev = m_pos[iPosParent].iElemChild;
			if ( iPosPrev != iPosRel )
			{
				while ( m_pos[iPosPrev].iElemNext != iPosRel )
					iPosPrev = m_pos[iPosPrev].iElemNext;
				iPosBefore = iPosPrev;
			}
		}
		else
		{
			iPosBefore = iPosRel;
		}
	}
	else if ( m_pos[iPosParent].iElemChild )
	{
		if ( ! bInsert )
		{
			YkInt iPosLast = m_pos[iPosParent].iElemChild;
			YkInt iPosNext = iPosLast;
			while ( iPosNext )
			{
				iPosLast = iPosNext;
				iPosNext = m_pos[iPosNext].iElemNext;
			}
			iPosBefore = iPosLast;
		}
	}
	
	nOffset = nStartL;
	iPosRel = iPosBefore;
}

YkBool YkMarkup::ParseDoc()
{
	YkString strResult = m_strError;
	
	ResetPos();
	m_mapSavedPos.clear();
	
	m_nPosFree = 1;
	AllocPosArray( (YkInt)m_strDoc.GetLength() / 64 + 8 );
	
	YkBool bWellFormed = false;
	m_strError.Empty();
	if ( m_strDoc.GetLength() )
	{
		m_pos[0].Clear();
		YkInt nPos = ParseElem( 0 );
		if ( nPos > 0 )
		{
			m_pos[0].iElemChild = nPos;
			bWellFormed = true;
		}
	}
	
	if ( ! bWellFormed )
	{
		m_pos[0].Clear();
		m_nPosFree = 1;
	}
	
	ResetPos();
	
	if ( ! m_strError.GetLength())
		m_strError = strResult;
	else
		m_strError = strResult + _U(", ") + m_strError;
	
	return bWellFormed;
}

YkInt YkMarkup::ParseElem( YkInt iPosParent )
{
	YkInt nPos = GetFreePos();
	m_pos[nPos].nStartL = m_pos[iPosParent].nEndL;
	m_pos[nPos].iElemParent = iPosParent;
	m_pos[nPos].iElemChild = 0;
	m_pos[nPos].iElemNext = 0;

	TokenPos token( m_strDoc.Cstr() );
	token.nNext = m_pos[iPosParent].nEndL;
	YkString strName;
	while ( strName.IsEmpty() )
	{
		m_pos[nPos].nStartL = token.nNext;
		if ( ! FindChar( token.szDoc, m_pos[nPos].nStartL, _U('<') ) )
			return ParseError( _U("Element tag not found") );

		m_pos[iPosParent].nEndL = m_pos[nPos].nStartL;

		token.nNext = m_pos[nPos].nStartL + 1;
		if ( FindToken( token ) )
		{
			if ( token.bIsString )
				return ParseError( _U("Tag starts with quote") );
			YkChar cFirstChar = m_strDoc[token.nL];
			if ( cFirstChar == _U('?') || cFirstChar == _U('!') )
			{
				token.nNext = m_pos[nPos].nStartL;
				if ( ! ParseNode(token) )
					return ParseError( _U("Invalid node") );
			}
			else if ( cFirstChar != _U('/') )
			{
				strName = GetToken( token );
				if ( ! FindChar(token.szDoc, token.nNext, _U('>')) )
					return ParseError( _U("End of tag not found") );
			}
			else
				return ReleasePos(); 
		}
		else
			return ParseError( _U("Abrupt end within tag") );
	}
	m_pos[nPos].nStartR = token.nNext;

	if ( m_strDoc[m_pos[nPos].nStartR-1] == _U('/') )
	{
		m_pos[nPos].nEndL = m_pos[nPos].nStartR-1;
		m_pos[nPos].nEndR = m_pos[nPos].nStartR;
	}
	else 
	{
		YkInt iInner, iInnerPrev = 0;
		m_pos[nPos].nEndL = m_pos[nPos].nStartR + 1;
		while ( (iInner = ParseElem( nPos )) > 0 )
		{
			if ( iInnerPrev )
				m_pos[iInnerPrev].iElemNext = iInner;
			else
				m_pos[nPos].iElemChild = iInner;
			iInnerPrev = iInner;

			m_pos[nPos].nEndL = m_pos[iInner].nEndR + 1;
		}
		if ( iInner == -1 )
			return -1;

		if ( ! FindChar( token.szDoc, m_pos[nPos].nEndL, _U('<') ) )
			return ParseError( _U("End tag of %s element not found"), strName.Cstr() );

		token.nNext = m_pos[nPos].nEndL + 1;
		YkInt nTokenCount = 0;
		while ( FindToken( token ) )
		{
			++nTokenCount;
			if ( ! token.bIsString )
			{
				if ( nTokenCount == 1 && m_strDoc[token.nL] != _U('/') )
					return ParseError( _U("Expecting end tag of element %s"), strName.Cstr() );

				else if ( nTokenCount == 2 && ! token.Match(strName.Cstr()) )
					return ParseError( _U("End tag does not correspond to %s"), strName.Cstr() );

				else if ( m_strDoc[token.nL] == _U('>') )
					break;
			}
		}

		if ( ! token.szDoc[token.nL] || nTokenCount < 2 )
			return ParseError( _U("End tag not completed for element %s"), strName.Cstr() );
		m_pos[nPos].nEndR = token.nL;
	}

	return nPos;
}

YkInt YkMarkup::ParseError( const YkChar* szError, const YkChar* szName )
{
	if ( szName )
	{
		YkChar szFormat[300];
		Yksprintf( szFormat, 300, szError, szName );
		m_strError = szFormat;
	}
	else
		m_strError = szError;
	ReleasePos();
	return -1;
}

YkInt YkMarkup::ParseNode( YkMarkup::TokenPos& token )
{
	YkInt nTypeFound = 0;
	const YkChar* szDoc = token.szDoc;
	token.nL = token.nNext;
	if ( szDoc[token.nL] == _U('<') )
	{
		if ( ! szDoc[token.nL+1] || ! szDoc[token.nL+2] )
			return 0;
		YkChar cFirstChar = szDoc[token.nL+1];
		const YkChar* szEndOfNode = NULL;
		if ( cFirstChar == _U('?') )
		{
			nTypeFound = MNT_PROCESSING_INSTRUCTION;
			szEndOfNode = _U("?>");
		}
		else if ( cFirstChar == _U('!') )
		{
			YkChar cSecondChar = szDoc[token.nL+2];
			if ( cSecondChar == _U('[') )
			{
				nTypeFound = MNT_CDATA_SECTION;
				szEndOfNode = _U("]]>");
			}
			else if ( cSecondChar == _U('-') )
			{
				nTypeFound = MNT_COMMENT;
				szEndOfNode = _U("-->");
			}
			else
			{
				nTypeFound = 0;
				YkInt nBrackets = 0;
				while ( FindToken(token) )
				{
					if ( ! token.bIsString )
					{
						YkChar cChar = szDoc[token.nL];
						if ( cChar == _U('[') )
							++nBrackets;
						else if ( cChar == _U(']') )
							--nBrackets;
						else if ( nBrackets == 0 && cChar == _U('>') )
						{
							nTypeFound = MNT_DOCUMENT_TYPE;
							break;
						}
					}
				}
				if ( ! nTypeFound )
					return 0;
			}
		}
		else if ( cFirstChar == _U('/') )
		{
			return 0;
		}
		else
		{
			nTypeFound = MNT_ELEMENT;
		}
		
		if ( szEndOfNode )
		{
			const YkChar* pEnd = Ykstrstr( &szDoc[token.nNext], szEndOfNode );
			if ( ! pEnd )
				return 0; 
			token.nNext = (pEnd - szDoc) + (YkInt)Ykstrlen(szEndOfNode);
		}
	}
	else if ( szDoc[token.nL] )
	{
		nTypeFound = MNT_WHITESPACE;
		
		token.nNext = token.nL;
		if ( FindAny(szDoc,token.nNext) )
		{
			if ( szDoc[token.nNext] != _U('<') )
			{
				nTypeFound = MNT_TEXT;
				FindChar( szDoc, token.nNext, _U('<') );
			}
		}
	}
	return nTypeFound;
}

YkInt YkMarkup::RemoveElem( YkInt nPos )
{
	YkInt iPosParent = m_pos[nPos].iElemParent;
	
	YkInt iPosLook = m_pos[iPosParent].iElemChild;
	YkInt iPosPrev = 0;
	while ( iPosLook != nPos )
	{
		iPosPrev = iPosLook;
		iPosLook = m_pos[iPosLook].iElemNext;
	}
	if ( iPosPrev )
		m_pos[iPosPrev].iElemNext = m_pos[nPos].iElemNext;
	else
		m_pos[iPosParent].iElemChild = m_pos[nPos].iElemNext;
	
	YkInt nAfterEnd = m_pos[nPos].nEndR + 1;
	const YkChar* szDoc = m_strDoc.Cstr();
	YkInt nChar = nAfterEnd;
	if ( ! FindAny(szDoc,nChar) || szDoc[nChar] == _U('<') )
		nAfterEnd = nChar;
	YkInt nLen = nAfterEnd - m_pos[nPos].nStartL;
	DocChange( m_pos[nPos].nStartL, nLen, YkString() );
	Adjust( nPos, - nLen, true );
	return iPosPrev;
}

YkBool YkMarkup::SetAttrib( YkInt nPos, const YkChar* szAttrib, YkInt nValue )
{
	YkChar szVal[25];
	Yksprintf( szVal, 25, _U("%d"), nValue );
	return SetAttrib( nPos, szAttrib, szVal );
}

YkBool YkMarkup::SetAttrib( YkInt nPos, const YkChar* szAttrib, const YkChar* szValue )
{
	TokenPos token( m_strDoc.Cstr() );
	YkInt nInsertAt=0;
	if ( nPos && m_nNodeType == MNT_ELEMENT )
	{
		token.nNext = m_pos[nPos].nStartL + 1;
		nInsertAt = m_pos[nPos].nStartR - (m_pos[nPos].IsEmptyElement()?1:0);
	}
	else
		return false;
	
	YkInt nReplace = 0;
	YkString strInsert;
	if ( FindAttrib( token, szAttrib ) )
	{
		strInsert = TextToDoc( szValue, true );
		nInsertAt = token.nL;
		nReplace = token.nR-token.nL+1;
	}
	else
	{
		YkString formatString(_U(" "));		
		formatString += szAttrib;
		formatString += _U("=");
		formatString += ATTRIBQUOTE;
		formatString += TextToDoc( szValue, true );
		formatString += ATTRIBQUOTE;
		strInsert = formatString;
	}
	
	DocChange( nInsertAt, nReplace, strInsert );
	YkInt nAdjust = (YkInt)strInsert.GetLength() - nReplace;
	m_pos[nPos].nStartR += nAdjust;
	m_pos[nPos].AdjustEnd( nAdjust );
	Adjust( nPos, nAdjust );
	MARKUP_SETDEBUGSTATE;
	return true;
}

YkBool YkMarkup::SetData( YkInt nPos, const YkChar* szData, YkInt nCDATA )
{
	YkString strInsert;
	
	
	if ( ! nPos || m_pos[nPos].iElemChild )
		return false;
	
	if ( nCDATA != 0 )
		if ( ! CreateNode(strInsert, MNT_CDATA_SECTION, szData) )
			nCDATA = 0;
		if ( nCDATA == 0 )
			 strInsert = TextToDoc( szData );
		YkInt nInsertAt=0, nReplace=0;
		if ( m_pos[nPos].IsEmptyElement() )
		{
			nInsertAt = m_pos[nPos].nEndL;
			nReplace = 1;
			
			YkString strTagName = GetTagName( nPos );
			m_pos[nPos].nStartR -= 1;
			m_pos[nPos].nEndL -= (1 + (YkInt)strTagName.GetLength());
			YkString formatString;
			formatString = _U(">");
			formatString += strInsert;
			formatString += _U("</");
			formatString += strTagName;
			strInsert = formatString;
		}
		else
		{
			nInsertAt = m_pos[nPos].nStartR+1;
			nReplace = m_pos[nPos].nEndL - m_pos[nPos].nStartR - 1;
		}
		DocChange( nInsertAt, nReplace, strInsert );
		YkInt nAdjust = (YkInt)strInsert.GetLength() - nReplace;
		Adjust( nPos, nAdjust );
		m_pos[nPos].AdjustEnd( nAdjust );
		MARKUP_SETDEBUGSTATE;
		return true;
}

YkBool YkMarkup::SetRealData( YkInt nPos, const YkChar* szData)
{
	YkString strInsert;
	
	YkInt nCDATA = 0;
	if ( ! nPos || m_pos[nPos].iElemChild )
		return false;
	
	if ( nCDATA != 0 )
		if ( ! CreateNode(strInsert, MNT_CDATA_SECTION, szData) )
			nCDATA = 0;
		if ( nCDATA == 0 )
			strInsert = szData;
		YkInt nInsertAt=0, nReplace=0;
		if ( m_pos[nPos].IsEmptyElement() )
		{
			nInsertAt = m_pos[nPos].nEndL;
			nReplace = 1;
			
			YkString strTagName = GetTagName( nPos );
			m_pos[nPos].nStartR -= 1;
			m_pos[nPos].nEndL -= (1 + (YkInt)strTagName.GetLength());
			YkString formatString;
			formatString = _U(">");
			formatString += strInsert;
			formatString += _U("</");
			formatString += strTagName;
			strInsert = formatString;
		}
		else
		{
			nInsertAt = m_pos[nPos].nStartR+1;
			nReplace = m_pos[nPos].nEndL - m_pos[nPos].nStartR - 1;
		}
		DocChange( nInsertAt, nReplace, strInsert );
		YkInt nAdjust = (YkInt)strInsert.GetLength() - nReplace;
		Adjust( nPos, nAdjust );
		m_pos[nPos].AdjustEnd( nAdjust );
		MARKUP_SETDEBUGSTATE;
		return true;
}

YkString YkMarkup::TextFromDoc( YkInt nLeft, YkInt nRight ) const
{
	static YkString szaCode[] = { _U("lt;"),_U("amp;"),_U("gt;"),_U("apos;"),_U("quot;") };
	static YkInt anCodeLen[] = { 3,4,3,5,5 };
	static YkString szSymbol = _U("<&>\'\"");

	YkString strResult;
	strResult.SetCapacity( nRight - nLeft + 1 );
	const YkChar* pSource = m_strDoc.Cstr();
	YkInt nChar = nLeft;
	YkChar cSource = pSource[nChar];
	while ( nChar <= nRight )
	{
		if ( cSource == _U('&') )
		{
			YkBool bCodeConverted = false;

			cSource = pSource[nChar+1];
			if ( cSource == _U('#') )
			{
				YkInt nBase = 10;
				YkInt nNumericChar = nChar + 2;
				cSource = pSource[nNumericChar];
				if ( cSource == _U('x') )
				{
					++nNumericChar;
					cSource = pSource[nNumericChar];
					nBase = 16;
				}

				YkInt nCodeLen = 0;
				while ( nCodeLen < 7 && cSource && cSource != _U(';') )
				{
					++nCodeLen;
					cSource = pSource[nNumericChar + nCodeLen];
				}

				if ( cSource == _U(';') )
				{
					YkInt nUnicode = Ykstrtol( &pSource[nNumericChar], NULL, nBase );
					if ( nUnicode < 0x80 )
						strResult += (YkChar)nUnicode;
					else if ( nUnicode < 0x800 )
					{
						strResult += (YkChar)(((nUnicode&0x7c0)>>6) | 0xc0);
						strResult += (YkChar)((nUnicode&0x3f) | 0x80);
					}
					else
					{
						strResult += (YkChar)(((nUnicode&0xf000)>>12) | 0xe0);
						strResult += (YkChar)(((nUnicode&0xfc0)>>6) | 0x80);
						strResult += (YkChar)((nUnicode&0x3f) | 0x80);
					}
					nChar = nNumericChar + nCodeLen + 1;
					bCodeConverted = true;
				}
			}
			else 
			{
				for ( YkInt nMatch = 0; nMatch < 5; ++nMatch )
				{
					if ( nChar <= nRight - anCodeLen[nMatch]
						&& Ykstrncmp(szaCode[nMatch],&pSource[nChar+1],anCodeLen[nMatch]) == 0 )
					{
						strResult += szSymbol[nMatch];
						nChar += anCodeLen[nMatch] + 1;
						bCodeConverted = true;
						break;
					}
				}
			}

			if ( ! bCodeConverted )
			{
				strResult += _U('&');
				++nChar;
			}
		}
		else 
		{
			strResult += cSource;
			nChar++;
		}
		cSource = pSource[nChar];
	}
	return strResult;
}

YkString YkMarkup::TextToDoc( const YkChar* szText, YkBool bAttrib ) const
{
	static YkString szaReplace[] = { _U("&lt;"),_U("&amp;"),_U("&gt;"),_U("&apos;"),_U("&quot;") };
	const YkChar* pFind = bAttrib?_U("<&>\'\""):_U("<&>");
	const YkChar* pSource = szText;
	YkString strResult;
	YkInt nLen = (YkInt)Ykstrlen( szText );
	strResult.SetCapacity( nLen + nLen / 10 );
	YkChar cSource = *pSource;
	const YkChar* pFound=NULL;	
	while ( cSource )
	{
		if ( (pFound=Ykstrchr(pFind,cSource)) != NULL )
		{
			pFound = szaReplace[pFound-pFind];
			
			strResult.Append(pFound,(YkInt)Ykstrlen(pFound));
		}
		else
		{
			strResult += cSource;
		}
		cSource = *(++pSource);
	}
	return strResult;
}

void YkMarkup::PosInsert( YkInt nPos, YkInt nInsertLength )
{
}

YkString YkMarkup::GetChildAttribName( YkInt n ) const
{
	YkString strResult;
	strResult.SetCapacity(16);
	TokenPos token( m_strDoc.Cstr() );
	if (m_nPosChild && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_pos[m_nPosChild].nStartL + 1;
	else
		return strResult;
	
	for ( YkInt nAttrib=0; nAttrib<=n; ++nAttrib )
	{
		if ( ! FindAttrib(token) )
			return strResult;
	}
		
	strResult = GetToken( token );
	return strResult;
}

YkBool YkMarkup::MoveNextElem()
{
	if ( m_pos.GetSize() )
	{	
		YkInt nPos = m_nPos;
		if ( nPos )
			nPos = m_pos[nPos].iElemNext;
		else
			nPos = m_pos[m_posParent].iElemChild;

		if(nPos)
		{
			SetPos( m_pos[nPos].iElemParent, nPos, 0 );
			return true;
		}
	}
	return false;
}

YkBool YkMarkup::MoveNextChildElem()
{
	if ( ! m_nPos )
		FindElem();
	
	YkInt iPosChild = m_nPosChild;
	if ( iPosChild )
		iPosChild = m_pos[iPosChild].iElemNext;
	else
		iPosChild = m_pos[m_nPos].iElemChild;

	if ( iPosChild )
	{
		YkInt nPos = m_pos[iPosChild].iElemParent;
		SetPos( m_pos[nPos].iElemParent, nPos, iPosChild );
		return true;
	}	
	return false;
}

YkBool YkMarkup::HasChild() const
{	
	if ( m_nPos && m_nNodeType == MNT_ELEMENT &&
		m_nPosChild > 0)
	{
		return true;
	}
	return false;
}

#pragma warning (disable:4127)
YkInt YkMarkup::GetAttribCount() const
{
	YkInt nCount = 0;
	TokenPos token( m_strDoc.Cstr() );
	if (m_nPos && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_pos[m_nPos].nStartL + 1;
	else
		return 0;
	while(true)
	{
		if ( ! FindAttrib(token) )
		{
			return nCount;
		}
		nCount++;
	}
	return nCount;
}

YkInt YkMarkup::GetChildAttribCount() const
{
	YkInt nCount = 0;
	TokenPos token( m_strDoc.Cstr() );
	if (m_nPosChild && m_nNodeType == MNT_ELEMENT )
		token.nNext = m_pos[m_nPosChild].nStartL + 1;
	else
		return 0;
	
	while(true)
	{
		if ( ! FindAttrib(token) )
		{
			return nCount;
		}
		nCount++;
	}
	return nCount;
}
#pragma warning (default:4127)

YkString YkMarkup::GetXMLFileHeader()
{
	YkString XMLFileHeader=XML_FILE_HEADER;
	XMLFileHeader.Replace(_U("%VERSION"),_U("1.0"));
	XMLFileHeader.Replace(_U("%ENCODING"),YkTextCodec::GetCharsetName(m_nEncoding));
	return XMLFileHeader;
}

YkMBString::Charset YkMarkup::GetEncoding()
{
	return m_nEncoding;
}

void YkMarkup::SetEncoding(YkMBString::Charset  nEncoding)
{
	m_nEncoding = nEncoding;
}

}
