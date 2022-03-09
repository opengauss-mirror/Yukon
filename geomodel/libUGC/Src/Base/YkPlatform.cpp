/*
 *
 * YkPlatform.cpp
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

#include "Base/YkPlatform.h"
#include "Base/YkString.h"

#if defined(__GNUC__) && 0
# define likely(X)    __builtin_expect((X),1)
# define unlikely(X)  __builtin_expect((X),0)
#else
# define likely(X)    !!(X)
# define unlikely(X)  !!(X)
#endif
#define FORMAT_DIGIT(x) ((x)>0x2F && (x)<0x3a)
#define FORMAT_TOLOWER(x) ((x) | 0x20)


#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SMALL	32		/* Must be 32 == 0x20 */
#define SPECIAL	64		/* 0x */

enum format_type {
	FORMAT_TYPE_NONE, /* Just a string part */
	FORMAT_TYPE_WIDTH,
	FORMAT_TYPE_PRECISION,
	FORMAT_TYPE_CHAR,
	FORMAT_TYPE_STR,
	FORMAT_TYPE_PTR,
	FORMAT_TYPE_PERCENT_CHAR,
	FORMAT_TYPE_INVALID,
	FORMAT_TYPE_LONG_LONG,
	FORMAT_TYPE_ULONG,
	FORMAT_TYPE_LONG,
	FORMAT_TYPE_UBYTE,
	FORMAT_TYPE_BYTE,
	FORMAT_TYPE_USHORT,
	FORMAT_TYPE_SHORT,
	FORMAT_TYPE_UINT,
	FORMAT_TYPE_INT,
	FORMAT_TYPE_NRCHARS,
	FORMAT_TYPE_SIZE_T,
	FORMAT_TYPE_PTRDIFF,
	FORMAT_TYPE_FLOAT,
	FORMAT_TYPE_DOUBLE,
	FORMAT_TYPE_LONG_DOUBLE
};

struct printf_spec {
	enum format_type	type;
	int			flags;		/* flags to number() */
	int			field_width;	/* width of output field */
	int			base;
	int			precision;	/* # of digits/chars */
	int			qualifier;
};

#ifdef _UGUNICODE

extern BASE_API YkInt yk_strlen( const YkWchar* ws )
{
	const YkWchar *eos = ws;
	while( *eos++ ) ;
	return( (YkInt)(eos - ws - 1) );
}

extern BASE_API YkWchar* yk_strchr( const YkWchar* ws, YkWchar wc )
{
	const YkWchar *eos = ws;
	YkWchar c;
	while ((c = *eos++))
		if (c == wc)
			return (YkWchar*)(--eos);
	return NULL;
}

extern BASE_API YkWchar* yk_strcpy( YkWchar* wsDes, const YkWchar* wsSrc )
{
	YKASSERT((wsDes!=NULL)&&(wsSrc!=NULL));
	YkWchar* eos = wsDes;
	while ((*eos++ = *wsSrc++));
	return wsDes;
}

extern BASE_API YkWchar* yk_strncpy( YkWchar* wsDes, const YkWchar* wsSrc, YkInt count )
{
	YKASSERT((wsDes!=NULL)&&(wsSrc!=NULL));
	YkWchar* eos = wsDes;
	while ((count--) && (*eos++ = *wsSrc++));
	return wsDes;
}

extern BASE_API YkWchar* yk_strcat( YkWchar* wsDes, const YkWchar* wsSrc )
{
	YKASSERT((wsDes!=NULL)&&(wsSrc!=NULL));
	YkWchar* eos = wsDes;
	while (*eos++);
	eos--;
	while ((*eos++ = *wsSrc++));
	return wsDes;
}

extern BASE_API YkInt yk_strcmp( const YkWchar* ws1, const YkWchar* ws2 )
{
	YKASSERT((ws1!=NULL)&&(ws2!=NULL));
	while ((*ws1 == *ws2) && *ws1)
		ws1++,ws2++;

	return (*ws1 - *ws2);
}

extern BASE_API YkInt yk_strncmp( const YkWchar* ws1, const YkWchar* ws2, YkInt count )
{
	YKASSERT((ws1!=NULL)&&(ws2!=NULL));
	while (count-- && *ws1 == *ws2)
		ws1++,ws2++;
	if (count==-1)
		return 0;
	return (*ws1 - *ws2);
}

