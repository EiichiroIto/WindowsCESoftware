#include	"lfunc.h"
#include	"base.h"

HLOCAL	StationHandle = NULL ;
HLOCAL	RosenHandle = NULL ;
#ifndef	TOOLS
HLOCAL	RStatHandle = NULL ;
#endif	/* TOOLS */
HLOCAL	SortHandle = NULL ;		/* ソートされたローマ字駅名情報 */
HLOCAL	KSortHandle = NULL ;	/* ソートされた漢字駅名情報 */
int		Max_s = MAX_S ;
int		Max_r = MAX_R ;
int		Num_s = 0 ;
int		Num_r = 0 ;
int		Num_rstat = 0 ;
int		F_inter = 0 ;			/* International 版の場合*真* */

/* 利用運行会社選択用領域 */
#define	MAX_KAISYA	(MAX_R/10)
WORD	Kaisya_idx[ MAX_KAISYA ] ;	/* 運行会社名の文字列IDX */
BYTE	Kaisya_len[ MAX_KAISYA ] ;	/* 運行会社名の長さ */
BYTE	Kaisya_use[ MAX_KAISYA ] ;	/* 路線使用フラグ */
int		Num_kaisya = 0 ;		/* 運行会社の個数 */

/*----------------------------------------------------------------------------
 * STATION
 *--------------------------------------------------------------------------*/
int
STATION::cmp( const BYTE *nm ) const
{
    if ( F_inter ) {
		BYTE	tmpbuf1[ MAX_ST_NAME * 2 ], tmpbuf2[ MAX_ST_NAME * 2 ] ;
		roma2kana( (const BYTE *)StringGet( roma_idx ), tmpbuf1 ) ;
		roma2kana( (const BYTE *)nm, tmpbuf2 ) ;
		return Strcmp( tmpbuf1, tmpbuf2 ) ;
    } else {
#ifdef	TOOLS
		return Strcmp( StringGet( name_idx ), nm ) ;
#else	/* TOOLS */
		WORD	c, c2 ;
		const BYTE	*p = StringGet( name_idx ) ;
		const BYTE	*nm_org = nm ;
		const BYTE	*p_org = p ;

		while ( ( c = 0xFF & *p++ ) != 0 ) {
			if ( iskanji( c ) ) {
				c  = (c  << 8) | (0xFF & *p++) ;
				c2 = 0xFF & *nm++ ;
				c2 = (c2 << 8) | (0xFF & *nm++) ;
				if ( c != c2 ) {
					if ( c != 0x8396U || (c2 != 0x8350U && c2 != 0x82AAU) ) {
						return Strcmp( p_org, nm_org ) ;
					}
				}
			} else {
				c2 = 0xFF & *nm++ ;
				if ( c != c2 ) {
					return c < c2 ? -1 : 1 ;
				}
			}
		}
		return *nm ? -1 : 0 ;
#endif	/* TOOLS */
    }
}

const BYTE*
STATION::get_name() const
{
    if ( F_inter ) {
#if 0
		static	char	tmpbuf[ 100 ] ;

		if ( roma_idx < 0 ) {
			return "" ;
		}
		kana2roma( StringGet( roma_idx ), tmpbuf ) ;
		return tmpbuf ;
#else
		return StringGet( roma_idx ) ;
#endif
    } else {
		return StringGet( name_idx ) ;
    }
}

const BYTE*
STATION::get_roma() const
{
    return roma_idx >= 0 ? StringGet( roma_idx ) : EmptyStr ;
}

/*----------------------------------------------------------------------------
 * ROSEN
 *--------------------------------------------------------------------------*/
const BYTE*
ROSEN::get_name() const
{
    return StringGet( name_idx ) ;
}

int
ROSEN::find( int st_id ) const
{
    for ( int i = 0 ; i < num ; i ++ ) {
		if ( get_s_id( i ) == st_id ) {
			return i + 1 ;
		}
    }
    return 0 ;
}

int
ROSEN::diff_time( int time_from, int time_to ) const
{
    if ( time_from < 0 || time_to < 0 ) {
		return -1 ;
    }
    int		tmp ;
    if ( time_from > time_to ) {
		tmp = time_from, time_from = time_to, time_to = tmp ;
    }
    int		result = time_to - time_from ;
    if ( circle ) {
		tmp = time_from + (0xFF & circle) - time_to ;
		if ( tmp < result ) {
			result = tmp ;
		}
    }
    return result ;
}

