/*
 * ���ԃt�@�C�����C���[�W�t�@�C���̃^�O
 */
#define		DATA_ID		"AD4\x1A"
#define		COMPATIBLE	"0.00"

/*
 * ��芷���C�������̑҂����Ԏw��̓���l
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
	short	num_stat ;	/* �H�����w���̍��v */
	short	num_adj ;	/* ADJ List �� */
	short	max_s ;		/* �o�^�w�� */
	short	max_r ;		/* �o�^�H���� */
	short	strnext ;	/* ������̌� */
	WORD	strsize ;	/* ������̈�̃o�C�g�� */
	short	num_node ;	/* �����I�� Node �̌� */
	short	dummy_s2 ;
	short	dummy_s3 ;
} ;

/*----------------------------------------------------------------------------
 * RESULT
 *--------------------------------------------------------------------------*/
#define		RESULT_START	0	/* �o�� */
#define		RESULT_DEPT		1	/* ���ԉw */
#define		RESULT_LINE		2	/* ���p�H�� */
#define		RESULT_PASS		3	/* �ʉ߉w */
#define		RESULT_ARIV		4	/* �����w */
#define		RESULT_CHANGE	5	/* ��芷�� */
#define		RESULT_END		6	/* ���� */
#define		RESULT_NONE		9	/* ���p�ł���o�H�������ꍇ */

typedef struct {
	char	type ;
	short	time ;
	union {
		short		start_wait ;	/* �҂����� */
		short		dept_s_id ;		/* �o���w */
		struct {
			short	r_id ;			/* ���p�H�� */
			short	jyosya ;		/* ��Ԏ��� */
		} line ;
		short		pass_s_id ;		/* �ʉ߉w */
		short		ariv_s_id ;		/* �����w */
		struct {
			short	walk ;			/* �k������ */
			short	wait ;			/* �҂����� */
		} change ;
		struct {
			short	walk ;			/* �k������ */
			short	wait ;			/* �҂����� */
			short	jyosya ;		/* ��Ԏ��� */
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
	WORD	fixtop ;	/* FNode �̐擪 IDX */
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
    extern	int		F_machi ;		/* ����̑҂����킹���s���ꍇ*�^* */
    extern	int		F_walk ;		/* �k���ɂ��T����ł��؂����l */
    extern	int		F_wait ;		/* �҂����Ԃőł��؂�ꍇ�̏���l */
    extern	int		F_change ;		/* �抷�񐔂őł��؂�ꍇ�̏���l */
    extern	int		F_nouse ;		/* ���p���Ȃ��H����ʂ̃r�b�g�}�b�v */
    extern	int		F_dispTime ;	/* �k�����ԓ���\������ꍇ*�^* */

    extern	LINEID	Sel_deptline ;	/* �o�������p�H�� */
    extern	LINEID	Sel_arivline ;	/* ���������p�H�� */
    extern	int		Walk_time ;		/* �k������ */
    extern	int		Wait_time ;		/* �҂����� */
    extern	int		Jyosya_time ;	/* ��Ԏ��� */
    extern	int		Total_time ;	/* ���v���� */

    extern	BYTE	Buf_tmp[] ;	/* �e���|�����p�o�b�t�@ */

    BYTE		*get_msg( int kind, int num ) ;	/* ���b�Z�[�W�擾 */
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
