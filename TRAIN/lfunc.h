/*
 * LCOMMON.C, LSYSMGR �p��`�t�@�C��   Copyright 1996, Eiichiroh Itoh
 *
 * �쐬��:96/02/08:�ɓ� �h��Y(GHC02331@niftyserve.or.jp)
 *
 */
#ifndef	__LFUNC
#define	__LFUNC

#ifndef	STRICT
#define	STRICT
#endif	/* STRICT */
#ifndef	UNICODE
#define	UNICODE
#endif	/* UNICODE */
#ifndef	_UNICODE
#define	_UNICODE
#endif	/* _UNICODE */

#include	<windows.h>
#include	<tchar.h>

#undef	isalpha

#ifdef	USE_TC
#undef	const
#endif	/* USE_TC */

#define	Assert(p) ((p) ? (void)0 : (void) __assertfail( #p, __FILE__, __LINE__ ) )
#define	Abs(x)		((x)<0?-(x):(x))
#define	toupper(x)	(((x)>='a'&&(x)<='z')?(x)-'a'+'A':(x))
#define	tolower(x)	(((x)>='A'&&(x)<='Z')?(x)-'A'+'a':(x))
#define	isalpha(x)	(((x)>='A'&&(x)<='Z')||((x)>='a'&&(x)<='z'))
#define	iskana(x)	((x)>=0xA0 && (x)<=0xDF)
#define	iskanji(x)	(((x)>=0x81 && (x)<=0x9F)||((x)>=0xE0 && (x)<=0xFC))
#define	CT_ANK		(0)
#define	CT_KJ1		(1)
#define	CT_KJ2		(2)
#define	CT_ILGL		(-1)
#define	MIN_PER_DAY	(24*60)
#define	MIN_PER_4DAY	(96*60)
#define	NORM_MIN(x)	(((x)+MIN_PER_DAY)%MIN_PER_DAY)

/*----------------------------------------------------------------------------
 * �֐���`
 *--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/* �������E�����񑀍�֐� */
void	Memset( void *dst, int c, int len ) ;
void	Memcpy( void *dst, const void *src, int len ) ;
int		Memcmp( const void *x, const void *y, int len ) ;
int		Strlen( const BYTE *str ) ;
void	Strcpy( LPBYTE dst, const BYTE *src ) ;
int		Strcmp( const BYTE *x, const BYTE *y ) ;
int		Strcmpi( const BYTE *x, const BYTE *y ) ;
void	StrAppend( BYTE **_dst, const BYTE *src ) ;
BYTE	*Strchr( const BYTE *p, int cc ) ;
BYTE	*Strichr( const BYTE *p, int cc ) ;

/* �f�[�^�E������ϊ��֐� */
WORD	Atoi( const BYTE *buf ) ;
int		XAtoi( const BYTE *buf ) ;
long	Atol( const BYTE *buf ) ;
long	XAtol( const BYTE *buf ) ;
void	Itoa( int v, BYTE *buf ) ;
void	Itoxa( WORD v, BYTE *buf ) ;
void	Ltoa( long v, BYTE *buf ) ;
void	ItoaN_pad( int v, BYTE *buf, int keta, int pad ) ;
#define	ItoaN( v, b, k )	ItoaN_pad( (v), (b), (k), ' ' )
#define	ItoaZN( v, b, k )	ItoaN_pad( (v), (b), (k), '0' )
int		Isdigit( int c ) ;
int		Isspace( int c ) ;

/* �����x���t�@�C���֐� */
int		process_file( const BYTE fn[], int (*proc)( const BYTE buf[] ) ) ;
int		FreadBuf( void* buf, int size ) ;

/* �჌�x���t�@�C���֐� */
BOOL	Fexist( LPCTSTR fn ) ;
BOOL	Fopen( LPCTSTR fn, LPCTSTR mode ) ;
int		Fread( void* buf, int size ) ;
WORD	Fread_far( void* buf, WORD size ) ;
int		Fwrite( void* buf, int size ) ;
int		Fwrite_far( void* buf, WORD size ) ;
int		Fwrite_str( void* buf ) ;
int		Fwrite_num( int num ) ;
int		Fwrite_hex( WORD num ) ;
void	Fclose( void ) ;
void	Fseek( long ptr, int whence ) ;
long	Ftell( void ) ;
void	Unlink( const BYTE *fn ) ;
int		Rename( const BYTE *src, const BYTE *dst ) ;
int		Copy( const BYTE *src, const BYTE *dst ) ;

