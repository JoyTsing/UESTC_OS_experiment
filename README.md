# UESTC_OS_experiment

电子科大的操作系统实验一

## 安装方法：

对于不同的平台使用方法不同，但要求g++能够支持C++11的版本。

### Linux

对于Debian一族来说，使用:

`sudo apt-get install libedit-dev`

对于Fedora一族来说，使用：

`su -c "yum install libedit-dev*"`

安装所需要的`editline`库

该库解决了Linux下使用小键盘出现[[^D的问题，能够使用小键盘移动来正常修改命令，同时能够像在bash中使用上下键来查看历史命令

### Windows

Win下无须安装额外的库可实现同样的效果,但为了保证下一步编译顺利进行，还需要有g++环境以及`gitbash`来执行命令脚本

## 编译运行

直接运行目录下的./complier.sh生成可执行文件，运行即可，输入help查看命令帮助。

## 运行说明

大部分命令的功能可通过内置的`help`命令来带得到帮助和提示。对于读文件功能，在命令行下来使用则可以通过`./OSshell 读取的文件名`来使用，在文件的同级目录下有着供测试用的基础文件 test.txt。而对于Windows下，可以在进入程序后，通过指令`load`在运行`osShell`的时候来读取命令并观察结果。

因为原生代码是在`ubuntu`下使用Vim编写的，首先推荐使用`Linux`环境。

## 代码说明

### 文件结构

**Prm.hpp**

所需要用到的各类数据结构比如RCB,Resource，PCB,以及他们的构造方法以及销毁方法都在其中，同时使用`enum`定义了进程的优先级以及进程的状态，同时还提供了他们转化为`const char*`的char_status和char_priority函数供打印的时候使用

### pmMange.hpp

定义了sh所需要用的资源和REPL中所需要用到的一些方法

### repl.cpp

用于与用户进行交互，读取输入的命令，并对收到的命令的个数和类型作出了限制，并使用宏函数对于一些常见的错误进行了提示，确保用户输入是有效的情况下在进行执行，其中`handle`用于根据对输入的指令进行判断和执行，`command_print`用于对无法识别的命令进行报错，其中内建的命令是在Prm.hpp中的枚举类`commands`进行定义。同时提供了读文件进行执行的功能，其中大部分功能模块之间是解耦的。

### complier.sh

用来生成可运行程序

## 额外重要说明

**代码没有将完全正确的部分上传**，而且比较推荐运用自己的知识做一下本次实验，hava fun :)



