#include <stdio.h>
#include <process.h>
#include <windows.h>
/*
使用关键段CRITICAL_SECTION
关键段CRITICAL_SECTION的4个函数
1.初始化
void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
定义关键字变量后必须先初始化

2.销毁
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
用完之后记得销毁

3.进入关键区域
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
系统保证各线程互斥的进入关键区域

4.离开关键区域
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);


*/

long g_nNum;  //全局资源

unsigned int _stdcall Fun(void *pPM);  //线程函数
const int THREAD_NUM = 10;  //子线程个数

//关键段变量声明
CRITICAL_SECTION g_csThreadParamer,g_csThreadCode;


int main()
{
    //关键段初始化
    InitializeCriticalSection(&g_csThreadParamer);
    InitializeCriticalSection(&g_csThreadCode);

    g_nNum = 0;
    HANDLE handle[THREAD_NUM];

    int i = 0;
    while(i<THREAD_NUM)
    {
        //进入子线程序号关键区域
        EnterCriticalSection(&g_csThreadParamer);
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        ++i;  //等待子线程接收到参数时，主线程才可能改变这个i值
    }
    //主线程可以多次进入这个关键区域所以，所以关键段对主线程不起作用


    //保证子线程已经全部运行结束
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //销毁
    DeleteCriticalSection(&g_csThreadCode);
    DeleteCriticalSection(&g_csThreadParamer);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{
    //由于创建线程需要一定开销,所以新线程并不能第一时间进行到这里来
    int nThreadNum = *(int *)pPM;  //子线程获取参数，强制类型转换
    //离开子线程序号关键区域
    LeaveCriticalSection(&g_csThreadParamer);

    Sleep(50);  //some work should to do

    //进入各子线程互斥区域
    EnterCriticalSection(&g_csThreadCode);
    g_nNum++;  //处理全局资源
    Sleep(0);  //some work should to do
    printf("线程编号为:%d 全局资源值为:%d\n",nThreadNum,g_nNum);
    //离开各子线程互斥区域
    LeaveCriticalSection(&g_csThreadCode);

    return 0;

}

/*
最后总结下关键段：

1．关键段共初始化化、销毁、进入和离开关键区域四个函数。

2．关键段可以解决线程的互斥问题，但因为具有“线程所有权”，所以无法解决同步问题。

3．推荐关键段与旋转锁配合使用。


*/
