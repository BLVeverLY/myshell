// 程序名：EX07_myshell_main.cpp
// 姓名：李佳园 学号：3190100930

#include <iostream>
#include <string>
#include <string.h>
#include <fstream>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "main.h"
#include "mycmd.h"
#include "myerr.h"
#include "process.h"

using namespace std;

string cmdline;
string arg[MAX_ARG];
int stdin_copy, stdout_copy;
int copy_argc;
string copy_argv[MAX_ARG];
struct proc_item proc_list[MAX_PROC];
int proc_num = 0;

int strSplit(string cmdline);
void proc_finish(void);

int main(int argc, char *argv[])
{
    //保存标准输入输出是文件描述符
    stdin_copy = dup(0), stdout_copy = dup(1);
    //保存命令行参数
    copy_argc = argc;
    for (int i = 0; i < argc; i++)
        copy_argv[i] = argv[i];

    //如果命令行参数个数为2  ./myshell xxx.txt
    //从文件xxx.txt读取命令
    if (argc == 2)
    {
        //以只读方式打开文件
        int cmd_file = open(argv[1], O_RDONLY);
        //若文件不存在或没有读取权限，则报错，退出程序
        if (cmd_file == -1)
        {
            cout << "\033[31mERROR: Can't open file '" << argv[1] << "'." << endl;
            return 0;
        }
        close(cmd_file);
        ifstream fin(argv[1]);
        //以行为单位，从文件中读取命令
        while (getline(fin, cmdline))
        {
            cout << "$ " << cmdline << endl;
            //分割指令，返回参数个数
            int num = strSplit(cmdline);
            //执行指令
            int res = execmd(0, num);
            //打印错误信息
            print_err(res);
            //如果是exit指令，退出循环，退出程序
            if (res == QUIT)
                return 0;
            //回收已经完成的后台进程，打印完成信息
            proc_finish();
        }
    }
    //如果命令行参数个数为2  ./myshell
    //从标准输入（控制台）读取命令
    else if (argc == 1)
    {
        //蓝色欢迎文字并闪烁
        system("clear");
        cout << "\033[5m\033[36mWelcome to myshell!\033[0m" << endl;

        char usrname[BUF], hostname[BUF], dirname[BUF];
        while (1)
        {
            //获取当前用户名
            struct passwd *pwd = getpwuid(getuid());
            strcpy(usrname, pwd->pw_name);
            //获取当前主机名
            gethostname(hostname, BUF);
            //获取当前目录路径
            getcwd(dirname, BUF);
            string rep = "\\home\\";
            rep += usrname;
            string dirname_s = dirname;
            dirname_s.replace(0, rep.length(), "~");
            //输出命令提示符
            cout << "\033[32m" << usrname << "@" << hostname << "\033[37m"
                 << ":"
                 << "\033[34m" << dirname_s << "\033[37m"
                 << "$ ";
            fflush(stdout);

            //以行为单位，从标准输入中读入命令
            getline(cin, cmdline);
            //分割指令，返回参数个数
            int num = strSplit(cmdline);
            //执行指令
            int res = execmd(0, num);
            //打印错误信息
            print_err(res);
            //如果是exit指令，退出循环，退出程序
            if (res == QUIT)
                return 0;
            //回收已经完成的后台进程，打印完成信息
            proc_finish();
        }
        return 0;
    }
    //命令行参数个数错误
    else
    {
        print_err(PARAMENT_NUM_ERR);
        return 0;
    }
}

//按空格分割指令，返回参数个数
int strSplit(string cmdline)
{
    int len = cmdline.length(), flag = 1, num = 0;
    for (int i = 0; i < len; i++)
    {
        //注释掉的是可选部分，可使管道、重定向与其他参数之间不再需要插入空格
        /*if(cmdline[i]=='|')
        {
            num++;
            arg[num-1]="|";
            flag=1;
            wordlen=0;
        }
        else if(cmdline[i]=='<')
        {
            num++;
            arg[num-1]="<";
            flag=1;
            wordlen=0;
        }
        else if(cmdline[i]=='>')
        {
            num++;
            arg[num-1]=">";
            flag=1;
            wordlen=0;
        }
        else*/
        //当前读到的字符是新参数的第一个字符
        if (cmdline[i] == ' ' || cmdline[i] == '\n')
            flag = 1;
        else
        {
            //flag=1说明当前读到的字符是新参数的第一个字符
            if (flag)
            {
                flag = 0;
                num++;
                arg[num - 1] = "";
            }
            arg[num - 1] += cmdline[i];
        }
    }
    return num;
}

//回收僵尸进程
void proc_finish(void)
{
    int i;
    for (i = 0; i < proc_num; i++)
    {
        int stat_val;
        //回收僵尸进程，若检测到进程已终止则waitpid函数会自动回收
        //注意这里使用WNOHANG，目的是为了在不阻塞的情况下检查进程状态
        if (waitpid(proc_list[i].pid, &stat_val, WNOHANG) != 0)
        {
            cout << "\033[35m[" << proc_list[i].pid << "] Finished " << proc_list[i].proc_name << endl;
            //已经终止的进程，从进程表中删除
            for (int j = i; j < proc_num - 1; j++)
            {
                proc_list[j].pid = proc_list[j + 1].pid;
                proc_list[j].isbg = proc_list[j + 1].isbg;
                proc_list[j].status = proc_list[j + 1].status;
                strcpy(proc_list[j].proc_name, proc_list[j + 1].proc_name);
            }
            proc_num--;
            i--;
        }
    }
}
