/*
������Ҳ��һ���ں˶���������ȷ��һ���̶߳�ռһ����Դ�ķ��ʡ�
��������ؼ��ε���Ϊ�ǳ����ƣ����һ������������ڲ�ͬ�����е��̻߳��������Դ��

��������4������
1.����������
HANDLE CreateMutex(LPSECURITY_ATTRIBUTES lpMutexAttributes,BOOL bInitialOwner,LPCTSTR lpName);

��һ��������ʾ��ȫ���ƣ�һ��ֱ�Ӵ���NULL��
�ڶ�����������ȷ���������ĳ�ʼӵ���ߡ�
�������TRUE��ʾ�����������ڲ����¼���������̵߳��߳�ID�Ų����ݹ��������Ϊ1�����ڸ��߳�ID���㣬���Ի���������δ����״̬��
�������FALSE����ô�����������ڲ����߳�ID�Ž�����ΪNULL���ݹ��������Ϊ0������ζ��������Ϊ�κ��߳�ռ�ã����ڴ���״̬��

�����������������û����������ƣ��ڶ�������е��߳̾���ͨ��������ȷ�����Ƿ��ʵ���ͬһ����������

��������ֵ��
�ɹ�����һ����ʾ�������ľ����ʧ�ܷ���NULL��

2.�򿪻�����
HANDLE OpenMutex(DWORD dwDesiredAccess,BOOL bInheritHandle,LPCTSTR lpName);

��һ��������ʾ����Ȩ�ޣ��Ի�����һ�㴫��MUTEX_ALL_ACCESS����ϸ���Ϳ��Բ鿴MSDN�ĵ���
�ڶ���������ʾ����������̳��ԣ�һ�㴫��TRUE���ɡ�
������������ʾ���ơ�ĳһ�������е��̴߳��������������������е��߳̾Ϳ���ͨ������������ҵ������������
��������ֵ��
�ɹ�����һ����ʾ�������ľ����ʧ�ܷ���NULL��

3.�뿪������
BOOL ReleaseMutex (HANDLE hMutex)
���ʻ�����ԴǰӦ��Ҫ���õȴ���������������ʱ��Ҫ����ReleaseMutex()����ʾ�Լ��Ѿ��������ʣ������߳̿��Կ�ʼ�����ˡ�

4.��������
���ڻ��������ں˶������ʹ��CloseHandle()�Ϳ��ԣ���һ�������ں˶���һ������


������Ҳ���߳�����Ȩ�ĸ�����߳�ӵ���߳�����Ȩ�������������̵߳�ͬ��
*/

#include <stdio.h>
#include <process.h>
#include <windows.h>

long g_nNum; //ȫ����Դ
const int THREAD_NUM = 10; //���̸߳���
unsigned int _stdcall Fun(void *pPM);  //�̺߳���


//����ؼ��κͻ�����
CRITICAL_SECTION g_csThreadCode;
HANDLE g_hThreadParameter;

int main()
{
    //��ʼ���������͹ؼ��� �ڶ�������ΪTRUE��ʾ������Ϊ�����߳�����
    InitializeCriticalSection(&g_csThreadCode);
    g_hThreadParameter = CreateMutex(NULL,FALSE,NULL); //FALSE��ʾ���������ڴ���״̬���߳̿��Ե���

    HANDLE handle[THREAD_NUM];
    g_nNum = 0;
    int i = 0;
    while(i < THREAD_NUM)
    {
        WaitForSingleObject(g_hThreadParameter,INFINITE);  //�ȴ����뻥����,�ȴ�����������
        handle[i] = (HANDLE)_beginthreadex(NULL,0,Fun,&i,0,NULL);  //�����߳�
        //WaitForSingleObject(g_hThreadParameter,INFINITE);  //�ȴ����뻥����,�ȴ�����������
        i++;
    }

    //�ȴ����߳�ȫ�����н���
    WaitForMultipleObjects(THREAD_NUM,handle,TRUE,INFINITE);

    //���ٹؼ��κ��¼�
    CloseHandle(g_hThreadParameter);
    DeleteCriticalSection(&g_csThreadCode);

    return 0;
}

unsigned int _stdcall Fun(void *pPM)
{

    int nThreadNum = *(int *)pPM;  //���̴߳��� i �ĵ�ַ.��λ�û�һ��, ���߳�û�е����߳�ȡ�� i ��ֵ�Ͱ� i++ ��.
    ReleaseMutex(g_hThreadParameter);  //�뿪������


    Sleep(50);  //some work should to do

    //����ؼ���
    EnterCriticalSection(&g_csThreadCode);
    g_nNum ++;
    Sleep(0);  //some work should to do
    printf("�̱߳��Ϊ:%d  ȫ����ԴֵΪ:%d\n",nThreadNum,g_nNum);

    //�뿪�ؼ���
    LeaveCriticalSection(&g_csThreadCode);
    return 0;
}
