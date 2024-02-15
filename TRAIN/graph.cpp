#include	"lfunc.h"
#include	"base.h"
#include	"graph.h"
#include	"pqueue.h"

LINEID	Sel_deptline = RID_NONE ;	/* 出発時利用路線 */
LINEID	Sel_arivline = RID_NONE ;	/* 到着時利用路線 */
int		F_machi = 1 ;		/* 初回の待ち合わせを行う場合*真* */
int		F_walk = 32767;		/* 徒歩により探索を打ち切る上限値 */
int		F_wait = 32767;		/* 待ち時間により探索を打ち切る場合の上限値 */
int		F_change = 100 ;	/* 乗り換え回数で探索を打ち切る場合の上限値 */
int		F_nouse = 0 ;		/* 利用しない路線種別のビットマップ */
int		F_dispTime = 1 ;	/* 徒歩時間等を表示する場合*真* */
BYTE	Buf_tmp[ MAX_BUF_TMP ] ;	/* テンポラリ用バッファ */
static	int		add_n = 100 ;
static	int		add_a = 20 ;
void	(*OtherJob)( int mode ) = 0 ;

/*----------------------------------------------------------------------------
 * Adj用データ
 *--------------------------------------------------------------------------*/
static	WORD	Num_adj = 0 ;
static	WORD	Num_init_adj = 0 ;	// 初期的に確保した数
static	WORD	Num_adj_max = 0 ;	// 実際に確保した数
static	AdjLink	*Start = 0 ;		// delete しない
static	AdjLink	*End = 0 ;			// delete しない
static	HLOCAL	AdjHandle = NULL ;

/*----------------------------------------------------------------------------
 * 結果表示関連データ
 *--------------------------------------------------------------------------*/
int		Jyosya_time = 0 ;	/* 乗車時間 */
int		Walk_time = 0 ;		/* 徒歩時間 */
int		Wait_time = 0 ;		/* 待ち時間 */
int		Total_time = 0 ;	/* 所要時間 */
int		Change_cnt = 0 ;	/* 乗換回数 */
static	RESULT	*Res = 0 ;
static	int	Num_res = 0 ;
static	int	Size_res = 0 ;

/*----------------------------------------------------------------------------
 * Node
 *--------------------------------------------------------------------------*/
#define			MAX_NODE	10000
static	HLOCAL	NodeHandle = NULL ;
static	WORD	Max_node = MAX_NODE ;
static	WORD	Num_node = 0 ;
static	WORD	Num_del_node = 0 ;
static	WORD	Size_node = 0 ;
static	int		Num_node_init = 0 ;

void
init_Node()
{
    Assert( NodeHandle == NULL ) ;
    Num_node  = 0 ;
    Size_node = Max_node ;
	NodeHandle = LocalAlloc( LPTR, sizeof (Node) * Size_node ) ;
}

#ifndef	TOOLS
static	void
free_Node()
{
    if ( NodeHandle ) {
		LocalFree( NodeHandle ) ;
		NodeHandle = NULL ;
    }
}
#endif	/* TOOLS */

WORD
new_Node( WORD v, int w )
{
    if ( Num_node >= Size_node ) {
		fatal_msg( 6 ) ;
    }
    WORD	new_node = Num_node ++ ;
    Node	*n_base = (Node*) NodeHandle ;
    Node	*node = &n_base[ new_node ] ;
    node->v = norm_node(v) ;
    node->w = w ;
    node->next = NULL_NODE ;
    return new_node ;
}

WORD
add_Node( WORD top, WORD v, int w )
{
    if ( NodeHandle == NULL ) {
		init_Node() ;
    }
    v = norm_node(v) ;
    WORD	cur = top ;
    if ( cur == NULL_NODE ) {
		return new_Node( v, w ) ;
    }
    Node	*n_base = (Node*) NodeHandle ;
    while ( 1 ) {
		Node	*cur_node = &n_base[ cur ] ;
		if ( norm_node(cur_node->v) == v ) {
			cur_node->w = w ;
			if ( isdel_node(cur_node->v) ) {
				cur_node->v = v ;
				Num_del_node -- ;
			}
			break ;
		}
		if ( cur_node->next == NULL_NODE ) {
			cur_node->next = new_Node( v, w ) ;
			break ;
		}
		cur = cur_node->next ;
    }
    return top ;
}

WORD
del_Node( WORD top, WORD v )
{
    Assert( NodeHandle ) ;
    v = norm_node(v) ;
    WORD	cur = top ;
    Node	*n_base = (Node*) NodeHandle ;
    while ( cur != NULL_NODE ) {
		Node	*cur_node = &n_base[ cur ] ;
		if ( !isdel_node(cur_node->v) && norm_node(cur_node->v) == v ) {
			cur_node->v |= NODE_DEL ;
			Num_del_node ++ ;
			break ;
		}
		cur = cur_node->next ;
    }
    return top ;
}

#ifdef	TOOLS
static	int
size_Node( WORD top )
{
    int		count = 0 ;
    WORD	cur = top ;
    Node	*n_base = (Node*) NodeHandle ;
    while ( cur != NULL_NODE ) {
		Node	*t = &n_base[ cur ] ;

		if ( !isdel_node(t->v) ) {
			count ++ ;
		}
		cur = t->next ;
    }
    return count ;
}
#endif	/* TOOLS */

/*----------------------------------------------------------------------------
 * Fixed Node
 *--------------------------------------------------------------------------*/
#define			MAX_FNODE	10000
HLOCAL			FNodeHandle = NULL ;
unsigned int	Max_fnode = MAX_FNODE ;
unsigned int	Num_fnode = 0 ;
unsigned int	Size_fnode = 0 ;

static	void
init_FNode()
{
    Assert( FNodeHandle == NULL ) ;
    Num_fnode  = 0 ;
    Size_fnode = Max_fnode ;
    FNodeHandle = LocalAlloc( LPTR, sizeof (FNode) * Size_fnode ) ;
}

#ifndef	TOOLS
static	void
free_FNode()
{
    if ( FNodeHandle ) {
		LocalFree( FNodeHandle ) ;
		FNodeHandle = NULL ;
    }
}
#endif	/* TOOLS */

static	WORD
new_FNode( WORD v, int w )
{
    if ( Num_fnode >= Size_fnode ) {
		fatal_msg( 6 ) ;
    }
    WORD	new_fnode = Num_fnode ++ ;
    FNode	*fnode = &((FNode*) FNodeHandle)[ new_fnode ] ;

    fnode->v = norm_node(v) ;
    fnode->w = w ;
    return new_fnode ;
}

FNode *
get_FNode( WORD idx )
{
#ifdef	DEBUG
    Assert( idx < Num_fnode ) ;
#endif	/* DEBUG */
    return &((FNode*) FNodeHandle)[ idx ] ;
}

int
del_FNode( WORD top, WORD size, WORD v )
{
    if ( size <= 0 ) {
		return 0 ;
    }
#ifdef	DEBUG
    Assert( FNodeHandle != NULL ) ;
#endif	/* DEBUG */
    v = norm_node(v) ;
    for ( FNode *ft = get_FNode( top ) ; size > 0 ; size --, ft ++ ) {
		if ( !isdel_node(ft->v) && norm_node(ft->v) == v ) {
			ft->v |= NODE_DEL ;
			return 1 ;
		}
    }
    return 0 ;
}

/*----------------------------------------------------------------------------
 * Value
 *--------------------------------------------------------------------------*/
struct Value {
    short	val ;
    WORD	dad ;
    char	val2 ;
    BYTE	flg ;
} ;

