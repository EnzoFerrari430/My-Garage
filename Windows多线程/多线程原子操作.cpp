#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <windows.h>


volatile long g_nLoginCount;  //��¼����
const int THREAD_NUM = 50;  //�߳�����50
unsigned int __stdcall Fun(void* pPM);  //�̺߳���

DWORD WINAPI ThreadFun(void* pPM)
{

    Sleep(100);
    //g_nLoginCount++;  ������3������ָ��ᱻӰ��
    //������ԭ�Ӳ���  LONG__cdedlInterlockedIncrement(LONG volatile* Addend);
    InterlockedIncrement((LPLONG)&g_nLoginCount);
    Sleep(50);
    return 0;

}


int main()
{
    printf("%d\n",sizeof(LPLONG));
    printf("      ԭ�Ӳ��� InterLockedϵ�к���ʹ��\n");
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
        printf("��%d���û���¼���¼�����%d\n",THREAD_NUM,g_nLoginCount);
    }



    return 0;
}
