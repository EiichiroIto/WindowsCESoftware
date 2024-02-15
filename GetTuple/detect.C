#include	<windows.h>
#include	<tchar.h>
#include	<types.h>
#include	<cardserv.h>
#include	<devload.h>
#include	"detect.h"

BOOL WINAPI
DllMain( HINSTANCE DllInstance, ULONG Reason, LPVOID Reserved )
{
	return TRUE ;
}

LPTSTR
DetectionFunction( CARD_SOCKET_HANDLE hSock, UCHAR DevType, LPTSTR DevKey, DWORD DevKeyLen )
{
	_tcscpy( DevKey, TEXT("GetTuple") ) ;
	return DevKey ;
}

typedef struct _CLIENT_CONTEXT {
	CARD_SOCKET_HANDLE	hSocket ;			// from active key
	CARD_CLIENT_HANDLE	hCardClient ;		// from Registration Callback
	CRITICAL_SECTION	ioctlCritSect ;		// ensures only one ioctl per context
} CLIENT_CONTEXT, *PCLIENT_CONTEXT;

extern	BOOL	GetRawTuple( CARD_SOCKET_HANDLE hSocket, CARD_CLIENT_HANDLE hCardClient, PBYTE pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned ) ;

STATUS
StatusCallback( CARD_EVENT EventCode, CARD_SOCKET_HANDLE hSocket, PCARD_EVENT_PARMS pParms )
{
	return CERR_SUCCESS ;
}

BOOL
getSocketHandle( LPTSTR activePath, PCARD_SOCKET_HANDLE phSocket )
{
	HKEY	hKey ;
	DWORD	valType, valLen ;

	if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE, activePath, 0, 0, &hKey ) ) {
		return FALSE ;
	}
	valLen = sizeof (CARD_SOCKET_HANDLE) ;
	if ( RegQueryValueEx( hKey, DEVLOAD_SOCKET_VALNAME, 0, &valType, (LPBYTE) phSocket, &valLen ) ) {
		RegCloseKey( hKey ) ;
		return FALSE ;
	}
	RegCloseKey( hKey ) ;
	return TRUE ;
}

DWORD
DET_Init( DWORD dwContext )
{
	PCLIENT_CONTEXT		pcc = 0 ;
	CARD_REGISTER_PARMS	parms ;
	CARD_WINDOW_PARMS	winparms ;
	CARD_WINDOW_HANDLE	hCardWindow ;

	pcc = (PCLIENT_CONTEXT) LocalAlloc( LPTR, sizeof (CLIENT_CONTEXT) ) ;
	if ( !pcc ) {
		goto errret ;
	}
	if ( !getSocketHandle( (LPTSTR) dwContext, &(pcc->hSocket) ) ) {
		goto errret ;
	}
	InitializeCriticalSection( &(pcc->ioctlCritSect) ) ;
	parms.fEventMask = EVENT_MASK_CARD_DETECT ;
	parms.uClientData = (UINT32)pcc ;
	parms.fAttributes = CLIENT_ATTR_MEM_DRIVER | CLIENT_ATTR_NOTIFY_SHARED
					   |CLIENT_ATTR_NOTIFY_EXCLUSIVE ;
	pcc->hCardClient = CardRegisterClient( StatusCallback, &parms ) ;
	if ( pcc->hCardClient == 0 ) {
		goto errret ;
	}
	winparms.fAttributes = 0 ;
	winparms.hSocket	 = pcc->hSocket ;
	winparms.uWindowSize = 4096 ;
	winparms.fAccessSpeed = WIN_SPEED_USE_WAIT ;
	hCardWindow = CardRequestWindow( pcc->hCardClient, &winparms ) ;
	if ( !hCardWindow ) {
		goto errret ;
	}
	return (DWORD) pcc ;

errret:
	if ( pcc && pcc->hCardClient ) {
		CardDeregisterClient( pcc->hCardClient ) ;
	}
	if ( pcc ) {
		LocalFree( pcc ) ;
	}
	return 0 ;
}

BOOL
DET_Deinit( DWORD dwClientContext )
{
	STATUS			status ;
	PCLIENT_CONTEXT	pcc = (PCLIENT_CONTEXT) dwClientContext ;

	status = CardDeregisterClient( pcc->hCardClient) ;
	LocalFree( pcc ) ;
	return TRUE ;
}

DWORD
DET_Open( DWORD dwClientContext, DWORD dwAccess, DWORD dwShareMode )
{
	return dwClientContext ;
}

BOOL
DET_Close( DWORD dwOpenContext )
{
	return TRUE ;
}

