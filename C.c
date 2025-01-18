#include "Client_function.h"
pthread_mutex_t sID_mutex;
char CurrentUserAC[128];
char OtherAc[128];
int sID1;
void *recvchathub(void *p)
{
     MSG msg;
     setbuf(stdout, NULL);
     pthread_mutex_lock(&sID_mutex);

     int list_ing_displayed = 0;       // 标记是否已显示list_ing相关内容
     int query_finished_displayed = 0; // 标记是否已显示查询完毕内容

     while (1)
     {
          int new_message_received = 0;
          while (read(sID1, &msg, sizeof(MSG)) > 0)
          {
               new_message_received = 1;

               // 根据不同情况决定是否清屏
               if (!list_ing_displayed && !query_finished_displayed)
               {
                    printf("\033[2J\033[1;1H");
               }

               // printf("线程收到消息\n");
               // printf("接收到的msg.type的值为: %d\n", msg.type);
               if (msg.type == list_ing)
               {
                    list_ing_displayed = 1;
                    query_finished_displayed = 0;
                    // printf("111\n");
                    CHAT *m = (CHAT *)msg.data;
                    printf("--------------------------------\n");
                    if (strcmp(m->dest_name, CurrentUserAC) == 0)
                    {
                         printf("对方(%s)%s \n%s %s\n", m->self_name, m->send_time, m->text, m->read);
                         strcpy(OtherAc, m->self_name);
                    }
                    else
                    {
                         printf("自己(%s)%s \n%s %s\n", m->self_name, m->send_time, m->text, m->read);
                    }
                    printf("--------------------------------\n");
               }
               else if (msg.type == 1)
               { // 假设yes用1表示，这里需要根据实际定义修改
                    list_ing_displayed = 0;
                    query_finished_displayed = 1;
                    // printf("121\n");
                    // printf("查询完毕\n");
               }
               else if (msg.type == 0)
               { // 假设no用0表示，这里需要根据实际定义修改
                    list_ing_displayed = 0;
                    query_finished_displayed = 0;
                    printf("131\n");
                    printf("--------------------------------\n");
                    printf("         暂无联系人发来消息\n");
                    printf("--------------------------------\n");
               }
               pthread_testcancel();
          }

          if (!new_message_received)
          {
               continue;
          }

          // 清屏并准备下一次显示更新，但根据标志判断避免误清屏
          if (!list_ing_displayed && !query_finished_displayed)
          {
               printf("\033[2J\033[1;1H");
          }
     }
     pthread_mutex_unlock(&sID_mutex);
     printf("发送:");
     fflush(stdout);
     // printf("线程结束\n");
     return NULL;
}

void *recvodhub(void *p)
{
     MSG msg;
     int is = 0;
     is = *(int *)p; // 0是初始 1是顾客 2是商家
     system("clear");

     pthread_mutex_lock(&sID_mutex);

     while (read(sID1, &msg, sizeof(MSG)) > 0)
     {
          // printf("订单线程收到消息\n");

          if (msg.type == list_ing)
          {
               ODR *od = (ODR *)msg.data;
               printf("--------------------------------\n");
               if (is == 1)
               {
                    printf("商品名：%s  商家名：%s 购买数量：%d 支付金额:%.2lf  状态：%s 订单号%s\n", od->product_Name, od->M_name, od->Quantity, od->Tol_Price, od->State, od->num);
               }
               else if (is == 2)
               {
                    printf("商品名：%s  买家名：%s 购买数量：%d 支付金额:%.2lf  状态：%s 订单号%s\n", od->product_Name, od->C_name, od->Quantity, od->Tol_Price, od->State, od->num);
               }
               printf("--------------------------------\n");
          }
          else if (msg.type == kong)
          {

               printf("--------------------------------\n");
               printf("---------暂无订单-----------\n");
               printf("--------------------------------\n");
          }
          pthread_testcancel();
     }
     pthread_mutex_unlock(&sID_mutex);
     printf("发送:");
     fflush(stdout);
     printf("线程结束\n");
}

// recvptmain

void *recvptmain(void *p)
{
     MSG msg;
     int is = 0;
     // is = *(int *)p; // 0是初始 1是顾客 2是商家
     system("clear");

     pthread_mutex_lock(&sID_mutex);

     while (read(sID1, &msg, sizeof(MSG)) > 0)
     {
          // printf("贴吧线程收到消息\n");

          if (msg.type == list_ing)
          {

               POST *po = (POST *)msg.data;
               setlocale(LC_ALL, "");
               wchar_t w_result[20];
               wchar_t *w_content = (wchar_t *)malloc((mbstowcs(NULL, po->content, 0) + 1) * sizeof(wchar_t));
               mbstowcs(w_content, po->content, strlen(po->content));

               if (wcslen(w_content) > 16)
               {
                    wcsncpy(w_result, w_content, 16);
                    w_result[16] = L'.';
                    w_result[17] = L'.';
                    w_result[18] = L'.';
                    w_result[19] = L'\0';
               }
               else
               {
                    wcscpy(w_result, w_content);
               }

               printf("--------------------------------\n");
               printf("帖子类型：%s 编号：%d\n标题：%s\n内容：%ls \n点赞数：%d   评论数:%d\n", po->Type, po->post_id, po->title, w_result, po->likes, po->comment_count);
               printf("--------------------------------\n");

               free(w_content);
          }
          else if (msg.type == kong)
          {
               printf("--------------------------------\n");
               printf("------------暂无帖子-----------\n");
               printf("--------------------------------\n");
          }
          pthread_testcancel();
     }
     pthread_mutex_unlock(&sID_mutex);

     printf("请输入要进入的帖子ID\n");
     fflush(stdout);
     // printf("线程结束\n");
}

// recvcomment

void *recvcomment(void *p)
{
     MSG msg;
     // system("clear");
     pthread_mutex_lock(&sID_mutex);

     while (read(sID1, &msg, sizeof(MSG)) > 0)
     {
          // printf("评论线程收到消息\n");

          if (msg.type == list_ing)
          {

               COMMENT *co = (COMMENT *)msg.data;

               printf("--------------------------------\n");

               printf("第%d楼   评论者：%s\n内容:%s \n 发送时间：%s \n", co->comment_id, co->author_ac, co->connent, co->comment_time);

               printf("--------------------------------\n");
          }
          else if (msg.type == kong)
          {
               printf("--------------------------------\n");

               printf("     暂无评论           \n");
               printf("--------------------------------\n");
          }
          else if (msg.type == yes)
          {
               // printf("--------------------------------\n");

               // printf("     查询完毕           \n");
               // printf("--------------------------------\n");
               break;
          }
          else if (msg.type == no)
          {
               // printf("--------------------------------\n");

               // printf("     查询失败           \n");
               // printf("--------------------------------\n");
               break;
          }
          pthread_testcancel();
     }

     pthread_mutex_unlock(&sID_mutex);
     printf("请选择你的操作 1.发评论 2.联系楼主 3.点赞 4.退出\n");
     // printf("线程结束\n");
}

// recvnrcount
// CCC

