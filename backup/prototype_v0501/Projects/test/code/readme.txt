文件夹中,radar.c是主流程文件,运行命令是:
1.  gcc radar.c -lvsip -lfftw3 -lm -o radar.o
2.  ./radar.o
如果想看中间结果,每一个组件后面的注释掉的writedata函数去掉注释就行了
