### ICMP报文格式
![](pictures/ICMP报文格式.jpg)
类型主要分为两类,**差错报文**和**查询报文**
### 以太网帧
![](pictures/以太网帧封装.png)

### arp请求报文
![](pictures/arp请求报文.jpg)

### dns报文
![](pictures/dns报文.jpg)
Linux在/etc/resolv.conf文件存放dns服务器地址

### ipv4头部
![](pictures/ipv4头部.jpg)
### icmp重定向报文
![](pictures/icmp重定向报文.jpg)
### ipv6报文
![](pictures/ipv6报文.jpg)
### tcp头部
![](pictures/tcp头部.jpg)
### service脚本程序
![](pictures/service脚本程序.jpg)
### http请求方法
![](pictures/http请求方法.jpg)
```http
//http请求格式:
GET http://www.baidu.com/index.html HTTP/1.0    //请求行
User-Agent: Wget/1.12 (linux-gnu)               //2-4为头部字段,可按任意顺序排列
Host: www.baidu.com
Connection: close
//所有头部之后,必须以一个空行,标识头部结束,只包含一个回车符'\r'和一个换行符'\n'
//空行之后为消息体,如果非空,头部需包含消息体长度字段:"Connect-Length"
```


### http状态码
![](pictures/http状态码.jpg)
```http
HTTP/1.0 200 OK                 //状态行
Server: BWS/1.0                 //2-7头部字段
Content-Length: 8024
Content-Type: text/html;charset =gbk
Set-Cookie: BAIDU=4c5a6cb6at6bda7....: FG=1; expires=Wed,04 -Jul-42 00:10:47 GMT; path=/; domain=.baidu.com
Via: 1.0 localhost (squid/3.0 STABLE)
```





### 5.1.1 主机字节序和网络字节序
根据装载的顺序不同分为大端字节序和小端字节序
大端字节序:一个整数的高位23~31bit存储在内存中的低地址处,小端字节序则反之.现代计算机通常采用小端字节序,由此格式化的数据在两台不同字节序的主机之间传输,需要更改为同一大端字节序,接收端主机根据自身字节序决定是否转换.
### 5.1.2 主机字节序和网络字节序
socket网络编程的socket地址结构体:sockaddr
```C++
#include<bits/socket.h>
struct sockaddr{
    sa_family_t sa_family;
    char sa_data[14];
};
```
```sa_family_t```是地址族类型.地址族类型通常和协议族类型对应:
![](pictures/地址族协议族1.jpg)
```PF_*和AF_*```宏定义在```bits/socket.h```中,且值完全相同,所以有时可以混用.
但是不同协议族的地址值有不同的含义和长度
![](pictures/地址族协议族2.jpg)
因此Linux定义了新通用socket地址结构体:
```C++
#include<bits/socket.h>
struct sockaddr_storage{
    sa_family_t sa_family;
    unsigned long int __ss_align;
    char __ss_padding[128-sizeof(__ss_align)];
}
```
保证有足够大空间存放ip地址并且内存对齐.()

### 5.1.3 专用socket地址
![](pictures/地址族协议族3.jpg)
![](pictures/地址族协议族4.jpg)

### 服务器程序实例

```C++
//HTTPSERVER EXAMPLE
#include<sys/socket.h>
#include<netinet.h>
#include<arpa/inet.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>

#define BUFFER_SIZE 1024

static const char* status_line[2] = {"200 OK", "500 Internet server error"};

int main(int argc, char* argv[]){
    if(argc <= 3){
        printf(" usage: %s ip_address port_number filename\n", basename(argv[0]));
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi(argv[2]);
    const char* file_name = argv[3];//target filename;

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    assert(sock != -1);

    int ret = bind(sock, (struct sockaddr*)&address, sizeof(address));
    assert(res != -1);

    ret = listen(sock, 5);
    assert(res != -1);

    struct sockaddr_in client;
    socklen_t clilen = sizeof(client);
    int connfd = accept(sock, (struct sockaddr *)&client, &clilen);
    if(connfd < 0)
        printf("errno is %d\n", errno);
    else{
        char header_buf[BUFFER_SIZE];
        memset(header_buf, '\0', BUFFER_SIZE);
        char *file_buf;
        struct stat file_stat;
        bool valid = true;
        int len = 0;
        if(stat(file_name, &file_stat) < 0)
            valid = false;
        else{
            if(S_ISDIR(file_stat.st_mode))
                valid = false;
            else if(file_stat.st_mode & S_IROTH){
                int fd = open(file_name, O_RDONLY);
                file_buf = new char[file_stat.st_size + 1];
                memset(file_buf, '\0', file_stat.st_size + 1);
                if(read(fd, file_buf, file_stat.st_size) < 0)
                    valid = false; 
            }
            else
                valid = false;
        }
        if(valid){如果文件有效
            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[0]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "Content-Length: %d\r\n", file_stat.st_size);
            len += res;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");

            struct iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = strlen(header_buf);
            iv[1].iov_base = file_buf;
            iv[1].iov_len = strlen(file_buf);
            ret = writev(connfd, iv, 2);
        }
        else{//如果文件无效,则
            ret = snprintf(header_buf, BUFFER_SIZE - 1, "%s %s\r\n", "HTTP/1.1", status_line[1]);
            len += ret;
            ret = snprintf(header_buf + len, BUFFER_SIZE - 1 - len, "%s", "\r\n");
            send(connfd, header_buf, strlen(header_buf), 0);
        }
        close(connfd);
        delete[] file_buf;

    }
    close(sock);
    return 0;
}
```
**fcntl函数**
```C++
#include<fcntl.h>
int fcntl( int fd, int cmd, ...);
```
cmd指定操作,通常设置为宏,具体不赘述,

