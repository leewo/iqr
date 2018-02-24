#pragma once
#include    "iqr.h"
#include "SendBufIQR.h"

class CSockCtrl
{
private:
    ST_PROC_INFO      * m_pstProcInfo                   ;

public:
    int                 m_nSocketFD                     ;
    int                 m_nUserUniq                     ;
    bool                m_bIsExit                       ;
    int                 m_nLoginProc                    ;
    int                 m_nRandomNumer                  ;
    struct timeval      m_tvLast                        ;

    uint                m_nCountOfEagain                ;

    CSendBufIQR       * m_pSendBufIqr                   ; // IQR에서 Buf 관리하는 클래스 포인터
    char              * m_pSendData                     ; // 전송할 데이터 포인터
    int                 m_nSentData                     ;
    int                 m_nDataLen                      ; // sizeof Send Data

    int                 m_nContained                    ;
    char                m_szIP                 [ SZ_IP ];
    char                m_szPrivateIP          [ SZ_IP ];
    int                 m_nPort                         ;
    char                m_UserID        [ USR_ID_LEN ];
    int                 m_nDate                         ; // Logon Date
    int                 m_nTime                         ; // Logon Time

    bool                m_bListMake                     ;
    ST_LISTDATA       * m_pstList                       ; // RealTime List's Pointer List

    PQUEUE              m_pDataQueue                    ; //

    char                m_szData    [ MAX_SOCKBUF_SIZE ];

public:
    CSockCtrl ( );
    CSockCtrl ( ST_PROC_INFO * p, int nSocketFD, bool bListMake );
    ~CSockCtrl ( );

    int                     IQRClientLogon  ( ST_COMHEAD * pstComHead, const char * szHostName, int nHNSize );
    int                     IQRClientLogoff ( ST_COMHEAD * pstComHead );

    // Send to client using AIO
    static inline void AIOSendToClient ( CSockCtrl * pSockCtrl, PQUEUE pQSend, sem_t * psemSend )
    {
        ST_AIOSEND_IQR stAioSend;
        stAioSend.nFD        = pSockCtrl->m_nSocketFD;
        stAioSend.pSockCtrl  = pSockCtrl;
        stAioSend.pSendbuf   = pSockCtrl->m_pSendData + pSockCtrl->m_nSentData;
        stAioSend.nSendBytes = pSockCtrl->m_nDataLen;
        stAioSend.nError     = 0;
        stAioSend.nSended    = 0;

        if ( !Push ( pQSend, &stAioSend, SZ_AIOSEND_IQR ) )
            Trace ( ELT, pSockCtrl->m_pstProcInfo, "CSockCtrl::AIOSendToClient::Push error" );
        sem_post ( psemSend );    
    }

    // Call AsyncI/O
    static inline void CallAIO ( CSockCtrl * pSockCtrl, CSendBufIQR * pSendBufIqr, PQUEUE pQSend, sem_t * psemSend )
    {
        // Check Sending Proc
        if ( ( IsEmptyQ ( pSockCtrl->m_pDataQueue ) ) && ( pSockCtrl->m_pSendBufIqr == NULL ) )
        {
            CSendBufIQR::GetBuf ( pSendBufIqr, pSockCtrl );
            AIOSendToClient ( pSockCtrl, pQSend, psemSend );
        }
        else
        {
            Push ( pSockCtrl->m_pDataQueue, &pSendBufIqr, sizeof ( CSendBufIQR * ) );
        }
    }

private:
    void                    Init            ( ST_PROC_INFO * p, int nSocketFD, bool bListMake );
    void                    Release         ( );

};
