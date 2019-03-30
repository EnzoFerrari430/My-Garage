/*
信号量常用的3个函数
1.创建信号量
HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName);
第一个参数表示安全控制，一般直接传入NULL。
第二个参数表示初始资源数量。
第三个参数表示最大并发数量。
第四个参数表示信号量的名称，传入NULL表示匿名信号量。

2.打开信号量
HANDLE OpenSemaphore(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);
第一个参数表示访问权限，对一般传入SEMAPHORE_ALL_ACCESS。详细解释可以查看MSDN文档。
第二个参数表示信号量句柄继承性，一般传入TRUE即可。
第三个参数表示名称，不同进程中的各线程可以通过名称来确保它们访问同一个信号量。

3.递增计数
BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount );
第一个参数是信号量的句柄。
第二个参数表示增加个数，必须大于0且不超过最大资源数量。
第三个参数可以用来传出先前的资源计数，设为NULL表示不需要传出。

销毁 CloseHandle()

递减计数
等待的系列函数如 WaitForSingleObject()

*/
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <iostream>
using namespace std;

long g_nNum;
unsigned int _stdcall Fun(void *pPM);
const int THREAD_NUM = 10;

//信号量与关键段
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadParameter;

int main()
{

    printf("信号量线程同步 \n\n");


    //初始化信号量和关键段
    g_hThreadParameter = CreateSemaphore(NULL,0,1,NULL);  //初始0,最大1个信号量
    InitializeCriticalSection(&g_csThreadCode);

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i<THREAD_NUM)
    {
        //WaitForSingleObject(g_hThreadParameter,INFINITE);  放上面一直等待
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        WaitForSingleObject(g_hThreadParameter,INFINITE);
        ++i;
    }

    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //销毁信号量和关键段0
    DeleteCriticalSection(&g_csThreadCode);
    CloseHandle(g_hThreadParameter);
    /*
    for(i = 0;i<THREAD_NUM;i++)
    {
        CloseHandle(handle[i]);
    }
    */
    return 0;
}

unsigned int  _stdcall Fun(void *pPM)
{
    int nThreadNum = *(int *)pPM;
    ReleaseSemaphore(g_hThreadParameter,1,NULL);  //信号量++

    Sleep(50);  //some work should to do

    EnterCriticalSection(&g_csThreadCode);
    ++g_nNum;
    Sleep(0);
    //printf("线程编号为:%d  全局资源为:%d\n",nThreadNum,g_nNum);
    cout<<"线程编号为:"<<nThreadNum<<" 全局资源为:"<<g_nNum<<endl;  //endl刷新了缓冲区和上面的printf效果相同
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}