#define		VAL_NONE	(-9999)

static HLOCAL	ValueHandle = NULL ;
static int		Num_value = 0 ;

static	void
clear_Value( void )
{
    Value	*v = (Value*) ValueHandle ;

    for ( int i = 0 ; i < Num_value ; i ++, v ++ ) {
		v->val = VAL_NONE, v->val2 = 0, v->dad = NULL_NODE, v->flg = 1 ;
    }
}

static	void
init_Value( int num_v )
{
    Assert( ValueHandle == NULL ) ;
    Num_value  = num_v ;
    ValueHandle = LocalAlloc( LPTR, sizeof (Value) * Num_value ) ;
}

#ifndef	TOOLS
static	void
free_Value( void )
{
    if ( ValueHandle ) {
		LocalFree( ValueHandle ) ;
		ValueHandle = NULL ;
    }
}

/*----------------------------------------------------------------------------
 * search main
 *--------------------------------------------------------------------------*/
void
find( register int k, int to )
{
    Value		*v_base = (Value*) ValueHandle ;
    AdjLink		*a_base = get_adj( 0 ) ;
    Node		*n_base = (Node*) NodeHandle ;
    int			count = 0 ;

    clear_Value() ;
    pq_setup() ;
    pq_update( k, -9999, 0, 0 ) ;
    while ( !pq_empty() ) {
		k = pq_remove() ;
		Value *	v_k = &v_base[ k ] ;
		if ( v_k->flg == 1 ) {
		    v_k->val = v_k->val2 = 0 ;
		}
		int		val_k  = v_k->val ;
		int		val2_k = v_k->val2 ;
		v_k->flg = 0 ;
		if ( !v_base[ to ].flg && v_base[ to ].val < val_k ) {
		    continue ;
		}
		if ( ++ count % 150 == 0 ) {
		    other_job( 2 ) ;
		}
		AdjLink		*a = a_base + k ;
		int			r_id1 = a->get_r_id() ;
		WORD		size = a->get_size() ;
		if ( size ) {
		    FNode	*ft = get_FNode( a->get_fixtop() ) ;
		    do {
				register WORD	v = ft->v ;
				if ( isdel_node(v) ) {
				    continue ;
				}
				v = norm_node(v) ;
				Value	* v2 = &v_base[ v ] ;
				if ( v2->flg ) {
					AdjLink	*a2 = a_base + v ;
					int		r_id2 = a2->get_r_id() ;
					int		priority = val_k + ft->w ;
					int		p2 = val2_k ;
					if ( r_id1 != r_id2 && r_id1 != 0 && r_id2 != 0 ) {
						p2 ++ ;
					}
					if ( p2 <= F_change && pq_update( v, priority, p2, 0 ) ) {
						v2->val  = priority ;
						v2->val2 = p2 ;
						v2->dad  = k ;
						v2->flg  = 2 ;
					}
				}
		    } while ( ft ++, -- size ) ;
		}
		WORD		cur = a->get_top() ;
		if ( cur != NULL_NODE ) {
			Node	*t ;
			do {
				t = &n_base[ cur ] ;
				register WORD	v = t->v ;
				if ( isdel_node(v) ) {
					continue ;
				}
				v = norm_node(v) ;
				Value	*v2 = &v_base[ v ] ;
				if ( v2->flg ) {
					AdjLink	*a2 = a_base + v ;
					int		r_id2 = a2->get_r_id() ;
					int		priority = val_k + t->w ;
					int		p2 = val2_k ;
					if ( r_id1 != r_id2 && r_id1 != 0 && r_id2 != 0 ) {
						p2 ++ ;
					}
					if ( p2 <= F_change && pq_update( v, priority, p2, 0 ) ) {
						v2->val  = priority ;
						v2->val2 = p2 ;
						v2->dad  = k ;
						v2->flg  = 2 ;
					}
				}
			} while ( cur = t->next, cur != NULL_NODE ) ;
		}
    }
}
#endif	/* TOOLS */

void
find_adj_junct( const int s_id, const int r_id, int *a_prev, int *time_prev, int *a_next, int *time_next )
{
    int		f_found = 0 ;
    int		t_prev = 0 ;
    int		t_next = 0 ;
    int		time_cur = 0 ;
    int		a_first = ADJ_NONE ;	// この路線内の最初の乗換駅
    int		t_first = 0 ;
    int		a_last = ADJ_NONE ;		// この路線内の最後の乗換駅
    int		t_last = 0 ;
    ROSEN	*r = get_rosen( r_id ) ;
    int		num = r->get_num() ;
    AdjLink	*a_base = get_adj( 0 ) ;

    *a_prev = *a_next = ADJ_NONE ;
    t_prev = t_next = 0 ;
    if ( r->get_lmark() ) {
		return ;
    }
    for ( int i = 0 ; i < num ; i ++ ) {
		int	_stat = r->get_s_id( i ) ;
		int	_time = r->get_time( i ) ;

		if ( s_id == _stat ) {
			f_found  = 1 ;
			time_cur = _time ;
			continue ;
		}
		for ( int a_idx = 2 ; a_idx < Num_adj ; a_idx ++ ) {
			AdjLink	*a = a_base + a_idx ;
			int		s_id2 = a->get_s_id() ;
			int		r_id2 = a->get_r_id() ;

			if ( r_id2 != r_id || s_id2 == s_id ) {
				// 違う路線の場合や同じ駅の場合は無視
				continue ;
			} else if ( s_id2 == _stat ) {
				// 乗り換え駅が見つかった場合
				if ( !f_found ) {
					(*a_prev) = a_idx ;
					t_prev    = _time ;
				} else if ( (*a_next) == ADJ_NONE ) {
					(*a_next) = a_idx ;
					t_next    = _time ;
				}
				if ( !f_found && a_first == ADJ_NONE ) {
					a_first = a_idx ;
					t_first = _time ;
				}
				if ( f_found ) {
					a_last = a_idx ;
					t_last = _time ;
				}
			}
		}
    }
    if ( r->get_circle() ) {
		if ( (*a_prev) == ADJ_NONE ) {
			(*a_prev) = a_last ;
			t_prev = t_last ;
		}
		if ( (*a_next) == ADJ_NONE ) {
			(*a_next) = a_first ;
			t_next = t_first ;
		}
    }
    (*time_prev) = r->diff_time( t_prev, time_cur ) ;
    (*time_next) = r->diff_time( t_next, time_cur ) ;
}

int
modify_adj_sub( int a_idx, int s_id, int r_id )
{
    // 乗換駅でない場合は、Adj に登録する。
    if ( a_idx == ADJ_NONE ) {
		a_idx = new_adj() ;
    }
    AdjLink		*a = get_adj( a_idx ) ;
    a->set( s_id, r_id ) ;
    // 隣接の乗り換え駅を加える
    int		a_prev = ADJ_NONE, a_next = ADJ_NONE ;
	int		time_prev = 0, time_next = 0 ;
    find_adj_junct( s_id, r_id, &a_prev, &time_prev, &a_next, &time_next ) ;
    ROSEN		*r = get_rosen( r_id ) ;
    if ( a_prev != ADJ_NONE ) {
		// 同じ路線の上り隣接駅を加える
		a->add( a_prev, time_prev ) ;
		if ( !r->get_ploop() ) {
			get_adj( a_prev )->add( a_idx, time_prev ) ;
		}
    }
    if ( a_next != ADJ_NONE ) {
		// 同じ路線の下り隣接駅を加える
		if ( !r->get_ploop() ) {
			a->add( a_next, time_next ) ;
		}
		get_adj( a_next )->add( a_idx, time_next ) ;
    }
    if ( a_prev != ADJ_NONE && a_next != ADJ_NONE ) {
		// その区間で既にノードがある場合は、そのノードを削除する
		get_adj( a_prev )->del( a_next ) ;
		get_adj( a_next )->del( a_prev ) ;
    }
    return a_idx ;
}

