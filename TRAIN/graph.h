/*
 * 中間ファイル＆イメージファイルのタグ
 */
#define		DATA_ID		"AD4\x1A"
#define		COMPATIBLE	"0.00"

/*
 * 乗り換え修正情報内の待ち時間指定の特殊値
 */
#define		WAIT_NORMAL		0
#define		WAIT_DEL		1
#define		WAIT_ZERO		2
#define		WAIT_ABS		3
#define		WAIT_ONEWAY		4
#define		NODE_DEL		0x8000
#define		NODE_VAL		0x7FFF
#define		isdel_node(x)	((x)&NODE_DEL)
#define		norm_node(x)	((x)&NODE_VAL)
#define		NULL_NODE		0xFFFF
#define		ADJ_NONE		0xFFFF
#define		MAX_BUF_TMP		400

/*----------------------------------------------------------------------------
 * Data File Format
 *--------------------------------------------------------------------------*/
struct DataHeader {
	char	id[ 4 ] ;
	long	data ;
	long	opt ;
	long	stridx ;
	long	strdata ;
	long	dummy_20, dummy_24, dummy_28, dummy_32 ;
	long	dummy_36, dummy_40 ;

	short	dummy_s1 ;
	short	num_stat ;	/* 路線内駅数の合計 */
	short	num_adj ;	/* ADJ List 数 */
	short	max_s ;		/* 登録駅数 */
	short	max_r ;		/* 登録路線数 */
	short	strnext ;	/* 文字列の個数 */
	WORD	strsize ;	/* 文字列領域のバイト数 */
	short	num_node ;	/* 初期的な Node の個数 */
	short	dummy_s2 ;
	short	dummy_s3 ;
} ;

/*----------------------------------------------------------------------------
 * RESULT
 *--------------------------------------------------------------------------*/
#define		RESULT_START	0	/* 出発 */
#define		RESULT_DEPT		1	/* 発車駅 */
#define		RESULT_LINE		2	/* 利用路線 */
#define		RESULT_PASS		3	/* 通過駅 */
#define		RESULT_ARIV		4	/* 到着駅 */
#define		RESULT_CHANGE	5	/* 乗り換え */
#define		RESULT_END		6	/* 到着 */
#define		RESULT_NONE		9	/* 利用できる経路が無い場合 */

typedef struct {
	char	type ;
	short	time ;
	union {
		short		start_wait ;	/* 待ち時間 */
		short		dept_s_id ;		/* 出発駅 */
		struct {
			short	r_id ;			/* 利用路線 */
			short	jyosya ;		/* 乗車時間 */
		} line ;
		short		pass_s_id ;		/* 通過駅 */
		short		ariv_s_id ;		/* 到着駅 */
		struct {
			short	walk ;			/* 徒歩時間 */
			short	wait ;			/* 待ち時間 */
		} change ;
		struct {
			short	walk ;			/* 徒歩時間 */
			short	wait ;			/* 待ち時間 */
			short	jyosya ;		/* 乗車時間 */
		} end ;
	} v ;
} RESULT ;

/*----------------------------------------------------------------------------
 * Node
 *--------------------------------------------------------------------------*/
struct Node {
	WORD	v ;
	WORD	w ;
	WORD	next ;
} ;

extern	WORD		add_Node( WORD top, WORD v, int w ) ;
extern	WORD		del_Node( WORD top, WORD v ) ;
extern	int			del_FNode( WORD top, WORD size, WORD v ) ;

/*----------------------------------------------------------------------------
 * Fixed Node
 *--------------------------------------------------------------------------*/
struct FNode {
	WORD	v ;
	WORD	w ;
} ;

/*----------------------------------------------------------------------------
 * AdjLink
 *--------------------------------------------------------------------------*/
class AdjLink {
	WORD	s_id ;
	WORD	r_id ;
	WORD	top ;
	WORD	fixtop ;	/* FNode の先頭 IDX */
	WORD	size_fnode ;

