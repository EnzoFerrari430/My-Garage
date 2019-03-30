/*
互斥量也是一个内核对象，它用来确保一个线程独占一个资源的访问。
互斥量与关键段的行为非常相似，并且互斥量可以用于不同进程中的线程互斥访问资源。

互斥量的4个函数
1.创建互斥量
HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCTSTR lpName);

第一个参数表示安全控制，一般直接传入NULL。
第二个参数用来确定互斥量的初始拥有者。
如果传入TRUE表示互斥量对象内部会记录创建它的线程的线程ID号并将递归计数设置为1，由于该线程ID非零，所以互斥量处于未触发状态。
如果传入FALSE，那么互斥量对象内部的线程ID号将设置为NULL，递归计数设置为0，这意味互斥量不为任何线程占用，处于触发状态。

第三个参数用来设置互斥量的名称，在多个进程中的线程就是通过名称来确保它们访问的是同一个互斥量。

函数访问值：
成功返回一个表示互斥量的句柄，失败返回NULL。

2.打开互斥量
HANDLE OpenMutex(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);

第一个参数表示访问权限，对互斥量一般传入MUTEX_ALL_ACCESS。详细解释可以查看MSDN文档。
第二个参数表示互斥量句柄继承性，一般传入TRUE即可。
第三个参数表示名称。某一个进程中的线程创建互斥量后，其它进程中的线程就可以通过这个函数来找到这个互斥量。
函数访问值：
成功返回一个表示互斥量的句柄，失败返回NULL。

3.离开互斥量
BOOL ReleaseMutex (HANDLE hMutex)
访问互斥资源前应该要调用等待函数，结束访问时就要调用ReleaseMutex()来表示自己已经结束访问，其它线程可以开始访问了。

4.清理互斥量
由于互斥量是内核对象，因此使用CloseHandle()就可以（这一点所有内核对象都一样）。


互斥量也有线程所有权的概念，主线程拥有线程所有权，所以做不到线程的同步
*/

#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //全局资源
const int THREAD_NUM = 10; //子线程个数
unsigned int _stdcall Fun(void *pPM);  //线程函数


//定义关键段和互斥量
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadParameter;

int main()
{
    //初始化互斥量和关键段 第二个参数为TRUE表示互斥量为创建线程所有
    InitializeCriticalSection(&g_csThreadCode);
    g_hThreadParameter = CreateMutex(NULL,FALSE,NULL); //FALSE表示互斥区属于触发状态，线程可以调用

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i < THREAD_NUM)
    {
        WaitForSingleObject(g_hThreadParameter,INFINITE);  //等待进入互斥区,等待互斥量触发
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);  //创建线程
        //WaitForSingleObject(g_hThreadParameter,INFINITE);  //等待进入互斥区,等待互斥量触发
        i++;
    }

    //等待子线程全部运行结束
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //销毁关键段和事件
    CloseHandle(g_hThreadParameter);
    DeleteCriticalSection(&g_csThreadCode);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{

    int nThreadNum = *(int *)pPM;  //主线程传入 i 的地址.把位置换一下, 主线程没有等子线程取得 i 的值就把 i++ 了.
    ReleaseMutex(g_hThreadParameter);  //离开互斥区


    Sleep(50);  //some work should to do

    //进入关键段
    EnterCriticalSection(&g_csThreadCode);
    g_nNum ++;
    Sleep(0);  //some work should to do
    printf("线程编号为:%d  全局资源值为:%d\n",nThreadNum,g_nNum);

    //离开关键段
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}
