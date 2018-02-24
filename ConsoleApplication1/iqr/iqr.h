/*-----------------------------------------------------------------------------
    File Name       : iqr.h
    Author          : Lee Woo-Young
    Compiler        : gcc
    Copyright       : umisoft
    Purpose         : IQR Server
                      iqr 의 클론인 iqs를 분리.
    Creation Date   : 2006.05.01
    Last Modified   : 2013.05.22
-----------------------------------------------------------------------------*/
#pragma once

#include    "main.h"
#include	"../inc/shm/shmmin.h"
#include    "../inc/userinfo.h"
#include    <unistd.h>


/*------------------------------------------------------------------
    Define Macros
  ------------------------------------------------------------------*/

#define THREAD_SIZE                     9

#define MAX_REAL_REQ_CNT                250
#define SZ_STOCK                        6

#define MAX_RETRY_EAGAIN                10

#define DEFAULT_LEVEL                   2

#ifndef MAX_SOCKBUF_SIZE
#define MAX_SOCKBUF_SIZE                SZ_TOT_COMHEAD * 10
#endif

class CSendBufIQR;
class CSockCtrl;
/*------------------------------------------------------------------
    Structures
  ------------------------------------------------------------------*/
struct ST_USER_IQR
{
    pthread_mutex_t     mutexUser;
    bool                bHasUser;
    int                 nUserUniq;
    CSockCtrl         * pSockCtrl;
};

struct ST_AIOSEND_IQR
{
    int             nFD;
    CSockCtrl     * pSockCtrl;
    char          * pSendbuf;
    int             nSendBytes;
    int             nError;
    int             nSended;
};
#define SZ_AIOSEND_IQR sizeof ( ST_AIOSEND_IQR )

// TR:CODE로 Real Data를 요청한 클라이언트의 정보
class CRealStatus
{
public:
    int                 m_nTotalClient              ;   // 해당 TR:CODE 조합으로 요청한 클라이언트의 수
    int                 m_nTrCode                   ;   // TR:CODE에서 TR
    int                 m_nStockCode                ;   // TR:CODE에서 CODE

private:
    pthread_mutex_t     m_mutexLock                 ;
    ST_PROC_INFO      * m_pstProcInfo               ;
    char                m_szClientMap [ EPOLL_SIZE ];   // 해당 TR:CODE 조합으로 요청한 클라이언트별 현황

public:
    CRealStatus ( )
    {
        Init ( );
    }

    CRealStatus ( int nTrCode, int nStockCode, ST_PROC_INFO * p )
    {
        Init ( );
        m_pstProcInfo = p;
        m_nTrCode     = nTrCode;
        m_nStockCode  = nStockCode;
    }

    ~CRealStatus ( )
    {
        pthread_mutex_destroy( &m_mutexLock );
    }

    bool IsSet ( int nClientFD )
    {
        return m_szClientMap [ nClientFD ];
    }

    void Reset ( int nClientFD )
    {
        if ( m_szClientMap [ nClientFD ] == 0 )
            return;

        --m_nTotalClient;
        m_szClientMap [ nClientFD ] = 0;
        //Trace ( ILT, m_pstProcInfo, "CRealStatus::ReSet. m_nTotalClient = %d, nTr = %d, FD = %d", m_nTotalClient, m_nTrCode, nClientFD );
    }

    void Set ( int nClientFD )
    {
        if ( m_szClientMap [ nClientFD ] )
            return;

        ++m_nTotalClient;
        m_szClientMap [ nClientFD ] = 1;
        //Trace ( ILT, m_pstProcInfo, "CRealStatus::Set  . m_nTotalClient = %d, nTr = %d, FD = %d", m_nTotalClient, m_nTrCode, nClientFD );
    }

    void Lock ( )
    {
        pthread_mutex_lock( &m_mutexLock );
    }

    void Unlock ( )
    {
        pthread_mutex_unlock( &m_mutexLock );
    }

private:
    void Init ( )
    {
        pthread_mutex_init( &m_mutexLock, NULL );
        memset ( m_szClientMap, 0x00, sizeof ( m_szClientMap ) );
        m_nStockCode   = 0;
        m_nTrCode      = 0;
        m_nTotalClient = 0;
    }
};