	AdjLink() ;

public:
	void	set( int s, int r ) {
		s_id = s, r_id = r ;
	}
	void	set_r_id( int r ) {
		r_id = r ;
	}
	void	set_top( WORD _top ) {
		top = _top ;
	}
	void	set_size( WORD sz ) {
		size_fnode = sz ;
	}
	void	init() {
		set( 0, 0 ) ;
		top = fixtop = NULL_NODE ;
		size_fnode = 0 ;
	}
	void	add( int v, int w ) {
		del_FNode( fixtop, size_fnode, v ) ;
		top = add_Node( top, v, w ) ;
	}
	void	del( int v ) {
		if ( !del_FNode( fixtop, size_fnode, v ) ) {
			top = del_Node( top, v ) ;
		}
	}
	int		get_s_id() const {
		return s_id ;
	}
	int		get_r_id() const {
		return r_id ;
	}
	WORD	get_top() const {
		return top ;
	}
	WORD	get_fixtop() const {
		return fixtop ;
	}
	WORD	get_size() const {
		return size_fnode ;
	}
	friend void	load_adj( void ) ;
	friend void	clear_adj( void ) ;

#ifdef	TOOLS
	void	print_name() const ;
	void	print() const ;
	friend int	save_data( const BYTE fn[] ) ;
#endif	/* TOOLS */
} ;

#ifdef __cplusplus
extern "C" {
#endif
    extern	int		F_machi ;		/* 初回の待ち合わせを行う場合*真* */
    extern	int		F_walk ;		/* 徒歩により探索を打ち切る上限値 */
    extern	int		F_wait ;		/* 待ち時間で打ち切る場合の上限値 */
    extern	int		F_change ;		/* 乗換回数で打ち切る場合の上限値 */
    extern	int		F_nouse ;		/* 利用しない路線種別のビットマップ */
    extern	int		F_dispTime ;	/* 徒歩時間等を表示する場合*真* */

    extern	LINEID	Sel_deptline ;	/* 出発時利用路線 */
    extern	LINEID	Sel_arivline ;	/* 到着時利用路線 */
    extern	int		Walk_time ;		/* 徒歩時間 */
    extern	int		Wait_time ;		/* 待ち時間 */
    extern	int		Jyosya_time ;	/* 乗車時間 */
    extern	int		Total_time ;	/* 所要時間 */

    extern	BYTE	Buf_tmp[] ;	/* テンポラリ用バッファ */

    BYTE		*get_msg( int kind, int num ) ;	/* メッセージ取得 */
    void		fatal_msg( int idx ) ;
#ifdef __cplusplus
} ;
#endif

extern	int			save_img( const BYTE *version ) ;
extern	int			load_img( const BYTE *version ) ;

extern	void		init_adj( int num ) ;
extern	void		free_adj() ;
extern	AdjLink*	get_adj( int index ) ;
extern	int			find_adj( int s_id, int r_id ) ;
extern	int			new_adj() ;
extern	void		del_adj( int idx ) ;

extern	void		find( int from, int to ) ;
extern	void		find_adj_junct(const int,const int,int*,int*,int*,int*) ;
extern	int			modify_adj_sub( int a_idx, int s_id, int r_id ) ;
extern	void		make_adj( int start_s, int end_s ) ;
extern	void		clear_data() ;

extern	int			make_route( RESULT* res, int size_r ) ;
extern	BYTE*		get_route( RESULT *res, int start_time, int addlen ) ;

extern	void		other_job( int mode ) ;

#ifdef	TOOLS
extern	void		load_data( void ) ;
extern	void		load_adj( void ) ;
extern	int			save_opt( void ) ;
extern	int			save_data( const BYTE fn[] ) ;
extern	int			size_Node( WORD top ) ;
extern	void		print_route( int from ) ;
extern	void		make_init_adj() ;
extern	int			read_modify( const BYTE fn[] ) ;
extern	void		delete_knot( void ) ;
#endif	/* TOOLS */