/*
 * 路線の使用フラグが未使用ならば、ノード情報内の Adj インデックスを負にする
 */
static	void
setup_use( int start_s, int end_s )
{
    AdjLink		*a_base = get_adj( 0 ) ;
    Node		*n_base = (Node*) NodeHandle ;

    for ( int i = 0 ; i < Num_adj ; i ++ ) {
		AdjLink		*a1 = a_base + i ;
		int			r_id1 = a1->get_r_id() ;
		WORD		size = a1->get_size() ;
		if ( size ) {
			FNode	*ft = get_FNode( a1->get_fixtop() ) ;
			do {
				if ( isdel_node(ft->v) ) {
					continue ;
				}
				AdjLink		*a2 = a_base + norm_node(ft->v) ;
				//int		s_id2 = a2->get_s_id() ;
				int		r_id2 = a2->get_r_id() ;
				if ( r_id1 != r_id2 ) {
					WORD	w1 = ft->w ;
					WORD	w2 = get_rosen( r_id1 )->get_wait() ;
					if ( w1 >= w2 ) {
						if ( w1 - w2 > F_walk || w2 > F_wait ) {
							/* 徒歩の区間を利用しない場合 */
							ft->v |= NODE_DEL ;
							continue ;
						}
					} else if ( w1 > F_walk ) {
						ft->v |= NODE_DEL ;
						continue ;
					}
				}
				/* 路線の使用フラグが未使用の場合 */
				ROSEN	*r = get_rosen( r_id2 ) ;
				if ( !r->available( F_nouse ) ) {
					ft->v |= NODE_DEL ;
					continue ;
				} else if ( r->get_lmark() ) {
					int		s_id2 = a2->get_s_id() ;

					if ( s_id2 != start_s && s_id2 != end_s ) {
					ft->v |= NODE_DEL ;
					continue ;
					}
				}
			} while ( ft ++, -- size ) ;
		}
		WORD	cur = a1->get_top() ;
		Node	*t ;
		for ( ; cur != NULL_NODE ; cur = t->next ) {
			t = &n_base[ cur ] ;
			if ( !isdel_node(t->v) ) {
				AdjLink	*a2 = a_base + norm_node(t->v) ;
				//int		s_id2 = a2->get_s_id() ;
				int		r_id2 = a2->get_r_id() ;
				if ( r_id1 != r_id2 ) {
					WORD	w1 = t->w ;
					WORD	w2 = get_rosen( r_id1 )->get_wait() ;
					if ( w1 >= w2 ) {
						if ( w1 - w2 > F_walk || w2 > F_wait ) {
							/* 徒歩の区間を利用しない場合 */
							t->v |= NODE_DEL ;
							continue ;
						}
					} else if ( w1 > F_walk ) {
						t->v |= NODE_DEL ;
						continue ;
					}
				}
				/* 路線の使用フラグが未使用の場合 */
				ROSEN	*r = get_rosen( r_id2 ) ;
				if ( !r->available( F_nouse ) ) {
					t->v |= NODE_DEL ;
					continue ;
				} else if ( r->get_lmark() ) {
					int		s_id2 = a2->get_s_id() ;

					if ( s_id2 != start_s && s_id2 != end_s ) {
						t->v |= NODE_DEL ;
						continue ;
					}
				}
			}
		}
    }
}

struct LISTUP {
    WORD	aid ;
    WORD	cost ;
} ;
static	LISTUP	*Listup = (LISTUP *) Buf_tmp ;
static	int	Max_listup = MAX_BUF_TMP / sizeof (LISTUP) ;
static	int	Num_listup = 0 ;

static	BOOL
put_listup( WORD aid, WORD cost )
{
    int		idx ;

    /* 既に列挙しているかどうかチェック */
    for ( idx = 0 ; idx < Num_listup ; idx ++ ) {
		if ( Listup[ idx ].aid == aid ) {
			break ;
		}
    }
    if ( idx >= Num_listup ) {
		/* まだ列挙していなかった場合 */
		if ( Num_listup >= Max_listup ) {
			/* バッファに入りきらなかった場合 */
			return FALSE ;
		}
		Listup[ Num_listup ].aid  = aid ;
		Listup[ Num_listup ].cost = cost ;
		Num_listup ++ ;
    }
    return TRUE ;
}

static	int
make_adj_listup( int s_id, int r_id )
{
    WORD	v, wait, cost3 ;
    int		idx2 ;
    AdjLink		*a_base = get_adj( 0 ) ;
    Node		*n_base = (Node*) NodeHandle ;

    Num_listup = 0 ;
    /* 同名の乗換駅をリストアップする */
    for ( v = 2 ; v < Num_adj ; v ++ ) {
		AdjLink	*a = a_base + v ;
		int		r_id2 = a->get_r_id() ;

		if ( r_id2 == 0 || a->get_s_id() != s_id ) {
			continue ;
		} else if ( r_id != RID_NONE && r_id2 != r_id ) {
			continue ;
		}
		if ( !put_listup( v, 0 ) ) {
			return Num_listup ;
		}
    }
    if ( Num_listup == 0 || r_id != RID_NONE ) {
		/* 同名の乗換駅が存在しなかった場合 */
		return Num_listup ;
    }
    /* 同名の乗換駅から徒歩で移動できる乗換駅をリストアップする */
    for ( idx2 = 0 ; idx2 < Num_listup ; idx2 ++ ) {
		WORD	a_id2 = Listup[ idx2 ].aid ;
		WORD	cost2 = Listup[ idx2 ].cost ;
		AdjLink	*a2 = a_base + a_id2 ;
		int		s_id2 = a2->get_s_id() ;
		int		r_id2 = a2->get_r_id() ;
		WORD	size  = a2->get_size() ;
		if ( size ) {
			FNode	*ft = get_FNode( a2->get_fixtop() ) ;
			do {
				if ( !isdel_node(ft->v) ) {
					v = norm_node(ft->v) ;
					AdjLink	*a3 = a_base + v ;
					if ( a3->get_s_id() != s_id2 && a3->get_r_id() != r_id2 ){
						/* 同名乗換駅から徒歩で移動できる駅を見つけた場合 */
						/* 乗換元の路線の待ち時間を引く */
						wait = get_rosen( r_id2 )->get_wait() ;
						cost3 = ft->w ;
						if ( cost3 >= wait ) {
							cost3 -= wait ;
						}
						/* 乗換時間だけを付加する */
						if ( !put_listup( v, cost2 + cost3 ) ) {
							return Num_listup ;
						}
					}
				}
			} while ( ft ++, -- size ) ;
		}
		WORD	cur = a2->get_top() ;
		while ( cur != NULL_NODE ) {
			Node	*t = &n_base[ cur ] ;
			if ( !isdel_node(t->v) ) {
				v = norm_node(t->v) ;
				AdjLink	*a3 = a_base + v ;
				if ( a3->get_s_id() != s_id2 && a3->get_r_id() != r_id2 ){
					/* 同名乗換駅から徒歩で移動できる駅を見つけた場合 */
					/* 乗換元の路線の待ち時間を引く */
					wait = get_rosen( r_id2 )->get_wait() ;
					cost3 = t->w ;
					if ( cost3 >= wait ) {
						cost3 -= wait ;
					}
					if ( !put_listup( v, cost2 + cost3 ) ) {
						return Num_listup ;
					}
				}
			}
			cur = t->next ;
		}
    }
    return Num_listup ;
}

