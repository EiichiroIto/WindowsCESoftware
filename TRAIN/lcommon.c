#include	"lfunc.h"

BYTE	Tmpbuf[ 256 ] ;

/*----------------------------------------------------------------------------
 * メモリ・文字列操作関連
 *--------------------------------------------------------------------------*/
void
Memset( void *dst, int c, int len )
{
	memset( dst, c, len ) ;
}

void
Memcpy( void *dst, const void *src, int len )
{
	memcpy( dst, src, len ) ;
}

int
Memcmp( const void *x, const void *y, int len )
{
	return memcmp( x, y, len ) ;
}

int
Strlen( const BYTE* str )
{
    int		result = 0 ;
    while ( *str++ ) {
		result ++ ;
    }
    return result ;
}

void
Strcpy( LPBYTE dst, const BYTE *src )
{
	while ( (*dst++ = *src++) != 0 ) {
	}
}

int
Strcmp( const BYTE *x, const BYTE *y )
{
    while ( *x == *y ) {
		if ( *x == 0 ) {
			return 0 ;
		}
		x ++, y ++ ;
    }
    return *x - *y ;
}

int
Strcmpi( const BYTE *x, const BYTE *y )
{
    while ( toupper(*x) == toupper(*y) ) {
		if ( *x == 0 ) {
			return 0 ;
		}
		x ++, y ++ ;
    }
    return toupper(*x) - toupper(*y) ;
}

void
StrAppend( BYTE **_dst, const BYTE *src )
{
    int		len = 0 ;
    BYTE	*dst = *_dst ;

    while ( (*dst++ = *src++) != 0 ) {
		len ++ ;
    }
    (*_dst) += len ;
}

BYTE*
Strchr( const BYTE *p, int cc )
{
    int		c ;

    while ( ( c = *p ) != 0 ) {
		if ( c == cc ) {
			return (BYTE*) p ;
		}
		p ++ ;
	}
    return 0 ;
}

BYTE*
Strichr( const BYTE *p, int cc )
{
    int		c ;

    cc = toupper(cc) ;
    while ( ( c = toupper(*p) ) != 0 ) {
		if ( c == cc ) {
			return (BYTE*) p ;
		}
		p ++ ;
    }
    return 0 ;
}

/*----------------------------------------------------------------------------
 * データ・文字列変換関連
 *--------------------------------------------------------------------------*/
WORD
Atoi( const BYTE *buf )
{
    WORD	result = 0 ;
    int		c, f_first = 1 ;
    while ( ( c = *buf++ ) != 0 ) {
		if ( c == ' ' && f_first ) {
			continue ;
		} else if ( c < '0' || c > '9' ) {
			break ;
		}
		f_first = 0 ;
		result *= 10U ;
		result += c - '0' ;
    }
    return result ;
}

int
XAtoi( const BYTE *buf )
{
    int		result = 0 ;
    int		c, f_first = 1 ;
    while ( ( c = *buf++ ) != 0 ) {
		c = toupper( c ) ;
		if ( c == ' ' && f_first ) {
			continue ;
		} else if ( !Isdigit(c) && (c < 'A' || c > 'F') ) {
			break ;
		}
		f_first = 0 ;
		result += result ;
		result += result ;
		result += result ;
		result += result ;
		if ( Isdigit( c ) ) {
			result += c - '0' ;
		} else {
			result += (c - 'A') + 10 ;
		}
    }
    return result ;
}

void
Itoa( int v, BYTE *buf )
{
    unsigned	uv = v ;
    int		r, first = 1 ;

    if ( v < 0 ) {
		uv = *(unsigned*) &v ;
    }
    for ( r = 10000 ; r > 0 ; r /= 10 ) {
		int	c = uv / r + '0' ;
		if ( !first || c != '0' ) {
			*buf++ = c ;
			first = 0 ;
		}
		uv %= r ;
    }
    if ( first ) {
		*buf++ = '0' ;
    }
    *buf = 0 ;
}

void
Itoxa( WORD v, BYTE *buf )
{
    int		i ;
    WORD	num ;
#if 0
    unsigned	uv = v ;
    if ( v < 0 ) {
		uv = *(unsigned*) &v ;
    }
#endif
    for ( i = 0 ; i < 4 ; i ++ ) {
		num = v & 0x0F ;
		v >>= 4 ;
		buf[ 3 - i ] = num >= 10 ? num - 10 + 'A' : num + '0' ;
    }
    buf[ 4 ] = 0 ;
}