## Linux服务器程序规范
* 一般为后台程序.称作守护进程daemon,没有控制终端,因此也没有意外输入
* 程序有日志系统,能输出日志到文件,在/var/log下通常拥有自己的日志目录.
* 一般以某个非root身份运行.比如,mysqld,httpd,syslogd,分别拥有运行账户mysql,apache,syslog,
* 可配置,服务器程序有配置文件,放在/etc目录下,例如squid的放在/etc/squid3/squid.conf
* 启动时生成一个PID文件存入/var/run目录中,例如syslogd,在/var/run/syslogd.pid.

## Linux系统日志
### 7.1 rsyslogd,(syslogd升级版)
不仅能接受进程日志,也能接收内核日志.
**用户通过调用`syslog`函数生成系统日志**,将日志输出到UNIX类型socket的文件/dev/log中,rsyslog监听该文件.
**内核日志**通过`printk`函数打印至**内核环状缓存**中,内容映射到/proc/kmsg文件中,rsyslog读取该文件获取内核日志.

**默认情况** 调试信息输出到/var/log/debug,其他消息/var/log/messages,内核/var/log/kern.log
**配置文件**/etc/rsyslog.conf 主要项:内核日志输入路径,是否接受UDP日志及其监听端口,(端口对应保存在/etc/services中),是否接受TCP日志及其监听端口,日志文件的权限,包含的子文件(/etc/rsyslog.d/*.conf),
**`syslog`函数**
```C++
#include<syslog.h>
void syslog(int priority, const char* message, ...);
```
* `priority`参数是所谓的*设施值与日志级别的按位或.设施值的默认值是LOG_USER,
日志级别: `LOG_EMERG`, `LOG_ALERT`,`LOG_CRIT`,`LOG_ERR`,`LOG_WARNING`,`LOG_NOTICE`,`LOG_INFO`,`LOG_DEBUG`;
**`void openlog( const char *ident, int logopt, int facility);`**
* ident 指定的字符串被添加到日志消息的日期和时间之后,通常被设置为程序的名字.
* logopt参数对后续的,syslog调用行为进行配置.可按下列值按位或:`LOG_PID`,`LOG_CONS`,`LOG_ODELAY`,`LOG_NDELAY`
* facility参数修改syslog中的默认设施值
* 设置日志掩码,使日志级别大于日志掩码的日志信息被忽略.: **`int setlogmask(int maskkpri);`** 该函数始终会成功,返回调用进程先前的掩码值.**`void closelog();`** 关闭日志功能.
### 7.2 UID, EUID, GID, EGID
启动身份和运行身份可能不同,身份对于**安全性来说很重要**
```C++
#include<sys/types.h>
#include<unistd.h>
uid_t getuid();    uid_t geteuid();
gid_t getgid();    gid_t getegid();
int setuid(uid_t uid);  int seteuid(uid_t uid);  
int setgid(gid_t uid);  int setegid(gid_t uid); 
```
**一个进程拥有两个id:uid和euid,**
euid方便用户资源访问.:*使得运行程序的用户拥有该程序的有效用户的权限* 比如su程序,任何用户运行之后可以获得root权限,如改账户时,su程序不得不访问**需要root权限的/etc/passwd**文件.

### 7.3 进程间关系
**1. 进程都隶属于一个进程组**
```C++
#include<unistd.h>
pid_t getpgid(pid_t pid);
```
* 进程有进程首领,其PID和PGID相同.
* `int setpgid(pit_t pid, pit_t pgid);
if pid == pgid, pid的进程为组首领, if pid == 1,当前进程PGID为pgid, if pgid == 0,则使用pid作为目标pgid,
* 进程只能设置子进程pgid,当子进程调用exec之后无法设置pgid,

**2. 会话**
```C++
pid_t setsid(void);
```
* 首领无法调用该函数.
* 非首领进程调用:
* 1. 调用进程变为**会话首领**.并为唯一成员
  2. 新建一个进程组,pgid即为该pid,
  3. 调用进程将甩开终端(如果有).
提供`pid_t getsid(pid_t pid)`来读取sid.

**3. ps命令查看进程关系**
`ps -o pid,ppid,pgid,sid,comm | less`

**4. 系统资源设置**
```C++
#include<sys/resource.h>  
int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
struct rlimit{
    rlimit_t rlim_cur;
    rlimit_t rlim_max;
};
```
rlim_cur是软限制,最好不要超越,不然系统可能会发终止信号给进程等.
rlim_max是硬限制,是上限,普通程序可以减少该值,root程序可以增大.
![](pictures/资源限制类型.jpg)


**5. 改变工作目录和根目录**
web服务器根目录一般是/var/www
```C++
#include<unistd.h>
char * getcwd( char* buf, size_t size);
int chdir( const char * path);
```
getcwd给buf指向的地方存储当前工作目录绝对路径名.如果路径名超出size返回NULL,设置errno为ERANGE,
* 如果buf为NULL,并size不为零,函数内部动态分配给buf,这个需要后期自己delete[],
* 成功后返回一个指针指向目标存储区,失败返回NULL设置errno
* chdir失败返回-1并设置errno
```C++
int chroot(const char* path);
```
不改变当前工作目录,需要再次调用chdir("/")进入逻辑根目录.

**6. 服务器后台化**
```C++
bool daemonize(){
    pid_t pid = fork();
    if(pid < 0){
        return false;
    }
    else if(pid > 0){
        eixt(0);//父进程退出
    }
    
    //子进程部分.
    /*设置文件权限码,当进程创建新文件,(open(const char* pathname, int flags, mode_t mode),文件的权限是 mode &0777*/
    umask(0);

    /*创建新会话*/
    pid_t sid = setsid();
    if(sid < 0){
        return false;
    }

    /*切换工作目录*/
    if( (chdir("/") < 0)){
        return false;
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    /*关闭其他文件描述符(主要是从父进程继承过来的)*/
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_RDWR);
    open("/dev/null", O_RDWR);
    return true;
}
```
*LInux库函数*
```C++
#include<unistd.h>
int daemon( int nochdir, int noclose);
```
nochdir 为0的话,工作目录设置为"/"
noclose 为0的话,标准输入标准输出标准错误输出都重定向到/dev/null

## 8 服务器程序框架
基本框架
![](pictures/服务器基本框架.jpg)
* I/O处理单元一般是接入服务器.实现负载均衡.
* 逻辑单元通常是进程或线程.服务器通常拥有多个逻辑单元.  
* 网络存储单元:数据库.缓存.文件等.(非必须)
* 请求队列是一个抽象概念.I/O如何通知逻辑单元,逻辑单元如何访问网络存储而不冲突就是请求队列需要解决的问题.
通常预先建立,为静态,永久的,通常被实现为池的一部分.
### I/O模型
**I/O复用通常向内核注册一组事件,内核通过I/O复用函数把其中就绪的事情通知应用程序.**
**I/O复用函数本身是阻塞的,关键在于能够监听多事件**
![](pictures/IO模型对比.jpg)
### reactor模式:
* 主线程(I/O处理单元):只负责监听文件描述上是否有事件发生,有的话就通知工作线程(逻辑处理单元)

![](pictures/reactor模式.jpg)

### proactor模式:
**所有I/O操作都交给主线程和内核执行.**
**异步I/O**
![](pictures/proactor模式1.jpg)
![](pictures/proactor模式2.jpg)
### 半同步/半异步模式
**并发中的概念**
* 同步:完全按照代码顺序执行
* 异步:执行需要**系统事件**驱动:如中断,信号等.
同步方式运行的线程称为**同步线程**,异步方式则称**异步线程**
* 异步线程复杂,难度大,效率高,实时性强.不适合高并发.
服务器通常采用**半同步/半异步**

* 半同步/半异步
同步用于逻辑处理.异步用于I/O处理.


### 领导者追随者模式
![](pictures/领导者追随者模式.jpg)
* **句柄集:**句柄Linux下是fd,句柄集管理众多句柄.使用wait_for_event来监听句柄上的I/O事件.并将就绪事件通知给领导者**线程**
* **:线程集**所有工作线程管理者.负责线程间同步/新领导者线程选定
线程三状态:
> * Leader
等待句柄集上I/O事件
> * Processing
正在处理事件,领导者接收到I/O事件之后,转移到本状态,并调用promote_new_leader选定新的领导者.
当processing状态完成如果没有领导者则它变为领导者.否则为追随者.
> * Follower
处于追随者身份.通过调用线程的join方法等待称为新的领导者.也有可能被指派任务.

### 有限状态机
**这是逻辑单元内部的一种高效方法**
有的应用层协议头部包含类型字段.**类型映射为逻辑单元的执行状态.**
* 状态之间的转移需要状态机内部驱动.
### 池
提前分配资源以便调用,省去了频繁调用内核的开销.
1. **内存池**
通常用于socket的接收缓存和发送缓存.对于**有限长度**的用户请求,预先分配一个够大的空间,如果用户请求长度超过,则动态分配.
2. **进程池和线程池**
用于并发编程.fork()和pthread_create();
3. **连接池**
用于服务器和内部机群的永久连接.
### 避免数据复制
比如服务器给客户机传输数据时,无需读取文件然后调用send,只需利用sendfile交给内核进行零拷贝发送.
进程之间需要传递大量数据时,应使用共享内存.
### 上下文切换和锁.
并发程序需要考虑的点.
* 进程或线程切换导致过多资源开销.
* 共享资源的保护.加锁通常会导致服务器效率低下
如果必须使用锁,考虑减小锁的粒度.例如读写锁.当程序都读取时,并不会增加额外开销,而有写操作时,才会锁住.

## I/O复用.
*需要使用的场合*
* 同时处理多个socket
* 同时处理用户输入和网络连接
* TCP服务器同时处理监听socket和连接socket(是I/O复用最多的场合)
* 同时处理TCP/UDP请求.
* 同时监听多个端口,处理多种服务.
### POLL模型.
```C++
#include<sy/epoll.h>
int epoll_create(int size);
//size不起作用.告诉内核需要多大.返回一个fd,
//该fd将用于其他所有epoll函数的第一个参数.
```
```C++
//从操作内核事件表
int epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
struct epoll_event{
    __unit32_t events;      /*events事件*/
    epoll_data_t data       /*用户数据*/
};
typedef union epoll_data{
    void * ptr;
    int fd;
    unit32_t u32;
    uint64_t u64;
}epoll_data_t;
```
**op的类型**
* EPOLL_CTL_ADD 往epfd上注册fd上的事件
* EPOLL_CTL_MOD 修改fd上的事件
* EPOLL_CTL_DEL 删除事件.
#### epoll_wait函数
```C++
#include<sy/epoll.h>
int epoll_wait(int epfd, struct epoll_event* events, int maxevetns, int timeout);
//成功返回就绪的描述符个数.失败-1并设置errno
```
**和poll的区别**
内核将所有就绪的事件从内核表中**复制到**events指向的数组中.(网上很多地方说不复制的,难道是错了吗)
#### LT和ET模式
**LT**模式:只要是高电平就一直触发(即epoll_wait一直向应用程序通告该事件),应用程序可以一直等到合适时机处理.
**ET**模式:只有边沿触发,也就是说第一次才会通告(只通告一次,因此效率比较高)
***注意采用ET模式的描述符应该都是非阻塞的***
### EPOLLONTSHOT事件
**缺点:**并发程序中可能对同一个fd的多次读取采用多个进程或线程.(前一个处理时,fd又有数据可读,因此唤醒有一个进/线程处理)
**保证一个fd只被一个线程处理**
```C++
//处理例子
void reset_oneshot{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}
```

***三种模型区别***
* **SELECT**
没有将描述符和对应事件绑定,只能塞入对应队列日rdset,wrset,exceptset,对应读写和异常三种事件不能处理更多事件,另外内核对fd_set进行修改,必须重置才能再次使用.
* **POLL**
将事件和描述符定义在pollfd中,而且用revents返回触发的条件,无需重置events.但是需要对N个描述符轮询(EPOLL则是回调)
* **EPOLL**
在内核中维护事件表.采用系统调用函数epoll_ctl进行添加删除和修改操作.并直接从内核中获得注册的事件,epoll_wait仅返回就绪的事件,复制到events参数对应地址中.时间复杂度O(1)
*但是*当活动连接比较多时,epoll_wait效率未必高,因为回调触发较为频繁.因此适用于多连接较少活跃的情况.
* EPOLL和POLL都能达到系统最大文件描述符65535(/proc/sys/fs/filemax),select通常有限制.
* EPOLL可以采用高效ET方式

![](pictures/三种IO复用.jpg)
### 非阻塞connect