void *recvnrcount(void *p)
{
     MSG msg;
     // 设置标准输出无缓冲，便于及时输出内容
     setbuf(stdout, NULL);
     pthread_mutex_lock(&sID_mutex);
     int count = 0;

     while (1)
     { // 外层循环持续运行来不断接收消息
          // 用于标记是否接收到新消息并需要更新显示内容
          int new_message_received = 0;
          while (read(sID1, &msg, sizeof(MSG)) > 0)
          {
               new_message_received = 1;
               printf("\033[2J\033[1;1H"); // 清屏并将光标定位到左上角
                                           //   printf("检查未读线程收到消息\n");

               if (msg.type == list_ing)
               {
                    count = atoi(msg.data);

                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-22s |\n", "功能", "         详情");
                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-24s |\n", "1.商城", "       购买商品");
                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-26s |\n", "2.订单", "     已购商品信息");
                    char nr[40];

                    printf("-------------------------------------------\n");
                    sprintf(nr, "     未读消息：%d", count);

                    printf("|%-20s |%-25s |\n", "3.联系人", nr);
                    printf("-------------------------------------------\n");
                    printf("|%-22s |%-28s |\n", "4.用户资料", "   设置当前用户信息");
                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-25s |\n", "5.退出", "      返回主界面");
                    printf("-------------------------------------------\n");
               }
               pthread_testcancel();
          }

          if (!new_message_received)
          {
               // 如果没有新消息，暂停5秒等待服务器下一次发送消息
               sleep(5);
               continue; // 直接进入下一次循环检查是否有新消息
          }

          // 这里可以添加代码根据用户输入等进行相应操作，当前示例暂未详细体现这部分逻辑

          // 清屏并准备下一次显示更新
          printf("\033[2J\033[1;1H");
     }

     pthread_mutex_unlock(&sID_mutex);
     // printf("线程结束\n");
     return NULL;
}
void *recvnrcountM(void *p)
{
     MSG msg;
     setbuf(stdout, NULL); // 设置标准输出无缓冲，便于及时输出内容
     pthread_mutex_lock(&sID_mutex);
     int count = 0;
     int first_display = 1; // 标记是否是第一次显示内容，用于控制清屏操作

     while (1)
     {
          int new_message_received = 0;
          while (read(sID1, &msg, sizeof(MSG)) > 0)
          {
               new_message_received = 1;
               if (first_display)
               {
                    printf("\033[2J\033[1;1H"); // 清屏并将光标定位到左上角，仅在第一次显示时清屏
                    first_display = 0;
               }
               else
               {
                    // 使用转义序列将光标定位到左上角，覆盖之前的内容显示新内容，避免清屏闪烁
                    printf("\033[1;1H");
               }
               // printf("检查未读线程收到消息\n");

               if (msg.type == list_ing)
               {
                    count = atoi(msg.data);
                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-22s |\n", "功能", "         详情");
                    printf("-------------------------------------------\n");
                    printf("|%-22s |%-24s |\n", "1.商品管理", "       发布与修改");
                    printf("-------------------------------------------\n");
                    printf("|%-22s |%-26s |\n", "2.订单管理", "     已售商品信息");
                    char nr[40];
                    printf("-------------------------------------------\n");
                    sprintf(nr, "     未读消息：%d", count);
                    printf("|%-21s |%-25s |\n", "3.联系人", nr);
                    printf("-------------------------------------------\n");
                    printf("|%-22s |%-25s |\n", "4.交流论坛", "      易物论购区 ");
                    printf("-------------------------------------------\n");
                    printf("|%-20s |%-25s |\n", "5.退出", "      返回主界面");
                    printf("-------------------------------------------\n");
               }
               pthread_testcancel();
          }

          if (!new_message_received)
          {
               // 如果没有新消息，这里可以根据实际需求添加等待逻辑，比如暂停一段时间再检查新消息
               // 例如可以使用sleep函数暂停几秒，像sleep(1);表示暂停1秒后再继续检查新消息
               continue;
          }

          // 这里可以添加代码用于处理用户输入等交互操作，如果有需要的话，当前示例暂未体现这部分逻辑

          // 每次消息处理完，准备下一次显示更新，将光标定位到左上角
          printf("\033[1;1H");
     }
     pthread_mutex_unlock(&sID_mutex);
     // printf("线程结束\n");
     return NULL;
}
int init_net()
{
     struct sockaddr_in sin;
     sin.sin_family = AF_INET;   // IPV4的网络地址版本
     sin.sin_port = htons(9002); // 把9000号端口转为网络字节序并赋值给端口参数
     pthread_mutex_init(&sID_mutex, NULL);
     u_int32_t ip = 0;
     // 把字符串ip地址转为网络字节序ip
     inet_pton(AF_INET, "192.168.17.129", &ip);
     sin.sin_addr.s_addr = ip;

     // 创建一个tcp套接字，参数1指定地址族，参数2指定协议，SOCK_STREAM代表是TCP，SOCK_DGRAM代表UDP
     int sID = socket(AF_INET, SOCK_STREAM, 0);

     while (-1 == connect(sID, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)))
     {
          perror("连接服务器中...");
          sleep(1);
     }

     printf("连接服务器成功!\n");
     return sID;
}

void choice_main_C(int sID, char *buf)
{
     USER ac;
     MSG msg;
     if (strcmp(buf, "1") == 0)
     {
          system("clear");
          // printf("XX购物平台-登录界面\n");
          // printf("请输入账号\n");
          // scanf("%s", ac.account);
          // printf("请输入密码\n");
          // scanf("%s", ac.password);

          printf("-------------------------------------------\n");
          printf("|            XX购物平台 - 登录            |\n");
          printf("-------------------------------------------\n");
          printf("%-17s", "        账号：");
          scanf("%s", ac.account);
          printf("%-17s", "        密码：");
          scanf("%s", ac.password);

          msg.type = login;
          memcpy(msg.data, &ac, sizeof(USER));

          int ret = send(sID, &msg, sizeof(MSG), 0);
          if (ret == -1)
          {
               perror("发送登录消息失败");
               sleep(1);
          }
          pthread_mutex_lock(&sID_mutex);
          ret = read(sID, &msg, sizeof(MSG));
          pthread_mutex_unlock(&sID_mutex);
          if (ret < 0)
          {
               printf("接收服务器消息失败\n");
               sleep(1);
          }
          else
          {

               strcpy(CurrentUserAC, ac.account);

               if (msg.type == go_main_C)
               {
                    printf("登录成功，进入顾客主页\n");
                    // sleep(1);
                    while (1)
                    {
                         system("clear");
                         printf("用户首页\n");
                         msg.type = update_nr;
                         strcpy(msg.data, CurrentUserAC);
                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_t pt5; // BBB
                         sID1 = sID;
                         pthread_create(&pt5, NULL, &recvnrcount, NULL);

                         // printf("1.商城 2.订单 3.私聊 （未读消息：%d) 4.用户资料 5.退出 \n");
                         scanf("%s", buf);
                         if (strcmp(buf, "5") == 0)
                         {

                              msg.type = log_off;
                              send(sID, &msg, sizeof(MSG), 0);
                              msg.type = cancel_p;
                              send(sID, &msg, sizeof(MSG), 0);
                              pthread_cancel(pt5);
                              pthread_join(pt5, NULL);
                              pthread_mutex_unlock(&sID_mutex);
                              break;
                         }
                         msg.type = cancel_p;
                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_cancel(pt5);
                         pthread_join(pt5, NULL);
                         pthread_mutex_unlock(&sID_mutex);
                         choice_main_U(sID, buf);
                    }
               }
               else if (msg.type == go_main_M)
               {
                    printf("登录成功，进入商家主页\n");
                    // sleep(1);
                    // while (1)
                    // {
                    //      system("clear");
                    //      printf("商家首页\n");
                    //      printf("1.商品 2.订单 3.私聊  4.广场 5.退出 \n");
                    //      scanf("%s", buf);
                    //      if (strcmp(buf, "quit") == 0 || strcmp(buf, "5") == 0)
                    //      {
                    //           msg.type = log_off;
                    //           send(sID, &msg, sizeof(MSG), 0);
                    //           break;
                    //      }
                    //      choice_main_M(sID, buf);
                    // }
                    while (1)
                    {
                         system("clear");
                         printf("商家首页\n");
                         msg.type = update_nr;
                         strcpy(msg.data, CurrentUserAC);
                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_t pt5; // BBB
                         sID1 = sID;
                         pthread_create(&pt5, NULL, &recvnrcountM, NULL);

                         // printf("1.商城 2.订单 3.私聊 （未读消息：%d) 4.用户资料 5.退出 \n");
                         scanf("%s", buf);
                         if (strcmp(buf, "5") == 0)
                         {

                              msg.type = log_off;
                              send(sID, &msg, sizeof(MSG), 0);
                              msg.type = cancel_p;
                              send(sID, &msg, sizeof(MSG), 0);
                              pthread_cancel(pt5);
                              pthread_join(pt5, NULL);
                              pthread_mutex_unlock(&sID_mutex);
                              break;
                         }
                         msg.type = cancel_p;
                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_cancel(pt5);
                         pthread_join(pt5, NULL);
                         pthread_mutex_unlock(&sID_mutex);
                         choice_main_M(sID, buf);
                    }
               }
               else if (msg.type == no)
               {
                    printf("登录失败，账号不存在或者密码错误\n");
                    sleep(1);
               }
          }
     }
     else if (strcmp(buf, "2") == 0)
     {

          system("clear");
          printf("XX购物平台-注册界面\n");
          printf("请输入账号\n");
          scanf("%s", ac.account);
          printf("请输入密码\n");
          scanf("%s", ac.password);
          while (1)
          {
               printf("请输入注册的身份(顾客/商家)\n");
               scanf("%s", ac.Identity);
               if (strcmp(ac.Identity, "顾客") == 0 || strcmp(ac.Identity, "商家") == 0)
               {
                    break;
               }
               else
                    printf("输入的身份有误!!\n");
          }

          msg.type = regist;
          strcpy(ac.Nick, "无");
          strcpy(ac.Address, "无");
          memcpy(msg.data, &ac, sizeof(USER));

          int ret = send(sID, &msg, sizeof(MSG), 0);
          if (ret == -1)
          {
               perror("发送注册消息失败");
               sleep(1);
          }

          pthread_mutex_lock(&sID_mutex);
          ret = read(sID, &msg, sizeof(MSG));
          pthread_mutex_unlock(&sID_mutex);
          if (ret < 0)
          {
               printf("接收服务器消息失败\n");
               sleep(1);
          }
          else
          {
               if (msg.type == yes)
               {
                    printf("注册成功\n");
                    // sleep(1);
               }
               else if (msg.type == no)
               {
                    printf("注册失败,用户名可能已被占用\n");
                    sleep(1);
               }
          }
     }
     else if (strcmp(buf, "3") == 0)
     {
          printf("正在退出\n");
          exit(0);
     }
     else
     {
          printf("输入的不是有效数字\n");
     }
}