static	void
make_adj_sub_s( int aid, int r_id )
{
    AdjLink	*a_base = get_adj( 0 ) ;
    AdjLink *a = a_base + aid ;
    WORD	size = a->get_size(), v ;
    WORD	cur = a->get_top() ;
    if ( size ) {
		FNode *	ft = get_FNode( a->get_fixtop() ) ;
		do {
			if ( !isdel_node(ft->v) ) {
				v = norm_node(ft->v) ;
				if ( (a_base + v)->get_r_id() != r_id ) {
					ft->v |= NODE_DEL ;
				}
			}
		} while ( ft ++, -- size ) ;
    }
    Node	*n_base = (Node*) NodeHandle ;
    while ( cur != NULL_NODE ) {
		Node	*t = &n_base[ cur ] ;
		if ( !isdel_node(t->v) ) {
			v = norm_node(t->v) ;
			if ( (a_base + v)->get_r_id() == r_id ) {
				t->v |= NODE_DEL ;
			}
		}
		cur = t->next ;
    }
}

static	void
make_adj_sub_e( int aid, int r_id )
{
    AdjLink	*a_base = get_adj( 0 ) ;
    AdjLink *a = a_base + aid ;
    WORD	size = a->get_size() ;
    WORD	cur = a->get_top() ;
    if ( size ) {
		FNode	*ft = get_FNode( a->get_fixtop() ) ;
		do {
			if ( !isdel_node(ft->v) ) {
				AdjLink	*a = a_base + norm_node(ft->v) ;
				if ( a->get_r_id() != r_id ) {
					a->del( aid ) ;
				}
			}
		} while ( ft ++, -- size ) ;
    }
    Node	*n_base = (Node*) NodeHandle ;
    while ( cur != NULL_NODE ) {
		Node	*t = &n_base[ cur ] ;
		if ( !isdel_node(t->v) ) {
			AdjLink	*a = a_base + norm_node(t->v) ;
			if ( a->get_r_id() != r_id ) {
				a->del( aid ) ;
			}
		}
		cur = t->next ;
    }
}

#ifndef	TOOLS
void
make_adj( int start_s, int end_s )
{
	WORD	wait ;
    int		i, aid, s_id, r_id ;
    AdjLink	*a = 0 ;

    /* 路線使用フラグを反映する */
    setup_use( start_s, end_s ) ;
    /* 開始(到着)・終了(出発)用 AdjLink を作成する */
    Assert( Start && End ) ;
    Start->init() ;
    End->init() ;
    Start->set( start_s, 0 ) ;
    End->set( end_s, 0 ) ;
    other_job( 1 ) ;
    /* 開始駅の情報を誤用しないため終了駅から作成する */
    if ( make_adj_listup( end_s, Sel_deptline ) ) {
		/* 既に AdjLink が存在する場合 */
		for ( i = 0 ; i < Num_listup ; i ++ ) {
			aid  = Listup[ i ].aid ;
			a    = get_adj( aid ) ;
			s_id = a->get_s_id() ;
			r_id = a->get_r_id() ;
			if ( Sel_deptline == RID_NONE ) {
				/* 終了(出発)時利用路線が指定されていない場合 */
				if ( s_id != start_s ) {
					wait = F_machi ? get_rosen( r_id )->get_wait() : 0 ;
					a->add( 1, Listup[ i ].cost + wait ) ;
				} else if ( Sel_arivline == RID_NONE ) {
					/* 直接、開始(到着)駅に至る場合 */
					Start->add( 1, Listup[ i ].cost ) ;
				}
			} else if ( s_id == end_s && r_id == Sel_deptline ) {
				wait = F_machi ? get_rosen( r_id )->get_wait() : 0 ;
				a->add( 1, Listup[ i ].cost + wait ) ;
				/* 終了駅への不要ノードを削除する */
				make_adj_sub_e( aid, Sel_deptline ) ;
				break ;
			}
		}
    } else {
		/* 乗換駅でない場合は、もっとも近い乗換駅を加える */
		for ( r_id = 0 ; r_id < Num_r ; r_id ++ ) {
			if ( Sel_deptline == RID_NONE || r_id == Sel_deptline ) {
				ROSEN	*r = get_rosen( r_id ) ;
				if ( r->available( F_nouse ) && r->find( end_s ) ) {
					aid  = modify_adj_sub( ADJ_NONE, end_s, r_id ) ;
					a    = get_adj( aid ) ;
					wait = F_machi ? r->get_wait() : 0 ;
					a->add( 1, wait ) ;
				}
			}
		}
    }
    other_job( 1 ) ;
    /* 開始駅のノードを作成する */
    if ( make_adj_listup( start_s, Sel_arivline ) ) {
		/* 既に AdjLink が存在する場合 */
		for ( i = 0 ; i < Num_listup ; i ++ ) {
			aid = Listup[ i ].aid ;
			Start->add( aid, Listup[ i ].cost ) ;
		}
		if ( Sel_arivline != RID_NONE ) {
			/* 開始(到着)時利用路線が指定されている場合 */
			Assert( Num_listup == 1 ) ;
			/* 開始駅からの不要なノードを削除する */
			make_adj_sub_s( Listup[ 0 ].aid, Sel_arivline ) ;
		}
    } else {
		/* 乗換駅でない場合は、もっとも近い乗換駅を加える */
		for ( r_id = 0 ; r_id < Num_r ; r_id ++ ) {
			if ( Sel_arivline == RID_NONE || r_id == Sel_arivline ) {
				ROSEN	*r = get_rosen( r_id ) ;
				if ( r->available( F_nouse ) && r->find( start_s ) ) {
					aid = modify_adj_sub( ADJ_NONE, start_s, r_id ) ;
					Start->add( aid, 0 ) ;
				}
			}
		}
    }
}
#endif	/* TOOLS */

#ifndef	TOOLS
static	void
fatal_file( int msg1, int msg2 )
{
    BYTE	tmpbuf[ 100 ], *p = tmpbuf ;

    StrAppend( &p, get_msg(2,7) ) ;
    StrAppend( &p, get_msg(2,msg1) ) ;
    *p++ = ' ' ;
    StrAppend( &p, get_msg(2,msg2) ) ;
    fatal( tmpbuf ) ;
}

static int	F_load_adj = 1 ;

WORD
read_word( const BYTE *buf )
{
	return (0xFF & buf[0]) + 256 * (0xFF & buf[1]) ;
}