extern BASE_API YkWchar* yk_strstr( YkWchar* ws, const YkWchar* wsub )
{
	if(!*wsub)
		return ws;
	YkWchar* bp;
	const YkWchar* sp;
	while (*ws)
	{
		bp = ws;
		sp = wsub;
		do 
		{
			if(!*sp)
				return ws;
		} while (*bp++ == *sp++);
		ws++;
	}
	return NULL;
}

int skip_atoi(const YkWchar **s)
{
	int i=0;

	while (FORMAT_DIGIT(**s))
		i = i*10 + *((*s)++) - L'0';
	return i;
}


YkWchar *YK_string(YkWchar *buf, YkWchar *end, YkWchar *s, struct printf_spec spec)
{
	int len, i;

	if ((unsigned long)s < /*PAGE_SIZE*/16)   //	WhiteBox_Ignore
	{
		len = 0;
	}
	else
	{
#ifdef _UGQSTRING
		len = yk_strlen(s);
#else
		len = u_strlen((const UChar*)s);
#endif
	}

	if(spec.precision>0 && len > spec.precision)
	{
		len = spec.precision;
	}

	if (!(spec.flags & LEFT)) {
		while (len < spec.field_width--) {
			if (buf < end)
				*buf = L' ';
			++buf;
		}
	}
	for (i = 0; i < len; ++i) {
		if (buf < end)
			*buf = *s;
		++buf; ++s;
	}
	while (len < spec.field_width--) {
		if (buf < end)
			*buf = L' ';
		++buf;
	}
	return buf;
}



