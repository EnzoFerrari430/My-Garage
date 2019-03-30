//������1�������� 1�������� 1��������

#include <stdio.h>
#include <process.h>
#include <windows.h>

//��������
const int PRO_NUM = 10;
int g_Buffer;  //������

//�¼���ؼ���
CRITICAL_SECTION g_cs;
HANDLE g_EventBufferEmpty,g_EventBufferFull;




//�����߳�
unsigned int _stdcall ProThreadFun(PVOID pM)
{
    int i ;
    for(i = 1; i<PRO_NUM; i++)
    {
        //�ȴ�������Ϊ��
        WaitForSingleObject(g_EventBufferEmpty,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        g_Buffer = i;
        printf("�����߽�����%d���뻺����\n",i);
        LeaveCriticalSection(&g_cs);

        //�����˴������������¼�!!!!!!
        SetEvent(g_EventBufferFull);
    }

}

//�����߳�
unsigned int _stdcall ComThreadFun(PVOID pM)
{
    volatile bool flag = TRUE;

    while(flag)
    {

        WaitForSingleObject(g_EventBufferFull,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        printf("  �����ߴӻ�����ȡ������%d\n",g_Buffer);
        if(g_Buffer == PRO_NUM)
        {
            flag = FALSE;
        }

        LeaveCriticalSection(&g_cs);

        SetEvent(g_EventBufferEmpty);  //�������������¼�

        Sleep(10);  //�ȴ�һ���
    }
    return 0;
}
int main()
{
    printf("�򵥵������������� 1�������� 1�������� 1��������\n");

    InitializeCriticalSection(&g_cs);
    //���������Զ���λ�¼���һ����ʾ�������Ƿ�Ϊ�գ���һ����ʾ�������Ƿ��Ѿ�����
    g_EventBufferEmpty = CreateEvent(NULL, FALSE, TRUE, NULL);
    g_EventBufferFull = CreateEvent(NULL, FALSE, FALSE, NULL);

    const int THREADNUM = 2;
    HANDLE hThread[THREADNUM];
    hThread[0] = (HANDLE)_beginthreadex(NULL,0,ProThreadFun,NULL,0,NULL);
    hThread[1] = (HANDLE)_beginthreadex(NULL,0,ComThreadFun,NULL,0,NULL);

    WaitForMultipleObjects(THREADNUM,hThread,TRUE,INFINITE);

    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    //�����¼��͹ؼ���
    CloseHandle(g_EventBufferEmpty);
    CloseHandle(g_EventBufferFull);
    DeleteCriticalSection(&g_cs);

    return 0;

}




