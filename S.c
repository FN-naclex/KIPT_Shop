#include "Server.function.h"
LGARR LoginARR[500];
int count_l = 0;
pthread_t pt[50];
int pt_i = 0;
NP np[100];
int init_net()
{
    struct sockaddr_in Server_Addr;
    Server_Addr.sin_family = AF_INET;
    Server_Addr.sin_port = htons(9002);

    u_int32_t ip = 0;
    inet_pton(AF_INET, "192.168.17.129", &ip);
    Server_Addr.sin_addr.s_addr = ip;

    int sID = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(sID, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("套接字属性设置失败");
    }
    if (-1 == bind(sID, (struct sockaddr *)&Server_Addr, sizeof(Server_Addr)))
    {

        perror("服务器创建失败");
        return 0;
    }
    else
        printf("服务器创建成功\n");
    listen(sID, 5);
    Create_Orders_Table();
    Create_ChatRecords_Table();

    Create_Products_Table();
    Create_User_Table();
    CREATE_COMMENTS();

    return sID;
}

void choice_main_S(int fd, MSG msg)
{
    USER *ac;
    PD *pd;
    CHAT *chatmsg;

    switch (msg.type)
    {

    case login:
        printf("收到一条登录消息\n");

        ac = (USER *)msg.data;
        printf("登录：账号 %s 密码 %s \n", ac->account, ac->password);

        Login_User(fd, msg);

        break;
    case regist:
        printf("收到一条注册消息\n");

        ac = (USER *)msg.data;
        printf("'%s', '%s','%s','%s','%s'", ac->account, ac->password, ac->Identity, ac->Address, ac->Nick);

        Create_User_Table();

        ADD_User(fd, msg);
        break;
    case put_product:
        printf("收到一条上架商品消息\n");

        pd = (PD *)msg.data;
        printf("商品：商品名 %s 价格 %lf 库存 %d 商家 %s 状态 %s\n", pd->product_Name, pd->Price, pd->Quantity, pd->M_name, pd->State);
        // Create_Products_Table();
        //  Create_User_Table();
        ADD_Product(fd, msg);
        break;
    case list_my_product:
        printf("收到一条展示商品消息\n");
        ac = (USER *)msg.data;
        printf("展示：商家名 %s\n", ac->account);
        List_Products_Table(fd, msg);
        break;
    case search_totalpro:
        printf("收到一条查询所有商品消息\n");
        char proname[200];
        strcpy(proname, msg.data);
        printf("商品关键名：%s\n", proname);
        List_Products_SearchU(fd, proname);
        break;
    case single_chat:
        printf("收到一条私聊消息\n");
        chatmsg = (CHAT *)msg.data;
        // printf("私聊：发信人：%s 收信人：%s \n内容：%s \n时间：%s 状态：%s\n", chatmsg->self_name, chatmsg->dest_name, chatmsg->text, chatmsg->send_time, chatmsg->read);
        // Create_ChatRecords_Table();
        //  Create_Products_Table();
        //  Create_User_Table();
        ADD_Chat(fd, msg);
        break;
    case queryrecord:
        printf("收到一条查询聊天记录请求\n");
        char destname[128];
        strcpy(destname, msg.data);
        // Create_ChatRecords_Table();
        //  Create_ChatRecords_Table();
        //  Create_Products_Table();
        //  Create_User_Table();
        printf("请求人：%s\n", destname);
        List_ChatRecord_List(fd, destname);
        break;
    case recv_chat:
        printf("收到一条私聊界面消息\n");
        chatmsg = (CHAT *)msg.data;

        printf("私聊：发信人：%s 收信人：%s \n", chatmsg->self_name, chatmsg->dest_name);
        Hub_Chat(fd, *chatmsg);
        break;
    case log_off:
        printf("收到一条下线请求\n");

        del_LoginArr(fd);

        break;
    case add_order:
        printf("收到一条添加订单请求\n");

        ADD_ODR(fd, msg);
        update_pro(fd, msg);

        break;
    case search_totalodU:
        printf("收到一条查询U订单请求\n");
        List_Order_SearchU(fd, msg.data);

        break;
    case search_totalodM:
        printf("收到一条查询M订单请求\n");
        List_Order_SearchM(fd, msg.data);
        break;
    case search_myUser:
        printf("收到一条查询账户请求\n");
        List_User_my(fd, msg);
        break;
    case Update_User:
        printf("收到一条更新账号请求\n");
        ac = (USER *)msg.data;
        printf("更新：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n", ac->account, ac->password, ac->Nick, ac->Identity, ac->Address);
        Update_UserCM(fd, msg);
        break;
    case add_post:
        printf("收到一条发帖请求\n");

        ADD_POSTS(fd, msg);
        break;
    case search_totalPT:
        printf("收到一条查贴请求\n");
        CREATE_POSTS();
        LIST_POSTS(fd, msg);
        break;
    case Join_Post:
        printf("收到一条进贴请求\n");
        search_post(fd, msg); // 显示主贴
        break;

    case search_totalCOM:

        LIST_COOMENTS(fd, msg);

        break;
    case add_comment:
        CREATE_COMMENTS();
        ADD_COMMMENT(fd, msg);
        break;
    case update_commentcount:
        UPDATE_COCOUNT(fd, msg);
        break;
    case add_likes:
        ADD_LIKE(fd, msg);
        break;
    case update_nr:

        if (1)
        {
            NP npz;
            npz.fd = fd;
            pthread_t pt;
            npz.pt = pt;
            strcpy(npz.name, msg.data);
            np[pt_i] = npz;
            printf("第%d NP fd为：%d pt为%ld 用户名为%s\n", pt_i, npz.fd, npz.pt, npz.name);
        }

        pthread_create(&np[pt_i].pt, NULL, pupdate_nrcount, (void *)&np[pt_i]);
        pt_i++;
        break;
    case cancel_p:

        printf("收到一条断开线程请求\n");
        cancel_pth(fd);
        // printf("尝试取消%ld的线程\n", pt[pt_i - 1]);
        // int ret_cancel = pthread_cancel(pt[pt_i - 1]);

        // int ret_join = pthread_join(pt[pt_i - 1], NULL);

        break;

    default:
        printf("输入的不是有效数字\n");
        break;
    }
}