/*
* Helper function to decode printf style format.
* Each call decode a token from the format and return the
* number of characters read (or likely the delta where it wants
* to go on the next call).
* The decoded token is returned through the parameters
*
* @fmt: the format string
* @type of the token returned
* @flags: various flags such as +, -, # tokens..
* @field_width: overwritten width
* @base: base of the number (octal, hex, ...)
* @precision: precision of a number
* @qualifier: qualifier of a number (long, size_t, ...)
*/
static int format_decode(const YkWchar *fmt, struct printf_spec *spec)
{
	const YkWchar *start = fmt;

	/* we finished early by reading the field width */
	if (spec->type == FORMAT_TYPE_WIDTH) {
		if (spec->field_width < 0) {
			spec->field_width = -spec->field_width;
			spec->flags |= LEFT;
		}
		spec->type = FORMAT_TYPE_NONE;
		goto precision;
	}

	/* we finished early by reading the precision */
	if (spec->type == FORMAT_TYPE_PRECISION) {
		if (spec->precision < 0)
			spec->precision = 0;

		spec->type = FORMAT_TYPE_NONE;
		goto qualifier;
	}

	/* By default */
	spec->type = FORMAT_TYPE_NONE;

	for (; *fmt ; ++fmt) {
		if (*fmt == L'%')
			break;
	}

	/* Return the current non-format string */
	if (fmt != start || !*fmt)
		return fmt - start;

	/* Process flags */
	spec->flags = 0;
#pragma warning (disable: 4127)
	while (1) { /* this also skips first '%' */
		bool found = true;

		++fmt;

		switch (*fmt) {
case L'-': spec->flags |= LEFT;    break;
case L'+': spec->flags |= PLUS;    break;
case L' ': spec->flags |= SPACE;   break;
case L'#': spec->flags |= SPECIAL; break;
case L'0': spec->flags |= ZEROPAD; break;
default:  found = false;
		}

		if (!found)
			break;
	}
#pragma warning (default: 4127)

	/* get field width */
	spec->field_width = -1;

	if (FORMAT_DIGIT(*fmt))
		spec->field_width = skip_atoi(&fmt);
	else if (*fmt == L'*') {
		/* it's the next argument */
		spec->type = FORMAT_TYPE_WIDTH;
		return ++fmt - start;
	}

precision:
	/* get the precision */
	spec->precision = -1;
	if (*fmt == L'.') {
		++fmt;
		if (FORMAT_DIGIT(*fmt)) {
			spec->precision = skip_atoi(&fmt);
			if (spec->precision < 0)
				spec->precision = 0;
		} else if (*fmt == '*') {
			/* it's the next argument */
			spec->type = FORMAT_TYPE_PRECISION;
			return ++fmt - start;
		}
	}

qualifier:
	/* get the conversion qualifier */
	spec->qualifier = -1;
	if (*fmt == L'h' || *fmt == L'l' || *fmt == L'L' ||
		*fmt == L'Z' || *fmt == L'z' || *fmt == L't') {
			spec->qualifier = *fmt++;
			if (unlikely(spec->qualifier == *fmt)) {
				if (spec->qualifier == L'l') {
					spec->qualifier = L'L';
					++fmt;
				} else if (spec->qualifier == L'h') {
					spec->qualifier = L'H';
					++fmt;
				}
			}
	}

	/* default base */
	spec->base = 10;
	switch (*fmt) {
case L'c':
	spec->type = FORMAT_TYPE_CHAR;
	return ++fmt - start;
case L'S':
case L's':
	spec->type = FORMAT_TYPE_STR;
	return ++fmt - start;
case L'f':
case L'F':
case L'e':
case L'E':
	spec->type = FORMAT_TYPE_DOUBLE;
	if(spec->qualifier == L'L')
	{
		if(spec->type == FORMAT_TYPE_DOUBLE)
		{
			spec->type = FORMAT_TYPE_LONG_DOUBLE;
		}
	}
	return ++fmt - start;
case L'g':
case L'G':
	spec->type = FORMAT_TYPE_FLOAT;
	if(spec->qualifier == L'L')
	{
		if(spec->type == FORMAT_TYPE_DOUBLE)
		{
			spec->type = FORMAT_TYPE_LONG_DOUBLE;
		}
	}
	return ++fmt - start;
case L'p':
	spec->type = FORMAT_TYPE_PTR;
	return ++fmt - start;
	/* skip alnum */

case L'n':
	spec->type = FORMAT_TYPE_NRCHARS;
	return ++fmt - start;

case L'%':
	spec->type = FORMAT_TYPE_PERCENT_CHAR;
	return ++fmt - start;

	/* integer number formats - set up the flags and "break" */
case L'o':
	spec->base = 8;
	break;

case L'x':
	spec->flags |= SMALL;

case L'X':
	spec->base = 16;
	break;

case L'd':
case L'i':
	spec->flags |= SIGN;
case L'u':
	break;
case L'l':
	{
		if (spec->qualifier == 'l')
			fmt++;
		if (*fmt == L'd')
		{
			spec->type = FORMAT_TYPE_LONG_LONG;
		}
	}
	return ++fmt - start;

default:
	spec->type = FORMAT_TYPE_INVALID;
	return fmt - start;
	}

	if (spec->qualifier == 'L')
		spec->type = FORMAT_TYPE_LONG_LONG;
	else if (spec->qualifier == 'l') {
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_LONG;
		else
			spec->type = FORMAT_TYPE_ULONG;
	} else if (spec->qualifier == L'Z' || spec->qualifier == L'z') {
		spec->type = FORMAT_TYPE_SIZE_T;
	} else if (spec->qualifier == L't') {
		spec->type = FORMAT_TYPE_PTRDIFF;
	} else if (spec->qualifier == L'H') {
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_BYTE;
		else
			spec->type = FORMAT_TYPE_UBYTE;
	} else if (spec->qualifier == L'h') {
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_SHORT;
		else
			spec->type = FORMAT_TYPE_USHORT;
	} else {
		if (spec->flags & SIGN)
			spec->type = FORMAT_TYPE_INT;
		else
			spec->type = FORMAT_TYPE_UINT;
	}

	return ++fmt - start;
}

