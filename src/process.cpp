// 程序名：EX07_myshell_process.cpp
// 姓名：李佳园 学号：3190100930

#include <iostream>
#include <string>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <signal.h>
#include "main.h"
#include "process.h"
#include "myerr.h"
#include "mycmd.h"

using namespace std;

extern string arg[MAX_ARG];
extern int stdin_copy, stdout_copy;
extern struct proc_item proc_list[MAX_PROC];
extern int proc_num;

int execmd_without_pipe(int l, int r);

//执行输入的命令，参数下标范围为(l,r)，返回enum ERR中所枚举的执行状态或错误信息
int execmd(int l, int r)
{
    //若此行没有输入，正常退出
    if (l == r)
        return EXIT_OK;
    //pip_pos从左到右第一个管道符号的位置
    int pip_pos;
    for (pip_pos = l; pip_pos < r; pip_pos++)
        if (arg[pip_pos] == "|")
            break;
    //管道符号在最左侧或者最右侧都是不合法的
    if (pip_pos == l || pip_pos == r - 1)
        return PARAMENT_NUM_ERR;
    //无管道，调用无管道命令执行函数
    else if (pip_pos == r)
        return execmd_without_pipe(l, r);

    //有管道的情况下
    //管道文件描述符
    int file_pipes[2];
    //新建管道，将管道读写描述符赋值
    int pipe_result = pipe(file_pipes);
    //如果管道创建成功
    if (pipe_result == 0)
    {
        //创建子进程
        pid_t fork_result = fork();
        //如果创建进程失败
        if (fork_result == -1)
            return FORK_FAILURE;
        //如果创建进程成功
        //fork_result==0，说明是子进程，即|前面的部分
        else if (fork_result == 0)
        {
            //关闭管道读入端
            close(file_pipes[0]);
            //将标准输出重定向管道输出端
            dup2(file_pipes[1], 1);
            //调用无管道命令执行函数
            int res = execmd_without_pipe(l, pip_pos);
            //执行完成，关闭管道输出端
            close(file_pipes[1]);
            //打印子进程错误信息
            print_err(res);
            //执行完成，终止进程，子进程返回错误代码
            exit(res);
        }
        //fork_result为pid(!= 0)，说明是父进程，即|后面的部分
        else
        {
            //关闭管道输出端，因为不会用到
            close(file_pipes[1]);
            int status;
            //前台进程，父进程阻塞
            waitpid(fork_result, &status, WUNTRACED);
            //如果子进程正常退出（指正常通过exit()函数退出，或return 0退出）
            if (WIFEXITED(status))
            {
                //获取子进程错误代码（即exit()中的值）
                int res_son = WEXITSTATUS(status);
                //如果子进程的指令正常执行
                if (res_son == EXIT_OK)
                {
                    //重定向标准输入到管道读入端，这样父进程就可以读入子进程输出的数据
                    dup2(file_pipes[0], 0);
                    //递归执行后续指令
                    int res = execmd(pip_pos + 1, r);
                    //关闭管道读入端
                    close(file_pipes[0]);
                    //恢复重定向，将0号文件描述符重定向到标准输入的备份
                    dup2(stdin_copy, 0);
                    //父进程返回错误代码
                    return res;
                }
                //如果子进程的指令没有正常执行（即res_son!=EXIT_OK）
                else
                {
                    //关闭管道读入端
                    close(file_pipes[0]);
                    //父进程返回错误代码
                    return res_son;
                }
            }
            //如果子进程没有正常退出（指子进程终止或异常退出）
            else
                return SUBPROCESS_FAILURE;
        }
    }
    //如果管道创建失败
    else
        return PIPE_FAILURE;
}

