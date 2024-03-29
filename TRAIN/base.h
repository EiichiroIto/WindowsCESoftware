typedef	int		STATIONID ;
typedef	int		LINEID ;

#define		MAX_S		3000
#define		MAX_R		400
#define		MAX_ST_NAME	20
#define		MAX_R_NAME	30

#define		SID_NONE	0xFFFF
#define		RID_NONE	0xFFFF
#define		IDX_NONE	0xFFFF

extern "C" {
	extern	HLOCAL	StationHandle ;
	extern	HLOCAL	RosenHandle ;
#ifndef	TOOLS
	extern	HLOCAL	RStatHandle ;
#endif	/* TOOLS */
	extern	HLOCAL	SortHandle ;		/* ソートされたローマ字駅名情報 */
	extern	HLOCAL	KSortHandle ;	/* ソートされた漢字駅名情報 */
	extern	int		Max_s ;
	extern	int		Max_r ;
	extern	int		Num_s ;
	extern	int		Num_r ;
	extern	int		Num_rstat ;
	extern	int		F_inter ;			/* International 版の場合*真* */
	extern	WORD	Kaisya_idx[] ;		/* 運行会社名の文字列IDX */
	extern	BYTE	Kaisya_len[] ;		/* 運行会社名の長さ */
	extern	BYTE	Kaisya_use[] ;		/* 路線利用フラグ */
	extern	int		Num_kaisya ;		/* 運行会社の個数 */

	void	init_station( void ) ;
	void	free_station( void ) ;
	void	init_rosen( void ) ;
	void	free_rosen( void ) ;
	void	load_data( void ) ;
	int		get_ksort_s_id( int index ) ;
	BOOL	name2stid( const BYTE *name, LPDWORD st_id ) ;
	int		get_sort_s_id( int index ) ;
	BOOL	roma2sortidx( const BYTE *roma, LPDWORD index ) ;
	void	init_kaisya() ;
	const BYTE *	get_station_kana( int s_id ) ;

#ifdef	TOOLS
    int		find_rosen( int s_id ) ;
    int		find_station( const BYTE *name, int start_sid = 0 ) ;
    int		add_station( const BYTE *name, int rosen_id ) ;
    int		read_rosen( const BYTE *fn ) ;
    int		read_roma( const BYTE *fn ) ;
    int		find_rosen_name( const BYTE *name ) ;
    void	sort_name_station() ;
    void	sort_roma_station() ;
    void	print_sort() ;
    int		save_data( const BYTE *fn ) ;
#endif	/* TOOLS */
} ;

class AdjLink ;
/*----------------------------------------------------------------------------
 * STATION
 *--------------------------------------------------------------------------*/
class STATION {
	WORD	name_idx ;		/* 駅名 */
	WORD	roma_idx ;		/* ローマ字名 */
#ifdef	TOOLS
	int		first_rosen_id ;	/* 初回登録時、乗り換え駅登録の為 */
#endif	/* TOOLS */

public:
	void	init( WORD n_idx, WORD r_idx ) {
		name_idx = n_idx ; roma_idx = r_idx ;
#ifdef	TOOLS
		first_rosen_id = 0 ;
#endif	/* TOOLS */
	}
	int		cmp( const BYTE *name ) const ;
	const BYTE*	get_name() const ;
	const BYTE*	get_roma() const ;
	int		set_roma( const BYTE* r ) ;

	friend void	load_data( void ) ;

#ifdef	TOOLS
    void	init( const BYTE *n, int rosen_id ) ;
	int		get_first() const ;
	friend int	save_data( const BYTE *fn ) ;
	friend void	load_all( void ) ;
#endif	/* TOOLS */
} ;

#define		FLAG_USE		0x0001
#define		FLAG_PLOOP		0x0002
#define		FLAG_LMARK		0x0004
#define		FLAG_KIND_EXP	0x0008
#define		FLAG_KIND_SEXP	0x0010
#define		FLAG_KIND_OTHER	0x0018
#define		MASK_KIND		0x0018