void
load_data( void )
{
    int		i ;
    BYTE	tmpbuf[ 512 ], *p = tmpbuf ;
    DataHeader	header ;

    // Header 情報
    Fseek( 0L, 0 ) ;
    if ( FreadBuf( (BYTE*) &header, sizeof header ) != sizeof header ) {
		fatal_file( 9, 8 ) ;
    }
    if ( Memcmp( header.id, DATA_ID, sizeof header.id ) != 0 ) {
		fatal_msg( 3 ) ;
    }
    StrDataOfs  = header.strdata ;
    Max_s = header.max_s ;
    Max_r = header.max_r ;
    StrNext = header.strnext ;
    StrSize = header.strsize ;
    Num_rstat = header.num_stat > 0 ? header.num_stat : Max_s * 2 ;
    // Station 情報
    Fseek( header.data, 0 ) ;
    init_station() ;
    init_rosen() ;
    StringAllocN( StrNext, StrSize ) ;
    Num_s = Max_s ;
    Num_r = Max_r ;
    STATION		*s = (STATION*) StationHandle ;
    for ( i = 0 ; i < Num_s ; i ++, s ++ ) {
		if ( FreadBuf( tmpbuf, 6 ) != 6 ) {
			fatal_file( 10, 8 ) ;
		}
		WORD	name_idx, roma_idx ;
//		int		first_rosen_id ;
		name_idx = read_word( &tmpbuf[0] ) ;
		roma_idx = read_word( &tmpbuf[2] ) ;
//		first_rosen_id = (int) read_word( &tmpbuf[4] ) ;
		s->init( name_idx, roma_idx ) ;
    }
    // Sort 情報
    for ( i = 0 ; i < Num_s ; i ++ ) {
		int	idx, idx2 ;

		if ( FreadBuf( tmpbuf, 4 ) != 4 ) {
			fatal_file( 11, 8 ) ;
		}
		idx = (int) read_word( &tmpbuf[0] ) ;
		idx2 = (int) read_word( &tmpbuf[2] ) ;
		((WORD*) SortHandle)[ i ]  = idx ;
		((WORD*) KSortHandle)[ i ] = idx2 ;
    }
    // Rosen 情報
    int		idx_rstat = 0 ;
    short	*statbase = (short*) RStatHandle ;
    for ( i = 0 ; i < Num_r ; i ++ ) {
		ROSEN		*r = get_rosen( i ) ;

		if ( FreadBuf( tmpbuf, 10 ) != 10 ) {
			fatal_file( 12, 8 ) ;
		}
		int		num ;
		WORD	name_idx, wait, stat, time ;
		BYTE	circle, len_kaisya ;

		name_idx = read_word( &tmpbuf[0] ) ;
		num = (int) read_word( &tmpbuf[4] ) ;
		wait = read_word( &tmpbuf[6] ) ;
		circle = 0xFF & tmpbuf[8] ;
		len_kaisya = 0xFF & tmpbuf[9] ;
		r->init( name_idx, num, wait, circle, len_kaisya ) ;
		if ( idx_rstat >= Num_rstat * 2 ) {
			fatal_msg( 19 ) ;
		}
		r->rstat_idx = idx_rstat ;
		for ( int j = 0 ; j < r->num ; j ++ ) {
			if ( FreadBuf( tmpbuf, 4 ) != 4 ) {
				fatal_file( 13, 8 ) ;
			}
			p = tmpbuf ;
			stat = (int) read_word( p ) ; p += 2 ;
			time = (int) read_word( p ) ; p += 2 ;
			statbase[ idx_rstat       ] = stat ;
			statbase[ idx_rstat + num ] = time ;
			idx_rstat ++ ;
		}
		idx_rstat += num ;
    }
    // String Index 情報
    int		rsize = 0 ;
    for ( i = 0 ; i < StrNext ; i ++ ) {
		if ( !rsize ) {
			if ( (StrNext - i) * 2 > sizeof tmpbuf ) {
				rsize = sizeof tmpbuf ;
			} else {
				rsize = (StrNext - i) * 2 ;
			}
			if ( FreadBuf( tmpbuf, rsize ) != rsize ) {
				fatal_file( 14, 8 ) ;
			}
			p = tmpbuf ;
		}
		((WORD *) StrIdxHandle)[ i ] = *(WORD*) p ;
		p += 2 ;
		rsize -= 2 ;
    }
    Assert( rsize == 0 ) ;
    // String Data 情報
    long	size = StrSize, index = 0 ;
    while ( size > 0 ) {
		const BYTE	*src = tmpbuf ;
		BYTE		*dst ;

		dst = &((BYTE *) StrPtrHandle)[ index ] ;
		rsize = size > sizeof tmpbuf ? sizeof tmpbuf : size ;
		if ( FreadBuf( tmpbuf, rsize ) != rsize ) {
			fatal_file( 15, 8 ) ;
		}
		for ( i = 0 ; i < rsize ; i ++ ) {
			*dst++ = *src++ ;
		}
		index += rsize ;
		size -= rsize ;
    }
    // オプション情報
    Fseek( header.opt, 0 ) ;
    // 利用路線情報
    for ( i = 0 ; i < Num_r ; i ++ ) {
		ROSEN	*r = get_rosen( i ) ;
		if ( FreadBuf( tmpbuf, 2 ) != 2 ) {
			fatal_file( 16, 8 ) ;
		}
		int		flag ;
		flag = (int) read_word( &tmpbuf[0] ) ;
		r->flag = flag ;
    }
    F_load_adj = 1 ;
    return ;
}

void
load_adj( void )
{
    BYTE	tmpbuf[ 100 ], *p = tmpbuf ;
    DataHeader	header ;

    if ( !F_load_adj ) {
		clear_adj() ;
		pq_clear() ;
		return ;
    }
    Fseek( 0L, 0 ) ;
    if ( FreadBuf( (BYTE*) &header, sizeof header ) != sizeof header ) {
		fatal_file( 9, 8 ) ;
    }
    if ( Memcmp( header.id, DATA_ID, sizeof header.id ) != 0 ) {
		fatal_msg( 3 ) ;
    }
    int		num_a = header.num_adj ;
    // Value 領域を解放する
    free_Value() ;
    // Node 領域を開放する
    free_Node() ;
    // FNode 領域を開放する
    free_FNode() ;
    // PQueue 領域を開放する
    pq_free() ;
    // Adj 領域を開放する
    free_adj() ;
    // Adj 情報を初期化する
    init_adj( num_a + add_a ) ;
    Num_adj = Num_init_adj = num_a ;
    // PQueue 情報を初期化する
    pq_init( num_a + add_a ) ;
    // FNode 情報を初期化する
    Max_fnode = header.num_node > 0 ? header.num_node : MAX_FNODE ;
    init_FNode() ;
    // Node 領域初期確保量を計算する
    Max_node = add_n ;
    // Node 情報を全てクリアする
    Num_node = 0 ;
    // AdjLink 情報
    for ( int i = 0 ; i < Num_adj ; i ++ ) {
		// AdjLink ヘッダー情報
		if ( FreadBuf( tmpbuf, 6 ) != 6 ) {
			fatal_file( 17, 8 ) ;
		}
		int	s_id, r_id ;
		WORD	size ;
		p = tmpbuf ;
		s_id = (int) read_word( p ) ;	p += 2 ;
		r_id = (int) read_word( p ) ;	p += 2 ;
		size = (int) read_word( p ) ;	p += 2 ;
		// AdjLink 情報をセットする
		AdjLink		*a = get_adj( i ) ;
		a->set( s_id, r_id ) ;
		a->set_size( size ) ;
		// Node 情報
		for ( WORD j = 0 ; j < size ; j ++ ) {
			if ( FreadBuf( tmpbuf, 4 ) != 4 ) {
				fatal_file( 18, 8 ) ;
			}
			WORD	v ;
			int		time ;
			p = tmpbuf ;
			v = (WORD) read_word( p ) ;		p += 2 ;
			time = (int) read_word( p ) ;	p += 2 ;
			WORD	cur = new_FNode( v, time ) ;
			if ( j == 0 ) {
				a->fixtop = cur ;
			}
		}
    }
    Num_node_init = Num_node ;
    // Value 領域を初期化する
    init_Value( Num_adj + add_a ) ;
    F_load_adj = 0 ;
    return ;
}

void
clear_data()
{
    // Value 領域を解放する
    free_Value() ;
    // Node 領域を開放する
    free_Node() ;
    // FNode 領域を開放する
    free_FNode() ;
    // PQueue 領域を開放する
    pq_free() ;
    // Adj 領域を開放する
    free_adj() ;
    // 文字列領域を開放する
    StringFree() ;
    // Rosen 領域を開放する
    free_rosen() ;
    // Station 領域を開放する
    free_station() ;
}