/**
* vsnprintf - Format a string and place it in a buffer
* @buf: The buffer to place the result into
* @size: The size of the buffer, including the trailing null space
* @fmt: The format string to use
* @args: Arguments for the format string
*
* This function follows C99 vsnprintf, but has some extensions:
* %pS output the name of a text symbol with offset
* %ps output the name of a text symbol without offset
* %pF output the name of a function pointer with its offset
* %pf output the name of a function pointer without its offset
* %pR output the address range in a struct resource
* %n is ignored
*
* The return value is the number of characters which would
* be generated for the given input, excluding the trailing
* '\0', as per ISO C99. If you want to have the exact
* number of characters written into @buf as return value
* (not including the trailing '\0'), use vscnprintf(). If the
* return is greater than or equal to @size, the resulting
* string is truncated.
*
* Call this function if you are already dealing with a va_list.
* You probably want snprintf() instead.
*/
size_t BASE_API YK_vsnprintf(YkWchar *buf, size_t size, const YkWchar *fmt, va_list args)
{
	YkUlong num;
	long double dlvalue;	//	WhiteBox_Ignore
	double dvalue;
	int nNumSize =  0;
	YkWchar *str, *end, c;
	int read;
	struct printf_spec spec = {(enum format_type)0};
	const int nBufferSize = 513;
#if SYSTEM_WCHAR_SIZE==2
	YkWchar dValueBuffer[nBufferSize];
	YkWchar dValueArgument[nBufferSize];
#else
	YkWchar dValueBuffer[nBufferSize];
	YkWchar dValueArgument[nBufferSize];
	YkWchar32 dValueBuffer32[nBufferSize];
	YkWchar32 dValueArgument32[513];
#endif

	int nCountdValue = 0;

	str = buf;
	end = buf + size;

	/* Make sure end is always >= buf */
	if (end < buf) {
		end = (YkWchar*)((void *)-1);
		size = (end - buf);
	}

	while (*fmt) {
		const YkWchar *old_fmt = fmt;

		read = format_decode(fmt, &spec);

		fmt += read;

		switch (spec.type) 
		{
		case FORMAT_TYPE_NONE: 
			{
				int copy = read;
				if (str < end) 
				{
					if (copy > end - str)
						copy = (end - str);
					memcpy(str, old_fmt, copy*sizeof(YkWchar));
				}
				str += read;
				break;
			}

		case FORMAT_TYPE_WIDTH:
			spec.field_width = va_arg(args, int);
			break;

		case FORMAT_TYPE_PRECISION:
			spec.precision = va_arg(args, int);
			break;

		case FORMAT_TYPE_CHAR:
			if (!(spec.flags & LEFT)) 
			{
				while (--spec.field_width > 0) {
					if (str < end)
						*str = L' ';
					++str;

				}
			}
			c = (YkWchar) va_arg(args, int);
			if (str < end)
				*str = c;
			++str;
			while (--spec.field_width > 0) {
				if (str < end)
					*str = L' ';
				++str;
			}
			break;

		case FORMAT_TYPE_STR:
			str = YK_string(str, end, va_arg(args, YkWchar *), spec);
			break;

		case FORMAT_TYPE_PTR:
			{
				void *ptr = (void *) va_arg(args, void*);
				const YkWchar *temp = fmt;
				--temp;
				dValueArgument[512] = L'\0';
				nCountdValue = 511;
				while(*temp!= L'%')
				{
					dValueArgument[nCountdValue] = *temp;
					--temp;
					--nCountdValue;
				}
				dValueArgument[nCountdValue]= L'%';
#ifdef WIN32
				swprintf_s(dValueBuffer,&dValueArgument[nCountdValue],ptr);	
#else
#if SYSTEM_WCHAR_SIZE==2
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],ptr);
#else
#ifdef _UGUNICODE
				YkString::WcharToWchar32(&dValueArgument[nCountdValue],dValueArgument32,nBufferSize);
				swprintf((wchar_t*)dValueBuffer32,nBufferSize,(const wchar_t*)dValueArgument32,ptr);
				YkString::Wchar32ToWchar(dValueBuffer32,dValueBuffer,nBufferSize);
#else
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],ptr);
#endif
#endif

