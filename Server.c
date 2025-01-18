#include "Server.function.h"

struct sockaddr_in ClientAddr;

#define MAX_EVENT 3000
int main()
{
    int sID = init_net();

    struct epoll_event ev, events[MAX_EVENT];
    int epoll_id = epoll_create(MAX_EVENT);

    ev.data.fd = sID;
    ev.events = EPOLLIN;

    epoll_ctl(epoll_id, EPOLL_CTL_ADD, sID, &ev);


    while (1)
    {

        int count = epoll_wait(epoll_id, events, MAX_EVENT, -1);

        if (count == -1)
        {
            perror("epoll_wait");
            return 0;
        }
        for (size_t i = 0; i < count; i++)
        {

            int fd = events[i].data.fd;
            printf("fd =%d\n", fd);
            if (fd == sID)
            {

                printf("服务器收到客户端链接请求\n");
                socklen_t len = sizeof(ClientAddr);
                int newFd = accept(sID, (struct sockaddr *)&ClientAddr, &len);
                printf("收到一个新客户端套接字%d，正在保存\n", newFd);
                ev.data.fd = newFd;
                if (0 == epoll_ctl(epoll_id, EPOLL_CTL_ADD, newFd, &ev))
                {
                    printf("保存套接字成功\n");
                }
            }
            else
            {
                MSG msg;
                int ret = read(fd, &msg, sizeof(MSG));
                if (ret <= 0)
                {
                    ev.data.fd = fd;
                    if (0 == epoll_ctl(epoll_id, EPOLL_CTL_DEL, fd, &ev))
                    {   
                        del_LoginArr(fd);
                        printf("移除套接字成功\n");
                    }
                    else
                    {
                        
                        perror("epoll");
                        printf("移除套接字失败\n");
                    }
                    close(fd);
                    continue;
                }
                printf("收到客户端发来的消息,类型是%d\n", msg.type);
                choice_main_S(fd,msg);
                
            }
        }

        
    }
    close(sID);

    return 0;
}