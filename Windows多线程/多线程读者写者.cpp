#include <windows.h>
#include <process.h>
#include <stdio.h>
//设置控制台输出颜色
BOOL SetConsoleColor(WORD wAttributes)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if(INVALID_HANDLE_VALUE == hConsole)
    {
        return false;
    }
    return SetConsoleTextAttribute(hConsole,wAttributes);
}

const int READER_NUM = 5;  //设置读者个数

//关键段和事件
CRITICAL_SECTION g_cs,g_cs_writer_count;
HANDLE g_hEventWriter,g_hEventNoReader;
int g_nReaderCount;
//读者线程输出函数(变参函数的实现)
void ReaderPrintf(char *pszFormat,...)
{
    va_list   pArgList;  //定义可变参数指针
    va_start(pArgList,pszFormat);  //pszFormat为最后一个固定参数
    EnterCriticalSection(&g_cs);  //进入关键段
    //vfprintf()会根据参数format字符串来转换并格式化数据，然后将结果输出到参数stream指定的文件中，
    //直到出现字符串结束（‘\0’）为止。关于参数format字符串的格式请参 考printf（）。
    vfprintf(stdout, pszFormat, pArgList);
    LeaveCriticalSection(&g_cs);  //离开关键段
    va_end(pArgList);
}
//读者线程函数
unsigned int _stdcall ReadThreadFun(PVOID pM)
{
    ReaderPrintf("    编号为%d的读者进入等待中...\n",GetCurrentThreadId());
    //等待写者完成
    WaitForSingleObject(g_hEventWriter,INFINITE);


    //读者个数增加
    EnterCriticalSection(&g_cs_writer_count);
    g_nReaderCount++;
    if(g_nReaderCount == 1)
    {
        ResetEvent(g_hEventNoReader);  //将事件设置为未触发
    }
    LeaveCriticalSection(&g_cs_writer_count);


    //读取文件
    ReaderPrintf("编号为%d的读者开始读取文件...\n",GetCurrentThreadId());

    Sleep(rand() % 100);  //读一会儿
    //阅读结束
    ReaderPrintf("  编号为%d的读者结束读取文件\n",GetCurrentThreadId());
    //读者个数减少
    EnterCriticalSection(&g_cs_writer_count);
    g_nReaderCount --;
    if(g_nReaderCount == 0)
    {
        SetEvent(g_hEventNoReader);  //触发事件
    }
    LeaveCriticalSection(&g_cs_writer_count);

    return 0;

}

//写者线程输出函数
void WriterPrintf(char *pzStr)
{
    EnterCriticalSection(&g_cs);
    SetConsoleColor(FOREGROUND_GREEN);
    printf("    %s\n",pzStr);
    SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    LeaveCriticalSection(&g_cs);
}

//写者线程函数
unsigned int _stdcall WriterThreadFun(PVOID pM)
{
    WriterPrintf("写者线程进入等待中...");
    //等待读文件的读者为0
    WaitForSingleObject(g_hEventNoReader,INFINITE);
    //标志着写者正在写文件
    ResetEvent(g_hEventWriter);  //设置为未触发

    //写文件
    WriterPrintf("  写者开始写文件...");
    Sleep(rand() % 100);
    WriterPrintf("  写者写文件结束");

    //标志写者结束写文件
    SetEvent(g_hEventWriter);  //触发这个事件
    return 0;
}

int main()
{
    printf("读者写者问题\n");

    //初始化事件和信号量
    InitializeCriticalSection(&g_cs);
    InitializeCriticalSection(&g_cs_writer_count);

    //手动置位，初始已触发
    g_hEventWriter = CreateEvent(NULL,TRUE,TRUE,NULL);  //触发没有写者
    g_hEventNoReader = CreateEvent(NULL,FALSE,TRUE,NULL);  //触发没有读者  自动置位,获取到权限后就关闭，不让别的线程进来
    g_nReaderCount = 0;

    int i;
    HANDLE hThread[READER_NUM + 1];
    //先启动2个读者线程
    for(i=1;i<=2;i++)
    {
        hThread[i] = (HANDLE)_beginthreadex(NULL,0,ReadThreadFun,NULL,0,NULL);
    }
    //启动写者线程
    hThread[0] = (HANDLE)_beginthreadex(NULL,0,WriterThreadFun,NULL,0,NULL);
    Sleep(50);
    //启动剩下的线程
    for(;i<=READER_NUM;i++)
    {
        hThread[i] = (HANDLE)_beginthreadex(NULL,0,ReadThreadFun,NULL,0,NULL);
    }
    WaitForMultipleObjects(READER_NUM +1,hThread,TRUE,INFINITE);
    for(i=0;i<=READER_NUM;i++)
    {
        CloseHandle(hThread[i]);
    }

    //销毁事件的信号量
    CloseHandle(g_hEventWriter);
    CloseHandle(g_hEventNoReader);
    DeleteCriticalSection(&g_cs);
    DeleteCriticalSection(&g_cs_writer_count);

    return 0;
}
