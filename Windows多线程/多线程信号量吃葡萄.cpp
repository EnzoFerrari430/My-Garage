#include <iostream>
#include <stdio.h>
#include <process.h>
#include <windows.h>

using namespace std;

/*
    一个盘子，相当于缓冲区，最多可以放3个葡萄

    需要2个线程，一个生产者线程，一个消费者线程
*/
unsigned int _stdcall CumFun(void *pPM);
unsigned int _stdcall ProFun(void *pPM);

//信号量与关键段
CRITICAL_SECTION g_cs;
HANDLE g_hBufferEmpty,g_hBufferFull;

const int BUFFER_SIZE = 8;
int g_Buffer[BUFFER_SIZE];  //各个缓冲池
int g_i,g_j;
unsigned int _stdcall CumFun(void *pPM)
{
    int sum = 0;
    int count = 0;
    while(1)
    {
        //等待盘子里有葡萄
        WaitForSingleObject(g_hBufferFull,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        printf("消费者从第%d缓冲区中取出数据%d...\n",g_j,g_Buffer[g_j]);
        count ++;
        sum = sum + g_Buffer[g_j];
        g_j = (g_j + 1)%BUFFER_SIZE;
        if(count == 3)
        {
            float f = (float)sum/3;
            printf("  3个的平均值为%.2f\n",f);
            count = 0;
            sum = 0;
        }
        //离开关键段
        LeaveCriticalSection(&g_cs);
        Sleep(rand()%1000);  //等待
        ReleaseSemaphore(g_hBufferEmpty,1,NULL);  //空位+1
    }


    return 0;
}

unsigned int _stdcall ProFun(void *pPM)
{
    while(1)
    {
        //等待盘子里有空的位置
        WaitForSingleObject(g_hBufferEmpty,INFINITE);

        //进入关键段
        EnterCriticalSection(&g_cs);
        g_Buffer[g_i] = rand()%100;
        printf("      生产者在缓冲池第%d个缓冲区中放入数据%d。。。\n",g_i,g_Buffer[g_i]);
        g_i = (g_i + 1)%BUFFER_SIZE;
        //离开关键段
        LeaveCriticalSection(&g_cs);
        Sleep(rand()%1000);  //等待

        ReleaseSemaphore(g_hBufferFull,1,NULL);  //有数据的缓冲区+1
    }
    return 0;


}
int main()
{
    //初始化信号量和关键段
    InitializeCriticalSection(&g_cs);
    g_hBufferEmpty = CreateSemaphore(NULL,8,8,NULL);  //信号量初始为8，最大为8
    g_hBufferFull = CreateSemaphore(NULL,0,8,NULL);   //信号量初始为8，最大为8

    g_i = 0;
    g_j = 0;

    HANDLE hThread[2];
    hThread[0] = (HANDLE)_beginthreadex(NULL,0,ProFun,NULL,0,NULL);

    Sleep(3000);
    hThread[1] = (HANDLE)_beginthreadex(NULL,0,CumFun,NULL,0,NULL);

    WaitForMultipleObjects(2,hThread,TRUE,INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    //销毁关键段和事件
    CloseHandle(g_hBufferEmpty);
    CloseHandle(g_hBufferFull);
    DeleteCriticalSection(&g_cs);

    return 0;
}
