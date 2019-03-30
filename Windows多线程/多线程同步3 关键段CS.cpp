#include <stdio.h>
#include <process.h>
#include <windows.h>
/*
ʹ�ùؼ���CRITICAL_SECTION
�ؼ���CRITICAL_SECTION��4������
1.��ʼ��
void InitializeCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
����ؼ��ֱ���������ȳ�ʼ��

2.����
void DeleteCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
����֮��ǵ�����

3.����ؼ�����
void EnterCriticalSection(LPCRITICAL_SECTION lpCriticalSection);
ϵͳ��֤���̻߳���Ľ���ؼ�����

4.�뿪�ؼ�����
void LeaveCriticalSection(LPCRITICAL_SECTION lpCriticalSection);


*/

long g_nNum;  //ȫ����Դ

unsigned int _stdcall Fun(void *pPM);  //�̺߳���
const int THREAD_NUM = 10;  //���̸߳���

//�ؼ��α�������
CRITICAL_SECTION g_csThreadParamer,g_csThreadCode;


int main()
{
    //�ؼ��γ�ʼ��
    InitializeCriticalSection(&g_csThreadParamer);
    InitializeCriticalSection(&g_csThreadCode);

    g_nNum = 0;
    HANDLE handle[THREAD_NUM];

    int i = 0;
    while(i<THREAD_NUM)
    {
        //�������߳���Źؼ�����
        EnterCriticalSection(&g_csThreadParamer);
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        ++i;  //�ȴ����߳̽��յ�����ʱ�����̲߳ſ��ܸı����iֵ
    }
    //���߳̿��Զ�ν�������ؼ��������ԣ����Թؼ��ζ����̲߳�������


    //��֤���߳��Ѿ�ȫ�����н���
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //����
    DeleteCriticalSection(&g_csThreadCode);
    DeleteCriticalSection(&g_csThreadParamer);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{
    //���ڴ����߳���Ҫһ������,�������̲߳����ܵ�һʱ����е�������
    int nThreadNum = *(int *)pPM;  //���̻߳�ȡ������ǿ������ת��
    //�뿪���߳���Źؼ�����
    LeaveCriticalSection(&g_csThreadParamer);

    Sleep(50);  //some work should to do

    //��������̻߳�������
    EnterCriticalSection(&g_csThreadCode);
    g_nNum++;  //����ȫ����Դ
    Sleep(0);  //some work should to do
    printf("�̱߳��Ϊ:%d ȫ����ԴֵΪ:%d\n",nThreadNum,g_nNum);
    //�뿪�����̻߳�������
    LeaveCriticalSection(&g_csThreadCode);

    return 0;

}

/*
����ܽ��¹ؼ��Σ�

1���ؼ��ι���ʼ���������١�������뿪�ؼ������ĸ�������

2���ؼ��ο��Խ���̵߳Ļ������⣬����Ϊ���С��߳�����Ȩ���������޷����ͬ�����⡣

3���Ƽ��ؼ�������ת�����ʹ�á�


*/