void
ItoaN_pad( int v, BYTE *buf, int keta, int pad )
{
    int		len ;

    Itoa( v, Tmpbuf ) ;
    len = Strlen( Tmpbuf ) ;
    Memset( buf, pad, keta ) ;
    buf[ keta ] = 0 ;
    if ( len >= keta ) {
		Memcpy( buf, Tmpbuf, keta ) ;
    } else {
		Memcpy( &buf[ keta - len ], Tmpbuf, len ) ;
    }
}

int
Isdigit( int c )
{
    return c >= '0'&& c <= '9' ;
}

int
Isspace( int c )
{
    return c == ' ' || (c >= 9 && c <= 13) ;
}

int
Fwrite_str( void* buf )
{
    return Fwrite( buf, Strlen( buf ) ) ;
}

int
Fwrite_num( int num )
{
    Itoa( num, Tmpbuf ) ;
    return Fwrite( Tmpbuf, Strlen( Tmpbuf ) ) ;
}

int
Fwrite_hex( WORD num )
{
    Itoxa( num, Tmpbuf ) ;
    return Fwrite( Tmpbuf, Strlen( Tmpbuf ) ) ;
}

/*----------------------------------------------------------------------------
 * 日本語処理関連
 *--------------------------------------------------------------------------*/
#ifndef	NO_ROMA
#include	"roma.c"
#endif	/* NO_ROMA */

int
nthctype( const BYTE *str, int nth )
{
    const BYTE	*p = str ;

    while ( *p && nth > 0 ) {
		if ( iskanji( 0xFF & *p ) ) {
			p ++, nth -- ;
		}
		p ++, nth -- ;
    }
    if ( nth > 0 ) {
		return CT_ILGL ;
    } else if ( nth < 0 ) {
		return CT_KJ2 ;
    } else {
		return iskanji( 0xFF & *p ) ? CT_KJ1 : CT_ANK ;
    }
}

/*----------------------------------------------------------------------------
 * 文字列確保関連
 *--------------------------------------------------------------------------*/
#define		MAX_STRING	6000
#define		MAX_STRBUF	32768L

long	StrDataOfs = 0L ;
int		StrNext = 0 ;				/* 使用しているインデックスの数 */
WORD	StrSize = 0 ;				/* 文字列領域の大きさ */
int		StrIndexNum = 0 ;			/* インデックス領域の大きさ */
HLOCAL	StrIdxHandle = NULL ;		/* 文字列インデックス領域のメモリハンドル */
HLOCAL	StrPtrHandle = NULL ;		/* 文字列データ領域のメモリハンドル */
int		Max_stridx = MAX_STRING ;	/* 初期時に確保する文字列数 */
WORD	Max_strbuf = MAX_STRBUF ;	/* 初期時に確保する文字列バッファ量 */

void
StringAllocN( int num_index, WORD num_str )
{
    if ( StrPtrHandle == NULL ) {
		/* メモリ確保 */
		StrIdxHandle = LocalAlloc( LPTR, sizeof (WORD) * num_index ) ;
		StrPtrHandle = LocalAlloc( LPTR, num_str ) ;
		/* 変数の初期化 */
		StrIndexNum  = num_index ;
    }
}

void
StringFree( void )
{
    /* ハンドル初期化 */
    StrIndexNum = 0 ;
    StrNext = 0 ;
    StrSize = 0 ;
    /* メモリ解放 */
	if ( StrIdxHandle ) {
		LocalFree( StrIdxHandle ) ;
		StrIdxHandle = NULL ;
	}
	if ( StrPtrHandle ) {
		LocalFree( StrPtrHandle ) ;
		StrPtrHandle = NULL ;
	}
}

const BYTE*
StringGet( int str_index )
{
    WORD	index ;

    index = ((WORD*) StrIdxHandle)[ str_index ] ;
    return &((BYTE*) StrPtrHandle)[ index ] ;
}

int
StringAlloc( const BYTE* src )
{
    BYTE	* dst ;
    int		len = Strlen( src ), i ;

    if ( StrPtrHandle == NULL ) {
		StringAllocN( Max_stridx, Max_strbuf ) ;
		StrSize = 0 ;
		StrNext = 0 ;
    }
    if ( StrSize + len >= Max_strbuf ) {
		fatal( "E093:Insufficient string(-b)" ) ;
    }
    if ( StrNext >= StrIndexNum ) {
		fatal( "E094:Insufficient string(-i)" ) ;
    }
    dst = &((BYTE*) StrPtrHandle)[ StrSize ] ;
    for ( i = 0 ; i < len ; i ++ ) {
		*dst++ = *src++ ;
    }
    *dst++ = 0 ;
    ((WORD*) StrIdxHandle)[ StrNext ] = StrSize ;
    StrSize += len + 1 ;
    return StrNext ++ ;
}