void
ROSEN::set_use( BOOL _flag )
{
    if ( _flag ) {
		flag |= FLAG_USE ;
    } else {
		flag &= ~FLAG_USE ;
    }
}

#ifndef	TOOLS
int
ROSEN::get_s_id( int idx ) const
{
    const short	*stat_time ;

    stat_time = &((short *) RStatHandle)[ rstat_idx ] ;
    return stat_time[ idx ] ;
}

int
ROSEN::get_time( int idx ) const
{
    const short	*stat_time ;
    stat_time = &((short *) RStatHandle)[ rstat_idx ] ;
    return stat_time[ num + idx ] ;
}
#endif	/* TOOLS */

/*----------------------------------------------------------------------------
 * common function
 *--------------------------------------------------------------------------*/
void
init_station()
{
    Assert( StationHandle == NULL ) ;
    StationHandle = LocalAlloc( LPTR, sizeof (STATION) * Max_s ) ;
    SortHandle = LocalAlloc( LPTR, sizeof (WORD) * Max_s ) ;
    KSortHandle = LocalAlloc( LPTR, sizeof (WORD) * Max_s ) ;
    Num_s = 0 ;
}

#ifndef	TOOLS
void
free_station()
{
	if ( StationHandle ) {
		LocalFree( StationHandle ) ;
		StationHandle = NULL ;
	}
	if ( SortHandle ) {
		LocalFree( SortHandle ) ;
	    SortHandle = NULL ;
	}
	if ( KSortHandle ) {
		LocalFree( KSortHandle ) ;
	    KSortHandle = NULL ;
	}
    Max_s = MAX_S ;
    Num_s = 0 ;
}
#endif	TOOLS

void
init_rosen()
{
    Assert( RosenHandle == NULL ) ;
    RosenHandle = LocalAlloc( LPTR, sizeof (ROSEN) * Max_r ) ;
#ifndef	TOOLS
    Assert( RStatHandle == NULL ) ;
    RStatHandle = LocalAlloc( LPTR, sizeof (short) * Num_rstat * 2 ) ;
#endif	/* TOOLS */
    Num_r = 0 ;
}

#ifndef	TOOLS
void
free_rosen( void )
{
	if ( RStatHandle ) {
		LocalFree( RStatHandle ) ;
		RStatHandle = NULL ;
	}
	if ( RosenHandle ) {
		LocalFree( RosenHandle ) ;
	    RosenHandle = NULL ;
	}
    Max_r = MAX_R ;
    Num_r = 0 ;
    Num_rstat = 0 ;
}
#endif	/* TOOLS */

LPSTATION
get_station( int index )
{
    Assert( index < Num_s ) ;
    return &((LPSTATION) StationHandle)[ index ] ;
}

LPROSEN
get_rosen( int index )
{
    Assert( index < Num_r ) ;
    return &((LPROSEN) RosenHandle)[ index ] ;
}

/*
 * ソート駅名関係
 */

/*
 * 漢字名ソート領域のインデックスから駅ＩＤを得る
 */
int
get_ksort_s_id( int index )
{
    Assert( index >= 0 && index < Num_s ) ;
    return ((WORD *) KSortHandle)[ index ] ;
}

/*
 * 漢字名で駅ＩＤを検索する
 */
BOOL
name2stid( const BYTE *name, LPDWORD pIndex )
{
	if ( Strlen( name ) <= 0 ) {
		return FALSE ;
	}
	int		upper, lower, half, s_id, ret ;
	upper = 0, lower = Num_s - 1 ;
	while ( 1 ) {
		half = (upper + lower) / 2 ;
		s_id = get_ksort_s_id( half ) ;
		ret = get_station( s_id )->cmp( name ) ;
		if ( ret == 0 ) {
			while ( half > 0 ) {
				s_id = get_ksort_s_id( half - 1 ) ;
				if ( get_station( s_id )->cmp( name ) ) {
					break ;
				}
				half -- ;
			}
			*pIndex = get_ksort_s_id( half ) ;
			return TRUE ;
		}
		if ( half == lower ) {
			return FALSE ;
		}
		if ( ret > 0 ) {
			lower = half - 1 ;
		} else {
			upper = half + 1 ;
		}
		if ( lower < upper ) {
			return FALSE ;
		}
	}
}