void
clear_adj( void )
{
    WORD	i ;
    WORD	cur ;

    for ( i = 0 ; i < Num_init_adj ; i ++ ) {
		AdjLink		*a = get_adj( i ) ;
		cur = a->top ;
		if ( cur != NULL_NODE && cur >= Num_node_init ) {
			a->top = NULL_NODE ;
		}
    }
    FNode *	fnode = get_FNode( 0 ) ;
    for ( i = 0 ; i < Num_fnode ; i ++, fnode ++ ) {
		fnode->v = norm_node(fnode->v) ;
    }
    Node	*node = (Node*) NodeHandle ;
    for ( i = 0 ; i < Num_node_init ; i ++, node ++ ) {
		cur = node->next ;
		if ( cur != NULL_NODE && cur >= Num_node_init ) {
			node->next = NULL_NODE ;
		}
		node->v = norm_node(node->v) ;
    }
    Num_adj  = Num_init_adj ;
    Num_node = Num_node_init ;
    Start = get_adj( 0 ) ;
    End   = get_adj( 1 ) ;
}

/*----------------------------------------------------------------------------
 * 結果表示関連
 *--------------------------------------------------------------------------*/
static	void
add_res_start( int wait )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_START ;
		Res->time = 0 ;
		Res->v.start_wait = wait ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_dept( int time, int s_id )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_DEPT ;
		Res->time = time ;
		Res->v.dept_s_id = s_id ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_line( int r_id, int jyosya )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_LINE ;
		Res->time = 0 ;
		Res->v.line.r_id   = r_id ;
		Res->v.line.jyosya = jyosya ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_pass( int time, int s_id )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_PASS ;
		Res->time = time ;
		Res->v.pass_s_id = s_id ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_ariv( int time, int s_id )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_ARIV ;
		Res->time = time ;
		Res->v.ariv_s_id = s_id ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_change( int wait, int walk )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_CHANGE ;
		Res->time = 0 ;
		Res->v.change.wait = wait ;
		Res->v.change.walk = walk ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_end( int time, int walk, int wait, int jyosya )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_END ;
		Res->time = time ;
		Res->v.end.walk = walk ;
		Res->v.end.wait = wait ;
		Res->v.end.jyosya = jyosya ;
		Res ++, Num_res ++ ;
    }
}

static	void
add_res_none( void )
{
    if ( Num_res < Size_res ) {
		Res->type = RESULT_NONE ;
		Res->time = 0 ;
		Res ++, Num_res ++ ;
    }
}

static	void
make_pass( int r_id, int from, int to, int start_time )
{
    const ROSEN	*r = get_rosen( r_id ) ;
    int			num = r->get_num() ;
    int			circle = r->get_circle() ;
    int			from_idx = IDX_NONE, to_idx = IDX_NONE, idx, dir = 0 ;
    int			from_time = 0, to_time = 0, time ;

    for ( idx = 0 ; idx < num ; idx ++ ) {
		if ( r->get_s_id( idx ) == from ) {
			from_idx = idx ;
		} else if ( r->get_s_id( idx ) == to ) {
			to_idx = idx ;
		}
    }
    if ( from_idx == IDX_NONE || to_idx == IDX_NONE || from_idx == to_idx ) {
		return ;
    }
    if ( circle ) {
		/* 環状線の場合 */
		from_time = r->get_time( from_idx ) ;
		to_time   = r->get_time( to_idx ) ;
		if ( from_time < to_time ) {
			if ( to_time - from_time < from_time + circle - to_time ) {
				dir = 1 ;
			} else {
				dir = -1 ;
			}
		} else {
			if ( from_time - to_time < to_time + circle - from_time ) {
				dir = -1 ;
			} else {
				dir = 1 ;
			}
		}
    } else if ( from_idx < to_idx ) {
		dir = 1 ;
    } else {
		dir = -1 ;
    }
    time = r->get_time( from_idx ) ;
    idx = from_idx ;
    while ( 1 ) {
		int	old_idx = idx, diff ;
		idx = (idx + dir + num) % num ;
		if ( idx == to_idx ) {
			break ;
		}
		diff = Abs( time - r->get_time( idx ) ) ;
		if ( circle ) {
			if ( dir < 0 && idx > old_idx ) {
				diff = circle - r->get_time( idx ) ;
			} else if ( dir > 0 && idx < old_idx ) {
				diff = circle - r->get_time( old_idx ) ;
			}
		}
		start_time += diff ;
		time = r->get_time( idx ) ;
		add_res_pass( start_time, r->get_s_id( idx ) ) ;
    }
}

