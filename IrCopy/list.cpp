#include	"list.h"

/*
 * List �N���X������
 */
void
List::insert( PLink ent, PLink afterof )
{
	nlist ++ ;
	if ( !last ) {
		/* �G���g����1���Ȃ��ꍇ */
		ent->next = 0 ;
		last = ent ;
		last->next = last ;
	} else if ( !afterof ) {
		/* �擪�ɒǉ�����ꍇ */
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
		/* �ŏI�f�[�^�Ȃ牽�����Ȃ� */
		return ;
	}
	nlist -- ;
	next = cur->next ;
	if ( last == next ) {
		/* �폜�Ώۂ��ŏI�f�[�^�̏ꍇ */
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
		/* �ŏI�f�[�^���Ȃ���Ή������Ȃ� */
		return ;
	}
	nlist -- ;
	to_delete = last ;
	if ( last == last->next ) {
		/* �Ō�̈�̏ꍇ */
		last = 0 ;
	} else {
		cur = last->next ;
		/* �Ōォ��2�Ԗڂ�T�� */
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
		/* �ŏI�f�[�^���Ȃ���Ή������Ȃ� */
		return ;
	}
	nlist -- ;
	to_delete = last->next ;
	if ( last == last->next ) {
		/* �Ō�̈�̏ꍇ */
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
		/* ���X�g�ɉ����Ȃ��ꍇ */
		return ;
	} else if ( last->next == p ) {
		/* ���ɐ擪�������ꍇ */
		return ;
	}
	PLink	oldtop = last->next, cur = oldtop ;
	/* �w�肵���f�[�^�̒��O�f�[�^��T�� */
	while ( cur->next != p ) {
		cur = cur->next ;
	}
	cur->next = p->next ;
	if ( last == p ) {
		/* �w�肵���f�[�^���ŏI�f�[�^�������ꍇ */
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