/*------------------------------------------------------------------
    Prototyping
  ------------------------------------------------------------------*/
void             ReadQueInfo          ( );
void             ReadRecvIpPortInfo   ( epoll_event      * pstRealEvents    ,
                                        int                nRealEpollFD     );
void             ReadTRCodesInfo      ( );
void             ReadGuestInfo        ( );
void             Bind                 ( );
int              Run                  ( );
void             AddEpollEvent        ( epoll_event      * pstEvent         ,
                                        const int          nEpollFD         ,
                                        int                nSocketFD        ,
                                        bool               bSetNonblock     );
          
void             RecvClient           ( int                nEventFD         );
void             ProcRandOK           ( ST_COMHEAD       * pstComHead       ,
                                        CSockCtrl        * pSockCtrl        ,
                                        int                nPacketSize      );
void             RecvMC               ( int                nEventFD         ,
                                        ST_HTABLE        * pRealReqHashTable);
void             RealToClient         ( int                nPacketLen       ,
                                        ST_COMHEAD       * pstComHead       ,
                                        ST_HTABLE        * pHashTable       );

CSockCtrl      * AddUserHash          ( int                nClientSocketFD  );
void             RemoveUserHash       ( int                nSocketFD        );
CSockCtrl      * FindUserHash         ( int                nSocketFD        );
CSockCtrl      * LockUserHash         ( int                nSocketFD        );
void             UnLockUserHash       ( int                nSocketFD        );
          
void             ReqRealClient        ( ST_COMHEAD       * pstComHead       ,
                                        CSockCtrl        * pSockCtrl        ,
                                        ST_HTABLE        * pRealReqHashTable,
                                        ST_HTABLE        * pRealCanHashTable);
          
void             AddUsingList         ( void             * pData            ,
                                        ST_LISTDATA      * pstSockList      );
void             RemUsingList         ( void             * pData            ,
                                        ST_LISTDATA      * pstSockList      );
                                                      
void           * ECSReadThread        ( void             * pIn              );
void           * ECSSendThread        ( void             * pIn              );
void           * SiqOutgoingThread    ( void             * pIn              );
void           * SiqIncomingThread    ( void             * pIn              );
void           * RealPriceThread      ( void             * pIn              );
void           * AIOThread            ( void             * pIn              );
void           * SendThread           ( void             * pIn              );
void           * PollingThread        ( void             * pIn              );
void             SendPollingData      ( CSockCtrl        * pSockCtrl        );
void           * SendHBCMS            ( void             * pIn              );
void             CreateThreads        ( int                nCreateCnt       );
void             CreateECSThread      ( );
void             CreateSIQThread      ( );
void             CreateRealThread     ( );
void             CreateAIOThread      ( );
void             CreateSendThread     ( );
void             CreatePollThread     ( );
                                   
void             SigProc              ( int                nSigno           );
void             ProcInit             ( );
void             ProcExit             ( );
                                                 
void             CloseClient          ( CSockCtrl       *& pSockCtrl        );
void             SendRandToClient     ( CSockCtrl        * pSockCtrl        );
bool             VerifyPath           ( CSockCtrl        * pSockCtrl        ,
                                        ST_COMHEAD       * pstComHead       );
bool             IsAllowID            ( CSockCtrl        * pSockCtrl        ,
                                        int              * pnLevel          );
void             SendToClient         ( int                nTR              ,
                                        ST_COMHEAD       * pstComHead       ,
                                        CSockCtrl        * pSockCtrl        );
void             SendToCloseMsg       ( int                nTR              ,
                                        int                nError           ,
                                        ST_COMHEAD       * pstComHead       ,
                                        CSockCtrl        * pSockCtrl        );
          
void             GetCodes             ( char             * pszIniFile       ,
                                        const char       * pszSection       ,
                                        int              * pnTotRealReqCnt  ,
                                        int              * pnTotRealCanCnt  ,
                                        int              * pnCodeDiffSize   ,
                                        ST_HTABLE       ** ppRealReqHashTable,
                                        ST_HTABLE       ** ppRealCanHashTable );

bool             GetRandData          ( int              * pRand            );
          
/*- EOF ---------------------------------------------------------------------*/
