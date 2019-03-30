#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //ȫ����Դ
const int THREAD_NUM = 10; //���̸߳���
unsigned int _stdcall Fun(void *pPM);  //�̺߳���


//����ؼ��κ��¼�
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadEvent;
/*
��һ��������ʾ��ȫ���ƣ�һ��ֱ�Ӵ���NULL��

�ڶ�������ȷ���¼����ֶ���λ�����Զ���λ������TRUE��ʾ�ֶ���λ������FALSE��ʾ�Զ���λ��
���Ϊ�Զ���λ����Ը��¼�����WaitForSingleObject()����Զ�����ResetEvent()ʹ�¼����δ����״̬��
���СС�ȷ����ֶ���λ�¼��൱�ڽ����ţ�������һ���򿪣������������������˶����Խ���ֱ����ʦȥ���Ͻ����ţ��¼����δ��������
�Զ���λ�¼����൱��ҽԺ����X��ķ����ţ��Ŵ򿪺�ֻ�ܽ���һ���ˣ�����˽�ȥ��Ὣ�Ź��ϣ������˲��ܽ�����������±��򿪣��¼����±���������

������������ʾ�¼��ĳ�ʼ״̬������TRUE��ʾ�Ѵ�����

���ĸ�������ʾ�¼������ƣ�����NULL��ʾ�����¼���

*/

int main()
{
    //��ʼ���¼��͹ؼ��� �Զ���λ����ʼ�޴����������¼�
    InitializeCriticalSection(&g_csThreadCode);
    g_hThreadEvent = CreateEvent(NULL,FALSE,FALSE,NULL);

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i < THREAD_NUM)
    {
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);
        WaitForSingleObject(g_hThreadEvent,INFINITE);  //�ȴ��¼�������,��Ϊ���Զ���λ�ģ���������仰����Զ����¼�����Ϊδ����
        i++;
    }

    //�ȴ����߳�ȫ�����н���
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //���ٹؼ��κ��¼�
    CloseHandle(g_hThreadEvent);
    DeleteCriticalSection(&g_csThreadCode);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{

    int nThreadNum = *(int *)pPM;  //���̴߳��� i �ĵ�ַ.��λ�û�һ��, ���߳�û�е����߳�ȡ�� i ��ֵ�Ͱ� i++ ��.
    SetEvent(g_hThreadEvent);  //�����¼�

    Sleep(50);  //some work should to do

    //����ؼ���
    EnterCriticalSection(&g_csThreadCode);
    g_nNum ++;
    Sleep(0);  //some work should to do
    printf("ThreadID:%d Global Resource:%d\n",nThreadNum,g_nNum);

    //�뿪�ؼ���
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}
