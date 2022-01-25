// 程序名：EX07_myshell_myerr.h
// 姓名：李佳园 学号：3190100930

#pragma once

//每条命令的返回状态与错误信息
enum ERR
{
    EXIT_OK,
    QUIT,
    CMD_ILLEGAL,
    PARAMENT_NUM_ERR,
    AUTHORITY_LIMITED,
    PIPE_FAILURE,
    FORK_FAILURE,
    SUBPROCESS_FAILURE,
    OTHERS
};

void print_err(int err);
