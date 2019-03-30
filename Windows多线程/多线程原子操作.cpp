#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <windows.h>


volatile long g_nLoginCount;  //登录次数
const int THREAD_NUM = 50;  //线程数量50
unsigned int __stdcall Fun(void* pPM);  //线程函数

DWORD WINAPI ThreadFun(void* pPM)
{

    Sleep(100);
    //g_nLoginCount++;  自增有3条操作指令，会被影响
    //自增的原子操作  LONG__cdedlInterlockedIncrement(LONG volatile* Addend);
    InterlockedIncrement((LPLONG)&g_nLoginCount);
    Sleep(50);
    return 0;

}


int main()
{
    printf("%d\n",sizeof(LPLONG));
    printf("      原子操作 InterLocked系列函数使用\n");
    printf("  --by MoreWindows( http:\\blog.csdn.net/MoreWindows ) --\n\n");
    int num = 20;
    while(num--)
    {
        g_nLoginCount = 0;
        int i;
        HANDLE handle[THREAD_NUM];
        for(i = 0 ; i < 50 ; ++i)
        {
            handle[i] = CreateThread(NULL,0,ThreadFun,NULL,0,NULL);
        }
        WaitForMultipleObjects(THREAD_NUM,handle,true,INFINITE);
        printf("有%d个用户登录后记录结果是%d\n",THREAD_NUM,g_nLoginCount);
    }



    return 0;
}
