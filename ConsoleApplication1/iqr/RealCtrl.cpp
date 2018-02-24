#include "RealCtrl.h"
#include "SockCtrl.h"

CRealCtrl::CRealCtrl ( )
: m_pRealIP ( NULL ), m_pRealIF ( NULL ), m_nPort ( 0 )
{
    Init ( NULL );
}

CRealCtrl::CRealCtrl ( ST_PROC_INFO * p, const char * pRealIP, const char * pRealIF, const int nPort, epoll_event * pstRealEvents, const int nRealEpollFD )
: m_pRealIP ( pRealIP ), m_pRealIF ( pRealIF ), m_nPort ( nPort )
{
    Init ( p );
    SetRealPort ( pstRealEvents, nRealEpollFD );
}

CRealCtrl::~CRealCtrl ( )
{
}

void CRealCtrl::Init ( ST_PROC_INFO * p )
{
    m_pSockCtrl   = NULL;
    m_pstProcInfo = p;
    m_nFD         = 0;
    m_nRecvCnt    = 0;

    memset ( &m_stIn, 0x00, sizeof ( m_stIn ) );
}

/*------------------------------------------------------------------
    Purpose   : RealPriceThread에서 사용할 MC 포트를 Epoll에 연결
    Parameter : 
    return    : 
  ------------------------------------------------------------------*/
void CRealCtrl::SetRealPort ( epoll_event * pstRealEvents, const int nRealEpollFD )
{
    if ( !MulticastReceiver_IF ( m_pRealIP, m_nPort, m_pRealIF, &m_nFD ) )
        Trace ( ELT, m_pstProcInfo, "SetRealPort::MulticastReceiver_IF error" );

    if ( m_nFD == ERROR )
    {
        Trace ( ELT, m_pstProcInfo, "SetRealPort::Create Error (%d) for RealPriceThread Mcast Socket : port(%d)", errno, m_nPort );
        ProcExit ( );
    }


    // Client Socket Alloc
    m_pSockCtrl = new CSockCtrl ( m_pstProcInfo, m_nFD, false );
    if ( m_pSockCtrl == NULL )
    {
        Trace ( ELT, m_pstProcInfo, "SetRealPort::CSockCtrl new fail. errno=%d", errno );
        ProcExit ( );
    }

    AddEpollEvent ( pstRealEvents, nRealEpollFD, m_nFD, false );

    Trace ( ILT, m_pstProcInfo, "SetRealPort::RealPriceThread Multicast Joining. IP=%.15s, IF=%s, Port=%d, FD=%d",
                                    m_pRealIP, m_pRealIF, m_nPort, m_nFD );
}

int CRealCtrl::GetRealFD ( )
{
    return m_nFD;
}
