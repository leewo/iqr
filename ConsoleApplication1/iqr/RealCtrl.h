#pragma once
#include    "iqr.h"

class CRealCtrl
{
public:
    CSockCtrl         * m_pSockCtrl     ;

private:
    ST_PROC_INFO      * m_pstProcInfo   ;
    const char        * m_pRealIP       ;
    const char        * m_pRealIF       ;
    const int           m_nPort         ;   // Multicast Port
    int                 m_nFD           ;   // Multicast 연결된 FD
    struct sockaddr_in  m_stIn          ;   // Multicast 연결된 struct sockaddr_in
    int                 m_nRecvCnt      ;   // receive count
                      
public:
    CRealCtrl ( );
    CRealCtrl ( ST_PROC_INFO * p, const char * pRealIP, const char * pRealIF, const int nPort, epoll_event * pstRealEvents, const int nRealEpollFD );
    ~CRealCtrl ( );

    int                     GetRealFD   ( );

private:
    void                    Init        ( ST_PROC_INFO * p );
    void                    SetRealPort ( epoll_event * pstRealEvents, const int nRealEpollFD );

};
