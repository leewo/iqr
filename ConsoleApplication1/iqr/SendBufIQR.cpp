#include "SendBufIQR.h"
#include "SockCtrl.h"

CSendBufIQR::CSendBufIQR ( )
{
    Init ( NULL );
}

CSendBufIQR::CSendBufIQR ( ST_PROC_INFO * p, int nPacketLen, ST_COMHEAD * pstComHead, int nRef )
{
    Init ( p );

    m_nRef       = nRef;
    m_nPacketLen = nPacketLen;

    if ( m_nPacketLen > SZ_TOT_COMHEAD )
    {
        Trace ( ELT, m_pstProcInfo, "CSendBufIQR()::nSize(%d) > SZ_TOT_COMHEAD(%d)", m_nPacketLen, SZ_TOT_COMHEAD );
        return;
    }

    m_pstSend = new char [ m_nPacketLen ];
    memcpy ( m_pstSend, pstComHead, m_nPacketLen );
    //fprintf ( stdout, "Create  ( )::this = %d, m_nRef = %d, nTr = %d, m_pstSend = %d, m_nPacketLen = %d\n", this, m_nRef, pstComHead->nTrCode, m_pstSend, m_nPacketLen );
}

CSendBufIQR::~CSendBufIQR ( )
{
    if ( m_pstSend )
    {
        delete [] m_pstSend;
        m_pstSend = NULL;
    }
}

// 참조카운트 해제
void CSendBufIQR::Release ( CSendBufIQR * pObj, CSockCtrl * pSockCtrl )
{
    pthread_mutex_lock( &pObj->m_mutexLock );
    --pObj->m_nRef;
    if ( pSockCtrl )
    {
        pSockCtrl->m_pSendBufIqr = NULL;
        pSockCtrl->m_pSendData   = NULL;
        pSockCtrl->m_nSentData   = 0;
        pSockCtrl->m_nDataLen    = 0;
    }

    ST_COMHEAD * pTmp = ( ST_COMHEAD * ) pObj->m_pstSend;
    //fprintf ( stdout, "Release ( )::this = %d, m_nRef = %d, nTr = %d, nFD = %d\n", pObj, pObj->m_nRef, pTmp->nTrCode, pSockCtrl->m_nSocketFD );

    if ( pObj->m_nRef <= 0 )  // 더이상 참조하는 클라이언트가 없다면 해제
    {
        pthread_mutex_unlock( &pObj->m_mutexLock );
        pthread_mutex_destroy( &pObj->m_mutexLock );
        delete pObj;
    }
    else
    {
        pthread_mutex_unlock( &pObj->m_mutexLock );
    }
}

void CSendBufIQR::GetBuf  ( CSendBufIQR * pObj, CSockCtrl * pSockCtrl )
{
    pSockCtrl->m_pSendBufIqr = pObj;
    pSockCtrl->m_pSendData   = pObj->m_pstSend;
    pSockCtrl->m_nSentData   = 0;
    pSockCtrl->m_nDataLen    = pObj->m_nPacketLen;
}

void CSendBufIQR::Init ( ST_PROC_INFO * pstProcInfo )
{
    pthread_mutex_init( &m_mutexLock, NULL );
    m_pstProcInfo = pstProcInfo;
    m_pstSend     = NULL;
    m_nPacketLen  = 0;
    m_nRef        = 0;
}
