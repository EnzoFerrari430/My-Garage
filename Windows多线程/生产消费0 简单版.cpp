//这里用1个生产者 1个消费者 1个缓冲区

#include <stdio.h>
#include <process.h>
#include <windows.h>

//生产个数
const int PRO_NUM = 10;
int g_Buffer;  //缓冲区

//事件与关键段
CRITICAL_SECTION g_cs;
HANDLE g_EventBufferEmpty,g_EventBufferFull;




//生产线程
unsigned int _stdcall ProThreadFun(PVOID pM)
{
    int i ;
    for(i = 1; i<PRO_NUM; i++)
    {
        //等待缓冲区为空
        WaitForSingleObject(g_EventBufferEmpty,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        g_Buffer = i;
        printf("生产者将数据%d放入缓冲区\n",i);
        LeaveCriticalSection(&g_cs);

        //别忘了触发缓冲区满事件!!!!!!
        SetEvent(g_EventBufferFull);
    }

}

//消费线程
unsigned int _stdcall ComThreadFun(PVOID pM)
{
    volatile bool flag = TRUE;

    while(flag)
    {

        WaitForSingleObject(g_EventBufferFull,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        printf("  消费者从缓冲区取出数据%d\n",g_Buffer);
        if(g_Buffer == PRO_NUM)
        {
            flag = FALSE;
        }

        LeaveCriticalSection(&g_cs);

        SetEvent(g_EventBufferEmpty);  //触发缓冲区空事件

        Sleep(10);  //等待一会儿
    }
    return 0;
}
int main()
{
    printf("简单的生产者消费者 1个生产者 1个消费者 1个缓冲区\n");

    InitializeCriticalSection(&g_cs);
    //创建二个自动复位事件，一个表示缓冲区是否为空，另一个表示缓冲区是否已经处理
    g_EventBufferEmpty = CreateEvent(NULL, FALSE, TRUE, NULL);
    g_EventBufferFull = CreateEvent(NULL, FALSE, FALSE, NULL);

    const int THREADNUM = 2;
    HANDLE hThread[THREADNUM];
    hThread[0] = (HANDLE)_beginthreadex(NULL,0,ProThreadFun,NULL,0,NULL);
    hThread[1] = (HANDLE)_beginthreadex(NULL,0,ComThreadFun,NULL,0,NULL);

    WaitForMultipleObjects(THREADNUM,hThread,TRUE,INFINITE);

    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    //销毁事件和关键段
    CloseHandle(g_EventBufferEmpty);
    CloseHandle(g_EventBufferFull);
    DeleteCriticalSection(&g_cs);

    return 0;

}




