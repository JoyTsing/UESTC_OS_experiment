if [[ "$OSTYPE" =~ ^linux  ]];then
    g++ -Wall ./repl.cpp  -ledit  -o osShell
    exit 0
fi
#如果是Windows下
g++ ./repl.cpp -o osShell
exit 0
