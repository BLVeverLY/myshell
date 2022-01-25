// 程序名：EX07_myshell_myerr.cpp
// 姓名：李佳园 学号：3190100930

#include "myerr.h"
#include <iostream>
using namespace std;

//输出错误信息
void print_err(int err)
{
    switch (err)
    {
    case EXIT_OK:
    case QUIT:
        break;
    case CMD_ILLEGAL:
        cout << "\033[31mERROR: Command not exists or illegal." << endl;
        break;
    case PARAMENT_NUM_ERR:
        cout << "\033[31mERROR: Too many or too few arguments." << endl;
        break;
    case AUTHORITY_LIMITED:
        cout << "\033[31mERROR: Authority limited." << endl;
        break;
    case PIPE_FAILURE:
    case FORK_FAILURE:
    case SUBPROCESS_FAILURE:
        cout << "\033[31mERROR: Pipe or subprocess failed!" << endl;
        break;
    //其余不具备普遍性的错误信息，已经直接在指令调用时输出
    case OTHERS:
        break;
    }
}
