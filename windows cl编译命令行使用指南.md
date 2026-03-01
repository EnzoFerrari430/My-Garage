# Windows 下使用 cl.exe 编译 C++ 代码总结

## 一、核心前提
1. 依赖环境：`cl.exe` 是 Visual Studio 自带编译器，需先安装 Visual Studio（任意版本，如 Community 2022），安装时必须勾选「桌面开发使用 C++」组件（包含 MSVC 工具链）。
2. 环境激活：不能直接在普通 cmd 中使用 `cl.exe`，需打开 VS 提供的「开发者命令提示符」：
   - 64 位：`x64 Native Tools Command Prompt for VS 2022`
   - 32 位：`x86 Native Tools Command Prompt for VS 2022`
3. 验证环境：打开后输入 `cl`，出现版本信息即成功。

---

## 二、基础编译命令

### 1. 单文件编译（最常用）
```bash
# 必加 /EHsc 启用 C++ 异常处理
cl /EHsc test.cpp
test.exe
```

### 2. 自定义输出文件名
```bash
cl /EHsc /Fe:myapp.exe test.cpp
```

### 3. 多文件编译
```bash
cl /EHsc main.cpp utils.cpp /Fe:myapp.exe
```


## 三、常用编译参数
|  参数   | 作用  |
|  ----  | ----  |
| /EHsc  | C++ 异常处理（必加） |
| /Fe:name  | 指定 exe 文件名 |
| /O2  | 发布版优化 |
| /Zi  | 生成调试信息 |
| /I"路径"  | 添加头文件目录 |
| /utf-8  | 源码与输出 UTF-8，解决中文乱码 |
| /link xxx.lib  | 链接额外库文件 |

常用组合示例：
```bash
cl /EHsc /utf-8 /O2 test.cpp /Fe:app.exe
```