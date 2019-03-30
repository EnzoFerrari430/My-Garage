/*
����2���¼���һ���ؼ�����ʵ������������������
2���¼��ֱ��ʾ���������ͻ�������


*/

#include <stdio.h>
#include <process.h>
#include <windows.h>

const int PRODUCE_NUMBER = 8;  //�����߲�Ʒ����
const int BUFFER_SIZE = 4;  //����������

int g_Buffer[BUFFER_SIZE];  //���������
int g_i,g_j;

//�ź����͹ؼ���
CRITICAL_SECTION g_cs;
HANDLE g_hSemaphoreBufferEmpty,g_hSemaphoreBufferFull;



//�������߳�
unsigned int _stdcall ProducerThreadFun(PVOID pM)
{
    int i;
    for(i = 1;i<= PRODUCE_NUMBER;i++)
    {
        //�ȴ��пյĻ���س���
        WaitForSingleObject(g_hSemaphoreBufferEmpty,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        g_Buffer[g_i] = i ;
        printf("�������ڻ���ص�%d����������Ͷ������%d\n", g_i, g_Buffer[g_i]);
        g_i = (g_i + 1)%BUFFER_SIZE;  //��һ�����������Ĳ�Ʒ�ڻ������е�λ��

        //�뿪�ؼ���
        LeaveCriticalSection(&g_cs);

        //֪ͨ����������������
        /*
        ��������
        BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount );
        ��һ���������ź����ľ����
        �ڶ���������ʾ���Ӹ������������0�Ҳ����������Դ������
        ������������������������ǰ����Դ��������ΪNULL��ʾ����Ҫ������
        */
        ReleaseSemaphore(g_hSemaphoreBufferFull,1,NULL);

    }

    printf("��������������߳̽�������\n");
    return 0;
}

//�������߳�
unsigned int _stdcall ConsumerThreadFun(PVOID pM)
{
    //int over = 0;
    while(1)
    {

        //�ȴ��ǿջ������ĳ���
        WaitForSingleObject(g_hSemaphoreBufferFull,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        printf("  ���Ϊ:%d�������ߴӻ���ص�%d��������ȡ������%d\n",GetCurrentThreadId(),g_j,g_Buffer[g_j]);

        //��ǰ���ѵ������һ����Ʒ
        if(g_Buffer[g_j] == PRODUCE_NUMBER)
        {
            //�뿪�ؼ���
            LeaveCriticalSection(&g_cs);

            //֪ͨ��������������������
            ReleaseSemaphore(g_hSemaphoreBufferEmpty,1,NULL);

            break;
        }
        g_j = (g_j + 1)%BUFFER_SIZE;  //��һ��������Ҫ���ѵĲ�Ʒ�ڻ������е�λ��
        //�뿪�ؼ���
        LeaveCriticalSection(&g_cs);
        Sleep(50);  //some other work to do

        ReleaseSemaphore(g_hSemaphoreBufferEmpty,1,NULL);
    }
    //ȫ������
    printf("  ���Ϊ:%d���������յ�֪ͨ���߳̽�������\n",GetCurrentThreadId());

    return 0;

}

int main()
{
    printf("����������������   1������  2������  4��������\n");

    //��ʼ���ؼ��� �ź���һ����¼�в�Ʒ�Ļ�����������һ����¼�ջ���������
    InitializeCriticalSection(&g_cs);

    g_hSemaphoreBufferEmpty = CreateSemaphore(NULL,4,4,NULL);
    g_hSemaphoreBufferFull = CreateSemaphore(NULL,0,4,NULL);

    g_i = 0;
    g_j = 0;
    /*
    void *memset(void *s, int ch, size_t n);
�����������ͣ���s��ǰn���ֽ��滻Ϊch������s��
    */
    memset(g_Buffer,0,sizeof(g_Buffer));
    const int THREADNUM = 2;
    HANDLE hThread[THREADNUM];

    hThread[0] = (HANDLE)_beginthreadex(NULL,0,ProducerThreadFun,NULL,0,NULL);

    hThread[1] = (HANDLE)_beginthreadex(NULL,0,ConsumerThreadFun,NULL,0,NULL);
    hThread[2] = (HANDLE)_beginthreadex(NULL,0,ConsumerThreadFun,NULL,0,NULL);

    WaitForMultipleObjects(THREADNUM,hThread,TRUE,INFINITE);
    int i;
    for(i = 0;i<THREADNUM;i++)
    {
        CloseHandle(hThread[i]);
    }

    //�����ź����͹ؼ���
    CloseHandle(g_hSemaphoreBufferEmpty);
    CloseHandle(g_hSemaphoreBufferFull);
    DeleteCriticalSection(&g_cs);
    return 0;
}
