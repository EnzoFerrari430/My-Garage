#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum;  //全局资源

unsigned int _stdcall Fun(void *pPM);  //线程函数
const int THREAD_NUM = 10;  //子线程个数


int main()
{
    g_nNum = 0;
    HANDLE handle[THREAD_NUM];

    int i = 0;
    while(i<THREAD_NUM)
    {
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        i++;  //等待子线程接收到参数时，主线程才可能改变这个i值
    }
    //保证子线程已经全部运行结束
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);
    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{
    //由于创建线程需要一定开销,所以新线程并不能第一时间进行到这里来
    int nThreadNum = *(int *)pPM;  //子线程获取参数，强制类型转换
    Sleep(50);  //some work should to do
    g_nNum++;  //处理全局资源
    Sleep(0);  //some work should to do
    printf("线程编号为:%d 全局资源值为:%d\n",nThreadNum,g_nNum);
    return 0;

}