/* �f�B���N�g���擾�֐� */
int		FindFirst( BYTE pat[], BYTE fn[] ) ;
int		FindNext( BYTE fn[] ) ;

///* NEAR �������擾�֐� */
//void*	Malloc( int size ) ;
//void	Free( void * ) ;
//int		LastAlloc() ;
//void	InitAlloc( WORD size ) ;

/* ���{�ꏈ���֐� */
void	roma2kana( const BYTE *src, LPBYTE _dst ) ;
void	kana2roma( const BYTE *src, LPBYTE _dst ) ;
int		nthctype( const BYTE *str, int nth ) ;
void	parse( const BYTE buf[], int sep, int comment ) ;

const BYTE*	han2zen_str( const BYTE *src ) ;

/* �Œ蕶����擾�֐� */
void	StringAllocN( int num_index, WORD num_str ) ;
void	StringFree( void ) ;
int		StringAlloc( const BYTE* src ) ;
const BYTE*	StringGet( int index ) ;

/* �჌�x���N���b�v�{�[�h�֐� */
int	CBopen( BYTE *name, BYTE *mode ) ;
void	CBwrite( void* buf, int size ) ;
void	CBread( void* buf, int size ) ;
int	CBgetsize( void ) ;
void	CBclose( void ) ;

/* �N���b�v�{�[�h�֐� */
int	cb_read( BYTE buf[], int *size ) ;

/* ���t�E�����֐� */
void	get_nowstr( BYTE buf[] ) ;

/* �C���[�W�\�� */
#define		REP_FORCE	0
#define		REP_AND		1
#define		REP_OR		2
#define		REP_XOR		3
#define		REP_NOTFORCE	4
#define		REP_NOTAND	5
#define		REP_NOTOR	6
#define		REP_NOTXOR	7
#define		REP_BITBLT	8

void	get_image( int x1, int y1, int x2, int y2, const BYTE *buf ) ;
void	put_image( int x, int y, const BYTE *buf, int reprule ) ;
void	HourglassOn( int x, int y ) ;
void	HourglassOff( void ) ;

/* ���b�Z�[�W�֐� */
void	read_msg( const BYTE *fn ) ;
BYTE*	get_msg( int kind, int idx ) ;
void	fatal_msg( int idx ) ;

/* ���̑��֐� */
int	parse_time( const BYTE *timestr, int *min ) ;
void	make_timestr( int min, BYTE *buf ) ;
void	make_timestr96( int min, BYTE *buf ) ;
void	fatal( const BYTE *str ) ;
void	wcs_fatal( LPCTSTR str ) ;
int	process_cb( int (*p)(const BYTE *b), BYTE *cb,int s,const BYTE *t );
int	check_param( int index ) ;
int	set_bool( const BYTE *buf, int if_null ) ;
void	copy_dirname( BYTE *dst, const BYTE *src ) ;
void	__assertfail( char *__cond, char *__file, int __line ) ;
void	Puts( const BYTE *buf ) ;
int	Kbhit( void ) ;
unsigned int	Getch( void ) ;
void	SendKey( WORD c ) ;

extern	int	Argc ;
extern	BYTE	*Argv[10] ;

extern	int	F_SaveMem ;
extern	int	F_noconv ;

extern	long	StrDataOfs ;
extern	int		StrNext ;
extern	WORD	StrSize ;
extern	HLOCAL	StrIdxHandle ;
extern	HLOCAL	StrPtrHandle ;
extern	int		Max_stridx ;	/* �������Ɋm�ۂ��镶���� */
extern	WORD	Max_strbuf ;	/* �������Ɋm�ۂ��镶����o�b�t�@�� */

extern	BYTE	EmptyStr[] ;

#ifdef __cplusplus
} ;
#endif
#endif	/* __LFUNC */