//无管道命令执行函数，参数下标范围为(l,r)，返回enum ERR中所枚举的执行状态或错误信息
int execmd_without_pipe(int l, int r)
{
    //若此行没有输入，正常退出
    if (l == r)
        return EXIT_OK;

    // 判断是否是后台进程
    int isbg = (arg[r - 1] == "&") ? 1 : 0;
    //newr为除去&与重定向后的实际命令范围
    int newr = r;
    if (arg[r - 1] == "&")
        newr--;

    //判断是否有输入输出重定向
    string infile = "", outfile = "";
    int outflag = 0;
    for (int i = l; i < r; i++)
    {
        //重定向输入
        if (arg[i] == "<")
        {
            //输入文件不存在
            if (i + 1 == r)
                return PARAMENT_NUM_ERR;
            //有多个输入重定向
            else if (infile != "")
                return PARAMENT_NUM_ERR;
            else
            {
                infile = arg[i + 1];
                if (newr > i)
                    newr = i;
            }
        }
        //重定向输出
        if (arg[i] == ">" || arg[i] == ">>")
        {
            //输出文件不存在
            if (i + 1 == r)
                return PARAMENT_NUM_ERR;
            //有多个输出重定向
            else if (outfile != "")
                return PARAMENT_NUM_ERR;
            else
            {
                outfile = arg[i + 1];
                if (newr > i)
                    newr = i;
            }
            //判断是重写（截断模式）还是在文件末尾追加（附加模式）
            if (arg[i] == ">>")
                outflag = 1;
        }
    }

    //先处理重定向
    int in_f_d = -1, out_f_d = -1;
    //输入重定向存在
    if (infile != "")
    {
        //输入文件不存在
        if (access(infile.c_str(), F_OK) == -1)
        {
            cout << "\033[31mERROR: The file " << infile << " not exist." << endl;
            return OTHERS;
        }
        //输入文件存在但没有读权限
        if (access(infile.c_str(), R_OK) == -1)
        {
            cout << "\033[31mERROR: The file " << infile << " is not permitted to be read." << endl;
            return OTHERS;
        }
        //打开输入文件
        in_f_d = open(infile.c_str(), O_RDONLY);
        //输入文件无法打开
        if (in_f_d == -1)
        {
            cout << "\033[31mERROR: Open file " << infile << " failure!" << endl;
            return OTHERS;
        }
    }
    //输出重定向存在
    if (outfile != "")
    {
        //输出文件存在但没有写权限
        if (access(outfile.c_str(), F_OK) != -1 && access(outfile.c_str(), W_OK) == -1)
        {
            cout << "\033[31mERROR: The file " << outfile << " is not permitted to be written." << endl;
            return OTHERS;
        }
        //打开或创建输出文件
        //outflag=1时打开的文件是附加模式
        if (outflag)
            out_f_d = open(outfile.c_str(), O_WRONLY | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        //outflag=0时打开的文件是截断模式
        else
            out_f_d = open(outfile.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        //打开或创建输出文件失败
        if (out_f_d == -1)
        {
            cout << "\033[31mERROR: Open file " << outfile << " failure!" << endl;
            return OTHERS;
        }
    }

    //内嵌指令，不需要向进程列表里添加相
    //先执行部分必须要在父进程执行的内部指令，因为退出子进程后还得保持这些属性
    //这些命令无法在后台执行，且实现不需要实现重定向
    //直接return错误信息到caller处再被输出
    if (arg[l] == "cd")
        return mycd(l, newr);
    else if (arg[l] == "clr")
        return myclr(l, newr);
    else if (arg[l] == "exit")
        return myexit(l, newr);
    else if (arg[l] == "bg")
        return mybg(l, newr);
    else if (arg[l] == "fg")
        return myfg(l, newr);
    else if (arg[l] == "unset")
        return myunset(l, newr);
    else if (arg[l] == "umask")
        return myumask(l, newr);
    else if (arg[l] == "exec")
        return myexec(l, newr);
    else if (arg[l] == "shift")
        return myshift(l, newr);

    //非内嵌指令，需要向进程列表里添加相
    //创建子进程
    pid_t fork_result = fork();
    //创建进程失败
    if (fork_result == -1)
        return FORK_FAILURE;
    //子进程，执行命令
    else if (fork_result == 0)
    {
        //实现重定向
        if (infile != "")
            dup2(in_f_d, 0);
        if (outfile != "")
            dup2(out_f_d, 1);

        //再执行剩下的可以通过创建子进程执行的内部命令
        //这些指令需要考虑重定向，可能要求在后台执行
        //需要先在子进程中打印错误信息，再exit离开子进程
        if (arg[l] == "pwd")
        {
            int res = mypwd(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "dir")
        {
            int res = mydir(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "echo")
        {
            int res = myecho(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "help")
        {
            int res = myhelp(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "jobs")
        {
            int res = myjobs(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "test")
        {
            int res = mytest(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "time")
        {
            int res = mytime(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "set")
        {
            int res = myset(l, newr);
            print_err(res);
            exit(0);
        }
        else if (arg[l] == "more")
        {
            int res = mymore(l, newr);
            print_err(res);
            exit(0);
        }
        //其余命令，直接调用execvp函数
        else
        {
            char *tmp[MAX_ARG];
            for (int i = l; i < newr; i++)
            {
                tmp[i - l] = (char *)malloc(BUF);
                strcpy(tmp[i - l], arg[i].c_str());
            }
            tmp[newr - l] = NULL;
            //调用execvp函数
            int res = execvp(arg[l].c_str(), tmp);
            //执行失败，指令类型不合法
            if (res == -1)
                exit(CMD_ILLEGAL);
            exit(0);
        }
    }
    //父进程，向进程列表内添加相，并判断是否是后台进程、是否有Ctrl+Z输入
    else
    {
        int workid = proc_num;
        //向进程表里添加项
        proc_list[proc_num].pid = fork_result;
        memset(proc_list[proc_num].proc_name, 0, sizeof(proc_list[proc_num].proc_name));
        strcpy(proc_list[proc_num].proc_name, arg[l].c_str());
        proc_list[proc_num].isbg = isbg;
        proc_list[proc_num].status = RUNNING;
        proc_num++;

        //如果前台进程，需要等待结束，可以等待Ctrl+Z转入后台并暂停
        if (!isbg)
        {
            //注册信号函数，用于捕捉Ctrl+Z输入
            signal(SIGTSTP, CtrlZHandler);
            int status;
            //父进程阻塞，等待子进程结束再进行父进程
            //WUNTRACED是为了在子进程在Ctrl+Z作用下停止的情况下也能正常返回
            waitpid(fork_result, &status, WUNTRACED);
            //恢复信号函数
            signal(SIGTSTP, SIG_DFL);

            //子进程正常退出（指正常通过exit()函数退出，或return 0退出）
            if (WIFEXITED(status))
            {
                //获取并打印错误代码
                int res_son = WEXITSTATUS(status);
                print_err(res_son);
                //从进程表中删除对应的进程
                for (int i = workid; i < proc_num - 1; i++)
                {
                    proc_list[i].pid = proc_list[i + 1].pid;
                    proc_list[i].isbg = proc_list[i + 1].isbg;
                    proc_list[i].status = proc_list[i + 1].status;
                    strcpy(proc_list[i].proc_name, proc_list[i + 1].proc_name);
                }
                proc_num--;

                return EXIT_OK;
            }
            //子进程停止（指子进程在Ctrl+Z作用下停止）
            else if (WIFSTOPPED(status))
            {
                //输出子进程信息
                cout << "\033[35m[" << workid + 1 << "] [" << proc_list[workid].pid << "] Stopped " << proc_list[workid].proc_name << endl;
                return EXIT_OK;
            }
            else
                return SUBPROCESS_FAILURE;
        }
        //如果后台进程，不需要等待结束
        else
        {
            //直接打印进程信息
            cout << "\033[35m[" << workid + 1 << "]" << proc_list[workid].pid << " Runnning " << proc_list[workid].proc_name << endl;
            return EXIT_OK;
        }
    }
}

//处理Ctrl-Z信号
//可以将一个正在前台执行的命令放到后台，并且暂停
void CtrlZHandler(int sig)
{
    //进程表为空则返回
    if (proc_num == 0)
        return;
    //获取子进程的pid，并发送SIGSTOP信号，令其停止
    pid_t pid = proc_list[proc_num - 1].pid;
    kill(pid, SIGSTOP);
    //更新进程表
    proc_list[proc_num - 1].status = STOPPED;
    proc_list[proc_num - 1].isbg = 1;
}
