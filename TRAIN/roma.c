#ifdef	MAIN
#include	<windows.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#endif	/* MAIN */

#define		Strlen(s)	strlen((s))
#define		Strcpy(d,s)	strcpy((d),(s))
#define		iskanji(x)	(((x)>=0x81 && (x)<=0x9F)||((x)>=0xE0 && (x)<=0xFC))

int		F_noconv = 0 ;

#undef	isalpha
#undef	isvowel
#define	isalpha(x)	(((x)>='A'&&(x)<='Z')||((x)>='a'&&(x)<='z'))
#define	isvowel(x)	((x)=='a'||(x)=='i'||(x)=='u'||(x)=='e'||(x)=='o')
#define	isvowel2(x)	((x)=='a'||(x)=='i'||(x)=='u'||(x)=='e'||(x)=='o'||(x)=='n'||(x)=='y')

/*
 * ローマ字→かな変換
 */
typedef	enum roma_mode {
    roma_none = 0,
    roma_k, roma_s, roma_t, roma_n, roma_h, roma_f, roma_m,
    roma_y, roma_r, roma_w,
    roma_l, roma_c, roma_g, roma_z, roma_j, roma_d, roma_b, roma_p,
    roma_v, roma_ky, roma_sy, roma_sh, roma_ty, roma_ch, roma_ts, roma_ny,
    roma_hy, roma_fy, roma_my, roma_ry,
    roma_gy, roma_zy, roma_dy, roma_dh, roma_by, roma_py,
    roma_ly, roma_lt,
    roma_hw, roma_kw, roma_nw,
    roma_gw
} ROMA_MODE ;

typedef enum kana_code {
    kana_a = 0xB1, kana_k = 0xB6, kana_s = 0xBB, kana_t = 0xC0, kana_n = 0xC5,
    kana_h = 0xCA, kana_m = 0xCF, kana_y = 0xD4, kana_r = 0xD7, kana_w = 0xDC,
    kana_wo = 0xA6, kana_nn = 0xDD, kana_daku = 0xDE, kana_handaku = 0xDF,
    kana_l = 0xA7, kana_ly = 0xAC, kana_ltu = 0xAF,
	kana_touten = 0xA1, kana_left = 0xA2, kana_right = 0xA3,
	kana_kuten = 0xA4, kana_naka = 0xA5
} KANA_CODE ;

static	BYTE	alpha_table[] = {
    0xFF, 0x00, roma_b, roma_c, roma_d, 3, roma_f, roma_g, roma_h, 1, roma_j,
    roma_k, roma_l, roma_m, roma_n, 4,
    roma_p, roma_none, roma_r, roma_s, roma_t, 2, roma_v, roma_w, roma_l,
    roma_y, roma_z, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
} ;

static	BYTE	kana_table[] = {
    kana_a,
    kana_k, kana_s, kana_t, kana_n, kana_h, kana_h, kana_m,
    kana_y, kana_r, kana_w,
    kana_l, kana_s, kana_k, kana_s, kana_s, kana_t, kana_h, kana_h,
    kana_a, kana_k, kana_s, kana_s, kana_t, kana_t, kana_t, kana_n,
    kana_h, kana_h, kana_m, kana_r,
    kana_k, kana_s, kana_t, kana_t, kana_h, kana_h,
    kana_ly, kana_ltu,
    kana_h, kana_k, kana_n,
    kana_k
} ;

/* 拗音への対処(インデックスは rmode) */
static	BYTE	youon_table[] = {
    roma_none,
    roma_ky, roma_sy, roma_ty, roma_ny, roma_hy, roma_fy, roma_my,
    roma_none, roma_ry, roma_none,
    roma_ly, roma_ty, roma_gy, roma_zy, roma_zy, roma_dy, roma_by, roma_py,
} ;
int		num_youon = sizeof youon_table / sizeof youon_table[0] ;

/* ハングル文字への対処(インデックスは rmode) */
static	BYTE	hankul_table[] = {
    roma_none,
    roma_kw, roma_none, roma_none, roma_nw, roma_hw, roma_none, roma_none,
    roma_none, roma_none, roma_none,
    roma_none, roma_none, roma_gw
} ;
int		num_hankul = sizeof hankul_table / sizeof hankul_table[0] ;

