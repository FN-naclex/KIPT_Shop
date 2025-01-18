#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <signal.h>
#include <sys/epoll.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include "Structs.h"


int init_net();
void choice_main_S(int fd,MSG msg);
void Create_User_Table();
void ADD_User(int fd,MSG msg);
void Login_User(int fd,MSG msg);
void Create_Products_Table();
void ADD_Product(int fd, MSG msg);
void List_Products_Table(int fd,MSG msg);
void List_Products_SearchU(int fd,char *proname);
char *getTime();
void Create_ChatRecords_Table();
void ADD_Chat(int fd, MSG msg);
void List_ChatRecord_List(int fd,char *destname);
void Hub_Chat(int fd,CHAT chatmsg);
void del_LoginArr(int fd);
void  ADD_ODR(int fd,MSG msg);
void Create_Orders_Table();
void update_pro(int fd,MSG msg);
void List_Order_SearchU(int fd, char * Uname);
void List_Order_SearchM(int fd, char *Uname);
void Update_UserCM(int fd,MSG msg);
void List_User_my(int fd,MSG msg);
void CREATE_POSTS();
void ADD_POSTS(int fd,MSG msg);
void LIST_POSTS(int fd,MSG msg);
void search_post(int fd,MSG msg);
void CREATE_COMMENTS();
void LIST_COOMENTS(int fd,MSG msg);
void ADD_COMMMENT(int fd,MSG msg);
void UPDATE_COCOUNT(int fd, MSG msg);
void ADD_LIKE(int fd,MSG msg);
void update_nrcount(int fd,MSG msg);
void *pupdate_nrcount(void *p);
void cancel_pth(int fd);