int
make_route( RESULT* res, int size_r )
{
    int		jyosya_time = 0 ;
    int		last_s_id = SID_NONE, last_r_id = RID_NONE, last_time = 0 ;
    int		start_s_id = 0 ;
    Value	*v_base = (Value*) ValueHandle ;
	WORD	from ;

    Res = res ;
    Size_res = size_r ;
    Num_res = 0 ;
    /* 所要時間等を初期化する */
    Jyosya_time = Wait_time = Walk_time = Total_time = Change_cnt = 0 ;
    if ( v_base[ 1 ].val == VAL_NONE ) {
		/* 利用できる経路がない場合 */
		add_res_none() ;
		return Num_res ;
    }
    Total_time  = v_base[ 1 ].val ;
    for ( from = 1 ; from != NULL_NODE ; from = v_base[ from ].dad ) {
		AdjLink	*a = get_adj( from ) ;
		int		s_id = a->get_s_id() ;
		int		r_id = a->get_r_id() ;
		int		time = v_base[ from ].val ;
		int		tmp, walk, wait ;

		if ( from == 1 ) {
			last_s_id = s_id ;
			last_r_id = r_id ;
			last_time = time ;
			continue ;
		}
		if ( s_id == last_s_id && r_id == 0 ) {
			/* 目的地（乗換駅）に到着した場合の処理 */
			if ( last_s_id != start_s_id ) {
				/* 利用路線の場合 */
				add_res_line( last_r_id, jyosya_time - last_time ) ;
				Jyosya_time += jyosya_time - last_time ;
				/* 通過駅の場合 */
				make_pass( last_r_id, start_s_id, last_s_id,
					   Total_time - jyosya_time ) ;
				/* 到着駅の場合 */
				add_res_ariv( Total_time - last_time, last_s_id ) ;
			}
			/* 目的地の場合 */
			add_res_end( Total_time - last_time,
				 Walk_time, Wait_time, Jyosya_time ) ;
		} else if ( v_base[ from ].dad == NULL_NODE ) {
			/* 目的地（乗換駅以外）に到着した場合の処理 */
			if ( s_id != start_s_id ) {
				if ( r_id ) {
					/* 利用路線の場合 */
					add_res_line( r_id, jyosya_time - time ) ;
					Jyosya_time += jyosya_time - time ;
					/* 通過駅の場合 */
					make_pass( last_r_id, start_s_id, s_id,
						   Total_time - jyosya_time ) ;
					/* 到着駅の場合 */
					add_res_ariv( Total_time - time, s_id ) ;
				} else {
					if ( last_r_id ) {
						/* 利用路線の場合 */
						add_res_line( last_r_id, jyosya_time - last_time ) ;
						Jyosya_time += jyosya_time - last_time ;
						/* 通過駅の場合 */
						make_pass( last_r_id, start_s_id, last_s_id,
							   Total_time - jyosya_time ) ;
					}
					/* 到着駅の場合 */
					add_res_ariv( Total_time - last_time, last_s_id ) ;
					/* 乗り換えの場合 */
					add_res_change( 0, last_time - time ) ;
					Walk_time += last_time - time ;
					/* 発車駅の場合 */
					add_res_dept( Total_time - time, s_id ) ;
				}
			}
			/* 目的地の場合 */
			add_res_end( Total_time - time,
						 Walk_time, Wait_time, Jyosya_time ) ;
		} else if ( s_id == last_s_id && r_id != 0 ) {
			/* 乗り換え駅の場合 */
			if ( last_r_id == 0 ) {
				/* 出発駅の場合 */
				add_res_start( last_time - time ) ;
				Wait_time += last_time - time ;
			} else {
				Change_cnt ++ ;
				/* 利用路線の場合 */
				add_res_line( last_r_id, jyosya_time - last_time ) ;
				Jyosya_time += jyosya_time - last_time ;
				/* 通過駅の場合 */
				make_pass( last_r_id, start_s_id, s_id,
						   Total_time - jyosya_time ) ;
				tmp = last_time - time ;
				wait = get_rosen( r_id )->get_wait() ;
				if ( tmp < wait ) {
					walk = 0, wait = tmp ;
				} else {
					walk = tmp - wait ;
				}
				if ( tmp > 0 ) {
					/* 到着駅の場合 */
					add_res_ariv( Total_time - last_time, last_s_id ) ;
					/* 乗り換えの場合 */
					add_res_change( wait, walk ) ;
					Wait_time += wait ;
					Walk_time += walk ;
				}
			}
			/* 発車駅の場合 */
			add_res_dept( Total_time - time, s_id ) ;
			/* 乗車時間計測の為に現在時刻を設定する */
			jyosya_time = time ;
			start_s_id = s_id ;
		} else if ( s_id != last_s_id && r_id != last_r_id ) {
			Change_cnt ++ ;
			/* 駅も路線も異なっている場合 */
			tmp = last_time - time ;
			wait = get_rosen( r_id )->get_wait() ;
			if ( tmp < wait ) {
				walk = 0, wait = tmp ;
			} else {
				walk = tmp - wait ;
			}
			if ( last_s_id != start_s_id ) {
				if ( last_r_id ) {
					/* 利用路線の場合 */
					add_res_line( last_r_id, jyosya_time - last_time ) ;
					Jyosya_time += jyosya_time - last_time ;
					/* 通過駅の場合 */
					make_pass( last_r_id, start_s_id, last_s_id,
						   Total_time - jyosya_time ) ;
				}
				/* 到着駅の場合 */
				add_res_ariv( Total_time - last_time, last_s_id ) ;
			}
			/* 乗り換えの場合 */
			add_res_change( wait, walk ) ;
			Wait_time += wait ;
			Walk_time += walk ;
			/* 発車駅の場合 */
			add_res_dept( Total_time - time, s_id ) ;
			/* 乗車時間計測の為に現在時刻を設定する */
			jyosya_time = time ;
			start_s_id = s_id ;
		}
		last_s_id = s_id ;
		last_r_id = r_id ;
		last_time = time ;
    }
    return Num_res ;
}

static	void
make_route_time( BYTE **buf, int kind, int time )
{
    **buf = ' ' ;
    (*buf) ++ ;
    switch ( kind ) {
    case 0:	StrAppend( buf, get_msg(0,5) ) ;	break ;
    case 1:	StrAppend( buf, get_msg(0,6) ) ;	break ;
    case 2:	StrAppend( buf, get_msg(0,7) ) ;	break ;
    case 3:	StrAppend( buf, get_msg(0,8) ) ;	break ;
    default:						return ;
    }
    make_timestr96( time, *buf ) ;
    *buf += 5 ;
}

BYTE *
get_route( RESULT *res, int start_time, int addlen )
{
    static BYTE	tmpbuf[ 100 ] ;
    BYTE	*p = tmpbuf, *q ;
    int		s_id, len ;

    switch ( res->type ) {
    case RESULT_START:
    case RESULT_DEPT:
    case RESULT_PASS:
    case RESULT_ARIV:
		make_timestr( start_time + res->time, p ), p += 5 ;
		break ;
    case RESULT_LINE:
		StrAppend( &p, get_msg(0,9) ) ;
		break ;
    case RESULT_NONE:
		StrAppend( &p, get_msg(1,0) ) ;
		break ;
    case RESULT_CHANGE:
		StrAppend( &p, get_msg(0,13) ) ;
		break ;
    case RESULT_END:
		*p++ = ' ' ; *p++ = '[' ;
		ItoaN( Change_cnt, p, 2 ), p += 2 ;
		*p++ = ']' ;
		break ;
    default:
		*p++ = ' ' ; *p++ = ' ' ; *p++ = ' ' ; *p++ = ' ' ; *p++ = ' ' ;
		break ;
    }
    *p++ = ' ' ;
    switch ( res->type ) {
    case RESULT_DEPT:
		s_id = res->v.dept_s_id ;
		break ;
    case RESULT_PASS:
		s_id = res->v.pass_s_id ;
		break ;
    case RESULT_ARIV:
		s_id = res->v.ariv_s_id ;
		break ;
    default:
		s_id = 0 ;
		break ;
    }
    switch ( res->type ) {
    case RESULT_START:
		if ( res->v.start_wait > 0 ) {
			Memset( p, ' ', 10 + addlen ), p += 10 + addlen ;
			make_route_time( &p, 1, res->v.start_wait ) ;
		}
		break ;
    case RESULT_DEPT:
    case RESULT_PASS:
    case RESULT_ARIV:
		StrAppend( &p, get_station( s_id )->get_name() ) ;
		if ( !F_inter ) {
			*p++ = '(' ;
			StrAppend( &p, get_station_kana( s_id ) ) ;
			*p++ = ')' ;
		}
		break ;
    case RESULT_LINE:
		*p++ = ' ' ;
		*p++ = ' ' ;
		q = p ;
		StrAppend( &p, get_rosen( res->v.line.r_id )->get_name() ) ;
		if ( F_dispTime ) {
			len = (16 + addlen) - Strlen( q ) ;
			if ( len > 0 ) {
			Memset( p, ' ', len ), p += len ;
			make_route_time( &p, 2, res->v.line.jyosya ) ;
			}
		}
		break ;
    case RESULT_CHANGE:
		if ( F_dispTime ) {
			Memset( p, ' ', 2 + addlen ), p += 2 + addlen ;
			make_route_time( &p, 0, res->v.change.walk ) ;
			make_route_time( &p, 1, res->v.change.wait ) ;
		}
		break ;
    case RESULT_END:
		Memset( p, ' ', 2 + addlen ), p += 2 + addlen ;
		make_route_time( &p, 0, res->v.end.walk ) ;
		make_route_time( &p, 1, res->v.end.wait ) ;
		make_route_time( &p, 2, res->v.end.jyosya ) ;
		break ;
    }
    *p = 0 ;
    return tmpbuf ;
}

void
other_job( int mode )
{
    if ( OtherJob ) {
	(*OtherJob)( mode ) ;
    }
}
#endif	/* TOOLS */

/*----------------------------------------------------------------------------
 * Adj
 *--------------------------------------------------------------------------*/
void
init_adj( int num )
{
    Assert( AdjHandle == NULL ) ;
    Num_adj = Num_adj_max = num ;
    AdjHandle = LocalAlloc( LPTR, sizeof (AdjLink) * Num_adj ) ;
    Start = End = 0 ;
    AdjLink	*a = (AdjLink*) AdjHandle ;
    for ( int i = 0 ; i < Num_adj ; i ++, a ++ ) {
		a->init() ;
		if ( i == 0 ) {
			Start = a ;
		} else if ( i == 1 ) {
			End   = a ;
		}
    }
}