#define		KID_NONE	0xFFFF

/*----------------------------------------------------------------------------
 * ROSEN
 *--------------------------------------------------------------------------*/
class ROSEN {
	WORD	name_idx ;		/* 路線名 */
#ifdef	TOOLS
	int		size ;			/* 路線内の駅の最大数 */
	int		near *stat ;		/* 駅番号の配列 */
	int		near *time ;		/* 起点駅からの所要時間 */
#else	/* TOOLS */
	WORD	rstat_idx ;
#endif	/* TOOLS */
    WORD	kid ;			/* 営業会社 ID */
    WORD	num ;			/* 実際に登録されている駅の数 */
    WORD	wait ;			/* 乗り換え時の平均待ち時間 */
    BYTE	circle ;		/* 環状線の場合一周に要する時間 */
    BYTE	len_kaisya ;	/* 運行会社名の長さ */
    WORD	flag ;			/* 路線のフラグ */

public:
	void	init( WORD n_idx, WORD n, WORD w, int c, int lk ) {
		name_idx = n_idx ; num = n ; wait = w ;
		circle = (BYTE) c ; len_kaisya = (BYTE) lk ;
		flag = FLAG_USE ;
#ifdef	TOOLS
		stat = time = 0 ;
		size = n ;
#else	/* TOOLS */
		rstat_idx = 0 ;
#endif	/* TOOLS */
		kid = KID_NONE ;
	}
	void	add( int st_id, int time ) ;	/* 新たに駅を追加する */
	const BYTE *	get_name() const ;
	WORD	get_wait() const { return wait ; }
#ifdef	TOOLS
	int		get_s_id( int idx ) const { return stat[ idx ] ; } ;
	int		get_time( int idx ) const { return time[ idx ] ; } ;
#else	/* TOOLS */
	int		get_s_id( int idx ) const ;
	int		get_time( int idx ) const ;
#endif	/* TOOLS */
	int		get_kid() const { return kid ; } ;
	void	set_kid( int idx ) { kid = idx ; } ;
	WORD	get_num() const { return num ; }
	int		get_circle() const { return (int) circle ; }
	int		get_len_kaisya() const { return (int) len_kaisya ; }
	int		get_use() const { return flag & FLAG_USE ; }
	int		get_ploop() const { return flag & FLAG_PLOOP ; }
	int		get_lmark() const { return flag & FLAG_LMARK ; }
	WORD	get_name_idx() const { return name_idx ; }
	void	set_use( BOOL _flag ) ;
	int		find( int st_id ) const ;
	int		diff_time( int time_from, int time_to ) const ;
	int		calc_time( int from, int to ) const ;
	int		nouse( int mask ) const {
		return ((mask & 0x01) && ((flag & MASK_KIND) == FLAG_KIND_EXP))
			|| ((mask & 0x02) && ((flag & MASK_KIND) == FLAG_KIND_SEXP))
			|| ((mask & 0x04) && ((flag & MASK_KIND) == FLAG_KIND_OTHER)) ;
	}
	int		available( int mask ) const {
		return get_use() && !nouse( mask ) ;
	}

	friend void	load_data( void ) ;

#ifdef	TOOLS
    void	init( const BYTE *nm, int size, int wait, int lk, int flg ) ;
    void	set_ploop( int _flag ) ;
	void	set_len_kaisya( int l ) { len_kaisya = (BYTE) l ; }
	void	find_adj( int st_id, int* s_prev, int* time_prev, int* s_next, int* time_next ) ;
	void	resize( int sz ) ;
    friend int	save_data( const BYTE *fn ) ;
	friend void	load_all( void ) ;
	friend int	save_opt( void ) ;
#endif	/* TOOLS */
} ;

typedef	STATION	*LPSTATION ;
typedef	ROSEN	*LPROSEN ;

LPSTATION	get_station( int index ) ;
LPROSEN		get_rosen( int index ) ;
