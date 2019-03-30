#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //全局资源
const int THREAD_NUM = 10; //子线程个数
unsigned int _stdcall Fun(void *pPM);  //线程函数


//定义关键段和事件
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadEvent;
/*
第一个参数表示安全控制，一般直接传入NULL。

第二个参数确定事件是手动置位还是自动置位，传入TRUE表示手动置位，传入FALSE表示自动置位。
如果为自动置位，则对该事件调用WaitForSingleObject()后会自动调用ResetEvent()使事件变成未触发状态。
打个小小比方，手动置位事件相当于教室门，教室门一旦打开（被触发），所以有人都可以进入直到老师去关上教室门（事件变成未触发）。
自动置位事件就相当于医院里拍X光的房间门，门打开后只能进入一个人，这个人进去后会将门关上，其它人不能进入除非门重新被打开（事件重新被触发）。

第三个参数表示事件的初始状态，传入TRUE表示已触发。

第四个参数表示事件的名称，传入NULL表示匿名事件。

*/

int main()
{
    //初始化事件和关键段 自动置位，初始无触发的匿名事件
    InitializeCriticalSection(&g_csThreadCode);
    g_hThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i < THREAD_NUM)
    {
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        WaitForSingleObject(g_hThreadEvent,INFINITE);  //等待事件被触发,因为是自动置位的，所以在这句话后会自动将事件设置为未触发
        i++;
    }

    //等待子线程全部运行结束
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //销毁关键段和事件
    CloseHandle(g_hThreadEvent);
    DeleteCriticalSection(&g_csThreadCode);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{

    int nThreadNum = *(int *)pPM;  //主线程传入 i 的地址.把位置换一下, 主线程没有等子线程取得 i 的值就把 i++ 了.
    SetEvent(g_hThreadEvent);  //触发事件

    Sleep(50);  //some work should to do

    //进入关键段
    EnterCriticalSection(&g_csThreadCode);
    g_nNum ++;
    Sleep(0);  //some work should to do
    printf("ThreadID:%d Global Resource:%d\n",nThreadNum,g_nNum);

    //离开关键段
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}