#ifndef	TOOLS
void
free_adj()
{
    if ( AdjHandle ) {
		LocalFree( AdjHandle ) ;
		AdjHandle = NULL ;
    }
    Num_adj = Num_init_adj = Num_adj_max = 0 ;
}
#endif	/* TOOLS */

AdjLink *
get_adj( int index )
{
#ifdef	DEBUG
    Assert( index >= 0 && index < Num_adj ) ;
#endif	/* DEBUG */
    return &((AdjLink *) AdjHandle)[ index ] ;
}

int
new_adj()
{
	if ( Num_adj >= Num_adj_max ) {
		fatal_msg( 20 ) ;
	}
	int		idx = Num_adj ++ ;
	get_adj( idx )->init() ;
	return idx ;
}

void
del_adj( int idx )
{
    Assert( idx < Num_adj ) ;
    get_adj( idx )->init() ;
}

#ifndef	TOOLS

struct ImageHeader {
	BYTE	id[ 4 ] ;
	BYTE	ver[ 4 ] ;
	WORD	max_s ;
	WORD	max_r ;
	WORD	strnext ;
	WORD	strsize ;
	WORD	num_rstat ;
	WORD	num_adj_max ;
	WORD	num_init_adj ;
	WORD	num_fnode ;
	WORD	num_node_init ;
	DWORD	dummy_l[ 26 ] ;
} ;

int
save_img( const BYTE *version )
{
	WORD		size ;
	ImageHeader	header ;
	LPBYTE		buf ;
    AdjLink		*a_base = get_adj( 0 ), *a ;

	clear_adj() ;
	Start->init() ;
	End->init() ;
	a = &a_base[ Num_init_adj ] ;
	for ( int i = Num_init_adj ; i < Num_adj_max ; i ++ ) {
		a->init() ;
		a ++ ;
	}
	Fseek( 0L, 0 ) ;
	/* ヘッダー情報の格納 */
	Memset( (BYTE*) &header, 0, sizeof header ) ;
	Memcpy( header.id, "IMG0", sizeof header.id ) ;
	Memcpy( header.ver, version, sizeof header.ver ) ;
	header.max_s = Max_s ;
	header.max_r = Max_r ;
	header.strnext = StrNext ;
	header.strsize = StrSize ;
	header.num_rstat = Num_rstat ;
	header.num_adj_max = Num_adj_max ;
	header.num_init_adj = Num_init_adj ;
	header.num_fnode = Num_fnode ;
	header.num_node_init = Num_node_init ;
	Fwrite( &header, sizeof header ) ;
	/* STATION */
	Fwrite( "STAT", 4 ) ;
	buf = (BYTE *) StationHandle ;
	size = sizeof (STATION) * Num_s ;
	Fwrite_far( buf, size ) ;
	/* SORT */
	Fwrite( "SORT", 4 ) ;
	buf = (BYTE *) SortHandle ;
	size = sizeof (WORD) * Num_s ;
	Fwrite_far( buf, size ) ;
	/* KSORT */
	Fwrite( "KSOR", 4 ) ;
	buf = (BYTE *) KSortHandle ;
	size = sizeof (WORD) * Num_s ;
	Fwrite_far( buf, size ) ;
	/* ROSEN */
	Fwrite( "ROSE", 4 ) ;
	buf = (BYTE *) RosenHandle ;
	size = sizeof (ROSEN) * Num_r ;
	Fwrite_far( buf, size ) ;
	/* STRIDX */
	Fwrite( "STRI", 4 ) ;
	buf = (BYTE *) StrIdxHandle ;
	size = sizeof (int) * StrNext ;
	Fwrite_far( buf, size ) ;
	/* STRDATA */
	Fwrite( "STRP", 4 ) ;
	buf = (BYTE *) StrPtrHandle ;
	size = sizeof (BYTE) * StrSize ;
	Fwrite_far( buf, size ) ;
	/* RSTAT */
	Fwrite( "RSTA", 4 ) ;
	buf = (BYTE *) RStatHandle ;
	size = sizeof (short) * Num_rstat * 2 ;
	Fwrite_far( buf, size ) ;
	/* ADJLINK */
	Fwrite( "ADJL", 4 ) ;
	buf = (BYTE *) AdjHandle ;
	size = sizeof (AdjLink) * Num_adj_max ;
	Fwrite_far( buf, size ) ;
	/* FNODE */
	Fwrite( "FNOD", 4 ) ;
	buf = (BYTE *) FNodeHandle ;
	size = sizeof (FNode) * Num_fnode ;
	Fwrite_far( buf, size ) ;
	return 0 ;
}

BOOL
load_img( const BYTE *version )
{
	WORD	size ;
	ImageHeader	header ;
	BYTE	tmpbuf[10] ;
	BYTE	*buf ;

	if ( Fread( (BYTE*) &header, sizeof header ) != sizeof header ) {
		return FALSE ;
	}
	if ( Memcmp( header.id, "IMG0", sizeof header.id ) != 0 ) {
		return FALSE ;
	}
	if ( Memcmp( header.ver, COMPATIBLE, sizeof header.ver ) < 0 ) {
		return FALSE ;
	}
	if ( Memcmp( header.ver, version, sizeof header.ver ) > 0 ) {
		return FALSE ;
	}
	Max_s = header.max_s ;
	Max_r = header.max_r ;
	StrNext = header.strnext ;
	StrSize = header.strsize ;
	Num_rstat = header.num_rstat ;
	init_station() ;
	init_rosen() ;
	StringAllocN( StrNext, StrSize ) ;
	Num_s = Max_s ;
	Num_r = Max_r ;
	init_adj( header.num_adj_max ) ;
	Num_adj = Num_init_adj = header.num_init_adj ;
	pq_init( header.num_adj_max ) ;
	Max_fnode = header.num_fnode ;
	init_FNode() ;
	Num_fnode = Max_fnode ;
	Max_node = add_n ;
	Num_node_init = Num_node = 0 ;	//header.num_node_init ;
	init_Value( Num_adj + add_a ) ;
	tmpbuf[4]=0;
	/* STATION */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) StationHandle ;
	size = sizeof (STATION) * Num_s ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* SORT */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) SortHandle ;
	size = sizeof (WORD) * Num_s ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* KSORT */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) KSortHandle ;
	size = sizeof (WORD) * Num_s ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* ROSEN */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) RosenHandle ;
	size = sizeof (ROSEN) * Num_r ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* STRIDX */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) StrIdxHandle ;
	size = sizeof (int) * StrNext ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* STRDATA */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) StrPtrHandle ;
	size = sizeof (BYTE) * StrSize ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* RSTAT */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) RStatHandle ;
	size = sizeof (short) * Num_rstat * 2 ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	init_kaisya() ;
	/* ADJLINK */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) AdjHandle ;
	size = sizeof (AdjLink) * Num_adj_max ;
	if ( Fread_far( buf, size ) <= 0 ) {
		goto error ;
	}
	/* FNODE */
	Fread( tmpbuf, 4 ) ;
	buf = (BYTE *) FNodeHandle ;
	size = sizeof (FNode) * Size_fnode ;
	if ( Fread_far( buf, size ) != size ) {
		goto error ;
	}
	F_load_adj = 0 ;
	return TRUE ;

error:
	clear_data() ;
	return FALSE ;
}
#endif	/* TOOLS */
