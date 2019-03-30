#include <stdio.h>
#include <windows.h>
#include <process.h>

/*
10������2��д�� дһ����
�����ٽ���Դ������һ�𿴣�Ҳ����һ��д
һ���¼�Ӧ�þ͹���,��Ҫ����ؼ��Σ���ֹͬ����̳߳�ͻ
����Ҫ1���¼����鱾 2���ؼ��θ������̺߳�д���߳�
*/
const int READER_NUM = 10;
const int WRITER_NUM = 2;
CRITICAL_SECTION g_csReader,g_csWriter;
HANDLE g_hBookEvent;
unsigned int _stdcall ReadThread(PVOID pM)
{
    //Sleep(rand()%100);
    printf("�����%d�Ķ������ڵȴ��鱾��Դ���ͷ�...\n",GetCurrentThreadId());
    WaitForSingleObject(g_hBookEvent,INFINITE);

    //����ؼ���
    EnterCriticalSection(&g_csReader);
    printf("  ���Ϊ%d�Ķ��������Ķ�����...\n",GetCurrentThreadId());
    Sleep(rand()%100);
    printf("    ���Ϊ%d�Ķ����Ķ�����...\n",GetCurrentThreadId());
    LeaveCriticalSection(&g_csReader);

    //�����¼�
    SetEvent(g_hBookEvent);

    return 0;

}

unsigned int _stdcall WriteThread(PVOID pM)
{
    printf("�����%d��д�����ڵȴ��鱾��Դ���ͷ�...\n",GetCurrentThreadId());
    WaitForSingleObject(g_hBookEvent,INFINITE);

    //����ؼ���
    EnterCriticalSection(&g_csWriter);
    printf("  ���Ϊ%d��д�����ڱ�д����...\n",GetCurrentThreadId());
    Sleep(rand()%100);
    printf("    ���Ϊ%d��д�߱��α�д����...\n",GetCurrentThreadId());
    LeaveCriticalSection(&g_csWriter);

    //�����¼�
    SetEvent(g_hBookEvent);

    return 0;
}

int main()
{

    system("cls");

    //��ʼ���ؼ��κ��¼�
    InitializeCriticalSection(&g_csReader);
    InitializeCriticalSection(&g_csWriter);

    g_hBookEvent = CreateEvent(NULL,FALSE,TRUE,NULL);


    int i,j;
    HANDLE hReaderThread[READER_NUM];
    HANDLE hWriterThread[WRITER_NUM];
    for(i=0;i<READER_NUM;i++)
    {
        hReaderThread[i] = (HANDLE)_beginthreadex(NULL,0,ReadThread,NULL,0,NULL);
    }
    for(j=0;j<WRITER_NUM;j++)
    {
        hWriterThread[j] = (HANDLE)_beginthreadex(NULL,0,WriteThread,NULL,0,NULL);
    }

    //�ȴ����߳�ȫ�����н���
    WaitForMultipleObjects(READER_NUM,hReaderThread,TRUE,INFINITE);
    WaitForMultipleObjects(WRITER_NUM,hWriterThread,TRUE,INFINITE);

    //���ٹؼ��κ��¼�
    CloseHandle(g_hBookEvent);
    DeleteCriticalSection(&g_csReader);
    DeleteCriticalSection(&g_csWriter);


    return 0;
}
