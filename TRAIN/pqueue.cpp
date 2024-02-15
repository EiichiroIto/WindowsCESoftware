#include	"lfunc.h"
#include	"pqueue.h"

struct far PQ_ITEM {
    short	a ;
    BYTE	b ;
    BYTE	c ;
    short	p ;
    short	info ;
} ;

static	HLOCAL	PQHandle = NULL ;
static	int	pq_num = 0 ;
static	int	pq_size = 0 ;
static	PQ_ITEM		*PQBase = 0 ;

#define		get_pq(x)	(&PQBase[(x)])

void
pq_setup( void )
{
    PQBase = (PQ_ITEM*) PQHandle ;
}

static void
pq_insert( int x, int v, BYTE v2, BYTE v3 )
{
    register PQ_ITEM	*p ;

#ifdef	DEBUG
    Assert( pq_num < pq_size ) ;
#endif	/* DEBUG */
    pq_num ++ ;
    p = get_pq( pq_num ) ;
    p->a = v ;
    p->b = v2 ;
    p->c = v3 ;
    p->info = x ;
    p = get_pq( x ) ;
    p->p = pq_num ;
}

int
pq_update( int x, int v, BYTE v2, BYTE v3 )
{
    register int	count = pq_num ;

    if ( count > 0 ) {
		register PQ_ITEM	*p = get_pq( 1 ) ;

		do {
			if ( p->info == x ) {
				PQ_ITEM		*q = get_pq( x ) ;
				q = get_pq( q->p ) ;
				int			a = q->a ;
				int			b = q->b ;
				if ( v < a || (v == a && v2 < b) || (v == a && v2 == b && v3 < q->c) ) {
					// ÇÊÇËè¨Ç≥Ç¢èÍçáÇÕïœçX
					q->a = v ;
					q->b = v2 ;
					q->c = v3 ;
					return 1 ;
				}
				// ÇÊÇËëÂÇ´Ç¢Ç©ìØÇ∂èÍçáÇÕÇ»Ç…Ç‡ÇµÇ»Ç¢
				return 0 ;
			}
			p ++ ;
		} while ( -- count ) ;
    }
    pq_insert( x, v, v2, v3 ) ;
    return 1 ;
}

int
pq_empty()
{
    return pq_num <= 0 ;
}

int
pq_remove()
{
    PQ_ITEM			*p1 = get_pq( 1 ) ;
    PQ_ITEM			*p2 = p1 + 1 ;
    register int	count = pq_num - 1 ;
    register int	min = 1 ;

    if ( count >= 1 ) {
		int	j = 2 ;

		do {
			int		p1a = p1->a ;
			int		p2a = p2->a ;
			int		p1b = p1->b ;
			int		p2b = p2->b ;
			if ( p2a < p1a || (p2a == p1a && p2b < p1b) || (p2a == p1a && p2b == p1b && p2->c < p1->c) ) {
				min = j ;
				p1 = p2 ;
			}
			p2 ++, j ++ ;
		} while ( -- count ) ;
    }
    p2 = get_pq( pq_num ) ;

    count = p2->a ;	p2->a = p1->a ;		p1->a = count ;
    count = p2->b ;	p2->b = p1->b ;		p1->b = count ;
    count = p2->c ;	p2->c = p1->c ;		p1->c = count ;
    count = p2->info ;	p2->info = p1->info ;	p1->info = count ;

    get_pq( count )->p = min ;
    pq_num -- ;
    return p2->info ;
}

void
pq_init( int num )
{
    Assert( PQHandle == NULL ) ;
    pq_size = num + 1 ;
    PQHandle = LocalAlloc( LPTR, sizeof (PQ_ITEM) * pq_size ) ;
    pq_clear() ;
}

void
pq_clear( void )
{
    PQ_ITEM		*p ;
    pq_setup() ;
    p = get_pq(0) ;
    for ( int i = 0 ; i < pq_size ; i ++, p ++ ) {
		p->a = -9999 ;
		p->b = 0 ;
		p->c = 0 ;
    }
    pq_num = 0 ;
}

void
pq_free()
{
    if ( PQHandle ) {
		LocalFree( PQHandle ) ;
		PQHandle = NULL ;
    }
}
