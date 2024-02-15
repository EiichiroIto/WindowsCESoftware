#ifndef	APP_LIST
#define	APP_LIST

#define		IDX_NONE	((DWORD)-1)

/* Link クラス宣言部 */
class Link {
friend class List ;
friend class Iterator ;
	Link	*next ;

public:
	virtual ~Link() {}

protected:
	Link	*get_next() { return next ; }
} ;

typedef Link	*PLink ;

/* List クラス宣言部 */
class List {
friend class Iterator ;
	PLink	last ;
	int		nlist ;

public:
	void	insert( PLink p, PLink afterof ) ;
	void	append( PLink p ) ;
	int		count() const ;
	int		size() const {
		return nlist ;
	}
	void	delete_next( PLink cur ) ;
//	void	delete_last() ;
	void	delete_first() ;
	void	clear() ;
//	void	move_top( PLink p ) ;
	PLink	operator[]( int idx ) ;
	PLink	get_top() ;

	List() { last = 0 ; nlist = 0 ; }
	virtual ~List() { clear() ; }
} ;

typedef List	*PList ;

/* Iterator クラス宣言部 */
class Iterator {
	PLink	ce ;
	PList	cs ;

public:
	PLink operator()() {
		PLink ret = ce ? ce = ce->next : 0 ;
		if ( cs && ce == cs->last ) {
			ce = 0 ;
		}
		return ret ;
	}
	void	set( PList s ) {
		cs = s ; ce = cs->last ;
	}
	void	set( int idx ) {
		PLink		e ;

		if ( !idx ) {
			return ;
		}
		while ( e = (*this)() ) {
			if ( !(-- idx ) ) {
				break ;
			}
		}
	}
	void	set( PList s, int num ) {
		set( s ) ;
		set( num ) ;
	}

	Iterator() { cs = 0 ; ce = 0 ; }
	Iterator( List& s ) { set( &s ) ; }
	Iterator( List& s, int num ) { set( &s, num ) ; }
	Iterator( List& s, PLink ent ) {
		cs = &s ;
		ce = (ent == cs->last) ? 0 : ent ;
	}
} ;
#endif	/* APP_LIST */