BYTE
get_kana_ly( BYTE idx )
{
    switch ( idx ) {
    case 0:	return kana_ly + 0 ;
    case 1:	return kana_l + 1 ;
    case 2:	return kana_ly + 1 ;
    case 3:	return kana_l + 3 ;
    case 4:	return kana_ly + 2 ;
    }
    return 0 ;
}

DWORD
is_dakuon( DWORD mode )
{
    if ( ( mode >= roma_g && mode <= roma_b )
      || ( mode == roma_v )
      || ( mode >= roma_gy && mode <= roma_by )
      || ( mode == roma_gw ) ) {
		return 1 ;
    }
    return 0 ;
}

void
roma2kana( const BYTE *src, LPBYTE _dst )
{
    const BYTE*		p = src ;
    LPBYTE			dst = _dst ;
    ROMA_MODE		rmode = roma_none ;
    BYTE			c ;

    while ( (c = *p++) != 0 ) {
		BYTE		idx ;

//printf( "rmode=%d, c=%c\n", rmode, c ) ;
		if ( iskanji( c ) ) {
			*dst++ = c ;
			*dst++ = *p++ ;
			continue ;
		} else if ( !isalpha(c) ) {
			if ( c == '.' ) {
				c = kana_touten ;
			} else if ( c == '[' ) {
				c = kana_left ;
			} else if ( c == ']' ) {
				c = kana_right ;
			} else if ( c == ',' ) {
				c = kana_kuten ;
			} else if ( c == '/' ) {
				c = kana_naka ;
			}
			if ( rmode == roma_n ) {
				*dst++ = kana_nn ;
				if ( c != '\'' && c != ' ' ) {
					*dst++ = c ;
				}
			} else {
				*dst++ = c ;
			}
			rmode = roma_none ;
			continue ;
		}
		c = tolower(c) ;
		idx = alpha_table[c-'`'] ;
		if ( rmode != roma_none ) {
			if ( rmode < num_youon && c == 'y' ) {
				idx = youon_table[ rmode ] ;
				if ( idx != roma_none ) {
					rmode = (ROMA_MODE) idx ;
					continue ;
				}
			} else if ( rmode < num_hankul && c == 'w' ) {
				idx = hankul_table[ rmode ] ;
				if ( idx != roma_none ) {
					rmode = (ROMA_MODE) idx ;
					continue ;
				}
			}
		}
		if ( rmode == roma_t && c == 'c' ) {
			rmode = roma_c ;
		}
		if ( !isvowel( c ) && idx == rmode && rmode != roma_n && rmode != roma_m && rmode != roma_none ) {
			*dst++ = kana_ltu ;
			continue ;
		} else if ( rmode == roma_m && (idx == roma_m || idx == roma_b || idx == roma_p ) ) {
			*dst++ = kana_nn ;
			rmode = (ROMA_MODE) idx ;
			continue ;
#ifdef	DItoDHI
		} else if ( rmode == roma_d && c == 'i' ) {
			rmode = roma_dh ;
#endif	/* DItoDHI */
		}
		switch ( rmode ) {
		case roma_none:	case roma_l:	case roma_s:
		case roma_t:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else if ( rmode == roma_l && c == 't' ) {
				rmode = roma_lt ;
			} else if ( rmode == roma_s && c == 'h' ) {
				rmode = roma_sh ;
			} else if ( rmode == roma_t && c == 's' ) {
				rmode = roma_ts ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_n:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else if ( c == 'n' ) {
				*dst++ = kana_nn ;
				rmode = roma_none ;
			} else {
				*dst++ = kana_nn ;
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_b:	case roma_g:	case roma_k:
		case roma_m:	case roma_p:	case roma_r:
		case roma_z:	case roma_d:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				} else if ( rmode == roma_p ) {
					*dst++ = kana_handaku ;
				}
			} else if ( rmode == roma_d && c == 'h' ) {
				rmode = roma_dh ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_h:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + idx ;
			} else {
				*dst++ = kana_a + 2 ;
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_c:	case roma_lt:
			if ( rmode == roma_c && c == 'h' ) {
				rmode = roma_ch ;
			} else if ( rmode == roma_lt && c == 'u' ) {
				*dst++ = kana_ltu ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_f:	case roma_v:	case roma_ts:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				if ( c != 'u' ) {
					*dst++ = kana_l + idx ;
				}
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_w:
			if ( c == 'a' ) {
				*dst++ = kana_w ;
			} else if ( c == 'i' || c == 'e' ) {
				*dst++ = kana_a + 2 ;
				*dst++ = kana_l + idx ;
			} else if ( c == 'u' ) {
				*dst++ = kana_a + 2 ;
			} else if ( c == 'o' ) {
#ifdef	HANKUL
				*dst++ = kana_a + 2 ;
				*dst++ = kana_l + 4 ;
#else	/* HANKUL */
				*dst++ = kana_wo ;
#endif	/* HANKUL */
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_y:
			if ( c == 'a' ) {
				*dst++ = kana_y + 0 ;
			} else if ( c == 'i' ) {
				*dst++ = kana_a + 1 ;
			} else if ( c == 'u' ) {
				*dst++ = kana_y + 1 ;
			} else if ( c == 'e' ) {
				*dst++ = kana_a + 1 ;
				*dst++ = kana_l + 3 ;
			} else if ( c == 'o' ) {
				*dst++ = kana_y + 2 ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_ky:	case roma_sy:	case roma_ty:
		case roma_ny:	case roma_hy:	case roma_my:
		case roma_ry:
		case roma_gy:	case roma_zy:	case roma_dy:
		case roma_by:	case roma_py:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 1 ;
				if ( rmode == roma_py ) {
					*dst++ = kana_handaku ;
				} else if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_j:	case roma_sh:	case roma_ch:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 1 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				if ( c != 'i' ) {
					*dst++ = get_kana_ly( idx ) ;
				}
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_fy:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_ly:
			if ( isvowel( c ) ) {
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_dh:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 3 ;
				*dst++ = kana_daku ;
				*dst++ = get_kana_ly( idx ) ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		case roma_gw:
		case roma_hw:
		case roma_kw:
		case roma_nw:
			if ( isvowel( c ) ) {
				*dst++ = kana_table[ rmode ] + 2 ;
				if ( is_dakuon( rmode ) ) {
					*dst++ = kana_daku ;
				}
				*dst++ = kana_l + idx ;
			} else {
				rmode = (ROMA_MODE) idx ;
			}
			break ;
		default:
			break ;
		}
		if ( isvowel( c ) ) {
			rmode = roma_none ;
		}
	}
	if ( rmode == roma_n ) {
		*dst++ = kana_nn ;
    }
    *dst = 0 ;
}

/*
 * かな→ヘボン式ローマ字変換
 */
typedef enum {
	con_a = 0, con_k, con_s, con_t, con_n,
	con_h, con_m, con_y, con_r, con_w,
	con_g, con_z, con_d, con_b, con_p,
	con_ky, con_sy, con_ty, con_ny, con_hy, con_my, con_ry,
	con_gy, con_zy, con_dy, con_by, con_py,
	con_kw, con_gw,
	con_si, con_ti, con_tu,
	con_zi, con_di, con_du,
	con_f,
	con_v, con_wi,
	con_l, con_ltu, con_ly,
	con_nn,
	con_tyo,
	con_null,
	con_err,
} consonant ;

char	pr_vowel[] = "aiueo" ;
char	*pr_con[] = {
	"", "k", "s", "t", "n",
	"h", "m", "y", "r", "w",
	"g", "z", "d", "b", "p",
	"ky", "sh", "ch", "ny", "hy", "my", "ry",
	"gy", "j", "dy", "by", "py",
	"kw", "gw",
	"s", "t", "t",
	"z", "d", "d",
	"f",
	"v", "w",
	"l", "ltu", "ly",
} ;

struct con_table {
	BYTE	con ;
	BYTE	vowel ;
	WORD	flag ;
} Con_table[] = {
	/*A6*/{con_w,4},
	/*A7*/{con_l,0,1}, {con_l,1,1}, {con_l,2,1}, {con_l,3,1}, {con_l,4,1}, 
	/*AC*/{con_ly,0,2}, {con_ly,2,2}, {con_ly,4,2},
	/*AF*/{con_ltu,2,5}, {con_tyo,0,6},
	/*B1*/{con_a,0}, {con_a,1}, {con_a,2}, {con_a,3}, {con_a,4},
	/*B6*/{con_k,0}, {con_k,1}, {con_k,2}, {con_k,3}, {con_k,4},
	/*BB*/{con_s,0}, {con_s,1}, {con_s,2}, {con_s,3}, {con_s,4},
	/*C0*/{con_t,0}, {con_t,1}, {con_t,2}, {con_t,3}, {con_t,4},
	/*C5*/{con_n,0}, {con_n,1}, {con_n,2}, {con_n,3}, {con_n,4},
	/*CA*/{con_h,0}, {con_h,1}, {con_h,2}, {con_h,3}, {con_h,4},
	/*CF*/{con_m,0}, {con_m,1}, {con_m,2}, {con_m,3}, {con_m,4},
	/*D4*/{con_y,0}, {con_y,2}, {con_y,4},
	/*D7*/{con_r,0}, {con_r,1}, {con_r,2}, {con_r,3}, {con_r,4},
	/*DC*/{con_w,0},
	/*DD*/{con_nn,0},
	/*DE*/{con_err,0,3},
	/*DF*/{con_err,0,4},
} ;
#define		CON_START	0xA6
#define		CON_END		0xDF

struct transtable {
	BYTE	con ;
	BYTE	vowel ;
	BYTE	next_con[ 4 ] ;
} ;

const struct transtable	TransTBL1[] = {
	{ con_a, 2, { con_wi, con_err, con_v, con_err } },
	{ con_k, 2, { con_kw, con_ky, con_g, con_err } },
	{ con_k, 5, { con_err, con_ky, con_g, con_err } },
	{ con_s, 2, { con_si, con_sy, con_z, con_err } },
	{ con_s, 5, { con_err, con_sy, con_z, con_err } },
	{ con_t, 3, { con_ti, con_ty, con_d, con_err } },
	{ con_t, 4, { con_ti, con_ty, con_d, con_err } },
	{ con_t, 5, { con_err, con_ty, con_d, con_err } },
	{ con_n, 5, { con_err, con_ny, con_err, con_err } },
	{ con_h, 2, { con_f, con_hy, con_b, con_p } },
	{ con_h, 5, { con_err, con_hy, con_b, con_p } },
	{ con_m, 5, { con_err, con_my, con_err, con_err } },
	{ con_r, 5, { con_err, con_ry, con_err, con_err } },
	{ con_g, 2, { con_gw, con_gy, con_err, con_err } },
	{ con_g, 5, { con_err, con_gy, con_err, con_err } },
	{ con_z, 2, { con_zi, con_zy, con_err, con_err } },
	{ con_z, 5, { con_err, con_zy, con_err, con_err } },
	{ con_d, 3, { con_di, con_dy, con_err, con_err } },
	{ con_d, 4, { con_di, con_dy, con_err, con_err } },
	{ con_d, 5, { con_err, con_dy, con_err, con_err } },
	{ con_b, 5, { con_err, con_by, con_err, con_err } },
	{ con_p, 5, { con_err, con_py, con_err, con_err } },
} ;
const	int		Num_transtbl1 = sizeof (TransTBL1) / sizeof (TransTBL1[0]) ;

static	int
transfer1( BYTE con, BYTE vowel, BYTE c, LPBYTE ncon, LPBYTE nv )
{
	int		i ;
	WORD	flag ;
	BYTE	con2, v2 ;

	if ( c < CON_START || c > CON_END ) {
		return 1 ;
	}
	c -= CON_START ;
	flag = Con_table[ c ].flag ;
	if ( flag == 0 ) {
		return 1 ;
	} else if ( flag == 5 ) {
		return 2 ;
	} else if ( flag == 6 ) {
		return 3 ;
	}
	for ( i = 0 ; i < Num_transtbl1 ; i ++ ) {
		if ( con == TransTBL1[ i ].con ) {
			v2 = TransTBL1[ i ].vowel ;
			if ( vowel == v2 || v2 == 5 ) {
				con2 = TransTBL1[ i ].next_con[ flag - 1 ] ;
				if ( con2 != con_err ) {
					*ncon = con2 ;
					if ( flag == 1 || flag == 2 ) {
						*nv = Con_table[ c ].vowel ;
					}
					return 0 ;
				}
				break ;
			}
		}
	}
	return 1 ;
}

void
kana2roma( const BYTE *src, LPBYTE _dst )
{
	const BYTE*	p = src ;
	LPBYTE		dst = _dst, dst2 ;
	int			ret ;
	BYTE		con = con_null ;
	BYTE		vowel ;
	BOOL		f_ltsu = FALSE ;
	char		*q ;
	BYTE		c ;

	while ( (c = *p++) != 0 ) {
		BOOL	f_next_ltsu = FALSE ;

		if ( c >= CON_START && c <= CON_END ) {
			c -= CON_START ;
			con   = Con_table[ c ].con ;
			vowel = Con_table[ c ].vowel ;
			ret = transfer1( con, vowel, *p, &con, &vowel ) ;
			if ( ret == 0 ) {
				p ++ ;
				ret = transfer1( con, vowel, *p, &con, &vowel ) ;
				if ( ret == 0 ) {
					p ++ ;
				}
			}
			if ( ret == 2 ) {
				f_next_ltsu = TRUE ;
				p ++ ;
			}
			if ( con == con_nn ) {
				*dst++ = 'n' ;
				*dst++ = '\'' ;
				continue ;
			} else if ( con == con_tyo ) {
				*dst++ = '-' ;
				continue ;
			} else if ( con > con_nn ) {
				continue ;
			}
			if ( con == con_s && vowel == 1 ) {
				q = "sh" ;
			} else if ( con == con_t && vowel == 1 ) {
				q = "ch" ;
			} else if ( con == con_t && vowel == 2 ) {
				q = "ts" ;
			} else if ( con == con_h && vowel == 2 ) {
				q = "f" ;
			} else if ( con == con_z && vowel == 1 ) {
				q = "j" ;
			} else {
				q = pr_con[ con ] ;
			}
			c = *q ;
			if ( f_ltsu && !isvowel( c ) ) {
				*dst++ = *q ;
			}
			Strcpy( (char*) dst, q ) ;
			dst += Strlen( q ) ;
			*dst++ = pr_vowel[ vowel ] ;
			f_ltsu = f_next_ltsu ;
		} else {
			*dst++ = c ;
		}
	}
	*dst = 0 ;
	/* 「ん」に関する補正をおこなう */
	dst = dst2 = _dst ;
	while ( (c = *dst++) != 0 ) {
		if ( c == 'n' && *dst == '\'' ) {
			BYTE	c2 = *(dst + 1) ;
			if ( c2 == 'm' || c2 == 'b' || c2 == 'p' ) {
				c = 'm' ;
			}
			if ( !isvowel2( c2 ) ) {
				dst ++ ;
			}
		} else if ( c == 'c' && *dst == 'c' ) {
			c = 't' ;
		}
		*dst2++ = c ;
	}
	*dst2 = 0 ;
}

#ifdef	MAIN
void
main( int argc, char *argv[] )
{
	BYTE	line[ 100 ] ;
	BYTE	kana[ 100 ] ;
	BYTE	org[ 100 ] ;
	BYTE	roma[ 100 ] ;

	FILE	*fp = fopen( argv[ 1 ], "r" ) ;
	if ( fp == NULL ) {
		return ;
	}
	for ( fgets( line, sizeof line, fp ) ; !feof( fp ) ; fgets( line, sizeof line, fp ) ) {
		sscanf( line, " %[^\r\n]", org ) ;
		roma2kana( org, kana ) ;
		kana2roma( kana, roma ) ;
		sprintf( line, "%s->%s\r\n", org, roma ) ;
		fputs( line, stdout ) ;
	}
	fclose( fp ) ;
}
#endif
