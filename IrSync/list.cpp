#include	"list.h"

/*
 * List クラス実現部
 */
void
List::insert( PLink ent, PLink afterof )
{
	nlist ++ ;
	if ( !last ) {
		/* エントリが1つもない場合 */
		ent->next = 0 ;
		last = ent ;
		last->next = last ;
	} else if ( !afterof ) {
		/* 先頭に追加する場合 */
		ent->next = last->next ;
		last->next = ent ;
	} else {
		PLink	l = last->next ;
		do {
			if ( l == afterof ) {
				break ;
			}
			l = l->next ;
		} while ( l != last ) ;
		if ( l == last ) {
			last = ent ;
		}
		ent->next = l->next ;
		l->next = ent ;
	}
}

void
List::append( PLink ent )
{
	nlist ++ ;
	if ( last ) {
		ent->next = last->next ;
		last = last->next = ent ;
	} else {
		ent->next = 0 ;
		last = ent ;
		last->next = last ;
	}
}

int
List::count() const
{
	if ( !last ) {
		return 0 ;
	}
	PLink		cur = last->next ;
	int			num = 1 ;
	while ( cur != last ) {
		num ++ ;
		cur = cur->next ;
	}
	return num ;
}

void
List::delete_next( PLink cur )
{
	PLink	next ;

	if ( !cur ) {
		return ;
	} else if ( last == cur ) {
		/* 最終データなら何もしない */
		return ;
	}
	nlist -- ;
	next = cur->next ;
	if ( last == next ) {
		/* 削除対象が最終データの場合 */
		last = cur ;
	}
	cur->next = next->next ;
	delete next ;
}

#if 0
void
List::delete_last()
{
	PLink	to_delete, cur ;

	if ( !last ) {
		/* 最終データがなければ何もしない */
		return ;
	}
	nlist -- ;
	to_delete = last ;
	if ( last == last->next ) {
		/* 最後の一個の場合 */
		last = 0 ;
	} else {
		cur = last->next ;
		/* 最後から2番目を探す */
		while ( cur->next != last ) {
			cur = cur->next ;
		}
		cur->next = last->next ;
		last = cur ;
	}
	delete to_delete ;
}
#endif

void
List::delete_first()
{
	PLink	to_delete ;

	if ( !last ) {
		/* 最終データがなければ何もしない */
		return ;
	}
	nlist -- ;
	to_delete = last->next ;
	if ( last == last->next ) {
		/* 最後の一個の場合 */
		last = 0 ;
	} else {
		last->next = last->next->next ;
	}
	delete to_delete ;
}

void
List::clear()
{
	PLink	l = last ;

	nlist = 0 ;
	if ( !l ) {
		return ;
	}
	do {
		PLink	ll = l ;
		l = l->next ;
		delete ll ;
	} while ( l != last ) ;
	last = 0 ;
}

#if 0
void
List::move_top( PLink p )
{
	if ( !last ) {
		/* リストに何もない場合 */
		return ;
	} else if ( last->next == p ) {
		/* 既に先頭だった場合 */
		return ;
	}
	PLink	oldtop = last->next, cur = oldtop ;
	/* 指定したデータの直前データを探す */
	while ( cur->next != p ) {
		cur = cur->next ;
	}
	cur->next = p->next ;
	if ( last == p ) {
		/* 指定したデータが最終データだった場合 */
		last = cur ;
	}
	last->next = p ;
	p->next = oldtop ;
}
#endif

PLink
List::operator[]( int idx )
{
	Iterator	l( *this, idx ) ;
	return l() ;
}

PLink
List::get_top()
{
	if ( !last ) {
		return 0 ;
	}
	return last->next ;
}
