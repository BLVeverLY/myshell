// 程序名：EX07_myshell_mycmd.cpp
// 姓名：李佳园 学号：3190100930

#include <iostream>
#include <string>
#include <iomanip>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>
#include <time.h>
#include <dirent.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "main.h"
#include "mycmd.h"
#include "myerr.h"
#include "process.h"
#include <string.h>

using namespace std;

extern string arg[MAX_ARG];
extern struct proc_item proc_list[MAX_PROC];
extern int proc_num;
extern int copy_argc;
extern string copy_argv[MAX_ARG];
extern int stdin_copy;

//改变当前目录
int mycd(int l, int r)
{
    //切换到主目录
    if (r - l == 1)
    {
        if (chdir(getenv("HOME")))
        {
            cout << "\033[31mERROR: Can't get HOME path." << endl;
            return OTHERS;
        }
        return EXIT_OK;
    }
    //切换到对应目录
    else if (r - l == 2)
    {
        if (chdir(arg[l + 1].c_str()))
        {
            cout << "\033[31mERROR: Can't get " << arg[l + 1] << " path." << endl;
            return OTHERS;
        }
        return EXIT_OK;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
}

//清屏
int myclr(int l, int r)
{
    if (r - l == 1)
    {
        cout << "\033c";
        cout << "\033[5m\033[36mWecome to myshell!\033[0m" << endl;
        return EXIT_OK;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
}

//退出当前进程
int myexit(int l, int r)
{
    if (r - l == 1)
        return QUIT;
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
}

//将一个被暂停的命令，变成再后台继续执行
int mybg(int l, int r)
{
    if (r - l == 2)
    {
        //将用户输入的作业号转换成int类型
        int id = atoi(arg[l + 1].c_str()) - 1;
        //进程表里有此进程号
        if (id >= 0 && id < proc_num)
        {
            //对于停止的进程，先更新进程表，然后向其发送SITCONT信号令其继续运行
            if (proc_list[id].pid && proc_list[id].status == STOPPED)
            {
                proc_list[id].status = RUNNING;
                proc_list[id].isbg = 1; //后台运行
                //向该进程发送SIGCONT信号令其继续执行
                kill(proc_list[id].pid, SIGCONT);
                //输出进程信息
                cout << "\033[35m[" << proc_list[id].pid << "] Running " << proc_list[id].proc_name << endl;
            }
            //此进程没有被暂停
            else
            {
                cout << "\033[31mERROR: The process with id " << arg[l + 1] << " not exists or is already running." << endl;
                return OTHERS;
            }
        }
        //进程表里没有此进程号
        else
        {
            cout << "\033[31mERROR: Wrong process id." << endl;
            return OTHERS;
        }
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//将后台运行的或挂起的任务切换到前台运行
int myfg(int l, int r)
{
    if (r - l == 2)
    {
        //将用户输入的作业号转换成int类型
        int id = atoi(arg[l + 1].c_str()) - 1;
        //进程表里有此进程号
        if (id >= 0 && id < proc_num)
        {
            //如果此进程在后台
            if (proc_list[id].pid && proc_list[id].isbg)
            {
                //如果此进程被挂起
                if (proc_list[id].status == STOPPED)
                {
                    //向该进程发送SIGCONT信号令其继续执行
                    kill(proc_list[id].pid, SIGCONT);
                    proc_list[id].status = RUNNING;
                }
                //进程表信息改成前台运行
                proc_list[id].isbg = 0;
                //输出进程信息
                cout << "\033[35m[" << proc_list[id].pid << "] " << proc_list[id].proc_name << endl;
            }
            //注册信号函数，用于捕捉Ctrl+Z输入
            signal(SIGTSTP, CtrlZHandler);
            int status;
            //前台进程，父进程阻塞
            waitpid(proc_list[id].pid, &status, WUNTRACED);
            //恢复信号函数
            signal(SIGTSTP, SIG_DFL);
            //子进程正常退出（指正常通过exit()函数退出，或return 0退出）
            if (WIFEXITED(status))
            {
                //获取错误代码
                int res_son = WEXITSTATUS(status);
                print_err(res_son);
                return EXIT_OK;
            }
            //子进程停止（指子进程在Ctrl+Z作用下停止）
            else if (WIFSTOPPED(status))
            {
                cout << "\033[35m[" << id + 1 << "]" << proc_list[id].pid << " Stopped " << proc_list[id].proc_name << endl;
                return EXIT_OK;
            }
            else
                return SUBPROCESS_FAILURE;
        }
        //进程表里没有此进程号
        else
        {
            cout << "\033[31mERROR: Wrong process id." << endl;
            return OTHERS;
        }
    }
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//输出环境变量
int myset(int l, int r)
{
    if (r - l == 1)
    {
        char **env = environ;
        while (*env)
        {
            cout << *env << endl;
            env++;
        }
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//删除环境变量
int myunset(int l, int r)
{
    if (r - l == 2)
    {
        if (unsetenv(arg[l + 1].c_str()) == -1)
        {
            //此环境变量不存在不会报错
            cout << "\033[31mERROR: Fail to unset the environmental variable." << endl;
            return OTHERS;
        }
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//打印掩码或修改掩码
int myumask(int l, int r)
{
    //无参数输出旧掩码
    if (r - l == 1)
    {
        //随便设置一个新掩码，会返回旧掩码
        mode_t mask = umask(0000);
        //恢复旧掩码
        umask(mask);
        cout << setw(4) << setfill('0') << mask << endl;
    }
    //有参数设置新掩码
    else if (r - l == 2)
    {
        umask(atoi(arg[l + 1].c_str()));
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//将当前进程替换为exec后的指令
int myexec(int l, int r)
{
    if (r - l > 1)
    {
        int res = execmd(l + 1, r);
        print_err(res);
        //退出进程
        return QUIT;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
}

//打印当前目录
int mypwd(int l, int r)
{
    if (r - l == 1)
    {
        char dir[BUF];
        getcwd(dir, BUF);
        cout << dir << endl;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//打印当前时间
int mytime(int l, int r)
{
    if (r - l == 1)
    {
        time_t *tstmp = new time_t;
        time(tstmp);
        //将时间转换为可输出的字符串模式
        char *t = ctime(tstmp);
        cout << t;
        fflush(stdout);
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//输出指定目录或当前目录下的所有文件或目录
int mydir(int l, int r)
{
    //输出当前目录下的所有文件
    if (r - l == 1)
    {
        char dir[BUF];
        //获取当前目录路径
        getcwd(dir, BUF);
        //打开当前目录
        DIR *ds = opendir(dir);
        //遍历所有文件
        struct dirent *entry;
        while ((entry = readdir(ds)) != NULL)
            //不打印隐藏文件或目录
            if (entry->d_name[0] != '.')
                cout << entry->d_name << endl;
        //关闭目录
        closedir(ds);
    }
    //输出指定目录下的所有文件
    else if (r - l == 2)
    {
        //打开指定目录
        DIR *ds = opendir(arg[l + 1].c_str());
        //无法打开
        if (ds == NULL)
        {
            cout << "\033[31mERROR: Can't get " << arg[l + 1] << " path." << endl;
            return OTHERS;
        }
        //遍历所有文件
        struct dirent *entry;
        while ((entry = readdir(ds)) != NULL)
            //不打印隐藏文件或目录
            if (entry->d_name[0] != '.')
                cout << entry->d_name << endl;
        //关闭目录
        closedir(ds);
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//打印
int myecho(int l, int r)
{
    for (int i = l + 1; i < r; i++)
    {
        //$*或$@，打印所有命令行参数值
        if (arg[i] == "$*" || arg[i] == "$@")
        {
            for (int j = 0; j < copy_argc; j++)
                cout << copy_argv[j];
        }
        else if (arg[i][0] == '$')
        {
            //$0或$1...，打印对应的命令行参数值
            for (int j = 0; j < copy_argc; j++)
                if (arg[i] == "$" + to_string(j))
                    cout << copy_argv[j];
            //$var，打印对应的参数值
            char *var = getenv(arg[i].substr(1, arg[i].size() - 1).c_str());
            if (var != NULL)
                cout << var;
        }
        //非参数或非变量的打印
        else
            cout << arg[i];
        if (i != r - 1)
            cout << " ";
    }
    cout << endl;
    return EXIT_OK;
}

//输出帮助信息
int myhelp(int l, int r)
{
    //只有一个help，输出所有自己写的命令的帮助信息
    if (r - l == 1)
    {
        cout << "\033\33m$var fot the value of variable var; [] for ommissible parameter; {} for ome of them." << endl;
        cout << "cd\t -- Change directory\nUsage: cd [$path]" << endl;
        cout << "pwd\t -- Print working directory\nUsage: pwd" << endl;
        cout << "bg\t -- Turn a process to be executed background\nUsage: bg <id> #<id> can be derived from 'jobs' command" << endl;
        cout << "clr\t -- Clear the screen\nUsage: clr" << endl;
        cout << "time\t -- Print the current time\nUsage: time" << endl;
        cout << "dir\t -- List the files under a given directory\nUsage dir [$path]" << endl;
        cout << "echo\t -- Print the strings or the values of parameters\nUsage: echo [para1] [para2] ... (or) echo $HOME ... (or) echo $0 $@ ..." << endl;
        cout << "exit\t -- Exit the shell\nUsage: exit" << endl;
        cout << "jobs\t -- Print all subprocesses\nUsage: jobs" << endl;
        cout << "fg\t -- Turn a process to be executed foreground\nUsage: bg <id> #<id> can be derived from 'jobs' command" << endl;
        cout << "set\t -- Print all environment variables\nUsage: set" << endl;
        cout << "unset\t -- Delete the given environment variable\nUsage: unset $env_name" << endl;
        cout << "umask\t -- Print the umask or set the umask\nUsage1: umask\nUsage2: umask [$new_val]" << endl;
        cout << "test\t -- Compare the strings\nUsage: test $str1 {'=','!=','<','>'} $str2" << endl;
        cout << "shift\t -- Shift parameters to left\nUsage: shift [$val] (default: $val = 1)" << endl;
        cout << "exec\t -- Replace the current process with a given command\nUsage: exec $command" << endl;
        cout << "more\t -- Filter the output\nUsage: more [$path] (or) $cmd ... | more" << endl;
        cout << "help\t -- Print help information\nUsage: help [$command]" << endl;
    }
    //help cmd，输出对应的命令的帮助信息
    else if (r - l == 2)
    {
        if (arg[l + 1] == "cd")
            cout << "cd\t -- Change directory\nUsage: cd [$path]" << endl;
        else if (arg[l + 1] == "pwd")
            cout << "pwd\t -- Print working directory\nUsage: pwd" << endl;
        else if (arg[l + 1] == "bg")
            cout << "bg\t -- Turn a process to be executed background\nUsage: bg <id> #<id> can be derived from 'jobs' command" << endl;
        else if (arg[l + 1] == "clr")
            cout << "clr -- Clear the screen\nUsage: clr" << endl;
        else if (arg[l + 1] == "time")
            cout << "time\t -- Print the current time\nUsage: time" << endl;
        else if (arg[l + 1] == "dir")
            cout << "dir\t -- List the files under a given directory\nUsage: dir [$path]" << endl;
        else if (arg[l + 1] == "echo")
            cout << "echo\t -- Print the strings or the values of parameters\nUsage: echo [para1] [para2] ... (or) echo $HOME ... (or) echo $0 $@ ..." << endl;
        else if (arg[l + 1] == "exit")
            cout << "exit\t -- Exit the shell\nUsage: exit" << endl;
        else if (arg[l + 1] == "jobs")
            cout << "jobs\t -- Print all subprocesses\nUsage: jobs" << endl;
        else if (arg[l + 1] == "fg")
            cout << "fg\t -- Turn a process to be executed foreground\nUsage: bg <id> #<id> can be derived from 'jobs' command" << endl;
        else if (arg[l + 1] == "set")
            cout << "set\t -- Print all environment variables\nUsage: set" << endl;
        else if (arg[l + 1] == "unset")
            cout << "unset\t -- Delete the given environment variable\nUsage: unset $env_name" << endl;
        else if (arg[l + 1] == "umask")
            cout << "umask\t -- Print the umask or set the umask\nUsage1: umask\nUsage2: umask [$new_val]" << endl;
        else if (arg[l + 1] == "test")
            cout << "test\t -- Compare the strings\nUsage: test $str1 {'=','!=','<','>'} $str2" << endl;
        else if (arg[l + 1] == "shift")
            cout << "shift\t -- Shift parameters to left\nUsage: shift [$val] (default: $val = 1)" << endl;
        else if (arg[l + 1] == "exec")
            cout << "exec\t -- Replace the current process with a given command\nUsage: exec $command" << endl;
        else if (arg[l + 1] == "more")
            cout << "more\t -- Filter the output\nUsage: more [$path] (or) $cmd ... | more" << endl;
        else if (arg[l + 1] == "help")
            cout << "help\t -- Print help information\nUsage: help [$command]" << endl;
        else
            cout << "Not the command I coded." << endl;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//打印当前进程下的所有作业（子进程）的信息
int myjobs(int l, int r)
{
    if (r - l == 1)
    {
        for (int i = 0; i < proc_num; i++)
            cout << "[" << i + 1 << "] [" << proc_list[i].pid << "] "
                 << ((proc_list[i].status == RUNNING) ? " Running " : " Stopped ")
                 << proc_list[i].proc_name << endl;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//将命令行参数（也就是argv[]）左移
int myshift(int l, int r)
{
    //左移输入的位数
    if (r - l == 2)
    {
        int num = atoi(arg[l + 1].c_str());
        for (int i = num; i < copy_argc; i++)
            copy_argv[i - num] = copy_argv[i];
        copy_argc -= num;
    }
    //缺省值为1
    else if (r - l == 1)
    {
        for (int i = 1; i < copy_argc; i++)
            copy_argv[i - 1] = copy_argv[i];
        copy_argc -= 1;
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//用于字符串比较
int mytest(int l, int r)
{
    if (r - l == 4)
    {
        if (arg[l + 2] == "=")
        {
            cout << ((arg[l + 1] == arg[l + 3]) ? "True" : "False") << endl;
        }
        else if (arg[l + 2] == "!=")
        {
            cout << ((arg[l + 1] != arg[l + 3]) ? "True" : "False") << endl;
        }
        else if (arg[l + 2] == "<")
        {
            cout << ((arg[l + 1] < arg[l + 3]) ? "True" : "False") << endl;
        }
        else if (arg[l + 2] == ">")
        {
            cout << ((arg[l + 1] > arg[l + 3]) ? "True" : "False") << endl;
        }
        //比较符输入错误
        else
        {
            cout << "\033[31mERROR: Wrong comparison." << endl;
            return OTHERS;
        }
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}

//过滤输入文本，每次显示一整个屏幕的内容
int mymore(int l, int r)
{
    //有参数，从文件中读入
    if (r - l == 2)
    {
        //以只读方式打开文件
        FILE *fp = fopen(arg[l + 1].c_str(), "r");
        //文件不存在或无法打开
        if (fp == NULL)
        {
            cout << "\033[31mERROR: Can't open file " << arg[l + 1] << "." << endl;
            return OTHERS;
        }
        char line[BUF];
        //获取文件总行数
        int all_line = 0;
        while (fgets(line, BUF, fp))
            all_line++;
        //上一页输出的行数
        int last_num_line = 0;
        //目前正准备输出的行数
        int num_line = 1;
        //回到文件起始位置
        fseek(fp, 0, SEEK_SET);
        //定义winsize结构体变量，用于获取控制台高度
        struct winsize size;
        //一行行读取文件内容
        while (fgets(line, BUF, fp))
        {
            //每行前面会有行号
            cout << num_line << " ";
            fputs(line, stdout);
            num_line++;
            //TIOCSWINSZ命令可以将此结构的新值存放到内核中，用于获取控制台高度
            ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
            if (num_line - last_num_line == size.ws_row)
            {
                //输出--More--(百分比%)
                cout << num_line << " \033[35m--More--(" << (double)num_line / all_line * 100 << "%)\033[0m";
                fflush(stdout);
                //达到输入不回显且输入不用回车的非缓存输入
                static struct termios oldt, newt;
                tcgetattr(STDIN_FILENO, &oldt);
                newt = oldt;
                newt.c_lflag &= ~(ICANON);
                tcsetattr(STDIN_FILENO, TCSANOW, &newt);
                system("stty -echo");
                //等待命令输入
                int in;
                while ((in = getchar()) != EOF)
                {
                    if (in == 'q' || in == ' ' || in == '\n')
                        break;
                }
                //恢复输入回显且需要回车来清除缓存
                system("stty echo");
                tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
                //输入q，退出
                if (in == 'q')
                {
                    //覆盖
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    break;
                }
                //输入回车，翻页
                else if (in == '\n')
                {
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    last_num_line = num_line;
                }
                //输入空格，再输出一行
                else if (in == ' ')
                {
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    last_num_line++;
                }
            }
        }
        fclose(fp);
    }
    //无参数，用于管道，从重定向后的标准输入读（通常是管道的一端）
    //此时用户输入还是输入到屏幕中的，所以在读用户输入的时候，要根据备份的stdin文件描述符新建文件流，从中读取用户输入
    else if (r - l == 1)
    {
        //调用fdopen，使用stdin的备份文件描述符打开文件流
        FILE *fp = fdopen(stdin_copy, "r");
        //r若文件流打开失败
        if (fp == NULL)
        {
            cout << "\033[31mERROR: Can't read from stdin." << endl;
            return OTHERS;
        }
        char **line = (char **)malloc(BUF * sizeof(char *));
        for (int i = 0; i < BUF; i++)
            line[i] = (char *)malloc(BUF);
        //获取总行数和所有行的内容
        int all_line = 0;
        while (fgets(line[all_line], BUF, stdin))
            all_line++;
        //上一页输出的行数
        int last_num_line = 0;
        //目前正准备输出的行数
        int num_line = 1;
        //一行行读取文件内容
        while (num_line < all_line)
        {
            //每行前面会有行号
            cout << num_line << " ";
            fputs(line[num_line], stdout);
            num_line++;
            if (num_line - last_num_line == 22)
            {
                //输出--More--(百分比%)
                cout << num_line << " \033[35m--More--(" << (double)num_line / all_line * 100 << "%)\033[0m";
                fflush(stdout);
                //等待命令输入
                int in;
                while ((in = getc(fp)) != EOF)
                {
                    if (in == 'q' || in == ' ' || in == '\n')
                    {
                        if (in != '\n') //除回车外，要再额外读一个用户输入的回车符号
                            getc(fp);
                        break;
                    }
                }
                //输入q，退出
                if (in == 'q')
                {
                    //覆盖
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    break;
                }
                //输入回车，翻页
                else if (in == '\n')
                {
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    last_num_line = num_line;
                }
                //输入空格，再输出一行
                else if (in == ' ')
                {
                    cout << "\r"
                         << "                       "
                         << "\r";
                    fflush(stdout);
                    last_num_line++;
                }
            }
        }
        fclose(fp);
        for (int i = 0; i < BUF; i++)
            free(line[i]);
        free(line);
    }
    //参数数量错误
    else
        return PARAMENT_NUM_ERR;
    return EXIT_OK;
}
