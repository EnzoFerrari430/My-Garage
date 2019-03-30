#include <stdio.h>
#include <windows.h>
#include <process.h>

/*
10个读者2个写者 写一本书
书是临界资源，不能一起看，也不能一起写
一个事件应该就够了,还要定义关键段，防止同类的线程冲突
所以要1个事件给书本 2个关键段给读者线程和写者线程
*/
const int READER_NUM = 10;
const int WRITER_NUM = 2;
CRITICAL_SECTION g_csReader,g_csWriter;
HANDLE g_hBookEvent;
unsigned int _stdcall ReadThread(PVOID pM)
{
    //Sleep(rand()%100);
    printf("编号是%d的读者正在等待书本资源的释放...\n",GetCurrentThreadId());
    WaitForSingleObject(g_hBookEvent,INFINITE);

    //进入关键段
    EnterCriticalSection(&g_csReader);
    printf("  编号为%d的读者正在阅读本书...\n",GetCurrentThreadId());
    Sleep(rand()%100);
    printf("    编号为%d的读者阅读结束...\n",GetCurrentThreadId());
    LeaveCriticalSection(&g_csReader);

    //触发事件
    SetEvent(g_hBookEvent);

    return 0;

}

unsigned int _stdcall WriteThread(PVOID pM)
{
    printf("编号是%d的写者正在等待书本资源的释放...\n",GetCurrentThreadId());
    WaitForSingleObject(g_hBookEvent,INFINITE);

    //进入关键段
    EnterCriticalSection(&g_csWriter);
    printf("  编号为%d的写者正在编写本书...\n",GetCurrentThreadId());
    Sleep(rand()%100);
    printf("    编号为%d的写者本次编写结束...\n",GetCurrentThreadId());
    LeaveCriticalSection(&g_csWriter);

    //触发事件
    SetEvent(g_hBookEvent);

    return 0;
}

int main()
{

    system("cls");

    //初始化关键段和事件
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

    //等待子线程全部运行结束
    WaitForMultipleObjects(READER_NUM,hReaderThread,TRUE,INFINITE);
    WaitForMultipleObjects(WRITER_NUM,hWriterThread,TRUE,INFINITE);

    //销毁关键段和事件
    CloseHandle(g_hBookEvent);
    DeleteCriticalSection(&g_csReader);
    DeleteCriticalSection(&g_csWriter);


    return 0;
}