void choice_main_M(int sID, char *buf)
{
     MSG msg;
     if (strcmp(buf, "1") == 0)
     {

          while (1)
          {
               system("clear");
               printf("商品管理界面 当前用户:%s\n", CurrentUserAC);
               printf("1.已上架的商品 2.发布商品 3.退出\n");
               scanf("%s", buf);
               if (strcmp(buf, "3") == 0)
               {
                    break;
               }
               choice_QueryPurchase_M(sID, buf);
          }
     }
     else if (strcmp(buf, "2") == 0)
     {
          system("clear");
          printf("订单管理界面\n");

          msg.type = search_totalodM;

          strcpy(msg.data, CurrentUserAC);
          int a = 2;
          send(sID, &msg, sizeof(MSG), 0);
          sID1 = sID;

          pthread_t pt3;
          pthread_create(&pt3, NULL, recvodhub, &a);

          while (1)
          {

               fflush(stdout);
               char opt[30];
               scanf("%s", opt);
               if (strcmp(opt, "quit") == 0)
               {
                    pthread_cancel(pt3);
                    pthread_join(pt3, NULL);
                    pthread_mutex_unlock(&sID_mutex);
                    break;
               }
          }
     }
     else if (strcmp(buf, "3") == 0) // 商家联系人
     {

          system("clear");
          printf("私聊界面\n");
          strcpy(msg.data, CurrentUserAC);
          msg.type = queryrecord;
          int ret = send(sID, &msg, sizeof(MSG), 0);
          if (ret == -1)
          {
               perror("查询未读消息失败");
          }
          CLIST list[50];
          int i = 0;
          while (1)
          {
               pthread_mutex_lock(&sID_mutex);
               ret = read(sID, &msg, sizeof(MSG));
               pthread_mutex_unlock(&sID_mutex);
               ;

               if (msg.type == list_ing)
               {
                    list[i] = *(CLIST *)msg.data;
                    printf("--------------------------------\n");
                    printf("ID:%d      联系人：%s     未读消息：%d\n", i + 1, list[i].other.name, list[i].other.countnr);
                    printf("--------------------------------\n");
                    i++;
               }
               else if (msg.type == yes)
               {
                    printf("查询完毕\n");
                    break;
               }
               else if (msg.type == kong)
               {
                    printf("--------------------------------\n");

                    printf("     暂无联系     \n");

                    printf("--------------------------------\n");

                    break;
               }
          }
          while (1)
          {
               printf("选择要联系的用户ID\n");
               scanf("%s", buf);
               if (strcmp(buf, "quit") == 0)
               {
                    return;
               }
               if (atoi(buf) > i)
               {
                    printf("该ID不存在！\n");
               }
               else
                    break;
          }

          msg.type = recv_chat;
          CHAT chatmsg;

          strcpy(OtherAc, list[atoi(buf) - 1].other.name);
          strcpy(chatmsg.self_name, OtherAc);
          strcpy(chatmsg.dest_name, CurrentUserAC);
          memcpy(msg.data, &chatmsg, sizeof(CHAT));
          send(sID, &msg, sizeof(MSG), 0);
          system("clear");
          sID1 = sID;

          pthread_t pt;
          pthread_create(&pt, NULL, recvchathub, NULL);

          while (1)
          {

               fflush(stdin);
               CHAT m1;
               scanf("%s", m1.text);
               if (strcmp(m1.text, "quit") == 0)
               {
                    pthread_cancel(pt);
                    pthread_join(pt, NULL);
                    pthread_mutex_unlock(&sID_mutex);

                    break;
               }

               strcpy(m1.dest_name, OtherAc);
               strcpy(m1.self_name, CurrentUserAC);
               char *time_now = getTime();
               strcpy(m1.send_time, time_now);
               strcpy(m1.read, "未读");
               msg.type = single_chat;
               memcpy(msg.data, &m1, sizeof(CHAT));
               int ret = send(sID, &msg, sizeof(MSG), 0);
               if (ret == -1)
               {
                    perror("私聊发不出去");
               }
               else
                    printf("发送成功\n");

               msg.type = recv_chat;
               CHAT chatmsg;
               strcpy(chatmsg.self_name, OtherAc);
               strcpy(chatmsg.dest_name, CurrentUserAC);
               memcpy(msg.data, &chatmsg, sizeof(CHAT));
               send(sID, &msg, sizeof(MSG), 0);
               system("clear");

               // CHAT *m = (CHAT *)msg.data;
               // printf("%d %s %s \n",msg.type,m->dest_name,m->self_name);
          }
     }
     else if (strcmp(buf, "4") == 0)
     {
          printf("已进入广场\n");
          // 首先要查询已有的帖子

          // 然后自己选择或者发帖子
          printf("请选择你的操作 1.进入帖子  2.发布帖子\n");
          char opt[50];
          scanf("%s", opt);
          if (strcmp(opt, "1") == 0)
          {
               msg.type = search_totalPT;

               send(sID, &msg, sizeof(MSG), 0);

               sID1 = sID;
               pthread_t pt3;
               pthread_create(&pt3, NULL, recvptmain, NULL);

               while (1)
               {

                    fflush(stdout);
                    char opt[30];
                    scanf("%s", opt);
                    if (strcmp(opt, "quit") == 0)
                    {
                         pthread_cancel(pt3);
                         pthread_join(pt3, NULL);
                         pthread_mutex_unlock(&sID_mutex);
                         break;
                    }
                    else
                    {
                         pthread_cancel(pt3);
                         pthread_join(pt3, NULL);
                         pthread_mutex_unlock(&sID_mutex);

                         // system("clear");

                         strcpy(msg.data, opt);
                         int now_post = atoi(msg.data);

                         // while (1)
                         // {
                    NOWPT:
                         system("clear");
                         msg.type = Join_Post;
                         // printf("输入的ID是%d\n", now_post);

                         sprintf(msg.data, "%d", now_post);
                         send(sID, &msg, sizeof(MSG), 0); // 进帖子

                         pthread_mutex_lock(&sID_mutex);
                         read(sID, &msg, sizeof(MSG));
                         pthread_mutex_unlock(&sID_mutex);

                         if (msg.type == yes)
                         {
                              POST po = *(POST *)msg.data;
                              printf("--------------------------------\n");
                              printf("帖子类型：%s 编号：%d\n标题：%s\n--------------------------------\n内容：%s \n--------------------------------\n点赞数：%d   评论数:%d\n", po.Type, po.post_id, po.title, po.content, po.likes, po.comment_count);
                              printf("--------------------------------\n");
                              // printf("--------------------------------\n");
                              printf("-------以下为评论区------\n");
                              // printf("--------------------------------\n");

                              msg.type = search_totalCOM;

                              sprintf(msg.data, "%d", po.post_id);
                              send(sID, &msg, sizeof(MSG), 0);

                              sID1 = sID;
                              pthread_t pt4;
                              pthread_create(&pt4, NULL, recvcomment, NULL);

                              while (1)
                              {

                                   fflush(stdout);
                                   char opt[30];
                                   scanf("%s", opt);
                                   if (strcmp(opt, "quit") == 0 || strcmp(opt, "4") == 0)
                                   {
                                        pthread_cancel(pt4);
                                        pthread_join(pt4, NULL);
                                        pthread_mutex_unlock(&sID_mutex);
                                        break;
                                   }
                                   if (strcmp(opt, "1") == 0)
                                   {
                                        system("clear");
                                        printf("请输入要回复的内容\n");
                                        COMMENT co;
                                        msg.type = add_comment;
                                        strcpy(co.author_ac, CurrentUserAC);
                                        char now_time[40];
                                        strcpy(now_time, getTime());
                                        strcpy(co.comment_time, now_time);
                                        co.post_id = now_post;

                                        scanf("%s", co.connent);
                                        if (strcmp(co.connent, "quit") == 0)
                                        {
                                             break;
                                        }
                                        memcpy(msg.data, &co, sizeof(COMMENT));
                                        send(sID, &msg, sizeof(MSG), 0);

                                        pthread_mutex_lock(&sID_mutex);
                                        read(sID, &msg, sizeof(MSG));
                                        pthread_mutex_unlock(&sID_mutex);
                                        if (msg.type == yes)
                                        {
                                             printf("发送成功\n");
                                             sleep(1);
                                        }
                                        else
                                        {
                                             printf("发送失败\n");
                                             sleep(1);
                                        }
                                   }
                                   else if (strcmp(opt, "2") == 0)
                                   {
                                        printf("与楼主联系\n");
                                        // AAA
                                        msg.type = recv_chat;
                                        CHAT chatmsg;

                                        strcpy(chatmsg.self_name, CurrentUserAC);
                                        strcpy(chatmsg.dest_name, po.poster);
                                        printf("发信人 %s 收信人%s\n", chatmsg.self_name, chatmsg.dest_name);
                                        sleep(1); // 商人
                                        memcpy(msg.data, &chatmsg, sizeof(CHAT));
                                        send(sID, &msg, sizeof(MSG), 0);
                                        system("clear");
                                        sID1 = sID;

                                        pthread_t pt2; // 商家
                                        pthread_create(&pt2, NULL, recvchathub, NULL);

                                        while (1)
                                        {

                                             fflush(stdout);
                                             CHAT m1;
                                             scanf("%s", m1.text);
                                             if (strcmp(m1.text, "quit") == 0)
                                             {
                                                  pthread_cancel(pt2);
                                                  pthread_join(pt2, NULL);
                                                  pthread_mutex_unlock(&sID_mutex);
                                                  printf("中断线程");
                                                  sleep(1);
                                                  break;
                                             }

                                             strcpy(m1.dest_name, OtherAc);
                                             strcpy(m1.self_name, CurrentUserAC);
                                             char *time_now = getTime();
                                             strcpy(m1.send_time, time_now);
                                             strcpy(m1.read, "未读");
                                             msg.type = single_chat;
                                             memcpy(msg.data, &m1, sizeof(CHAT));
                                             int ret = send(sID, &msg, sizeof(MSG), 0);
                                             if (ret == -1)
                                             {
                                                  perror("私聊发不出去");
                                             }
                                             msg.type = recv_chat;
                                             CHAT chatmsg;
                                             strcpy(chatmsg.self_name, OtherAc);
                                             strcpy(chatmsg.dest_name, CurrentUserAC);
                                             memcpy(msg.data, &chatmsg, sizeof(CHAT));
                                             send(sID, &msg, sizeof(MSG), 0);
                                        }
                                   }
                                   else if (strcmp(opt, "3") == 0)
                                   {
                                        printf("点赞成功\n");
                                        msg.type = add_likes;
                                        sprintf(msg.data, "%d", now_post);
                                        send(sID, &msg, sizeof(MSG), 0);
                                   }
                                   {

                                        printf("输入的不是有效数字\n");
                                        // }
                                   }
                                   sprintf(msg.data, "%d", now_post);
                                   msg.type = update_commentcount;
                                   send(sID, &msg, sizeof(MSG), 0);
                                   goto NOWPT;
                              }
                              break;
                         }
                         else if (msg.type == kong)
                         {
                              printf("你输入的ID不存在！\n");
                         }
                         else if (msg.type == no)
                         {
                              printf("进贴失败，请检查连接\n");
                         }
                    }
               }
          }
          else if (strcmp(opt, "2") == 0)
          {
               POST po;
               while (1)
               {
                    printf("请选择帖子类型 (出售/交流)\n");
                    scanf("%s", po.Type);
                    if (strcmp(po.Type, "出售") == 0 || strcmp(po.Type, "交流") == 0)
                    {
                         printf("请输入标题\n");
                         scanf("%s", po.title);
                         if (strcmp(po.title, "quit") == 0)
                         {
                              break;
                         }
                         char *time_now = getTime();
                         strcpy(po.poster_time, time_now);
                         strcpy(po.poster, CurrentUserAC);

                         char num[40];
                         createOrderNumber(num);

                         printf("请输入内容\n");
                         scanf("%s", po.content);
                         if (strcmp(po.content, "quit") == 0)
                         {
                              break;
                         }

                         printf("确认发送?(输入确认)\n");
                         scanf("%s", opt);
                         po.comment_count = 0;
                         po.likes = 0;
                         if (strcmp(opt, "确认") == 0)
                         {
                              printf("帖子:类型 %s 标题 %s 内容 %s 发送者 %s 发送时间%s 点赞量%d 评论数 %d\n", po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

                              msg.type = add_post;
                              memcpy(msg.data, &po, sizeof(POST));

                              int ret = send(sID, &msg, sizeof(MSG), 0);
                              if (ret == -1)
                              {

                                   perror("发帖失败\n");
                                   return;
                              }
                              pthread_mutex_lock(&sID_mutex);
                              read(sID, &msg, sizeof(MSG));
                              pthread_mutex_unlock(&sID_mutex);
                              if (msg.type == yes)
                              {
                                   printf("发帖成功\n");
                                   sleep(1);
                              }
                              else
                              {
                                   printf("发帖失败\n");
                                   sleep(1);
                              }
                         }
                         else
                         {
                              printf("正在退出\n");
                              return;
                         }

                         break;
                    }
                    else if (strcmp(po.Type, "quit") == 0)
                    {
                         printf("正在返回\n");
                         return;
                    }
                    else
                    {
                         printf("输入的类型有误\n");
                    }
               }
          }

          return;
     }
     else
     {
          printf("输入的不是有效数字\n");
     }
}

void choice_QueryPurchase_M(int sID, char *buf)
{
     MSG msg;
     if (strcmp(buf, "1") == 0)
     {

          system("clear");

          printf("已上架的商品\n");
          msg.type = list_my_product;
          USER ac;
          strcpy(ac.account, CurrentUserAC);
          memcpy(msg.data, &ac, sizeof(USER));
          send(sID, &msg, sizeof(MSG), 0); // 列出商品列表
          char array[200];
          while (1)
          {

               pthread_mutex_lock(&sID_mutex);
               read(sID, &msg, sizeof(MSG));
               pthread_mutex_unlock(&sID_mutex);
               if (msg.type == list_ing)
               {
                    PD *pd = (PD *)msg.data;
                    printf("商品ID:%d 名称：%s 价格：%.2lf 库存：%d 状态：%s\n", pd->ID, pd->product_Name, pd->Price, pd->Quantity, pd->State);
                    strcpy(array, insert_arraystr(array, pd->ID));
               }
               else if (msg.type == yes)
               {
                    printf("查询完毕\n");
                    break;
               }
               else if (msg.type == no)
               {
                    printf("查询失败\n");
               }
          }
          // printf("%s\n", array);
          while (1)
          {
               printf("请输入你要修改的商品ID (输入quit退出)\n");
               fflush(stdin);

               scanf("%s", buf);
               if (strcmp("quit", buf) == 0)
               {
                    printf("正在退出\n");
                    break;
               }
               PD pd;
               if (-1 == check_arraystr(array, atoi(buf)))
               {
                    printf("该ID不存在！\n");
                    sleep(1);
               }
               else if (1 == check_arraystr(array, atoi(buf)))
               {
                    printf("该ID存在！\n");
                    printf("修改内容：1.名称 2.价格 3.库存 4.状态\n");
                    char opt[30];
                    scanf("%s", opt);
                    if (strcmp(opt, "1") == 0)
                    {
                    }
                    // sleep(1);
               }
          }
     }
     else if (strcmp(buf, "2") == 0)
     {

          while (1)
          {
               system("clear");
               PD pd;
               printf("发布商品\n");

               printf("请输入商品名称\n");

               strcpy(pd.M_name, CurrentUserAC);

               fflush(stdin);
               scanf("%s", pd.product_Name);
               if (strcmp(pd.product_Name, "quit") == 0)
                    break;
               printf("请输入价格\n");
               fflush(stdin);
               scanf("%lf", &pd.Price);
               printf("请输入当前库存\n");
               fflush(stdin);
               scanf("%d", &pd.Quantity);
               strcpy(pd.State, "在售");
               strcpy(pd.Details, "无");

               msg.type = put_product;
               memcpy(msg.data, &pd, sizeof(PD));
               int ret = send(sID, &msg, sizeof(MSG), 0);
               if (ret < 0)
               {
                    perror("上架商品失败：");
               }
               pthread_mutex_lock(&sID_mutex);
               read(sID, &msg, sizeof(MSG));
               pthread_mutex_unlock(&sID_mutex);

               printf("上架成功\n");
               sleep(1);
          }
     }
     else if (strcmp(buf, "3") == 0)
     {

          return;
     }
     else
     {
          printf("输入的不是有效数字\n");
     }
}

char *insert_arraystr(char *buf, int a)
{

     sprintf(buf, "%s,%d", buf, a);

     return buf;
}

int check_arraystr(char *buf, int a)
{

     char *bufCopy = (char *)malloc(strlen(buf) + 1);
     strcpy(bufCopy, buf);

     char *token;
     token = strtok(bufCopy, ",");
     int found = 0;
     while (token != NULL)
     {
          int num = atoi(token);
          if (num == a)
          {
               found = 1;
               break;
          }
          token = strtok(NULL, ",");
     }
     free(bufCopy);
     if (found)
     {

          return 1;
     }
     else
     {
          return -1;
     }
}
void choice_main_U(int sID, char *buf)
{

     MSG msg;

     if (strcmp(buf, "1") == 0)
     {

          while (1)
          {
               system("clear");
               printf("-------------------------------------------\n");
               printf("|%-20s |%-22s |\n", "商城", "         详情");
               printf("-------------------------------------------\n");
               printf("|%-22s |%-25s |\n", "1.交易论坛", "      易物论购区");
               printf("-------------------------------------------\n");
               printf("|%-22s |%-27s |\n", "2.搜索商品", "    搜索商品关键词");
               printf("-------------------------------------------\n");
               printf("|%-22s |%-26s |\n", "3.退出商城", "     返回个人主页");
               printf("-------------------------------------------\n");

               scanf("%s", buf);
               if (strcmp(buf, "3") == 0)
               {
                    break;
               }
               choice_QueryPurchase_U(sID, buf);
          }
     }
     else if (strcmp(buf, "2") == 0)
     {

          system("clear");
          printf("订单管理界面\n");

          msg.type = search_totalodU;

          strcpy(msg.data, CurrentUserAC);
          int a = 1;
          send(sID, &msg, sizeof(MSG), 0);
          sID1 = sID;

          pthread_t pt3;
          pthread_create(&pt3, NULL, recvodhub, &a);

          while (1)
          {

               fflush(stdout);
               char opt[30];
               scanf("%s", opt);
               if (strcmp(opt, "quit") == 0)
               {
                    pthread_cancel(pt3);
                    pthread_join(pt3, NULL);
                    pthread_mutex_unlock(&sID_mutex);
                    break;
               }
          }
     }
     else if (strcmp(buf, "3") == 0)
     {

          system("clear");
          printf("私聊界面\n");
          strcpy(msg.data, CurrentUserAC);
          msg.type = queryrecord;
          int ret = send(sID, &msg, sizeof(MSG), 0);
          if (ret == -1)
          {
               perror("查询未读消息失败");
          }
          CLIST list[50];
          int i = 0;
          while (1)
          {

               pthread_mutex_lock(&sID_mutex);
               ret = read(sID, &msg, sizeof(MSG));
               pthread_mutex_unlock(&sID_mutex);

               if (msg.type == list_ing)
               {
                    list[i] = *(CLIST *)msg.data;
                    printf("--------------------------------\n");
                    printf("ID:%d      联系人：%s     未读消息：%d\n", i + 1, list[i].other.name, list[i].other.countnr);
                    printf("--------------------------------\n");
                    i++;
               }
               else if (msg.type == yes)
               {
                    printf("查询完毕\n");
                    break;
               }
               else if (msg.type == no)
               {
                    printf("查询失败\n");
                    break;
               }
               else if (msg.type == kong)
               {
                    printf("--------------------------------\n");
                    printf("         暂无联系人发来消息\n");
                    printf("--------------------------------\n");
                    break;
               }
          }
          while (1)
          {
               printf("选择要联系的用户ID\n");
               scanf("%s", buf);
               if (strcmp(buf, "quit") == 0)
               {
                    i = 0;
                    return;
               }
               if (atoi(buf) > i)
               {
                    printf("该ID不存在！\n");
               }
               else
                    break;
          }
          msg.type = recv_chat;
          CHAT chatmsg;
          strcpy(chatmsg.self_name, list[atoi(buf) - 1].other.name);
          strcpy(chatmsg.dest_name, CurrentUserAC);
          memcpy(msg.data, &chatmsg, sizeof(CHAT));
          send(sID, &msg, sizeof(MSG), 0);
          system("clear");
          sID1 = sID;

          pthread_t pt1; // 用户
          pthread_create(&pt1, NULL, recvchathub, NULL);

          while (1)
          {

               fflush(stdout);
               CHAT m1;
               scanf("%s", m1.text);
               if (strcmp(m1.text, "quit") == 0)
               {
                    pthread_cancel(pt1);
                    pthread_join(pt1, NULL);
                    pthread_mutex_unlock(&sID_mutex);
                    break;
               }

               strcpy(m1.dest_name, OtherAc);
               strcpy(m1.self_name, CurrentUserAC);
               char *time_now = getTime();
               strcpy(m1.send_time, time_now);
               strcpy(m1.read, "未读");
               msg.type = single_chat;
               memcpy(msg.data, &m1, sizeof(CHAT));
               int ret = send(sID, &msg, sizeof(MSG), 0);
               if (ret == -1)
               {
                    perror("私聊发不出去");
               }
               msg.type = recv_chat;
               CHAT chatmsg;
               strcpy(chatmsg.self_name, OtherAc);
               strcpy(chatmsg.dest_name, CurrentUserAC);
               memcpy(msg.data, &chatmsg, sizeof(CHAT));
               send(sID, &msg, sizeof(MSG), 0);
               system("clear");
          }
     }
     else if (strcmp(buf, "4") == 0)
     {
          msg.type = search_myUser;
          strcpy(msg.data, CurrentUserAC);

          send(sID, &msg, sizeof(MSG), 0);
          pthread_mutex_lock(&sID_mutex);
          read(sID, &msg, sizeof(MSG)); // 接收到自己的用户信息
          pthread_mutex_unlock(&sID_mutex);
          USER *ac = (USER *)msg.data;
          if (msg.type == yes)
          {
               system("clear");
               // printf("账号：%s 昵称：%s 身份：%s 地址；%s\n", ac->account, ac->Nick, ac->Identity, ac->Address);
               printf("-------------------------------------------\n");
               printf("|%-10s  %-30s |\n", "账号 ", ac->account);
               printf("-------------------------------------------\n");
               printf("|%-10s  %-30s |\n", "昵称 ", ac->Nick);
               printf("-------------------------------------------\n");
               printf("|%-10s  %-30s |\n", "身份 ", ac->Identity);
               printf("-------------------------------------------\n");
                printf("|%-10s  %-30s |\n", "地址 ", ac->Address);
               printf("-------------------------------------------\n");
          }
          else
          {
               printf("查询失败\n");
               return;
          }
          // printf("请输入要修改的地方\n1.昵称 2.密码 3.地址 4.退出\n");
          while (1)
          {

               printf("请输入要修改的地方\n1.昵称 2.密码 3.地址 4.退出\n");
               char opt[20];
               scanf("%s", opt);
               if (strcmp(opt, "1") == 0)
               {
                    printf("请输入新昵称\n");
                    scanf("%s", ac->Nick);
                    printf("确认保存？(输入确认完成保存)");
                    scanf("%s", opt);
                    if (strcmp(opt, "确认") == 0)
                    {
                         msg.type = Update_User;

                         printf("更新：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n", ac->account, ac->password, ac->Nick, ac->Identity, ac->Address);

                         memcpy(msg.data, ac, sizeof(USER)); // 不使用&ac 因为ac本身就是指针

                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_mutex_lock(&sID_mutex);
                         read(sID, &msg, sizeof(MSG));
                         pthread_mutex_unlock(&sID_mutex);
                         if (msg.type == yes)
                         {
                              printf("保存成功\n");
                              // sleep(1);
                         }
                         else
                         {
                              printf("保存失败\n");
                              sleep(1);
                         }
                    }
                    else
                    {
                         printf("正在返回\n");
                         sleep(1);
                    }
               }
               else if (strcmp(opt, "2") == 0)
               {
                    printf("请输入新密码\n");
                    scanf("%s", ac->password);
                    printf("确认保存？(输入确认完成保存)");
                    scanf("%s", opt);
                    if (strcmp(opt, "确认") == 0)
                    {
                         msg.type = Update_User;

                         printf("更新：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n", ac->account, ac->password, ac->Nick, ac->Identity, ac->Address);

                         memcpy(msg.data, ac, sizeof(USER)); // 不使用&ac 因为ac本身就是指针

                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_mutex_lock(&sID_mutex);
                         read(sID, &msg, sizeof(MSG));
                         pthread_mutex_unlock(&sID_mutex);
                         if (msg.type == yes)
                         {
                              printf("保存成功\n");
                              // sleep(1);
                         }
                         else
                         {
                              printf("保存失败\n");
                              sleep(1);
                         }
                    }
                    else
                    {
                         printf("正在返回\n");
                         sleep(1);
                    }
               }
               else if (strcmp(opt, "3") == 0)
               {
                    printf("请输入新地址\n");
                    scanf("%s", ac->Address);
                    printf("确认保存？(输入确认完成保存)");
                    scanf("%s", opt);
                    if (strcmp(opt, "确认") == 0)
                    {
                         msg.type = Update_User;

                         printf("更新：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n", ac->account, ac->password, ac->Nick, ac->Identity, ac->Address);

                         memcpy(msg.data, ac, sizeof(USER)); // 不使用&ac 因为ac本身就是指针

                         send(sID, &msg, sizeof(MSG), 0);
                         pthread_mutex_lock(&sID_mutex);
                         read(sID, &msg, sizeof(MSG));
                         pthread_mutex_unlock(&sID_mutex);
                         if (msg.type == yes)
                         {
                              printf("保存成功\n");
                              // sleep(1);
                         }
                         else
                         {
                              printf("保存失败\n");
                              sleep(1);
                         }
                    }
                    else
                    {
                         printf("正在返回\n");
                         sleep(1);
                    }
               }
               else if (strcmp(opt, "4") == 0)
               {
                    printf("正在返回\n");
                    return;
               }
               else
               {
                    printf("输入的不是有效数字\n");
               }
          }
     }
     else
     {
          printf("输入的不是有效数字\n");
     }
}
void choice_QueryPurchase_U(int sID, char *buf)
{
     MSG msg;
     if (strcmp(buf, "1") == 0)
     {
          printf("进入广场\n");

          printf("已进入广场\n");
          // 首先要查询已有的帖子

          // 然后自己选择或者发帖子
          printf("请选择你的操作 1.进入帖子  2.发布帖子\n");
          char opt[50];
          scanf("%s", opt);

          if (strcmp(opt, "1") == 0)
          {
               msg.type = search_totalPT;

               send(sID, &msg, sizeof(MSG), 0);

               sID1 = sID;
               pthread_t pt3;
               pthread_create(&pt3, NULL, recvptmain, NULL);

               while (1)
               {

                    fflush(stdout);
                    char opt[30];
                    scanf("%s", opt);
                    if (strcmp(opt, "quit") == 0)
                    {
                         pthread_cancel(pt3);
                         pthread_join(pt3, NULL);
                         pthread_mutex_unlock(&sID_mutex);
                         break;
                    }
                    else
                    {
                         pthread_cancel(pt3);
                         pthread_join(pt3, NULL);
                         pthread_mutex_unlock(&sID_mutex);

                         // system("clear");

                         strcpy(msg.data, opt);
                         int now_post = atoi(msg.data);

                         // while (1)
                         // {
                    NOWPT:
                         system("clear");
                         msg.type = Join_Post;
                         // printf("输入的ID是%d\n", now_post);

                         sprintf(msg.data, "%d", now_post);
                         send(sID, &msg, sizeof(MSG), 0); // 进帖子

                         pthread_mutex_lock(&sID_mutex);
                         read(sID, &msg, sizeof(MSG));
                         pthread_mutex_unlock(&sID_mutex);

                         if (msg.type == yes)
                         {
                              POST po = *(POST *)msg.data;
                              printf("--------------------------------\n");
                              printf("帖子类型：%s 编号：%d\n标题：%s\n--------------------------------\n内容：%s \n--------------------------------\n点赞数：%d   评论数:%d\n", po.Type, po.post_id, po.title, po.content, po.likes, po.comment_count);
                              printf("--------------------------------\n");
                              // printf("--------------------------------\n");
                              printf("-------以下为评论区------\n");
                              // printf("--------------------------------\n");

                              msg.type = search_totalCOM;

                              sprintf(msg.data, "%d", po.post_id);
                              send(sID, &msg, sizeof(MSG), 0);

                              sID1 = sID;
                              pthread_t pt4;
                              pthread_create(&pt4, NULL, recvcomment, NULL);

                              while (1)
                              {

                                   fflush(stdout);
                                   char opt[30];
                                   scanf("%s", opt);
                                   if (strcmp(opt, "quit") == 0 || strcmp(opt, "4") == 0)
                                   {
                                        pthread_cancel(pt4);
                                        pthread_join(pt4, NULL);
                                        pthread_mutex_unlock(&sID_mutex);
                                        break;
                                   }
                                   if (strcmp(opt, "1") == 0)
                                   {
                                        system("clear");
                                        printf("请输入要回复的内容\n");
                                        COMMENT co;
                                        msg.type = add_comment;
                                        strcpy(co.author_ac, CurrentUserAC);
                                        char now_time[40];
                                        strcpy(now_time, getTime());
                                        strcpy(co.comment_time, now_time);
                                        co.post_id = now_post;

                                        scanf("%s", co.connent);
                                        if (strcmp(co.connent, "quit") == 0)
                                        {
                                             break;
                                        }
                                        memcpy(msg.data, &co, sizeof(COMMENT));
                                        send(sID, &msg, sizeof(MSG), 0);

                                        pthread_mutex_lock(&sID_mutex);
                                        read(sID, &msg, sizeof(MSG));
                                        pthread_mutex_unlock(&sID_mutex);
                                        if (msg.type == yes)
                                        {
                                             printf("发送成功\n");
                                             sleep(1);
                                        }
                                        else
                                        {
                                             printf("发送失败\n");
                                             sleep(1);
                                        }
                                   }
                                   else if (strcmp(opt, "2") == 0)
                                   {
                                        printf("与楼主联系\n");
                                        // AAA
                                        msg.type = recv_chat;
                                        CHAT chatmsg;

                                        strcpy(chatmsg.self_name, CurrentUserAC);
                                        strcpy(chatmsg.dest_name, po.poster);
                                        memcpy(msg.data, &chatmsg, sizeof(CHAT));
                                        send(sID, &msg, sizeof(MSG), 0);
                                        system("clear");
                                        sID1 = sID;

                                        pthread_t pt2; // 商家
                                        pthread_create(&pt2, NULL, recvchathub, NULL);

                                        while (1)
                                        {

                                             fflush(stdout);
                                             CHAT m1;
                                             scanf("%s", m1.text);
                                             if (strcmp(m1.text, "quit") == 0)
                                             {
                                                  pthread_cancel(pt2);
                                                  pthread_join(pt2, NULL);
                                                  pthread_mutex_unlock(&sID_mutex);
                                                  printf("中断线程");
                                                  sleep(1);
                                                  break;
                                             }

                                             strcpy(m1.dest_name, OtherAc);
                                             strcpy(m1.self_name, CurrentUserAC);
                                             char *time_now = getTime();
                                             strcpy(m1.send_time, time_now);
                                             strcpy(m1.read, "未读");
                                             msg.type = single_chat;
                                             memcpy(msg.data, &m1, sizeof(CHAT));
                                             int ret = send(sID, &msg, sizeof(MSG), 0);
                                             if (ret == -1)
                                             {
                                                  perror("私聊发不出去");
                                             }
                                             msg.type = recv_chat;
                                             CHAT chatmsg;
                                             strcpy(chatmsg.self_name, OtherAc);
                                             strcpy(chatmsg.dest_name, CurrentUserAC);
                                             memcpy(msg.data, &chatmsg, sizeof(CHAT));
                                             send(sID, &msg, sizeof(MSG), 0);
                                        }
                                   }
                                   else if (strcmp(opt, "3") == 0)
                                   {
                                        printf("点赞成功\n");
                                        msg.type = add_likes;
                                        sprintf(msg.data, "%d", now_post);
                                        send(sID, &msg, sizeof(MSG), 0);
                                   }
                                   {

                                        printf("输入的不是有效数字\n");
                                        // }
                                   }
                                   sprintf(msg.data, "%d", now_post);
                                   msg.type = update_commentcount;
                                   send(sID, &msg, sizeof(MSG), 0);
                                   goto NOWPT;
                              }
                              break;
                         }
                         else if (msg.type == kong)
                         {
                              printf("你输入的ID不存在！\n");
                         }
                         else if (msg.type == no)
                         {
                              printf("进贴失败，请检查连接\n");
                         }
                    }
               }
          }
          else if (strcmp(opt, "2") == 0)
          {
               POST po;
               while (1)
               {
                    printf("请选择帖子类型 (出售/交流)\n");
                    scanf("%s", po.Type);
                    if (strcmp(po.Type, "求购") == 0 || strcmp(po.Type, "交流") == 0)
                    {
                         printf("请输入标题\n");
                         scanf("%s", po.title);
                         if (strcmp(po.title, "quit") == 0)
                         {
                              break;
                         }
                         char *time_now = getTime();
                         strcpy(po.poster_time, time_now);
                         strcpy(po.poster, CurrentUserAC);

                         char num[40];
                         createOrderNumber(num);

                         printf("请输入内容\n");
                         scanf("%s", po.content);
                         if (strcmp(po.content, "quit") == 0)
                         {
                              break;
                         }

                         printf("确认发送?(输入确认)\n");
                         scanf("%s", opt);
                         po.comment_count = 0;
                         po.likes = 0;
                         if (strcmp(opt, "确认") == 0)
                         {
                              printf("帖子:类型 %s 标题 %s 内容 %s 发送者 %s 发送时间%s 点赞量%d 评论数 %d\n", po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

                              msg.type = add_post;
                              memcpy(msg.data, &po, sizeof(POST));

                              int ret = send(sID, &msg, sizeof(MSG), 0);
                              if (ret == -1)
                              {

                                   perror("发帖失败\n");
                                   return;
                              }
                              pthread_mutex_lock(&sID_mutex);
                              read(sID, &msg, sizeof(MSG));
                              pthread_mutex_unlock(&sID_mutex);
                              if (msg.type == yes)
                              {
                                   printf("发帖成功\n");
                                   sleep(1);
                              }
                              else
                              {
                                   printf("发帖失败\n");
                                   sleep(1);
                              }
                         }
                         else
                         {
                              printf("正在退出\n");
                              return;
                         }

                         break;
                    }
                    else if (strcmp(po.Type, "quit") == 0)
                    {
                         printf("正在返回\n");
                         return;
                    }
                    else
                    {
                         printf("输入的类型有误\n");
                    }
               }
          }
     }
     else if (strcmp(buf, "2") == 0)
     {
          system("clear");
          printf("请输入商品关键词：");

          fflush(stdout);
          // msg.data是char类型，直接发
          char now_pro[128];
          scanf("%s", now_pro);
          // msg.type = search_totalpro;
          // int ret = send(sID, &msg, sizeof(MSG), 0);

          printf("正在搜索...\n");
          // sleep(1);
          // system("clear");

          PD pd[20];
          int co = 0;
          while (1)
          {
               int co = 0;
               char array[500] = "";
               system("clear");
               msg.type = search_totalpro;
               strcpy(msg.data, now_pro);
               int ret = send(sID, &msg, sizeof(MSG), 0);

               if (ret == -1)
               {
                    printf("查询失败，请检查网络！\n");
                    break;
               }
               while (1)
               {
                    pthread_mutex_lock(&sID_mutex);
                    ret = read(sID, &msg, sizeof(MSG));
                    pthread_mutex_unlock(&sID_mutex);
                    if (msg.type == list_ing)
                    {
                         pd[co] = *(PD *)msg.data;

                         // printf("商品ID:%d 名称：%s 价格：%.2lf 库存：%d 状态：%s 商家名称:%s \n", pd[co].ID, pd[co].product_Name, pd[co].Price, pd[co].Quantity, pd[co].State, pd[co].M_name);

                         char a[400];
                         char b[400];
                         sprintf(a, "商品ID：%d    名称：%s    状态：%s", pd[co].ID, pd[co].product_Name, pd[co].State);
                         sprintf(b, "价格:%.2lf  库存：%d       商家名称:%s ", pd[co].Price, pd[co].Quantity, pd[co].M_name);

                         printf("-------------------------------------------\n");
                         printf("|%-53s |\n", a);
                         printf("|%-49s |\n", b);
                         printf("-------------------------------------------\n");

                         strcpy(array, insert_arraystr(array, pd[co].ID));

                         co++;
                    }
                    else if (msg.type == yes)
                    {
                         // printf("查询完毕\n");
                         break;
                    }
                    else if (msg.type == no)
                    {
                         printf("查询失败\n");
                         break;
                    }
               }

               // printf("%s\n", array);

               int co1 = 0;
               // printf("请输入购买的商品ID (输入quit退出)\n");
               printf("|%-49s |\n", "请输入购买的商品ID (输入quit退出)");
               printf("-------------------------------------------\n");
               fflush(stdin);

               scanf("%s", buf);
               if (strcmp("quit", buf) == 0)
               {
                    printf("正在退出\n");
                    break;
               }

               if (-1 == check_arraystr(array, atoi(buf)))
               {
                    printf("该ID不存在！\n");
                    sleep(1);
               }
               else if (1 == check_arraystr(array, atoi(buf)))
               {
                    // printf("该ID存在！\n");

                    for (int i = 0; i < co; i++)
                    {
                         if (pd[i].ID == atoi(buf))
                         {
                              co1 = i;
                         }
                    }
                    system("clear");
                    char a[400];
                    char b[400];
                    sprintf(a, "商品ID：%d    名称：%s    状态：%s", pd[co1].ID, pd[co1].product_Name, pd[co1].State);
                    sprintf(b, "价格:%.2lf  库存：%d       商家名称:%s ", pd[co1].Price, pd[co1].Quantity, pd[co1].M_name);

                    printf("-------------------------------------------\n");
                    printf("|%-53s |\n", a);
                    printf("|%-49s |\n", b);
                    printf("-------------------------------------------\n");
                    printf("请选择操作：1.购买商品 2.查看商品介绍 3.联系商家 4.状态\n");

                    char opt[30];
                    scanf("%s", opt);
                    if (strcmp(opt, "1") == 0)
                    {
                         system("clear");
                         ODR od;

                         // printf("商品ID:%d 名称：%s 价格：%.2lf 库存：%d 状态：%s 商家名称:%s \n", pd[co1].ID, pd[co1].product_Name, pd[co1].Price, pd[co1].Quantity, pd[co1].State, pd[co1].M_name);

                         char a[400];
                         char b[400];
                         sprintf(a, "商品ID：%d    名称：%s    状态：%s", pd[co1].ID, pd[co1].product_Name, pd[co1].State);
                         sprintf(b, "价格:%.2lf  库存：%d       商家名称:%s ", pd[co1].Price, pd[co1].Quantity, pd[co1].M_name);

                         printf("-------------------------------------------\n");
                         printf("|%-53s |\n", a);
                         printf("|%-49s |\n", b);
                         printf("-------------------------------------------\n");

                         strcpy(od.C_name, CurrentUserAC);
                         od.Pro_ID = pd[co1].ID;
                         strcpy(od.M_name, pd[co1].M_name);
                         strcpy(od.product_Name, pd[co1].product_Name);
                         strcpy(od.State, "未发货");
                         od.Price = pd[co1].Price;

                         printf("请输入要购买的数量\n");
                         scanf("%d", &od.Quantity);
                         if (od.Quantity <= 0)
                         {
                              printf("输入的不是有效数字\n");
                         }
                         if (od.Quantity > pd[co1].Quantity)
                         {
                              printf("库存不够，请重新输入\n");
                              sleep(1);
                         }
                         else
                         {

                              od.Tol_Price = pd->Price * od.Quantity;
                              char num[40];
                              createOrderNumber(num);
                              strcpy(od.num, num);
                              char is[20];
                              printf("支付金额为%.2lf,是否购买?(输入 确认 购买/其他 取消)\n", od.Tol_Price);

                              scanf("%s", is);
                              if (strcmp(is, "确认") == 0)
                              {
                                   // printf("购买成功\n订单:ID:%d 商品名称：%s 商品ID：%d  单价：%.2lf 购买数量：%d  总价：%.2lf 状态：%s 商家名称:%s 买家名称：%s 订单号:%s\n", 0, od.product_Name, od.Pro_ID, od.Price, od.Quantity, od.Tol_Price, od.State, od.M_name, od.C_name, od.num);

                                   printf("购买成功\n");

                                   msg.type = add_order;
                                   memcpy(msg.data, &od, sizeof(ODR));
                                   send(sID, &msg, sizeof(MSG), 0);

                                   pthread_mutex_lock(&sID_mutex);
                                   ret = read(sID, &msg, sizeof(MSG));
                                   pthread_mutex_unlock(&sID_mutex);
                                   if (ret < 0)
                                   {
                                        perror("add_order");
                                   }
                                   sleep(1);
                              }
                              else
                              {
                                   printf("正在取消订单\n");
                              }
                         }
                    }
                    else if (strcmp(opt, "2") == 0)
                    {
                         printf("商品介绍....\n");
                    }
                    else if (strcmp(opt, "3") == 0)
                    {
                         // system("clear");
                         // printf("商家名：%s\n", pd[co1].M_name);
                         // while (1)
                         // {
                         //      printf("发送内容：");
                         //      fflush(stdout);
                         //      fflush(stdin);
                         //      char text[700];
                         //      scanf("%s", text);
                         //      if (strcmp(text, "quit") == 0)
                         //      {
                         //           break;
                         //      }
                         //      CHAT chatmsg;
                         //      strcpy(chatmsg.dest_name, pd[co1].M_name);
                         //      strcpy(chatmsg.self_name, CurrentUserAC);
                         //      strcpy(chatmsg.text, text);
                         //      char *time_now = getTime();
                         //      strcpy(chatmsg.send_time, time_now);
                         //      strcpy(chatmsg.read, "未读");

                         //      memcpy(msg.data, &chatmsg, sizeof(CHAT));
                         //      msg.type = single_chat;
                         //      int ret = send(sID, &msg, sizeof(MSG), 0);
                         //      if (ret == -1)
                         //      {
                         //           perror("发送失败\n");
                         //      }
                         //      free(time_now);

                         msg.type = recv_chat;
                         CHAT chatmsg;

                         strcpy(OtherAc, pd[co1].M_name);
                         strcpy(chatmsg.self_name, OtherAc);
                         strcpy(chatmsg.dest_name, CurrentUserAC);
                         memcpy(msg.data, &chatmsg, sizeof(CHAT));
                         send(sID, &msg, sizeof(MSG), 0);
                         system("clear");
                         sID1 = sID;

                         pthread_t pt2; // 商家
                         pthread_create(&pt2, NULL, recvchathub, NULL);

                         while (1)
                         {

                              fflush(stdout);
                              CHAT m1;
                              scanf("%s", m1.text);
                              if (strcmp(m1.text, "quit") == 0)
                              {
                                   pthread_cancel(pt2);
                                   pthread_join(pt2, NULL);
                                   pthread_mutex_unlock(&sID_mutex);
                                   printf("中断线程");
                                   sleep(1);
                                   break;
                              }

                              strcpy(m1.dest_name, OtherAc);
                              strcpy(m1.self_name, CurrentUserAC);
                              char *time_now = getTime();
                              strcpy(m1.send_time, time_now);
                              strcpy(m1.read, "未读");
                              msg.type = single_chat;
                              memcpy(msg.data, &m1, sizeof(CHAT));
                              int ret = send(sID, &msg, sizeof(MSG), 0);
                              if (ret == -1)
                              {
                                   perror("私聊发不出去");
                              }
                              msg.type = recv_chat;
                              CHAT chatmsg;
                              strcpy(chatmsg.self_name, OtherAc);
                              strcpy(chatmsg.dest_name, CurrentUserAC);
                              memcpy(msg.data, &chatmsg, sizeof(CHAT));
                              send(sID, &msg, sizeof(MSG), 0);
                         }
                    }
                    else if (strcmp(opt, "4") == 0)
                    {
                    }
                    else
                    {
                         printf("输入的不是有效数字\n");
                    }
               }
          }
     }
     else
     {
          printf("你输入的不是有效数字\n");
     }
}

char *getTime()
{

     time_t t = time(0);
     struct tm *t1 = localtime(&t);
     char *str = malloc(sizeof(char) * 50);

     strftime(str, sizeof(char) * 50, "%Y年%m月%d日%H时%M分%S秒", t1);

     return str;
}

void createOrderNumber(char *num)
{
     time_t t = time(0);
     __pid_t p = getpid();
     srand((unsigned int)(t + p)); // 这样每次运行程序时，由于不同的进程具有不同的 ID，所以可以得到不同的随机数序列

     int a = rand() % 1000;
     char str1[10];
     char str2[40];
     sprintf(str1, "%d", a);
     sprintf(str2, "%ld", t);
     strcat(str2, str1);
     strcpy(num, str2);

     return;
}