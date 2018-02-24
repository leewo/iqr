#include "SockCtrl.h"

CSockCtrl::CSockCtrl ( )
{
    Init ( NULL, 0, false );
}

CSockCtrl::CSockCtrl ( ST_PROC_INFO * p, int nSocketFD, bool bListMake )
{
    Init ( p, nSocketFD, bListMake );
}

CSockCtrl::~CSockCtrl ( )
{
    Release ( );
}

void CSockCtrl::Init ( ST_PROC_INFO * p, int nSocketFD, bool bListMake )
{
    m_pstProcInfo       = p;
    m_nSocketFD         = nSocketFD;
    m_nUserUniq         = 0;
    m_bIsExit           = false;
    m_nLoginProc        = 0;
    m_nRandomNumer      = 0;
    m_nCountOfEagain    = 0;
    m_pSendBufIqr       = NULL;
    m_pSendData         = NULL;
    m_nSentData         = 0;
    m_nDataLen          = 0;
    m_nContained        = 0;
    m_nPort             = 0;
    m_nDate             = 0;
    m_nTime             = 0;
    m_bListMake         = bListMake;
    m_pstList           = NULL;

    memset ( m_szIP       ,  ' ', sizeof ( m_szIP        ) );
    memset ( m_szPrivateIP,  ' ', sizeof ( m_szPrivateIP ) );
    memset ( m_UserID   , 0x00, sizeof ( m_UserID    ) );
    memset ( m_szData     , 0x00, sizeof ( m_szData      ) );

    if ( bListMake )
    {
        m_pstList = InitList ( 100 );

        char szIP [ SZ_IP ];
        GetPeerAddrPort ( nSocketFD, szIP, &m_nPort );
        memcpy ( m_szIP, szIP, strlen ( szIP ) );
        Trace ( ILT, m_pstProcInfo, "CSockCtrl::Init()::UserIP=%.15s, Port=%d, FD=%d", m_szIP, m_nPort, m_nSocketFD );
    }
    else
    {
        strcpy ( m_szIP, "0.0.0.0" );
    }

    m_pDataQueue        = InitQ ( );
    if ( m_pDataQueue == NULL )
        Trace ( ELT, m_pstProcInfo, "CSockCtrl::Init()::InitQ Fail = IP=%.15s", m_szIP );
}

void CSockCtrl::Release ( )
{
    if ( m_nSocketFD != INVALID_SOCKET )
    {
        // User Socket Free
        if ( m_pstList )
        {
            for ( ST_NODE * pNode = m_pstList->pNodeHead; pNode != NULL; pNode = pNode->pNext )     // List of My used StockList's pointer
            {
                CRealStatus * pRealStatus = ( CRealStatus * ) pNode->pData;
                pRealStatus->Lock ( );
                pRealStatus->Reset ( m_nSocketFD );                             // Remove my Socket at Realtime Linkedlist
                pRealStatus->Unlock ( );

            }
            RemoveAllL ( m_pstList );

            free ( m_pstList );
        }

        //Trace ( ILT, m_pstProcInfo, "CSockCtrl::Release()::IP=%.15s, Port=%d, FD=%d", m_szIP, m_nPort, m_nSocketFD );

        shutdown ( m_nSocketFD, 1 );
        close ( m_nSocketFD );

        m_nSocketFD = INVALID_SOCKET;
    }

    if ( m_pSendBufIqr )
    {
        CSendBufIQR::Release ( m_pSendBufIqr, this );
    }

    if ( m_pDataQueue )
    {
        int           nLen        = 0;
        CSendBufIQR * pSendBufIqr = NULL;

        bool bData = Pop ( m_pDataQueue, &pSendBufIqr, &nLen );
        while ( bData ) 
        {
            CSendBufIQR::Release ( pSendBufIqr, this );
            bData = Pop ( m_pDataQueue, &pSendBufIqr, &nLen );
        }

        RemoveQ ( m_pDataQueue );
        m_pDataQueue = NULL;
    }
}

/*------------------------------------------------------------------
    Purpose   : IQR 사용자 로그인 정보를 ecs로 전송하기 위해 TR을 완성한다
    Parameter : 
    return    : 
  ------------------------------------------------------------------*/
int CSockCtrl::IQRClientLogon ( ST_COMHEAD * pstComHead, const char * szHostName, int nHNSize )
{
	// Make logon data
    Trace ( ILT, m_pstProcInfo, "CSockCtrl::IQRClientLogon()::Send to ECD. nToken=%d. IP=%.15s. PrivateIP=%.15s, nPort=%d, ID=%.20s",
                                m_nRandomNumer, m_szIP, m_szPrivateIP, m_nPort, m_UserID );
    return 0;
}

/*------------------------------------------------------------------
    Purpose   : 로그인 과정 통과한 IQR 사용자 로그아웃 정보를 ecs로 전송하기 위해 TR을 완성한다
    Parameter : 
    return    : 
  ------------------------------------------------------------------*/
int CSockCtrl::IQRClientLogoff ( ST_COMHEAD * pstComHead )
{
	// Make logoff data

    return 0;
}
