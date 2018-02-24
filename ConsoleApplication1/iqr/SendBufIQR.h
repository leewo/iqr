#pragma once
#include    "iqr.h"

class CSockCtrl;
class CSendBufIQR
{
private:
    ST_PROC_INFO      * m_pstProcInfo   ;
    pthread_mutex_t     m_mutexLock     ;
                      
    char              * m_pstSend       ;   // ������ ������
    int                 m_nPacketLen    ;   // ������ ũ��
    int                 m_nRef          ;   // ���� ȸ��

public:
    CSendBufIQR ( );
    CSendBufIQR ( ST_PROC_INFO * p, int nPacketLen, ST_COMHEAD * pstComHead, int nRef = 1 );
    ~CSendBufIQR ( );

    static void             Release ( CSendBufIQR * pObj, CSockCtrl * pSockCtrl );
    static void             GetBuf  ( CSendBufIQR * pObj, CSockCtrl * pSockCtrl );

private:
    void                    Init    ( ST_PROC_INFO * pstProcInfo );

};