DWORD
DET_Read( DWORD dwOpenContext, LPVOID pBuffer, DWORD dwNumBytes )
{
	return 0 ;
}

DWORD
DET_Write( DWORD dwOpenContext, LPCVOID pBuffer, DWORD dwNumBytes )
{
	return 0 ;
}

DWORD
DET_Seek( DWORD dwOpenContext, long lDistance, DWORD dwMoveMethod )
{
	return 0 ;
}

BOOL
DET_IOControl( DWORD dwOpenContext, DWORD dwIoControlCode,
			   PBYTE pInBuf, DWORD nInBufSize,
			   PBYTE pOutBuf, DWORD nOutBufSize,
			   PDWORD pBytesReturned )
{
	BOOL			retval = FALSE ;
	PCLIENT_CONTEXT	pcc = (PCLIENT_CONTEXT) dwOpenContext ;

	EnterCriticalSection( &pcc->ioctlCritSect ) ;
	*pBytesReturned = 0 ;
	switch ( dwIoControlCode ) {
	case IOCTL_PCMCIA_TEST_TUPLES:
		retval = GetRawTuple( pcc->hSocket, pcc->hCardClient, pOutBuf, nOutBufSize, pBytesReturned ) ;
		break;
	}
	LeaveCriticalSection( &pcc->ioctlCritSect ) ;
	return retval ;
}

void
DET_PowerUp( void )
{
}

void
DET_PowerDown( void )
{
}

#include	<excpt.h>
#include	<tuple.h>

CARD_CLIENT_HANDLE	hClient ;

#define	BUFFER_SIZE	4096

UCHAR	buffer[BUFFER_SIZE] ;

VOID
PrivateGetTuple( CARD_SOCKET_HANDLE hSock, BOOL fLinks, PBYTE pOutBuf, PDWORD pBytesReturned )
{
    PUCHAR				tupleBuffer = buffer ;
    UCHAR				tupleCode ;
    UCHAR				linkValue ;
    PCARD_TUPLE_PARMS	pTuple ;
    PCARD_DATA_PARMS	pData ;
    STATUS				status ;
    int					cNulls = 0 ;
	DWORD				size = *pBytesReturned ;

    pTuple = (PCARD_TUPLE_PARMS) buffer ;
    pData = (PCARD_DATA_PARMS) buffer ;
    pTuple->hSocket = hSock ;
    pTuple->fAttributes = (fLinks == TRUE) ? 1 : 0 ;
    pTuple->uDesiredTuple = CISTPL_LINKTARGET ;
    status = CardGetFirstTuple( pTuple ) ;
    pTuple->uDesiredTuple = 0xff ;
    status = CardGetFirstTuple( pTuple ) ;
    if ( status ) {
        return ;
    }
    while ( 1 ) {
        tupleCode = pTuple->uTupleCode ;
        linkValue = pTuple->uTupleLink ;
		if ( size >= 2 ) {
			size -= 2 ;
			*pOutBuf++ = tupleCode ;
			*pOutBuf++ = linkValue ;
		}
        if ( tupleCode == CISTPL_NULL ) {
            cNulls ++ ;
            if ( cNulls == 10 ) {
                break ;
            }
        } else {
            cNulls = 0 ;
        }
        pData->uBufLen = BUFFER_SIZE - sizeof(CARD_DATA_PARMS) ;
        pData->uTupleOffset = 0 ;
        status = CardGetTupleData( pData ) ;
        if ( status ) {
			break ;
        }
        linkValue = (UINT8) pData->uDataLen ;
        tupleBuffer = (PUCHAR) pData ;
        tupleBuffer += sizeof (CARD_DATA_PARMS) ;
		if ( size >= linkValue ) {
			size -= linkValue ;
			memcpy( pOutBuf, tupleBuffer, linkValue ) ;
			pOutBuf += linkValue ;
		}
        status = CardGetNextTuple(pTuple);
        if ( status ) {
			break ;
        }
    }
	*pBytesReturned = *pBytesReturned - size ;
}

BOOL
GetRawTuple( CARD_SOCKET_HANDLE hSocket, CARD_CLIENT_HANDLE hCardClient, PBYTE pOutBuf, DWORD nOutBufSize, PDWORD pBytesReturned )
{
	hClient = hCardClient ;

	if ( !pOutBuf || !nOutBufSize ) {
		return FALSE ;
	}
	try {
		PrivateGetTuple( hSocket, FALSE, pOutBuf, &nOutBufSize ) ;
		*pBytesReturned = nOutBufSize ;
	} except ( GetExceptionCode() == STATUS_ACCESS_VIOLATION ) {
		return FALSE;
	}
	return TRUE;
}
