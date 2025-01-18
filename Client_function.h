#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "Structs.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <wchar.h>
#include <locale.h>
int init_net();
void choice_main_C(int sID,char *buf);

void choice_main_M(int sID,char *buf);
void choice_QueryPurchase_M(int sID,char * buf);
char * insert_arraystr(char * buf,int a);
int check_arraystr(char * buf,int a);
void choice_main_U(int sID,char * buf);
void choice_QueryPurchase_U(int sID,char * buf);
char *getTime();
void createOrderNumber(char * num);