#endif
				nCountdValue=0;
				while (dValueBuffer[nCountdValue] != L'\0')
				{
					if (str < end)
						*str =dValueBuffer[nCountdValue];
					++nCountdValue;
					++str;
				}
				break;
			}



		case FORMAT_TYPE_PERCENT_CHAR:
			if (str < end)
				*str = L'%';
			++str;
			break;

		case FORMAT_TYPE_INVALID:
			if (str < end)
				*str = L'%';
			++str;
			break;

		case FORMAT_TYPE_NRCHARS: 
			{
				int qualifier = spec.qualifier;

				if (qualifier == L'l') {
					long *ip = va_arg(args, long *);	// WhiteBox_Ignore
					*ip = (str - buf);
				} else if (qualifier == L'Z' ||
					qualifier == L'z') {
						size_t *ip = va_arg(args, size_t *);
						*ip = (str - buf);
				} else {
					int *ip = va_arg(args, int *);
					*ip = (str - buf);
				}
				break;
			}
			//在linux long double是12b 大于 double的8b
		case FORMAT_TYPE_FLOAT:
		case FORMAT_TYPE_DOUBLE:
			{
				dvalue = va_arg(args, double);
				const YkWchar *temp = fmt;
				--temp;
				dValueArgument[512] = L'\0';
				nCountdValue = 511;
				while(*temp!= L'%')
				{
					dValueArgument[nCountdValue] = *temp;
					--temp;
					--nCountdValue;
				}
				dValueArgument[nCountdValue]= L'%';
#ifdef WIN32
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],dvalue);
#else
#if SYSTEM_WCHAR_SIZE==2
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],dvalue);
#else
#ifdef _UGUNICODE
				YkString::WcharToWchar32(&dValueArgument[nCountdValue],dValueArgument32,nBufferSize);
				swprintf((wchar_t*)dValueBuffer32,nBufferSize,(const wchar_t*)dValueArgument32,dvalue);
				YkString::Wchar32ToWchar(dValueBuffer32,dValueBuffer,nBufferSize);
#else
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],dvalue);
#endif
#endif
#endif
				nCountdValue=0;
				while (dValueBuffer[nCountdValue] != L'\0')
				{
					if(dValueBuffer[nCountdValue] == L',')
					{
						if(nCountdValue >=1)
						{
							if(FORMAT_DIGIT(dValueBuffer[nCountdValue-1])&& FORMAT_DIGIT(dValueBuffer[nCountdValue+1]))
							{
								dValueBuffer[nCountdValue] = L'.';
							}
						}

					}
					if (str < end)
						*str =dValueBuffer[nCountdValue];
					++nCountdValue;
					++str;
				}
				break;
			}
		case FORMAT_TYPE_LONG_DOUBLE:
			{
				dlvalue = va_arg(args, long double);	//	WhiteBox_Ignore
				const YkWchar *temp = fmt;
				--temp;
				dValueArgument[512] = L'\0';
				nCountdValue = 511;
				while(*temp!= L'%')
				{
					dValueArgument[nCountdValue] = *temp;
					--temp;
					--nCountdValue;
				}
				dValueArgument[nCountdValue]= L'%';
#ifdef WIN32
				swprintf_s(dValueBuffer,&dValueArgument[nCountdValue],dlvalue);		
#else
#if SYSTEM_WCHAR_SIZE==2
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],dlvalue);
#else
#ifdef _UGUNICODE
				YkString::WcharToWchar32(&dValueArgument[nCountdValue],dValueArgument32,nBufferSize);
				swprintf((wchar_t*)dValueBuffer32,nBufferSize,(const wchar_t*)dValueArgument32,dlvalue);
				YkString::Wchar32ToWchar(dValueBuffer32,dValueBuffer,nBufferSize);
#else
				swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],dlvalue);
#endif

