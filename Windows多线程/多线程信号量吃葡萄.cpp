#include <iostream>
#include <stdio.h>
#include <process.h>
#include <windows.h>

using namespace std;

/*
    һ�����ӣ��൱�ڻ������������Է�3������

    ��Ҫ2���̣߳�һ���������̣߳�һ���������߳�
*/
unsigned int _stdcall CumFun(void *pPM);
unsigned int _stdcall ProFun(void *pPM);

//�ź�����ؼ���
CRITICAL_SECTION g_cs;
HANDLE g_hBufferEmpty,g_hBufferFull;

const int BUFFER_SIZE = 8;
int g_Buffer[BUFFER_SIZE];  //���������
int g_i,g_j;
unsigned int _stdcall CumFun(void *pPM)
{
    int sum = 0;
    int count = 0;
    while(1)
    {
        //�ȴ�������������
        WaitForSingleObject(g_hBufferFull,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        printf("�����ߴӵ�%d��������ȡ������%d...\n",g_j,g_Buffer[g_j]);
        count ++;
        sum = sum + g_Buffer[g_j];
        g_j = (g_j + 1)%BUFFER_SIZE;
        if(count == 3)
        {
            float f = (float)sum/3;
            printf("  3����ƽ��ֵΪ%.2f\n",f);
            count = 0;
            sum = 0;
        }
        //�뿪�ؼ���
        LeaveCriticalSection(&g_cs);
        Sleep(rand()%1000);  //�ȴ�
        ReleaseSemaphore(g_hBufferEmpty,1,NULL);  //��λ+1
    }


    return 0;
}

unsigned int _stdcall ProFun(void *pPM)
{
    while(1)
    {
        //�ȴ��������пյ�λ��
        WaitForSingleObject(g_hBufferEmpty,INFINITE);

        //����ؼ���
        EnterCriticalSection(&g_cs);
        g_Buffer[g_i] = rand()%100;
        printf("      �������ڻ���ص�%d���������з�������%d������\n",g_i,g_Buffer[g_i]);
        g_i = (g_i + 1)%BUFFER_SIZE;
        //�뿪�ؼ���
        LeaveCriticalSection(&g_cs);
        Sleep(rand()%1000);  //�ȴ�

        ReleaseSemaphore(g_hBufferFull,1,NULL);  //�����ݵĻ�����+1
    }
    return 0;


}
int main()
{
    //��ʼ���ź����͹ؼ���
    InitializeCriticalSection(&g_cs);
    g_hBufferEmpty = CreateSemaphore(NULL,8,8,NULL);  //�ź�����ʼΪ8�����Ϊ8
    g_hBufferFull = CreateSemaphore(NULL,0,8,NULL);   //�ź�����ʼΪ8�����Ϊ8

    g_i = 0;
    g_j = 0;

    HANDLE hThread[2];
    hThread[0] = (HANDLE)_beginthreadex(NULL,0,ProFun,NULL,0,NULL);

    Sleep(3000);
    hThread[1] = (HANDLE)_beginthreadex(NULL,0,CumFun,NULL,0,NULL);

    WaitForMultipleObjects(2,hThread,TRUE,INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);

    //���ٹؼ��κ��¼�
    CloseHandle(g_hBufferEmpty);
    CloseHandle(g_hBufferFull);
    DeleteCriticalSection(&g_cs);

    return 0;
}