void Create_User_Table()
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("User.db", &db);
    if (data < 0)
    {
        printf("创建数据表User.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS User(ID INTEGER PRIMARY KEY AUTOINCREMENT,Nick VARCHAR(80) NOT NULL, Account VARCHAR(128) NOT NULL UNIQUE, Password VARCHAR(128) NOT NULL ,Identity VARCHAR(20) NOT NULL,Address VARCHAR(128));";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("SQL语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void ADD_User(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[2048];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    USER *ac = (USER *)msg.data;
    // printf("'%s', '%s','%s','%s','%s'", ac->account, ac->password, ac->Identity, ac->Address, ac->Nick);

    data = sqlite3_open("User.db", &db);
    if (data < 0)
    {
        printf("打开数据表User.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    // USER *ac = (USER *)msg.data;

    sprintf(sql, "SELECT * FROM User WHERE Account = '%s' ;", ac->account);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    if (row > 0)
    {
        msg.type = no;
        send(fd, &msg, sizeof(MSG), 0); // 用户名已被注册
        sqlite3_free(msg_table);
        sqlite3_close(db);
        return;
    }

    sprintf(sql, "INSERT INTO User (Account,Password,Identity,Address,Nick) VALUES ('%s', '%s','%s','%s','%s');", ac->account, ac->password, ac->Identity, ac->Address, ac->Nick);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("注册语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void Login_User(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("User.db", &db);
    if (data < 0)
    {
        printf("打开数据表User.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    USER *ac = (USER *)msg.data;
    printf("123\n");
    sprintf(sql, "SELECT * FROM User WHERE Account = '%s' AND Password = '%s' ;", ac->account, ac->password);
    printf("123\n");
    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
    printf("123\n");
    msg.type = kong;
    if (row != 1)
    {
        msg.type = no;
        send(fd, &msg, sizeof(MSG), 0); // 账号不存在或者密码错误
    }
    else
    {
        printf("123\n");
        sprintf(sql, "SELECT Identity FROM User WHERE Account = '%s' AND Password = '%s' ;", ac->account, ac->password);
        LoginARR[count_l].fd = fd; // 加入用户数组
        strcpy(LoginARR[count_l].ac, ac->account);
        count_l++;
        if (strcmp(Result[1 * column + 4], "顾客") == 0)
        {
            msg.type = go_main_C;
        }
        else if (strcmp(Result[1 * column + 4], "商家") == 0)
        {
            msg.type = go_main_M;
        }
        else
        {
            printf("用户身份有问题，请检查\n");
        }
        printf("登录数组%d 的fd为%d 账号为%s\n", count_l, LoginARR[count_l - 1].fd, LoginARR[count_l - 1].ac);
        send(fd, &msg, sizeof(MSG), 0); // 登录成功
    }

    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void Create_Products_Table()
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Products.db", &db);
    if (data < 0)
    {
        printf("创建数据表Products.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS Products(ID INTEGER PRIMARY KEY AUTOINCREMENT, ProductName VARCHAR(128) NOT NULL, Merchant VARCHAR(30) NOT NULL ,Price DECIMAL(9,4) NOT NULL,Quantity INTEGER NOT NULL,State VARCHAR(20) NOT NULL,char Details VARCHAR(600) );";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("SQL语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void ADD_Product(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[2048];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Products.db", &db);
    if (data < 0)
    {
        printf("打开数据表Products.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    PD *pd = (PD *)msg.data;

    sprintf(sql, "INSERT INTO Products (ProductName,Merchant,Price,Quantity,State) VALUES ('%s', '%s',%.3lf,%d,'%s');", pd->product_Name, pd->M_name, pd->Price, pd->Quantity, pd->State);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("上架商品执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void List_Products_Table(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Products.db", &db);
    if (data < 0)
    {
        printf("打开数据表Products.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    USER *ac = (USER *)msg.data;

    sprintf(sql, "SELECT * FROM Products WHERE Merchant = '%s' ;", ac->account);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        PD pd;
        for (int i = 0; i < row; i++)
        {

            pd.ID = atoi(Result[(i + 1) * column + 0]);
            strcpy(pd.product_Name, Result[(i + 1) * column + 1]);
            strcpy(pd.M_name, Result[(i + 1) * column + 2]);
            pd.Price = atof(Result[(i + 1) * column + 3]);
            pd.Quantity = atoi(Result[(i + 1) * column + 4]);
            strcpy(pd.State, Result[(i + 1) * column + 5]);
            printf("商品ID: %d 名称:%s 价格：%.2lf 库存：%d 状态：%s\n", pd.ID, pd.product_Name, pd.Price, pd.Quantity, pd.State);

            memcpy(msg.data, &pd, sizeof(PD));
            msg.type = list_ing;
            send(fd, &msg, sizeof(MSG), 0);
        }

        msg.type = yes;
    }
    else
    {

        printf("上架商品执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void List_Products_SearchU(int fd, char *proname)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Products.db", &db);
    if (data < 0)
    {
        printf("打开数据表Products.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    sprintf(sql, "SELECT * FROM Products WHERE ProductName LIKE '%%%s%%' COLLATE NOCASE;", proname);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    MSG msg;
    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        PD pd;
        for (int i = 0; i < row; i++)
        {

            pd.ID = atoi(Result[(i + 1) * column + 0]);
            strcpy(pd.product_Name, Result[(i + 1) * column + 1]);
            strcpy(pd.M_name, Result[(i + 1) * column + 2]);
            pd.Price = atof(Result[(i + 1) * column + 3]);
            pd.Quantity = atoi(Result[(i + 1) * column + 4]);
            strcpy(pd.State, Result[(i + 1) * column + 5]);
            printf("商品ID: %d 名称:%s 价格：%.2lf 库存：%d 状态：%s\n", pd.ID, pd.product_Name, pd.Price, pd.Quantity, pd.State);

            memcpy(msg.data, &pd, sizeof(PD));
            msg.type = list_ing;
            send(fd, &msg, sizeof(MSG), 0);
        }

        msg.type = yes;
    }
    else
    {

        printf("上架商品执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

char *getTime()
{

    time_t t = time(0);
    struct tm *t1 = localtime(&t);
    char *str = malloc(sizeof(char) * 50);

    strftime(str, sizeof(char) * 50, "%Y年%m月%d日%H时%M分%S秒", t1);

    return str;
}
void Create_ChatRecords_Table()
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("ChatRecords.db", &db);
    if (data < 0)
    {
        printf("创建数据表ChatRecords.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS ChatRecords(ID INTEGER PRIMARY KEY AUTOINCREMENT, SelfName VARCHAR(80) NOT NULL, Destname VARCHAR(80) NOT NULL ,\
        Test VARCHAR(700) NOT NULL,SendTime VARCHAR(80) NOT NULL,IsRead VARCHAR(40) NOT NULL);";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("SQL语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}
void ADD_Chat(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[2048];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("ChatRecords.db", &db);
    if (data < 0)
    {
        printf("打开数据表ChatRecords.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    CHAT *chatmsg = (CHAT *)msg.data;

    sprintf(sql, "INSERT INTO ChatRecords ( SelfName, Destname,Test ,SendTime ,IsRead ) \
                    VALUES ('%s', '%s','%s','%s','%s');",
            chatmsg->self_name, chatmsg->dest_name, chatmsg->text, chatmsg->send_time, chatmsg->read);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("上架商品执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0); // 告诉发信人发送成功

    for (int i = 0; i < count_l; i++)
    {
        if (strcmp(chatmsg->dest_name, LoginARR[i].ac) == 0)
        {
            printf("收信人在线，正在给他转发\n");
            msg.type = list_ing;
            send(LoginARR[i].fd, &msg, sizeof(MSG), 0);
        }
    }

    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void List_ChatRecord_List(int fd, char *destname)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    MSG msg;
    msg.type = kong;

    data = sqlite3_open("ChatRecords.db", &db);
    if (data < 0)
    {
        printf("打开数据表ChatRecords.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    // 查询全部
    sprintf(sql, "SELECT SelfName, COUNT(*) as unreadCount FROM ChatRecords WHERE Destname= '%s'  GROUP BY SelfName;", destname);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
    CLIST list[row];

    if (msg_table == NULL)
    {

        printf("查询所有历史 共%d行\n", row);
        for (int i = 0; i < row; i++)
        {
            list[i].num = row;
            list[i].other.countnr = 0;
            strcpy(list[i].other.name, Result[(i + 1) * column + 0]);
            printf("总行数 名字 %d %s\n", list[i].num, list[i].other.name);
        }
    }
    else
    {
        printf("查询所有历史执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    // 查询未读
    sprintf(sql, "SELECT SelfName, COUNT(*) as unreadCount FROM ChatRecords WHERE Destname= '%s' AND IsRead = '未读' GROUP BY SelfName;", destname);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    if (msg_table == NULL)
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < list[0].num; j++)
            {
                if (strcmp(Result[(i + 1) * column + 0], list[j].other.name) == 0)
                {
                    list[j].other.countnr = atoi(Result[(i + 1) * column + 1]);
                    printf("%s的未读消息数量为%d\n", list[j].other.name, list[j].other.countnr);
                }
            }
        }

        for (int i = 0; i < list[0].num; i++)
        {

            msg.type = list_ing;
            printf("正在转发：名字 %s 未读数量%d 列表总数%d", list[i].other.name, list[i].other.countnr, list[i].num);
            memcpy(msg.data, &list[i], sizeof(CLIST));
            int ret = send(fd, &msg, sizeof(MSG), 0);
            if (ret == -1)
            {
                perror("转发错误");
            }
        }
        msg.type = yes;
        if (row == 0)
        {
            msg.type = kong;
        }
    }
    else
    {
        printf("查询未读消息失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    int ret = send(fd, &msg, sizeof(MSG), 0);
    if (ret == -1)
    {
        perror("总结错误");
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void Hub_Chat(int fd, CHAT chatmsg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    MSG msg;
    msg.type = kong;

    data = sqlite3_open("ChatRecords.db", &db);
    if (data < 0)
    {
        printf("打开数据表ChatRecords.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    // 查询全部
    sprintf(sql, "SELECT * FROM ChatRecords WHERE (SelfName = '%s' AND Destname = '%s') OR (SelfName = '%s' AND Destname = '%s');", chatmsg.self_name, chatmsg.dest_name, chatmsg.dest_name, chatmsg.self_name);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
    CHAT m;
    printf("\n\n\n%d\n\n\n", row);
    if (msg_table == NULL)
    {
        if (row != 0)
        {
            for (int i = 0; i < row; i++)
            {

                strcpy(m.self_name, Result[(i + 1) * column + 1]);
                strcpy(m.dest_name, Result[(i + 1) * column + 2]);
                strcpy(m.text, Result[(i + 1) * column + 3]);
                strcpy(m.send_time, Result[(i + 1) * column + 4]);
                strcpy(m.read, "未读");

                printf("%s %s %s %s %s \n", Result[(i + 1) * column + 1], Result[(i + 1) * column + 2], Result[(i + 1) * column + 3], Result[(i + 1) * column + 4], Result[(i + 1) * column + 5]);

                msg.type = list_ing;

                printf("ID:%s\n", Result[(i + 1) * column + 0]);
                if (strcmp(m.dest_name, Result[(i + 1) * column + 2]) == 0 && strcmp(chatmsg.self_name, Result[(i + 1) * column + 1]) == 0 || strcmp("已读", Result[(i + 1) * column + 5]) == 0)
                {
                    sprintf(sql, "UPDATE ChatRecords SET IsRead = '已读' WHERE ID = %s ;", Result[(i + 1) * column + 0]);
                    sqlite3_exec(db, sql, 0, 0, &msg_table);
                    strcpy(m.read, "已读");
                }

                memcpy(msg.data, &m, sizeof(CHAT));

                int ret = send(fd, &msg, sizeof(MSG), 0);
                if (ret == -1)
                {
                    perror("转发失败");
                }
                else
                {
                    printf("转发成功\n");
                }
            }
        }
        else
        {
            msg.type = kong;
            send(fd, &msg, sizeof(MSG), 0);
        }
        msg.type = yes;
    }
    else
    {
        perror("查询聊天失败");
        msg.type = no;
    }
    send(fd, &msg, sizeof(MSG), 0);
}

void del_LoginArr(int fd)
{

    if (LoginARR[count_l - 1].fd != fd)
    {
        for (int i = 0; i < count_l; i++)
        {
            if (LoginARR[i].fd == fd)
            {
                LoginARR[i].fd = LoginARR[i + 1].fd;
                strcpy(LoginARR[i].ac, LoginARR[i + 1].ac);
            }
        }
    }
    count_l--;

    if (count_l == 0)
    {
        printf("暂时没有用户连接\n");
    }
    else
    {
        printf("移除成功，现在最后数量%d 最后一个fd为%d ac为%s\n", count_l, LoginARR[count_l - 1].fd, LoginARR[count_l - 1].ac);
    }
}

void ADD_ODR(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[2048];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Orders.db", &db);
    if (data < 0)
    {
        printf("打开数据表Orders.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    ODR *od = (ODR *)msg.data;

    sprintf(sql, "INSERT INTO Orders (ProductName, CustomerName, MerchantName, Price, Quantity, TotalPrice, OrderNumber, State) VALUES ('%s', '%s', '%s', %.2lf, %d,%.2lf, '%s', '%s');", od->product_Name, od->C_name, od->M_name, od->Price, od->Quantity, od->Tol_Price, od->num, od->State);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("添加订单语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void Create_Orders_Table()
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Orders.db", &db);
    if (data < 0)
    {
        printf("创建数据表Orders.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS Orders (\
        ID INTEGER PRIMARY KEY AUTOINCREMENT,\
        ProductName VARCHAR(128),\
        CustomerName VARCHAR(128),\
        MerchantName VARCHAR(128),\
        Price DECIMAL(10, 3),\
        Quantity INTEGER,\
        TotalPrice DECIMAL(10, 3),\
        OrderNumber VARCHAR(40),\
        State VARCHAR(20));";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("创建Order表语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void update_pro(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[200];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Products.db", &db);
    if (data < 0)
    {
        printf("创建数据表Products.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {
        ODR *od = (ODR *)msg.data;

        printf("%d\n", od->Pro_ID);
        sprintf(sql, "SELECT Quantity FROM Products WHERE ID = %d ;", od->Pro_ID);

        data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
        if (msg_table == NULL)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("查询库存语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }

        int newquantity = atoi(Result[column + 0]) - od->Quantity;
        printf("%s %d %d\n", Result[column + 0], od->Pro_ID, newquantity);

        sprintf(sql, "UPDATE Products SET Quantity = %d WHERE ID = %d ;", newquantity, od->Pro_ID);
        sqlite3_exec(db, sql, 0, 0, &msg_table);

        if (newquantity == 0)
        {
            sprintf(sql, "UPDATE Products SET State = '%s' WHERE ID = %d ;", "缺货", od->Pro_ID);
            sqlite3_exec(db, sql, 0, 0, &msg_table);
        }

        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (msg_table == NULL)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("更新库存语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void List_Order_SearchU(int fd, char *Uname)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Orders.db", &db);
    if (data < 0)
    {
        printf("打开数据表Orders.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    sprintf(sql, "SELECT * FROM Orders WHERE CustomerName = '%s';", Uname);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    MSG msg;
    msg.type = kong;
    if (row == 0)
    {

        send(fd, &msg, sizeof(MSG), 0);

        return;
    }
    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        ODR od;
        for (int i = 0; i < row; i++)
        {
            // pd.ID = atoi(Result[(i + 1) * column + 0]);
            strcpy(od.product_Name, Result[(i + 1) * column + 1]);
            strcpy(od.C_name, Result[(i + 1) * column + 2]);
            strcpy(od.M_name, Result[(i + 1) * column + 3]);
            od.Price = atoi(Result[(i + 1) * column + 4]);
            od.Quantity = atoi(Result[(i + 1) * column + 5]);
            od.Tol_Price = atoi(Result[(i + 1) * column + 6]);
            strcpy(od.num, Result[(i + 1) * column + 7]);
            strcpy(od.State, Result[(i + 1) * column + 8]);

            printf("商品名：%s 买家名:%s 商家名：%s 购买数量：%d 支付金额:%.2lf  状态：%s 订单号%s\n", od.product_Name, od.C_name, od.M_name, od.Quantity, od.Tol_Price, od.State, od.num);
            memcpy(msg.data, &od, sizeof(ODR));
            msg.type = list_ing;
            send(fd, &msg, sizeof(MSG), 0);
        }

        msg.type = yes;
    }
    else
    {

        printf("查询订单执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void List_Order_SearchM(int fd, char *Uname)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Orders.db", &db);
    if (data < 0)
    {
        printf("打开数据表Orders.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    sprintf(sql, "SELECT * FROM Orders WHERE MerchantName = '%s';", Uname);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    MSG msg;
    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        ODR od;
        for (int i = 0; i < row; i++)
        {

            // pd.ID = atoi(Result[(i + 1) * column + 0]);
            strcpy(od.product_Name, Result[(i + 1) * column + 1]);
            strcpy(od.C_name, Result[(i + 1) * column + 2]);
            strcpy(od.M_name, Result[(i + 1) * column + 3]);
            od.Price = atoi(Result[(i + 1) * column + 4]);
            od.Quantity = atoi(Result[(i + 1) * column + 5]);
            od.Tol_Price = atoi(Result[(i + 1) * column + 6]);
            strcpy(od.num, Result[(i + 1) * column + 7]);
            strcpy(od.State, Result[(i + 1) * column + 8]);

            printf("商品名：%s 买家名:%s 商家名：%s 购买数量：%d 支付金额:%.2lf  状态：%s 订单号%s\n", od.product_Name, od.C_name, od.M_name, od.Quantity, od.Tol_Price, od.State, od.num);
            memcpy(msg.data, &od, sizeof(ODR));
            msg.type = list_ing;
            send(fd, &msg, sizeof(MSG), 0);
        }

        msg.type = yes;
    }
    else
    {

        printf("上架商品执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}
void Update_UserCM(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[900];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("User.db", &db);
    if (data < 0)
    {
        printf("创建数据表User.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {
        USER *ac = (USER *)msg.data;
        printf("更新：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n", ac->account, ac->password, ac->Nick, ac->Identity, ac->Address);

        // sprintf(sql, "SELECT * FROM User WHERE Account = '%s' ;", ac->account);

        // data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
        // if (msg_table == NULL)
        // {
        //     // 数据库操作成功后的处理逻辑
        // }
        // else
        // {
        //     // 数据库操作失败后的处理逻辑
        //     printf("查询库存语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        // }

        // int newquantity = atoi(Result[column + 0]) - od->Quantity;
        // printf("%s %d %d\n", Result[column + 0], od->Pro_ID, newquantity);

        sprintf(sql, "UPDATE User SET Password = '%s' , Nick ='%s' , Address = '%s' WHERE Account = '%s' ;", ac->password, ac->Nick, ac->Address, ac->account);
        sqlite3_exec(db, sql, 0, 0, &msg_table);

        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (msg_table == NULL)
        {
            printf("更新成功\n");
            // 数据库操作成功后的处理逻辑
            msg.type = yes;
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("更新用户语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    send(fd, &msg, sizeof(MSG), 0);

    sqlite3_free(msg_table);
    sqlite3_close(db);
}
void List_User_my(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1500];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("User.db", &db);
    if (data < 0)
    {
        printf("打开数据表User.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    sprintf(sql, "SELECT * FROM User WHERE Account = '%s';", msg.data);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        USER ac;

        // for (int i=0;i<6;i++){
        //     printf("%s\n",Result[( 1) * column + i]);
        // }

        strcpy(ac.Nick, Result[(1) * column + 1]);
        strcpy(ac.account, Result[(1) * column + 2]);
        strcpy(ac.password, Result[(1) * column + 3]);
        strcpy(ac.Identity, Result[(1) * column + 4]);
        strcpy(ac.Address, Result[(1) * column + 5]);
        //    printf("查询：账号 %s 新密码：%s 新昵称：%s 身份：%s 地址：%s\n",ac.account,ac.password,ac.Nick,ac.Identity,ac.Address);

        memcpy(msg.data, &ac, sizeof(USER));

        msg.type = yes;
    }
    else
    {

        printf("查询用户执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void CREATE_POSTS()
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Posts.db", &db);
    if (data < 0)
    {
        printf("创建数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS Posts(\
        ID INTEGER PRIMARY KEY AUTOINCREMENT,\
  Type VARCHAR(30) NOT NULL,\
    Title VARCHAR(128) NOT NULL,\
    Content VARCHAR(400) NOT NULL,\
    Poster VARCHAR(128) NOT NULL,\
    SendTime VARCHAR(50) NOT NULL,\
    Likes INTEGER NOT NULL,\
    CommentCount INTEGER NOT NULL\
);";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("创建posts表失败,错误信息:%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void ADD_POSTS(int fd, MSG msg)
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1500];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Posts.db", &db);
    if (data < 0)
    {
        printf("打开数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    POST po = *(POST *)msg.data;

    //   sql = "CREATE TABLE IF NOT EXISTS Posts(\
    //   ID INTEGER PRIMARY KEY AUTOINCREMENT
    // PostID VARCHAR(50) NOT NULL,\
    // Type VARCHAR(30) NOT NULL,\
    // Title VARCHAR(128) NOT NULL,\
    // Content VARCHAR(400) NOT NULL,\
    // Poster VARCHAR(128) NOT NULL,\
    // SendTime VARCHAR(50) NOT NULL,\
    // Likes INTEGER NOT NULL,\
    // CommentCount INTEGER NOT NULL\

    printf("帖子:类型 %s 标题 %s 内容 %s 发送者 %s 发送时间%s 点赞量%d 评论数 %d\n", po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

    sprintf(sql, "INSERT INTO Posts (Type,Title,Content,Poster,SendTime,Likes,CommentCount) VALUES ( '%s','%s','%s','%s','%s',%d,%d);", po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("注册语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void LIST_POSTS(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Posts.db", &db);
    if (data < 0)
    {
        printf("打开数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    strcpy(sql, "SELECT * FROM Posts");

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        POST po;
        for (int i = 0; i < row; i++)
        {

            po.post_id = atoi(Result[(i + 1) * column + 0]);
            strcpy(po.Type, Result[(i + 1) * column + 1]);
            strcpy(po.title, Result[(i + 1) * column + 2]);
            strcpy(po.content, Result[(i + 1) * column + 3]);
            strcpy(po.poster, Result[(i + 1) * column + 4]);
            strcpy(po.poster_time, Result[(i + 1) * column + 5]);
            po.likes = atoi(Result[(i + 1) * column + 6]);
            po.comment_count = atoi(Result[(i + 1) * column + 7]);

            printf("帖子: ID:%d 类型 %s 标题 %s 内容 %s 发送者 %s 发送时间%s 点赞量%d 评论数 %d\n", po.post_id, po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

            memcpy(msg.data, &po, sizeof(POST));
            msg.type = list_ing;
            send(fd, &msg, sizeof(MSG), 0);
        }

        msg.type = yes;
    }
    else
    {

        printf("进贴执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void search_post(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Posts.db", &db);
    if (data < 0)
    {
        printf("打开数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    char IDN[30];
    strcpy(IDN, msg.data);
    sprintf(sql, "SELECT * FROM Posts WHERE ID ='%s';", IDN);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        if (row == 0)
        {
            msg.type = kong;
            send(fd, &msg, sizeof(MSG), 0);
            sqlite3_free(msg_table);
            sqlite3_close(db);
            return;
        }
        else
        {
            POST po;
            for (int i = 0; i < row; i++)
            {

                po.post_id = atoi(Result[(i + 1) * column + 0]);
                strcpy(po.Type, Result[(i + 1) * column + 1]);
                strcpy(po.title, Result[(i + 1) * column + 2]);
                strcpy(po.content, Result[(i + 1) * column + 3]);
                strcpy(po.poster, Result[(i + 1) * column + 4]);
                strcpy(po.poster_time, Result[(i + 1) * column + 5]);
                po.likes = atoi(Result[(i + 1) * column + 6]);
                po.comment_count = atoi(Result[(i + 1) * column + 7]);

                printf("帖子: ID:%d 类型 %s 标题 %s 内容 %s 发送者 %s 发送时间%s 点赞量%d 评论数 %d\n", po.post_id, po.Type, po.title, po.content, po.poster, po.poster_time, po.likes, po.comment_count);

                memcpy(msg.data, &po, sizeof(POST));
            }

            msg.type = yes;
        }
    }
    else
    {

        printf("进贴执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void CREATE_COMMENTS()
{
    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char *sql;
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Comments.db", &db);
    if (data < 0)
    {
        printf("创建数据表Comments.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {

        sql = "CREATE TABLE IF NOT EXISTS Comments(\
        ID INTEGER PRIMARY KEY AUTOINCREMENT,\
        PostID INTEGER NOT NULL ,\
        Content VARCHAR(400) NOT NULL,\
        Author VARCHAR(128) NOT NULL,\
        SendTime VARCHAR(50) NOT NULL\
        );";
        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (result == SQLITE_OK)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("创建Comments表失败,错误信息:%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void LIST_COOMENTS(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Comments.db", &db);
    if (data < 0)
    {
        printf("打开数据表Comments.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    char postidnow[30];
    strcpy(postidnow, msg.data);
    sprintf(sql, "SELECT * FROM Comments WHERE PostID = '%s';", postidnow);

    data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

    msg.type = kong;

    printf("%d %d\n", row, column);
    sleep(1);
    if (msg_table == NULL)
    {
        if (row == 0)
        {
            msg.type = kong;
            send(fd, &msg, sizeof(MSG), 0);
            sqlite3_free(msg_table);
            sqlite3_close(db);
            msg.type = yes;
        }
        else
        {
            COMMENT co;
            for (int i = 0; i < row; i++)
            {

                co.comment_id = atoi(Result[(i + 1) * column + 0]);
                co.post_id = atoi(Result[(i + 1) * column + 1]);
                strcpy(co.connent, Result[(i + 1) * column + 2]);
                strcpy(co.author_ac, Result[(i + 1) * column + 3]);
                strcpy(co.comment_time, Result[(i + 1) * column + 4]);
                co.comment_id = i + 2;
                printf("帖子编号 %d  评论者：%s\n内容:%s \n 发送时间：%s \n", co.post_id, co.author_ac, co.connent, co.comment_time);

                memcpy(msg.data, &co, sizeof(COMMENT));
                msg.type = list_ing;
                send(fd, &msg, sizeof(MSG), 0);
            }

            msg.type = yes;
        }
    }
    else
    {

        printf("进贴执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}
void ADD_COMMMENT(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1500];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Comments.db", &db);
    if (data < 0)
    {
        printf("打开数据表Comments.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    COMMENT co = *(COMMENT *)msg.data;

    //  sql = "CREATE TABLE IF NOT EXISTS Comments(\
    //     ID INTEGER PRIMARY KEY AUTOINCREMENT,\
    //     PostID INTEGER NOT NULL ,\
    //     Content VARCHAR(400) NOT NULL,\
    //     Author VARCHAR(128) NOT NULL,\
    //     SendTime VARCHAR(50) NOT NULL\
    //     );";

    printf(" 主贴 %d 内容 %s 发送者 %s 发送时间%s \n", co.post_id, co.connent, co.author_ac, co.comment_time);

    sprintf(sql, "INSERT INTO Comments (PostID,Content,Author,SendTime) VALUES ( %d,'%s','%s','%s');", co.post_id, co.connent, co.author_ac, co.comment_time);

    int result = sqlite3_exec(db, sql, 0, 0, &msg_table);

    msg.type = kong;
    if (msg_table == NULL)
    {

        msg.type = yes;
    }
    else
    {

        printf("注册语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        msg.type = no;
    }

    send(fd, &msg, sizeof(MSG), 0);
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void UPDATE_COCOUNT(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[200];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    // 打开指定的数据库文件，如果不存在将创建一个同名的数据库文件
    data = sqlite3_open("Comments.db", &db);
    if (data < 0)
    {

        printf("创建数据表Comments.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }
    else
    {
        int now_postid;
        now_postid = atoi(msg.data);

        printf("%d\n", now_postid);
        sprintf(sql, "SELECT COUNT(*) FROM Comments WHERE PostID = %d; ;", now_postid);
        int newcount;
        data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);
        if (msg_table == NULL)
        {
            // 数据库操作成功后的处理逻辑
            newcount = atoi(Result[1]);
            printf("更新后本帖行数%d", newcount);
        }
        else
        {
            sqlite3_free(msg_table);
            sqlite3_close(db);
            // 数据库操作失败后的处理逻辑
            printf("查询库存语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
            return;
        }

        sqlite3_free(msg_table);
        sqlite3_close(db);

        data = sqlite3_open("Posts.db", &db);
        if (data < 0)
        {

            printf("创建数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
            return;
        }

        sprintf(sql, "UPDATE Posts SET CommentCount = %d WHERE ID = %d ;", newcount, now_postid);
        sqlite3_exec(db, sql, 0, 0, &msg_table);

        int result = sqlite3_exec(db, sql, 0, 0, &msg_table);
        if (msg_table == NULL)
        {
            // 数据库操作成功后的处理逻辑
        }
        else
        {
            // 数据库操作失败后的处理逻辑
            printf("更新库存语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }
    }
    sqlite3_free(msg_table);
    sqlite3_close(db);
}

void ADD_LIKE(int fd, MSG msg)
{

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果

    data = sqlite3_open("Posts.db", &db);
    if (data < 0)
    {
        printf("打开数据表Posts.db失败:%s\n", sqlite3_errmsg(db));
        return;
    }

    char postidnow[30];

    strcpy(postidnow, msg.data);
    sprintf(sql, "UPDATE Posts SET Likes = Likes + 1 WHERE ID = %s ;", postidnow);
    sqlite3_exec(db, sql, 0, 0, &msg_table);
}

void *pupdate_nrcount(void *p)
{

    // 将void *类型的指针转换为char *类型，因为假设它指向一个字符串
    NP np = *(NP *)p;

    // char part1[len - 1 + 1]; // 定义足够空间存储除最后一个字符外的部分，len - 1获取前面部分字符数量，+1为字符串结束符预留空间
    // char part2[2];
    // // 复制除最后一个字符外的部分到part1，并添加字符串结束符
    // strncpy(part1, str, len - 1);
    // part1[len - 1] = '\0';
    // // 获取最后一个字符并存储到part2，添加字符串结束符
    // part2[0] = str[len - 1];
    // part2[1] = '\0';

    // printf("拆分后的两个部分：%s %s\n", part1, part2);

    sqlite3 *db = NULL;
    int data = -1;
    char *msg_table = 0;
    char sql[1024];
    int row = 0, column = 0; // 行   列
    char **Result = NULL;    // 结果
    MSG msg;
    msg.type = kong;

    data = sqlite3_open("ChatRecords.db", &db);
    if (data < 0)
    {
        printf("打开数据表ChatRecords.db失败:%s\n", sqlite3_errmsg(db));
        return NULL;
    }

    while (1)
    {

        // 查询全部
        sprintf(sql, "SELECT  COUNT(*) as unreadCount FROM ChatRecords WHERE Destname= '%s' AND IsRead = '未读' GROUP BY SelfName;", np.name);

        data = sqlite3_get_table(db, sql, &Result, &row, &column, &msg_table);

        if (msg_table == NULL)
        {

            if (row == 0)
            {
                // printf("1\n");
                strcpy(msg.data, "0");
            }
            else
            {
                // printf("2\n");
                strcpy(msg.data, Result[1]);
            }
            unsigned long pt_value = (unsigned long)pt[pt_i - 1];
            printf("未读数量 %s fd %d pt :%lu\n", msg.data, np.fd, np.pt);

            msg.type = list_ing;
            send(np.fd, &msg, sizeof(MSG), 0);
            pthread_testcancel();
        }
        else
        {
            printf("更新未读语句执行失败，错误信息：%s\n", sqlite3_errmsg(db));
        }

        sleep(2);
    }
    printf("线程已结束\n");

    pthread_exit(NULL);

    sqlite3_free(msg_table);
    sqlite3_close(db);
}
void cancel_pth(int fd)
{

    for (int i = 0; i < pt_i; i++)
    {

        if (np[i].fd == fd)
        {

            printf("尝试取消fd:%d的线程\n", fd);
            pthread_cancel(np[i].pt);

            pthread_join(np[i].pt, NULL);
        }
    }
}