/*----------------------------------------------------------------------------
 * その他
 *--------------------------------------------------------------------------*/
static	BYTE	Argbuf[512] = "" ;
int		Argc = 0 ;
BYTE		*Argv[10] ;

static	void
rtrim( BYTE *start, BYTE *ptr )
{
    while ( start != ptr ) {
		int	c = *(-- ptr) ;
		if ( c == ' ' || c == '\t' ) {
			*ptr = 0 ;
		} else {
			break ;
		}
    }
}

void
parse( const BYTE buf[], int sep, int comment )
{
    int		c, f_skip ;
    BYTE	*p = Argbuf ;

    Strcpy( Argbuf, buf ) ;
    Argc = 0 ;
    Argv[ Argc ] = Argbuf ;
    f_skip = 1 ;
    while ( ( c = 0xFF & *p ) != 0 ) {
		if ( c == comment ) {
			rtrim( Argv[ Argc ], p ) ;
			*p = 0 ;
			break ;
		} else if ( iskanji( c ) ) {
			p += 2 ;
			f_skip = 0 ;
		} else if ( c == sep ) {
			rtrim( Argv[ Argc ], p ) ;
			*p++ = 0 ;
			f_skip = 1 ;
			Argv[ ++ Argc ] = p ;
		} else if ( (c == ' ' || c == '\t') && f_skip ) {
			p ++ ;
			Argv[ Argc ] = p ;
		} else {
			p ++ ;
			f_skip = 0 ;
		}
    }
    Argc ++ ;
}

/*
 * 「時間：分」の形式の文字列を調べ、分単位の値を返す
 */
BOOL
parse_time( const BYTE *timestr, int *min )
{
    int		flag = 0, h = 0, m, c ;
    const BYTE	*str = timestr ;
    BYTE	*dst = Tmpbuf ;

    while ( ( c = (0xFF & *str++) ) != 0 ) {
		if ( c == ':' ) {
			flag = 1 ;
			*dst = 0 ;
			h = Atoi( Tmpbuf ) ;
			dst = Tmpbuf ;
		} else {
			*dst++ = c ;
		}
    }
    if ( !flag ) {
		return FALSE ;
    }
    *dst = 0 ;
    m = Atoi( Tmpbuf ) ;
    *min = h * 60 + m ;
    return TRUE ;
}

/*
 * 分単位の値から、「時間：分」の文字列を作成する
 */
void
make_timestr( int min, BYTE *buf )
{
    min = (min + MIN_PER_DAY) % MIN_PER_DAY ;
    ItoaN_pad( min / 60, &buf[0], 2, '0' ) ;
    buf[2] = ':' ;
    ItoaN_pad( min % 60, &buf[3], 2, '0' ) ;
}

/*
 * 分単位の値から、「時間：分」の文字列を作成する(96時間制)
 */
void
make_timestr96( int min, BYTE *buf )
{
    min = (min + MIN_PER_4DAY) % MIN_PER_4DAY ;
    ItoaN_pad( min / 60, &buf[0], 2, '0' ) ;
    buf[2] = ':' ;
    ItoaN_pad( min % 60, &buf[3], 2, '0' ) ;
}

/*
 * Assert の本体
 */
void
__assertfail( char *__cond, char *__file, int __line )
{
    BYTE	linestr[ 10 ], *p = Tmpbuf ;

    StrAppend( &p, "Assertion failed: " ) ;
    StrAppend( &p, __cond ) ;
    StrAppend( &p, ", file " ) ;
    StrAppend( &p, __file ) ;
    StrAppend( &p, ", line " ) ;
    Itoa( __line, linestr ) ;
    StrAppend( &p, linestr ) ;
    StrAppend( &p, "\n" ) ;
    fatal( Tmpbuf ) ;
}

#if 0
/*
 * パラメータ処理
 */
static	WORD	Flag_param[2] = {0,0} ;	/* パラメータ設定フラグ */

int
check_param( int index )
{
    int		aridx = index / 16 ;
    WORD	armask = 2 << (index % 16) ;

    if ( Flag_param[ aridx ] & armask ) {
		return 0 ;
    }
    Flag_param[ aridx ] |= armask ;
    return -1 ;
}

int
set_bool( const BYTE *buf, int if_null )
{
    return *buf ? Atoi( buf ) : if_null ;
}
#endif
