// 程序名：EX07_myshell_process.h
// 姓名：李佳园 学号：3190100930

#pragma once

#include "main.h"

//存储进程信息
struct proc_item
{
    pid_t pid;           //进程编号（唯一）
    char proc_name[BUF]; //进程名
    int isbg;            //是否后台
    int status;          //进程状态
};

//进程状态
enum ProcStatus
{
    RUNNING,
    STOPPED
};

int execmd(int l, int r);
void CtrlZHandler(int sig);
