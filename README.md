# UESTC_OS_experiment

电子科大的操作系统实验一

## 使用方法：

对于不同的平台使用方法不同

### Linux

对于Debian一族来说，使用:
`sudo apt-get install libedit-dev`
对于Fedora一族来说，使用：
`su -c "yum install libedit-dev*"`
安装所需要的`editline`库
使用该库解决了Linux下使用小键盘出现[[^D的问题，能够使用小键盘移动来正常修改命令，同时能够像在bash中使用上下键来查看历史命令


### Windows

Win下无须安装额外的库，但同时不提供sh中查看历史的功能

### 编译运行

直接运行目录下的./complier.sh生成可执行文件，运行即可，输入help查看命令帮助

