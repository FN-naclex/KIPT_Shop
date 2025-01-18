#include <time.h>
typedef enum
{
    regist,
    login,
    yes,
    no,
    kong,
    go_main_C,
    go_main_M,
    put_product,
    list_my_product,
    list_ing,
    search_totalpro,
    single_chat,
    recv_chat,
    queryrecord,
    pre_list,
    log_off,
    add_order,
    search_myUser,
    search_totalodU,
    search_totalodM,
    Update_User,
    add_post,
    search_totalPT,
    Join_Post,
    search_totalCOM,
    add_comment,
    update_commentcount,
    add_likes,
    update_nr,
    cancel_p,
    


} MY_TYPE;

typedef struct msg
{
    MY_TYPE type;
    char data[1020];
} MSG;

typedef struct user
{
    char Nick[128];
    char account[128];
    char password[128];
    char Identity[128];
    char Address[128];

} USER;

typedef struct products
{
    char product_Name[128];
    char M_name[128];
    double Price;
    int Quantity;
    char State[20];
    int ID;
    char Details[600];
} PD;

typedef struct chat
{
    char self_name[128];
    char dest_name[128];
    char text[700];
    char read[20];
    char send_time[50];
    int noreadcount;
} CHAT;

typedef struct nr
{
    char name[128];
    int countnr;
} NR;

typedef struct Conversationlist
{
    struct nr other;
    int num;

} CLIST;
typedef struct loginarr
{
    int fd;
    char ac[128];
} LGARR;

typedef struct order
{
    char product_Name[128];
    char M_name[128];
    char C_name[128];
    double Price;
    double Tol_Price;
    int Quantity;
    char State[20];
    char num[40];
    char Pro_ID;
} ODR;

typedef struct post
{
    int  post_id;
    char title[128];
    char Type[30];
    char poster[128];
    char poster_time[50];
    char content[400];
    int likes;
    int comment_count;
} POST;

typedef struct comment
{
    int comment_id;
    int post_id;
    char author_ac[128];
    char connent[400];
    char comment_time[50];

} COMMENT;

typedef struct nr_pt{

    pthread_t pt;
    int fd;
    char name[128];
}NP;