/*
 * ローマ字名ソート領域のインデックスから駅ＩＤを得る
 */
int
get_sort_s_id( int index )
{
    Assert( index >= 0 && index < Num_s - 1 ) ;
    return ((WORD *) SortHandle)[ index ] ;
}

/*
 * ローマ字名から駅のソートIndexを求める
 */
BOOL
roma2sortidx( const BYTE *roma, LPDWORD index )
{
    BYTE	kana[ 100 ] ;

    roma2kana( roma, kana ) ;
    int		len = Strlen( kana ) ;
    if ( len <= 0 ) {
		return FALSE ;
    }
    int		upper, lower, half, s_id, ret ;
    upper = 0, lower = Num_s - 2 ;
    while ( 1 ) {
		half = (upper + lower) / 2 ;
		s_id = get_sort_s_id( half ) ;
		ret = Memcmp( get_station( s_id )->get_roma(), kana, len ) ;
		if ( ret == 0 ) {
			while ( half > 0 ) {
				s_id = get_sort_s_id( half - 1 ) ;
				if ( Memcmp( get_station( s_id )->get_roma(), kana, len ) ) {
					break ;
				}
				half -- ;
			}
			*index = half ;
			return TRUE ;
		}
		if ( half == lower ) {
			return FALSE ;
		}
		if ( ret > 0 ) {
			lower = half - 1 ;
		} else {
			upper = half + 1 ;
		}
		if ( lower < upper ) {
			return FALSE ;
		}
    }
}

/*
 * 運行会社一覧のためのデータ作成
 */
void
init_kaisya()
{
    int		last_len = 0, r_id, i, f_other = 0 ;
    BYTE	last_name[ 100 ] ;
    BYTE	name[ 100 ] ;

    last_name[0] = name[0] = 0 ;
    Num_kaisya = 0 ;
    for ( r_id = 1 ; r_id < Num_r ; r_id ++ ) {
		ROSEN	*r = get_rosen( r_id ) ;
		int		len = r->get_len_kaisya() ;
		WORD	idx = r->get_name_idx() ;

		Strcpy( name, StringGet( idx ) ) ;
		if ( len == 0 ) {
			f_other = 1 ;
			r->set_kid( KID_NONE ) ;
			continue ;
		}
		if ( last_len != len || Memcmp( last_name, name, len ) != 0 ) {
			for ( i = 0 ; i < Num_kaisya ; i ++ ) {
				WORD	o_idx = Kaisya_idx[ i ] ;
				int		o_len = Kaisya_len[ i ] ;
				const BYTE	*o_name = StringGet( o_idx ) ;
				if ( o_len == len && Memcmp( o_name, name, len ) == 0 ) {
					r->set_kid( i ) ;
					goto next ;
				}
			}
			/* 運行会社を記録する必要がある場合 */
			Assert( Num_kaisya < MAX_KAISYA ) ;
			Kaisya_idx[ Num_kaisya ] = idx ;
			Kaisya_len[ Num_kaisya ] = len ;
			Num_kaisya ++ ;
			/* 後のデータとの比較用 */
			Strcpy( last_name, name ) ;
			last_len = len ;
		}
		r->set_kid( Num_kaisya - 1 ) ;
		next:;
    }
    if ( f_other ) {
		/* その他の運行会社の登録 */
		Assert( Num_kaisya < MAX_KAISYA ) ;
		Kaisya_idx[ Num_kaisya ] = 0 ;
		Kaisya_len[ Num_kaisya ] = 0 ;
		Num_kaisya ++ ;
		for ( r_id = 1 ; r_id < Num_r ; r_id ++ ) {
			ROSEN	*r = get_rosen( r_id ) ;
			if ( r->get_kid() == KID_NONE ) {
				r->set_kid( Num_kaisya - 1 ) ;
			}
		}
    }
}

#ifdef __cplusplus
extern "C" {
#endif
BOOL	han2zen_kanastr( const BYTE *src, LPBYTE dst, DWORD dstlen, BOOL fHira ) ;
#ifdef __cplusplus
} ;
#endif

const BYTE *
get_station_kana( int s_id )
{
	const BYTE	*q ;
    static BYTE	kana[ MAX_ST_NAME * 2 + 1 ] ;

	q = get_station( s_id )->get_roma() ;
	han2zen_kanastr( q, kana, sizeof kana, TRUE ) ;
	return kana ;
}
