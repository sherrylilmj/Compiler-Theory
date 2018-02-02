#ifndef SYMBOL_H_INCLUDED
#define SYMBOL_H_INCLUDED
#include<stdio.h>
#include<string.h>
#include <stdbool.h>
#define maxn 7
#define len 50
#define maxx 1000
struct Node{
    int child[maxn];//孩子节点
    int line;//所在行号
    char type[len];//内容
    int child_num;
    char cont[len];
    int t;
    int brother;// bro
    int tag;//1 变量 2 函数 3 常数 4 数组 5 结构体
    char s[len];//语法单元的名字
    char ty[len];//数据类型,主要用于等号和操作符左右类型匹配判断
    char content[10][10];//函数所有形参名字或结构体中所有变量名字
    int ini;//是否被定义
    float value;//常数值
    int val;
    int num;//函数所含形参个数或结构体所含变量个数
}node[maxx];

struct quater{
 char target[10];
 char op;
 char arg1[10];
 char arg2[10];
 struct quater *next;
};

struct temp_var{
    char name[10];
    int offset;
    struct temp_var *next;
};

struct intercode{
    char content[1024];
    struct intercode* next;
};

struct objectcode{
    char content[1024];
    struct ojectcode* next;
};
enum sym_type {
    S_INT, S_FLOAT, 
    S_VAR, S_ARRAY,
};

struct sym_node {
    enum sym_type symtype;
    char name[255];
    int number;
    int size;
    int lineno;
    int array_size;
    struct sym_node *next;
    struct sym_node *parent;
    struct sym_node *type;
    bool param;
};

struct fun_node{
    char name[255];
    struct sym_node *ret;
    int size;
    struct sym_node *param;
    struct fun_node *next;
    int lineno;
};

struct float_node {
    float v;
    struct float_node *n;
};

struct exp_type {
    bool isLeftVal;
    struct sym_node *leftVal;
    struct sym_node *prop;
};
#endif // SYMBOL_H_INCLUDED
//var
//array
//func