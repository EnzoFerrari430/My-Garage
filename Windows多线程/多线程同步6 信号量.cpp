/*
�ź������õ�3������
1.�����ź���
HANDLE CreateSemaphore(LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,LONG lInitialCount,LONG lMaximumCount,LPCTSTR lpName);
��һ��������ʾ��ȫ���ƣ�һ��ֱ�Ӵ���NULL��
�ڶ���������ʾ��ʼ��Դ������
������������ʾ��󲢷�������
���ĸ�������ʾ�ź��������ƣ�����NULL��ʾ�����ź�����

2.���ź���
HANDLE OpenSemaphore(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);
��һ��������ʾ����Ȩ�ޣ���һ�㴫��SEMAPHORE_ALL_ACCESS����ϸ���Ϳ��Բ鿴MSDN�ĵ���
�ڶ���������ʾ�ź�������̳��ԣ�һ�㴫��TRUE���ɡ�
������������ʾ���ƣ���ͬ�����еĸ��߳̿���ͨ��������ȷ�����Ƿ���ͬһ���ź�����

3.��������
BOOL ReleaseSemaphore(HANDLE hSemaphore,LONG lReleaseCount,LPLONG lpPreviousCount );
��һ���������ź����ľ����
�ڶ���������ʾ���Ӹ������������0�Ҳ����������Դ������
������������������������ǰ����Դ��������ΪNULL��ʾ����Ҫ������

���� CloseHandle()

�ݼ�����
�ȴ���ϵ�к����� WaitForSingleObject()

*/
#include <stdio.h>
#include <process.h>
#include <windows.h>
#include <iostream>
using namespace std;

long g_nNum;
unsigned int _stdcall Fun(void *pPM);
const int THREAD_NUM = 10;

//�ź�����ؼ���
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadParameter;

int main()
{

    printf("�ź����߳�ͬ�� \n\n");


    //��ʼ���ź����͹ؼ���
    g_hThreadParameter = CreateSemaphore(NULL,0,1,NULL);  //��ʼ0,���1���ź���
    InitializeCriticalSection(&g_csThreadCode);

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i<THREAD_NUM)
    {
        //WaitForSingleObject(g_hThreadParameter,INFINITE);  ������һֱ�ȴ�
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        WaitForSingleObject(g_hThreadParameter,INFINITE);
        ++i;
    }

    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //�����ź����͹ؼ���0
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
    ReleaseSemaphore(g_hThreadParameter,1,NULL);  //�ź���++

    Sleep(50);  //some work should to do

    EnterCriticalSection(&g_csThreadCode);
    ++g_nNum;
    Sleep(0);
    //printf("�̱߳��Ϊ:%d  ȫ����ԴΪ:%d\n",nThreadNum,g_nNum);
    cout<<"�̱߳��Ϊ:"<<nThreadNum<<" ȫ����ԴΪ:"<<g_nNum<<endl;  //endlˢ���˻������������printfЧ����ͬ
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}