#endif
#endif
				nCountdValue=0;
				while (dValueBuffer[nCountdValue] != L'\0')
				{
					if(dValueBuffer[nCountdValue] == L',')
					{
						if(nCountdValue >=1)
						{
							if(FORMAT_DIGIT(dValueBuffer[nCountdValue-1])&& FORMAT_DIGIT(dValueBuffer[nCountdValue+1]))
							{
								dValueBuffer[nCountdValue] = L'.';
							}

						}

					}
					if (str < end)
						*str =dValueBuffer[nCountdValue];
					++nCountdValue;
					++str;
				}
				break;
			}
		default:
			switch (spec.type) {
		case FORMAT_TYPE_LONG_LONG:
			num = va_arg(args, YkLong);
			nNumSize = sizeof(YkLong);
			break;
		case FORMAT_TYPE_ULONG:
			num = va_arg(args, unsigned long);	//	WhiteBox_Ignore
			nNumSize = sizeof(unsigned long);	//	WhiteBox_Ignore
			break;
		case FORMAT_TYPE_LONG:
			num = va_arg(args, long);	//	WhiteBox_Ignore
			nNumSize = sizeof(long);	//	WhiteBox_Ignore
			break;
		case FORMAT_TYPE_SIZE_T:
			num = va_arg(args, size_t);
			nNumSize = sizeof(size_t);
			break;
		case FORMAT_TYPE_PTRDIFF:
			num = va_arg(args, ptrdiff_t);
			nNumSize = sizeof(ptrdiff_t);
			break;
		case FORMAT_TYPE_UBYTE:
			num = (unsigned char) va_arg(args, int);
			nNumSize = sizeof(int);
			break;
		case FORMAT_TYPE_BYTE:
			num = (signed char) va_arg(args, int);
			nNumSize = sizeof(int);
			break;
		case FORMAT_TYPE_USHORT:
			num = (unsigned short) va_arg(args, int);
			nNumSize = sizeof(int);
			break;
		case FORMAT_TYPE_SHORT:
			num = (short) va_arg(args, int);
			nNumSize = sizeof(int);
			break;
		case FORMAT_TYPE_INT:
			num = (int) va_arg(args, int);
			nNumSize = sizeof(int);
			break;
		default:
			num = va_arg(args, unsigned int);
			nNumSize = sizeof(unsigned int);
			}
			const YkWchar *temp = fmt;
			--temp;
			dValueArgument[512] = L'\0';
			nCountdValue = 511;
			while(*temp!= L'%')
			{
				dValueArgument[nCountdValue] = *temp;
				--temp;
				--nCountdValue;
			}
			dValueArgument[nCountdValue]= L'%';
#ifdef WIN32
			swprintf_s(dValueBuffer,&dValueArgument[nCountdValue],num);
#else

#if SYSTEM_WCHAR_SIZE==2
			swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],num);
#else
#ifdef _UGUNICODE
			YkString::WcharToWchar32(&dValueArgument[nCountdValue],dValueArgument32,nBufferSize);
			swprintf((wchar_t*)dValueBuffer32,nBufferSize,(const wchar_t*)dValueArgument32,num);
			YkString::Wchar32ToWchar(dValueBuffer32,dValueBuffer,nBufferSize);
#else
			swprintf(dValueBuffer,nBufferSize,&dValueArgument[nCountdValue],num);
#endif
#endif
#endif

			nCountdValue=0;
			while (dValueBuffer[nCountdValue] != L'\0')
			{
				if (str < end)
					*str =dValueBuffer[nCountdValue];
				++nCountdValue;
				++str;
			}
		}
	}

	if (size > 0) {
		if (str <= end)
			*str = L'\0';
		else
			end[-1] = L'\0';
	}

	/* the trailing null byte doesn't count towards the total */
	return str-buf;

}
#endif

extern size_t BASE_API YKstrftime( YkChar* szBuffer, size_t maxsize, const YkChar* Format, const struct tm *timeptr )
{
#ifdef _UGUNICODE
#if SYSTEM_WCHAR_SIZE==2
	return wcsftime(szBuffer,maxsize,Format, timeptr);
#else
	YkInt nFormatLen = YKstrlen(Format);
	const YkInt nMaxTimeBufferSize = 128;
	YkWchar32 pTempBuffer[nMaxTimeBufferSize];
	YkWchar32 pTempFormat[nMaxTimeBufferSize];
	wmemset((wchar_t*)pTempBuffer, '\0', nMaxTimeBufferSize);
	wmemset((wchar_t*)pTempFormat, '\0', nMaxTimeBufferSize);
	YkString::WcharToWchar32(Format, pTempFormat, nFormatLen);
	YkInt nCharaterCount = wcsftime((wchar_t*)pTempBuffer,maxsize,(wchar_t*)pTempFormat, timeptr);
	YkString::Wchar32ToWchar(pTempBuffer, szBuffer, maxsize);
	return nCharaterCount;
#endif
#else
	return strftime(szBuffer, maxsize, Format, timeptr);
#endif
}
