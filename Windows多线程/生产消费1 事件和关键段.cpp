/*
利用2个事件和一个关键段来实现生产者消费者问题
2个事件分别表示缓冲区满和缓冲区空


*/

#include <stdio.h>
#include <process.h>
#include <windows.h>

const int PRODUCE_NUMBER = 8;  //生产者产品个数
const int BUFFER_SIZE = 4;  //缓冲区个数

int g_Buffer[BUFFER_SIZE];  //各个缓冲池
int g_i,g_j;

//信号量和关键段
CRITICAL_SECTION g_cs;
HANDLE g_hSemaphoreBufferEmpty,g_hSemaphoreBufferFull;



//生产者线程
unsigned int _stdcall ProducerThreadFun(PVOID pM)
{
    int i;
    for(i = 1;i<= PRODUCE_NUMBER;i++)
    {
        //等待有空的缓冲池出现
        WaitForSingleObject(g_hSemaphoreBufferEmpty,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        g_Buffer[g_i] = i ;
        printf("生产者在缓冲池第%d个缓冲区中投放数据%d\n", g_i, g_Buffer[g_i]);
        g_i = (g_i + 1)%BUFFER_SIZE;  //下一个生产出来的产品在缓冲区中的位置

        //离开关键段
        LeaveCriticalSection(&g_cs);

        //通知消费者有新数据了
        /*
        递增计数
        BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount );
        第一个参数是信号量的句柄。
        第二个参数表示增加个数，必须大于0且不超过最大资源数量。
        第三个参数可以用来传出先前的资源计数，设为NULL表示不需要传出。
        */
        ReleaseSemaphore(g_hSemaphoreBufferFull,1,NULL);

    }

    printf("生产者完成任务，线程结束运行\n");
    return 0;
}

//消费者线程
unsigned int _stdcall ConsumerThreadFun(PVOID pM)
{
    //int over = 0;
    while(1)
    {

        //等待非空缓冲区的出现
        WaitForSingleObject(g_hSemaphoreBufferFull,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        printf("  编号为:%d的消费者从缓冲池第%d个缓冲区取出数据%d\n",GetCurrentThreadId(),g_j,g_Buffer[g_j]);

        //当前消费的是最后一个产品
        if(g_Buffer[g_j] == PRODUCE_NUMBER)
        {
            //离开关键段
            LeaveCriticalSection(&g_cs);

            //通知其他消费者有新数据了
            ReleaseSemaphore(g_hSemaphoreBufferEmpty,1,NULL);

            break;
        }
        g_j = (g_j + 1)%BUFFER_SIZE;  //下一个消费者要消费的产品在缓冲区中的位置
        //离开关键段
        LeaveCriticalSection(&g_cs);
        Sleep(50);  //some other work to do

        ReleaseSemaphore(g_hSemaphoreBufferEmpty,1,NULL);
    }
    //全部结束
    printf("  编号为:%d的消费者收到通知，线程结束运行\n",GetCurrentThreadId());

    return 0;

}

int main()
{
    printf("生产者消费者问题   1生产者  2消费者  4个缓冲区\n");

    //初始化关键段 信号量一个记录有产品的缓冲区个数，一个记录空缓冲区个数
    InitializeCriticalSection(&g_cs);

    g_hSemaphoreBufferEmpty = CreateSemaphore(NULL,4,4,NULL);
    g_hSemaphoreBufferFull = CreateSemaphore(NULL,0,4,NULL);

    g_i = 0;
    g_j = 0;
    /*
    void *memset(void *s, int ch, size_t n);
　　函数解释：将s中前n个字节替换为ch并返回s；
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

    //销毁信号量和关键段
    CloseHandle(g_hSemaphoreBufferEmpty);
    CloseHandle(g_hSemaphoreBufferFull);
    DeleteCriticalSection(&g_cs);
    return 0;
}
