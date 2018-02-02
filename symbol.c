#include<math.h>
#include<stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include "syntax.tab.h"
#include "symbol.h"
extern int yylineno;
extern char* yytext;
FILE *fp;
int lineno=1;
int labelcount=0;
int tempcount=0;
int global_num=0;
int reg_count=0;
int func_offset=0;
struct sym_node *propTable = NULL, *varTable = NULL;
struct fun_node *funcTable = NULL;
struct float_node *flconst = NULL, *fltail = NULL; 
struct fun_node *curFunc = NULL;
struct fun_node *trans_curFunc = NULL;
struct intercode* trans_program(int x);
struct intercode* trans_args(int x);
struct intercode* trans_comp_st(int x);
struct temp_var *temp_head=NULL;
int argList[1024];
int temp_var_count=0;
int arg_len=0;
extern int cnt;
void open_file(){
    fp=fopen("final.asm","w");
}
void close_file(){
    fclose(fp);
}
int new_tmp(){
    ++tempcount;
    return tempcount;
}
int new_label(){
    ++labelcount;
    return labelcount;
}
struct temp_var* add_temp_var(char *x){
    struct temp_var *temp=(struct temp_var *) malloc(sizeof(struct temp_var));
    if(temp_var_count==0){
        strcpy(temp->name,x);
        temp->offset=0;
        temp_head=temp;
        temp_head->next=NULL;
        temp_var_count++;
    }
    else{
        int i=0;
        struct temp_var *t=temp_head;
        for(i=0;i<temp_var_count-1;i++){
            t=t->next;
        }
        strcpy(temp->name,x);
        temp->offset=temp_var_count*4;
        t->next=temp;
        temp->next=NULL;
        temp_var_count++;
    }
    return temp;
}
struct temp_var* find_temp_var(char *x){
    int i=0;
    struct temp_var *temp=temp_head;
    if(temp_var_count!=0&&strcmp(temp->name,x)==0) return temp_head;
    for(i=0;i<temp_var_count;i++){
        if(strcmp(temp->name,x)==0) return temp;
        temp=temp->next;
    }
    return NULL;
}
char *reg(char *x){
    char regg[10];
    sprintf(regg,"$t%d",reg_count);
    reg_count++;
    return regg;
}
void print_oject_code(){
    fprintf(fp,".data\n");
    fprintf(fp,"_prompt: .asciiz \"Enter an integer:\"\n");
    fprintf(fp,"_ret: .asciiz \"\\n\"\n");
    fprintf(fp,".globl main\n");
    fprintf(fp,".text\n");
    fprintf(fp,"read:\n");
    fprintf(fp,"    li $v0, 4\n");
    fprintf(fp,"    la $a0, _prompt\n");
    fprintf(fp,"    syscall\n");
    fprintf(fp,"    li $v0, 5\n");
    fprintf(fp,"    syscall\n");
    fprintf(fp,"    jr $ra\n");
    fprintf(fp,"\n");
    fprintf(fp,"write:\n");
    fprintf(fp,"    li $v0, 1\n");
    fprintf(fp,"    syscall\n");
    fprintf(fp,"    li $v0, 4\n");
    fprintf(fp,"    la $a0, _ret\n");   
    fprintf(fp,"    syscall\n");
    fprintf(fp,"    move $v0, $0\n");
    fprintf(fp,"    jr $ra\n");
    fprintf(fp,"\n");
}

struct intercode* merge(struct intercode* i1,struct intercode* i2){
    if(i1==NULL){
        if(i2==NULL){
            return NULL;
        }
        return i2;
    }
    struct intercode* temp=i1;
    while(temp->next){
        temp=temp->next;
    }
    temp->next=i2;
    return i1;
};

void print_intercode(struct intercode* i){
    struct intercode* temp=i;
    while(temp!=NULL){
        printf("%s",temp->content);
        temp=temp->next;
    }
}

void print_sym(){
    struct sym_node *head=propTable;
    while(head!=NULL){
        printf("%s\n",head->name);
        head=head->next;
    }
    printf("-----\n");
    head=varTable;
    while(head!=NULL){
        printf("%s\n",head->name);
        head=head->next;
    }
    printf("-----\n");
   struct fun_node *head_func=funcTable;
    while(head!=NULL){
        printf("%s\n",head->name);
        head=head->next;
    }
}
int newtree0(int cnt,char *str,int line,char *s,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,s);
    strcpy(node[cnt].s,s);
    if(!strcmp(node[cnt].type,"INT")){
        strcpy(node[cnt].ty,"int");
        node[cnt].val=atoi(yytext);
    }
    else if(!strcmp(node[cnt].type,"FLOAT")){
        strcpy(node[cnt].ty,"float");
        node[cnt].value=atof(yytext);
    }
    else if(!strcmp(node[cnt].type,"TYPE")){
        strcpy(node[cnt].ty,yytext);
    }
    else{
        strcpy(node[cnt].s,s);
    }
    //printf("%s\n",node[cnt].s);
    node[cnt].line=line;
    node[cnt].t=t;
    node[cnt].child_num=0;
    return cnt;
}
int newtree1(int cnt,int x,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[node[cnt].child[0]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<1; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    //one child
    strcpy(node[cnt].type,node[node[cnt].child[0]].type);
    node[cnt].tag=node[node[cnt].child[0]].tag;
    strcpy(node[cnt].ty,node[node[cnt].child[0]].ty);
    node[cnt].child_num=1;
    return cnt;
}
int newtree2(int cnt,int x,int y,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<2; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    node[cnt].child_num=2;
    return cnt;
}
int newtree3(int cnt,int x,int y,int z,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[cnt].child[2]=z;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=z;
    node[node[cnt].child[2]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<3; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    node[cnt].child_num=3;
    return cnt;
}
int newtree4(int cnt,int x,int y,int z,int w,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[cnt].child[2]=z;
    node[cnt].child[3]=w;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=z;
    node[node[cnt].child[2]].brother=w;
    node[node[cnt].child[3]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<4; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    node[cnt].child_num=4;
    return cnt;
}
int newtree5(int cnt,int x,int y,int z,int w,int p,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[cnt].child[2]=z;
    node[cnt].child[3]=w;
    node[cnt].child[4]=p;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=z;
    node[node[cnt].child[2]].brother=w;
    node[node[cnt].child[3]].brother=p;
    node[node[cnt].child[4]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<5; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    node[cnt].child_num=5;
    return cnt;
}
int newtree6(int cnt,int x,int y,int z,int w,int p,int r,char *s,int line,char *str,int t)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[cnt].child[2]=z;
    node[cnt].child[3]=w;
    node[cnt].child[4]=p;
    node[cnt].child[5]=r;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=z;
    node[node[cnt].child[2]].brother=w;
    node[node[cnt].child[3]].brother=p;
    node[node[cnt].child[4]].brother=r;
    node[node[cnt].child[5]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<6; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=t;
    node[cnt].child_num=6;
    return cnt;
}
int newtree7(int cnt,int x,int y,int z,int w,int p,int r,int t,char *s,int line,char *str,int tt)
{
    int i=0;
    for(i=0; i<maxn; i++)
    {
        node[cnt].child[i]=-1;
    }
    node[cnt].child[0]=x;
    node[cnt].child[1]=y;
    node[cnt].child[2]=z;
    node[cnt].child[3]=w;
    node[cnt].child[4]=p;
    node[cnt].child[5]=r;
    node[cnt].child[6]=t;
    node[node[cnt].child[0]].brother=y;
    node[node[cnt].child[1]].brother=z;
    node[node[cnt].child[2]].brother=w;
    node[node[cnt].child[3]].brother=p;
    node[node[cnt].child[4]].brother=r;
    node[node[cnt].child[5]].brother=t;
    node[node[cnt].child[6]].brother=-1;
    node[cnt].line=line;
    for(i=0; i<7; i++)
    {
        if(node[cnt].line>node[node[cnt].child[i]].line)node[cnt].line=node[node[cnt].child[i]].line;
    }
    strcpy(node[cnt].s,s);
    strcpy(node[cnt].type,str);
    strcpy(node[cnt].cont,str);
    node[cnt].t=tt;
    node[cnt].child_num=7;
    return cnt;
}
void init()
{
    int i=0;
    int j=0;
    for(i=0; i<1000; i++)
    {
        for(j=0; j<maxn; j++)
            node[i].child[j]=-1;
    }

}
void dfs(int root,int h)
{
    int i=0;
    int flag=1;
    for(i=0; i<maxn; i++)
    {
        if(node[root].child[i]!=-1)
        {
            flag=0;
        }
    }
    if(!flag)
    {
        for(i=0; i<h; i++)
        {
            printf("  ");
        }
        printf("%s (%d)\n",node[root].cont,node[root].line);
    }
    else
    {
        if(node[root].t==0)
        {
            for(i=0; i<h; i++)
            {
                printf("  ");
            }
            printf("%s: %s\n",node[root].cont,node[root].s);
            lineno=node[root].line;
        }
    }
    for(i=0; i<maxn; i++)
    {
        if(node[root].child[i]!=-1)
        {
            dfs(node[root].child[i],h+1);
        }
    }
    return;
}


struct sym_node *createsym_node(enum sym_type symtype, struct sym_node *type, const char *name, struct sym_node *next, int lineno, struct sym_node *parent) {
    struct sym_node *temp;
    struct sym_node *n = (struct sym_node *) malloc(sizeof(struct sym_node));
    n->symtype = symtype;
    n->type = type;
    strcpy(n->name, name);
    n->size = 0;
    n->next = next;
    n->lineno = lineno;
    n->param = false;
    n->parent = parent;
    n->number=global_num;
    return n;
}

void initSymTable() {
    propTable = createsym_node(S_FLOAT, NULL, "float", NULL, 0, NULL);
    propTable = createsym_node(S_INT, NULL, "int", propTable, 0, NULL);
}

struct fun_node *lookupFunc(const char *s) {
    struct fun_node *p = funcTable;
    while (p != NULL) {
        if (strcmp(p->name, s) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

struct sym_node *lookupSym(struct sym_node *n, const char *s) {
    if (!s) return NULL;
    struct sym_node *p = n;
    while (p != NULL) {
        if (p->name && strcmp(p->name, s) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

void addFunc(struct fun_node *f) {
    if (!f) return;
    f->next = funcTable;
    funcTable = f;
}

struct sym_node *addTableItem(struct sym_node **table, struct sym_node *n) {
    if (!n) return *table;
    struct sym_node *p = n;
    while (p->next != NULL) {
        p = p->next;
    }
    p->next = *table;
    return *table = n;
}

void addProp(struct sym_node *n) {
    if (!n) return;
    addTableItem(&propTable, n);
}

void addVar(struct sym_node *n) {
    global_num++;
    if (!n) return;
    addTableItem(&varTable, n);
}

struct sym_node *sym_specifier(int x);
struct sym_node *sym_var_dec(struct sym_node *type, int x);
void sym_comp_st(int x);
struct exp_type sym_exp(int x);

struct exp_type initExpType(struct sym_node *leftVal, struct sym_node *prop) {
    struct exp_type type;
    type.isLeftVal = (leftVal != NULL);
    type.leftVal = leftVal;
    type.prop = prop;
    return type;
}

bool sym_args(int x, struct sym_node *n, struct fun_node *f, int lv) {
    bool ret = true;
    if (strcmp(node[x].s,"Args")==0) {
        struct exp_type type;
        if (node[x].child_num==3) {
            type = sym_exp(node[x].child[0]);
            ret = sym_args(node[x].child[2], n->next, f, lv + 1);
        } else if (node[x].child_num==1) {
            type = sym_exp(node[x].child[0]);
            if (lv < f->size) {
                return false;
            }
        }
        if (n == NULL) {
            return false;
        }
        if (type.prop == n->type) {
            return ret;
        } else {
            return false;
        }
    }
}

struct exp_type sym_exp(int x) {
    if (node[x].child_num==3&&strcmp(node[node[x].child[1]].s,"ASSIGNOP")==0) {
        struct exp_type type = sym_exp(node[x].child[0]);
        struct exp_type type2 = sym_exp(node[x].child[2]);
        return initExpType(NULL, type.prop);
    } else if (node[x].child_num==3&&(strcmp(node[node[x].child[1]].s,"AND")==0||strcmp(node[node[x].child[1]].s,"OR")==0)){
        struct exp_type type = sym_exp(node[x].child[0]);
        struct exp_type type2 = sym_exp(node[x].child[2]);
        return initExpType(NULL, type.prop);
    } else if (node[x].child_num==3&&(strcmp(node[node[x].child[1]].s,"RELOP")==0||strcmp(node[node[x].child[1]].s,"PLUS")==0||strcmp(node[node[x].child[1]].s,"MINUS")==0||strcmp(node[node[x].child[1]].s,"STAR")==0||strcmp(node[node[x].child[1]].s,"DIV")==0)) {
        struct exp_type type = sym_exp(node[x].child[0]);
        struct exp_type type2 = sym_exp(node[x].child[2]);
        return initExpType(NULL, type.prop);
    } else if (node[x].child_num==3&&strcmp(node[node[x].child[1]].s,"Exp")==0) {
        return sym_exp(node[x].child[1]);
    } else if (node[x].child_num==2&&strcmp(node[node[x].child[0]].s,"MINUS")==0) {
        struct exp_type type = sym_exp(node[x].child[1]);
        return initExpType(NULL, type.prop);
    } else if (node[x].child_num==2&&strcmp(node[node[x].child[0]].s,"NOT")==0) {
        struct exp_type type = sym_exp(node[x].child[1]);
        return initExpType(NULL, type.prop);
    } else if (node[x].child_num>=3&&strcmp(node[node[x].child[1]].s,"LP")==0) {
        if (node[x].child_num==4) {
            struct fun_node *f = lookupFunc(node[node[x].child[0]].type);
            sym_args(node[x].child[2], f->param, f, 1);
            return initExpType(NULL, f->ret);
        } else if (node[x].child_num==3) {
            struct fun_node *f = lookupFunc(node[node[x].child[0]].type);
            return initExpType(NULL, f->ret);
        }
        return initExpType(NULL, NULL);
    } else if (node[x].child_num==4&&strcmp(node[node[x].child[0]].s,"Exp")==0) {
        struct exp_type type = sym_exp(node[x].child[0]);
        struct exp_type type2 = sym_exp(node[x].child[2]);
        return initExpType(type.leftVal->type, type.leftVal->type->type);
    } else if (strcmp(node[node[x].child[0]].s,"ID")==0) {
        struct sym_node *n = lookupSym(varTable, node[node[x].child[0]].type);
        return initExpType(n, n->type);
    } else if (strcmp(node[node[x].child[0]].s,"INT")==0) {
        struct sym_node *n = lookupSym(propTable, "int");     
        return initExpType(NULL,n);
    } else if (strcmp(node[node[x].child[0]].s,"FLOAT")==0) {
        struct float_node *fl = (struct float_node *) malloc(sizeof(struct float_node));
        fl->v = node[node[x].child[0]].value;
        fl->n = NULL;
        if (fltail) {
            fltail->n = fl;
            fltail = fl;
        } else {
            fltail = flconst = fl;
        }
        return initExpType(NULL, lookupSym(propTable, "float"));
    }
}

struct sym_node *sym_dec(struct sym_node *type, int x) {
    if (strcmp(node[x].s,"Dec")==0) {
        if (node[x].child_num == 1) {
            struct sym_node *n = sym_var_dec(type, node[x].child[0]);
            addVar(n);
            return n;
        } else if (node[x].child_num ==3) {
            struct sym_node *n = sym_var_dec(type, node[x].child[0]);
            struct exp_type exptype = sym_exp(node[x].child[2]);
            addVar(n);
            return n;
        }
    }
}

struct sym_node *sym_dec_list(struct sym_node *type, int x) {
    if (strcmp(node[x].s,"DecList")==0) {
        if (node[x].child_num==1) {
            return sym_dec(type, node[x].child[0]);
        } else if (node[x].child_num==3) {
            struct sym_node *n = sym_dec_list(type, node[x].child[2]);
            struct sym_node *n2 = sym_dec(type, node[x].child[0]);
            struct sym_node *temp=varTable;
            return n2;
        }
    }
    return NULL;
}

struct sym_node *sym_def(int x) {
    if (strcmp(node[x].s,"Def")==0) {
        struct sym_node *type = sym_specifier(node[x].child[0]);
        struct sym_node *n = sym_dec_list(type, node[x].child[1]);
        return n;
    }
    return NULL;
}

struct sym_node *sym_def_list(int x) {
    if (strcmp(node[x].s,"DefList")==0) {
        if (node[x].child_num == 2) {
            struct sym_node *n = sym_def(node[x].child[0]);
            struct sym_node *n2 = sym_def_list(node[x].child[1]);
            return n2;
        }
    }
    return NULL;
}

struct sym_node *sym_specifier(int x) {
    if (strcmp(node[node[x].child[0]].s,"Type")==0) {
        return lookupSym(propTable, node[node[x].child[0]].type);
    } else if (strcmp(node[node[x].child[0]].s,"Struct")==0) {
    }
    return NULL;
}

struct sym_node *sym_var_dec(struct sym_node *type, int x) {
    if (strcmp(node[x].s,"VarDec")==0) {
        if (node[x].child_num==1) {
            return createsym_node(S_VAR, type, node[node[x].child[0]].type, NULL, node[node[x].child[0]].line, NULL);
            struct sym_node *n = sym_var_dec(type, node[x].child[0]);
            if (!n) return NULL;
            struct sym_node *ret = createsym_node(S_ARRAY, n, n->name, NULL, node[node[x].child[0]].line, NULL);
            n->parent = ret;
            ret->size = node[node[x].child[1]].val;
            return ret;
        }
    }
    return NULL;
}

void sym_ext_dec_list(struct sym_node *type, int x) {
    if (strcmp(node[x].s,"ExtDecList")==0) {
        if (node[x].child_num==1) {
            addVar(sym_var_dec(type, node[x].child[0]));
        } else if (node[x].child_num==3) {
            addVar(sym_var_dec(type, node[x].child[0]));
            sym_ext_dec_list(type, node[x].child[2]);
        }
    }
}

struct fun_node *createFunNode(const char *name, struct sym_node *ret, int size, struct sym_node *param, int lineno) {
    struct fun_node *f = (struct fun_node *) malloc(sizeof(struct fun_node));
    struct fun_node *temp;
    if (temp = lookupFunc(name)) {
        f->name[0] = '\0';
    } else {
        strcpy(f->name, name);
    }
    f->ret = ret;
    f->size = size;
    f->param = param;
    f->next = NULL;
    f->lineno = lineno;
    return f;
}

struct sym_node *sym_param_dec(int x) {
    if (strcmp(node[x].s,"ParamDec")==0) {
        struct sym_node *type = sym_specifier(node[x].child[0]);
        struct sym_node *n = sym_var_dec(type, node[x].child[1]);
        return n;
    }
    return NULL;
}

struct sym_node *sym_var_list(int x, int *ct) {
    if (strcmp(node[x].s,"VarList")==0) {
        if (node[x].child_num==3) {
            int l;
            struct sym_node *n = sym_param_dec(node[x].child[0]);
            struct sym_node *n2 = sym_var_list(node[x].child[2], &l);
            *ct = l + 1;
            addTableItem(&n2, n);
            return n2;
        } else if (node[x].child_num==1) {
            *ct = 1;
            return sym_param_dec(node[x].child[0]);
        }
    }
}

struct fun_node *sym_fun_dec(int x) {
    if (strcmp(node[x].s,"FunDec")==0) {
        if (node[x].child_num==4) {
            int ct;
            struct sym_node *n = sym_var_list(node[x].child[2], &ct);
            addVar(n);
            struct fun_node *f = createFunNode(node[node[x].child[0]].type, NULL, ct, n, node[x].line);
            return f;
        } else if (node[x].child_num==3) {
            // ID LP RP
            struct fun_node *f = createFunNode(node[node[x].child[0]].type, NULL, 0, NULL, node[x].line);
            return f;
        }
    }
}

void sym_stmt(int x) {
    if (strcmp(node[node[x].child[0]].s,"Exp")==0) {
        sym_exp(node[x].child[0]);
    } else if (strcmp(node[node[x].child[0]].s,"CompSt")==0) {
        sym_comp_st(node[x].child[0]);
    } else if (strcmp(node[node[x].child[0]].s,"RETURN")==0) {
        struct exp_type type = sym_exp(node[x].child[1]);
    } else if (node[x].child_num==5&&strcmp(node[node[x].child[0]].s,"IF")==0) {
        struct exp_type type = sym_exp(node[x].child[2]);
        sym_stmt(node[x].child[0]);
    } else if (node[x].child_num==7) {
        struct exp_type type = sym_exp(node[x].child[2]);
        sym_stmt(node[x].child[4]);
        sym_stmt(node[x].child[6]);
    } else if (strcmp(node[node[x].child[0]].s,"WHILE")==0) {
        struct exp_type type = sym_exp(node[x].child[2]);
        sym_stmt(node[x].child[4]);
    }
}

void sym_stmt_list(int x) {
    if (strcmp(node[x].s,"StmtList")==0) {
        if (node[x].child_num==2) {
            sym_stmt(node[x].child[0]);
            sym_stmt_list(node[x].child[1]);
        }
    }
}

void sym_comp_st(int x) {
    if (strcmp(node[x].s,"CompSt")==0) {
        struct sym_node *n = sym_def_list(node[x].child[1]);
        sym_stmt_list(node[x].child[2]);
    }
}

bool sym_ext_def(int x) {
    if (strcmp(node[x].s,"ExtDef")==0) {
        if (strcmp(node[node[x].child[1]].s,"ExtDecList")==0) {
            struct sym_node *type = sym_specifier(node[x].child[0]);
            sym_ext_dec_list(type, node[x].child[1]);
        } else if (node[x].child_num==2) {
            sym_specifier(node[x].child[0]);
        } else if (strcmp(node[node[x].child[1]].s,"FunDec")==0) {
            struct sym_node *ret = sym_specifier(node[x].child[0]);
            struct fun_node *f = sym_fun_dec(node[x].child[1]);
            f->ret = ret;
            addFunc(f);
            curFunc = f;
            sym_comp_st(node[x].child[2]);
            curFunc = NULL;
        }
        return true;
    }
    return false;
}

void sym_ext_def_list(int x) {
        if (node[x].child_num == 2) {
            sym_ext_def(node[x].child[0]);
            sym_ext_def_list(node[x].child[1]);
        }
}

void sym_program(int x) {
    init_sym();
    if (strcmp(node[x].s,"Program")==0) {
        sym_ext_def_list(node[x].child[0]);
    }
    open_file();
    trans_program(x);
    close_file();
}

void init_sym(){
    struct sym_node* ret=lookupSym(propTable,"int");
    struct fun_node* f=createFunNode("read",NULL,0,NULL,0);
    f->ret=ret;
    addFunc(f);
    struct sym_node* type=lookupSym(propTable,"int");
    struct sym_node* n=createsym_node(S_VAR,type,"const",NULL,0,NULL);
    addVar(n);
    f=createFunNode("write",NULL,1,n,0);
    addFunc(f);
}


struct intercode*trans_exp(int x,int place) {
    if (strcmp(node[node[x].child[1]].s,"ASSIGNOP")==0) {
        if(strcmp(node[node[node[x].child[0]].child[0]].s,"ID")==0){
            int t1=new_tmp();
            struct intercode* i1=trans_exp(node[x].child[2],t1);
            struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
            struct sym_node *temp=lookupSym(varTable,node[node[node[x].child[0]].child[0]].type);
            if(place==-1){
                sprintf(i2->content,"v%d := t%d\n",temp->number,t1);
                char xx[10];
                char y[10];
                sprintf(xx,"v%d",temp->number);
                sprintf(y,"t%d",t1);
                trans_move(xx,y);
            }
            else{
                    char xx[10];
                    char y[10];
                    char z[10];
                    sprintf(xx,"v%d",temp->number);
                    sprintf(y,"t%d",t1);
                    sprintf(z,"t%d",place);
                    trans_move(xx,y);
                    trans_move(z,xx);
                    sprintf(i2->content,"v%d := t%d\nt%d := v%d\n",temp->number,t1,place,temp->number);
            }
            i2->next=NULL;
            i1=merge(i1,i2);
            return i1;
        }
    } 
    else if (strcmp(node[node[x].child[1]].s,"Exp")==0) {
        return trans_exp(node[x].child[1],place);
    } 
    else if(node[x].child_num>1&&strcmp(node[node[x].child[0]].s,"ID")==0){
        if(node[x].child_num==3){
            if(!strcmp(node[node[x].child[0]].type,"read")){
                struct intercode* cd=(struct intercode *) malloc(sizeof(struct intercode));
                sprintf(cd->content,"READ t%d\n",place);
                char xx[10];
                sprintf(xx,"t%d",place);
                struct temp_var *t1=find_temp_var(xx);
                if(t1==NULL){
                    t1=add_temp_var(xx);
                }
                fprintf(fp,"addi $sp, $sp, -4\nsw $ra, 0($sp)\njal read\nlw $ra, 0($sp)\naddi $sp, $sp, 4\n");
                fprintf(fp,"move $t1, $v0\n");
                fprintf(fp,"sw $t1, %d($sp)\n",t1->offset);
                cd->next=NULL;
                return cd;
            }
            else{
                struct intercode* cd=(struct intercode *) malloc(sizeof(struct intercode));
                sprintf(cd->content,"t%d := CALL %s\n",place,node[node[x].child[0]].type);
                char xx[10];
                sprintf(xx,"t%d",place);
                fprintf(fp,"addi $sp, $sp, -4\nsw $ra, 0($sp)\n");
                trans_jal(node[node[x].child[0]].type,xx);
                fprintf(fp,"lw $ra, 0($sp)\naddi $sp, $sp, 4\n");
                cd->next=NULL;
                return cd;
            }
        }
        else{
            struct fun_node *f = lookupFunc(node[node[x].child[0]].type);
            arg_len=0;
            struct intercode* i1=trans_args(node[x].child[2]);
            char str[10];
            strcpy(str,i1->content);
            if(!strcmp(node[node[x].child[0]].type,"write")){
                    char xx[10];
                    int str_len=strlen(str);
                    int i=0;
                    while(str[i]!=' '){
                        xx[i]=str[i];
                        i++;
                    }
                    xx[i]='\0';
                    struct temp_var *t1=find_temp_var(xx);
                    if(t1==NULL){
                        t1=add_temp_var(xx);
                    }
                    fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
                    fprintf(fp,"move $a0, $t1\n");
                    fprintf(fp,"addi $sp, $sp, -4\nsw $ra, 0($sp)\njal write\nlw $ra, 0($sp)\naddi $sp, $sp, 4\n");
                struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
                sprintf(i2->content,"WRITE t%d\n",argList[1]);
                i2->next=NULL;
                i1=merge(i1,i2);
                return i1;
            }else{
                int i;
                for(i=arg_len;i>=1;i--){
                    struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
                    sprintf(i2->content,"ARG t%d\n",argList[i]);
                    char xx[10];
                    sprintf(xx,"t%d",argList[i]);
                    struct temp_var *temp=find_temp_var(xx);
                    fprintf(fp,"lw $t1, %d($sp)\n",temp->offset);
                    fprintf(fp,"move $a%d, $t1\n",arg_len-i);
                    i2->next=NULL;
                    merge(i1,i2);
                }
                struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
                sprintf(cd3->content,"t%d := CALL %s\n",place,node[node[x].child[0]].type);
                char xx[10];
                sprintf(xx,"t%d",place);
                trans_jal(node[node[x].child[0]].type,xx);
                cd3->next=NULL;
                i1=merge(i1,cd3);
                return i1;
            }
        }
        
    }else if(strcmp(node[node[x].child[0]].s,"ID")==0){
        struct intercode* cd=(struct intercode *) malloc(sizeof(struct intercode));
        struct sym_node *temp=lookupSym(varTable,node[node[x].child[0]].type);
        sprintf(cd->content,"t%d := v%d\n",place,temp->number);
        char xx[10];
        char y[10];
        sprintf(xx,"t%d",place);
        sprintf(y,"v%d",temp->number);
        trans_move(xx,y);
        cd->next=NULL;
        return cd;
    }else if(strcmp(node[node[x].child[0]].s,"INT")==0){
        struct intercode* cd=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd->content,"t%d := #%s\n",place,node[node[x].child[0]].type);
        int k=atoi(node[node[x].child[0]].type);
        char xx[10];
        sprintf(xx,"t%d",place);
        trans_li(xx,k);
        cd->next=NULL;
        return cd;
    }else if(strcmp(node[node[x].child[0]].s,"MINUS")==0){
        int t1=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i2->content,"t%d := #0 - t%d\n",place,t1);
        i2->next=NULL;
        return merge(i1,i2);
    }else if(strcmp(node[node[x].child[1]].s,"STAR")==0){
        int t1=new_tmp();
        int t2=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=trans_exp(node[x].child[2],t2);
        struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd3->content,"t%d := t%d * t%d\n",place,t1,t2);
        char xx[10];
        char y[10];
        char z[10];
        sprintf(xx,"t%d",place);
        sprintf(y,"t%d",t1);
        sprintf(z,"t%d",t2);
        trans_mul(xx,y,z);
        cd3->next=NULL;
        i1=merge(i1,i2);
        i1=merge(i1,cd3);
        return i1;
    }else if(strcmp(node[node[x].child[1]].s,"PLUS")==0){
        int t1=new_tmp();
        int t2=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=trans_exp(node[x].child[2],t2);
        struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd3->content,"t%d := t%d + t%d\n",place,t1,t2);
        char xx[10];
        char y[10];
        char z[10];
        sprintf(xx,"t%d",place);
        sprintf(y,"t%d",t1);
        sprintf(z,"t%d",t2);
        trans_add(xx,y,z);
        cd3->next=NULL;
        i1=merge(i1,i2);
        i1=merge(i1,cd3);
        return i1;
    }else if(strcmp(node[node[x].child[1]].s,"MINUS")==0){
        int t1=new_tmp();
        int t2=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=trans_exp(node[x].child[2],t2);
        struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd3->content,"t%d := t%d - t%d\n",place,t1,t2);
        char xx[10];
        char y[10];
        char z[10];
        sprintf(xx,"t%d",place);
        sprintf(y,"t%d",t1);
        sprintf(z,"t%d",t2);
        trans_sub(xx,y,z);
        cd3->next=NULL;
        i1=merge(i1,i2);
        i1=merge(i1,cd3);
        return i1;
    }else if(strcmp(node[node[x].child[1]].s,"DIV")==0){
        int t1=new_tmp();
        int t2=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=trans_exp(node[x].child[2],t2);
        struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd3->content,"t%d := t%d / t%d\n",place,t1,t2);
        char xx[10];
        char y[10];
        char z[10];
        sprintf(xx,"t%d",place);
        sprintf(y,"t%d",t1);
        sprintf(z,"t%d",t2);
        trans_div(xx,y,z);
        cd3->next=NULL;
        i1=merge(i1,i2);
        i1=merge(i1,cd3);
        return i1;
    }
}

struct intercode *trans_args(int x){
    //printf("trans_args\n");
    if(strcmp(node[x].s,"Args")==0){
        if(node[x].child_num==3){
            // Exp COMMA Args
            int t1=new_tmp();
            struct intercode* i1=trans_exp(node[x].child[0],t1);
            argList[++arg_len]=t1;
            struct intercode* i2=trans_args(node[x].child[1]);
            i1=merge(i1,i2);
            return i1;
        }else{
            //Exp
            int t1=new_tmp();
            struct intercode *i1=trans_exp(node[x].child[0],t1);
            argList[++arg_len]=t1;
            return i1;
        }
    }
}



struct intercode* trans_cond(int x,int label1,int label2){
    if(strcmp(node[node[x].child[1]].s,"RELOP")==0){
        int t1=new_tmp();
        int t2=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[0],t1);
        struct intercode* i2=trans_exp(node[x].child[2],t2);
        struct intercode* cd3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(cd3->content,"IF t%d %s t%d GOTO LABEL%d\nGOTO LABEL%d\n",t1,node[node[x].child[1]].type,t2,label1,label2);
        char xx[10];
        char y[10];
        char z[10];
        char w[10];
        sprintf(xx,"t%d",t1);
        sprintf(y,"t%d",t2);
        sprintf(z,"LABEL%d",label1);
        sprintf(w,"LABEL%d",label2);
        if(strcmp(node[node[x].child[1]].type,"==")==0){
            trans_beq(xx,y,z);
        }
        else if(strcmp(node[node[x].child[1]].type,"!=")==0){
            trans_bne(xx,y,z);
        }
        else if(strcmp(node[node[x].child[1]].type,">")==0){
            trans_bgt(xx,y,z);
        }
        else if(strcmp(node[node[x].child[1]].type,"<")==0){
            trans_blt(xx,y,z);
        }
        else if(strcmp(node[node[x].child[1]].type,">=")==0){
            trans_bge(xx,y,z);
        }
        else if(strcmp(node[node[x].child[1]].type,"<=")==0){
            trans_ble(xx,y,z);
        }
        trans_goto(w);
        cd3->next=NULL;
        i1=merge(i1,i2);
        i1=merge(i1,cd3);
        return i1;
    }
    else if(strcmp(node[node[x].child[0]].s,"NOT")==0){
        return trans_cond(node[x].child[1],label2,label1);
    }
    else if(strcmp(node[node[x].child[0]].s,"AND")==0){
        int l1=new_label();
        struct intercode* i1=trans_cond(node[x].child[0],l1,label2);
        fprintf(fp,"LABEL%d\n",l1);
        struct intercode* i2=trans_cond(node[x].child[1],label1,label2);
        struct intercode* i3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i3->content,"LABEL%d\n",l1);
        i1=merge(i1,i3);
        i1=merge(i1,i2);
        return i1;
    }
    else if(strcmp(node[node[x].child[0]].s,"OR")==0){
        int l1=new_label();
        struct intercode* i1=trans_cond(node[x].child[0],label1,l1);
        fprintf(fp,"LABEL%d\n",l1);
        struct intercode* i2=trans_cond(node[x].child[1],label1,label2);
        struct intercode* i3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i3->content,"LABEL%d\n",l1);
        i1=merge(i1,i3);
        i1=merge(i1,i2);
        return i1;
    }
    else{
        int t1=new_tmp();
        struct intercode* i1=trans_exp(x,t1);
        struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i2->content,"IF t%d != #0 GOTO LABEL%d\nGOTO LABEL%d\n",t1,label1,label2);//to do
        i1=merge(i1,i2);
        return i1;
    }
}

struct intercode* trans_stmt(int x) {
    if (strcmp(node[node[x].child[0]].s,"Exp")==0) {
        return trans_exp(node[x].child[0],-1);
    } 
    else if (node[x].child_num==5&&strcmp(node[node[x].child[0]].s,"IF")==0) {
        int label1=new_label();
        int label2=new_label();
        struct intercode* i1=trans_cond(node[x].child[2],label1,label2);
        fprintf(fp,"LABEL%d :\n",label1);
        struct intercode* i2=trans_stmt(node[x].child[4]);
        struct intercode* lb1=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb1->content,"LABEL LABEL%d :\n",label1);
        lb1->next=NULL;
        struct intercode* lb2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb2->content,"LABEL LABEL%d :\n",label2);
        fprintf(fp,"LABEL%d :\n",label2);
        lb2->next=NULL;
        struct intercode* head=merge(i1,lb1);
        head=merge(head,i2);
        head=merge(head,lb2);
        return head; 
    } else if (node[x].child_num==7) {
        int label1=new_label();
        int label2=new_label();
        int label3=new_label();
        struct intercode* i1=trans_cond(node[x].child[2],label1,label2);
        fprintf(fp,"LABEL%d :\n",label1);
        struct intercode* i2=trans_stmt(node[x].child[4]);
        char xx[10];
        sprintf(xx,"LABEL%d",label3);
        trans_goto(xx);
        fprintf(fp,"LABEL%d :\n",label2);
        struct intercode* cd3=trans_stmt(node[x].child[6]);
        fprintf(fp,"LABEL%d :\n",label3);
        struct intercode* lb1=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb1->content,"LABEL LABEL%d :\n",label1);
        lb1->next=NULL;
        struct intercode* lb2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb2->content,"LABEL LABEL%d :\n",label2);
        lb2->next=NULL;
        struct intercode* lb3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb3->content,"LABEL LABEL%d :\n",label3);
        lb3->next=NULL;
        struct intercode* gotolb3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(gotolb3->content,"GOTO LABEL%d\n",label3);
        gotolb3->next=NULL;
        struct intercode* head=merge(i1,lb1);
        head=merge(head,i2);
        head=merge(head,gotolb3);
        head=merge(head,lb2);
        head=merge(head,cd3);
        head=merge(head,lb3);
        return head;
    }else if(strcmp(node[node[x].child[0]].s,"RETURN")==0){
        int t1=new_tmp();
        struct intercode* i1=trans_exp(node[x].child[1],t1);
        struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i2->content,"RETURN t%d\n",t1);
        char xx[10];
        sprintf(xx,"t%d",t1);
        trans_jr(xx);
        i2->next=NULL;
        return merge(i1,i2);
    }
    else if(strcmp(node[node[x].child[0]].s,"WHILE")==0){
        int label1=new_label();
        int label2=new_label();
        int label3=new_label();
        fprintf(fp,"LABEL%d :\n",label1);
        struct intercode* i1=trans_cond(node[x].child[2],label2,label3);
        fprintf(fp,"LABEL%d :\n",label2);
        struct intercode* i2=trans_stmt(node[x].child[4]);
        char xx[10];
        sprintf(xx,"LABEL%d",label1);
        trans_goto(xx);
        fprintf(fp,"LABEL%d :\n",label3);
        struct intercode* lb1=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb1->content,"LABEL LABEL%d :\n",label1);
        lb1->next=NULL;
        struct intercode* lb2=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb2->content,"LABEL LABEL%d :\n",label2);
        lb2->next=NULL;
        struct intercode* lb3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(lb3->content,"LABEL LABEL%d :\n",label3);
        lb3->next=NULL;
        struct intercode* gotolb3=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(gotolb3->content,"GOTO LABEL%d\n",label1);
        gotolb3->next=NULL;
        struct intercode* head=merge(lb1,i1);
        head=merge(head,lb2);
        head=merge(head,i2);
        head=merge(head,gotolb3);
        head=merge(head,lb3);
        return head;
    }
    else if(strcmp(node[node[x].child[0]].s,"CompSt")==0){
        return trans_comp_st(node[x].child[0]);
    }
    return NULL;
}
struct intercode* trans_stmt_list(int x) {
    if (node[x].child_num==2) {
        struct intercode* i1=trans_stmt(node[x].child[0]);
        struct intercode* i2=trans_stmt_list(node[x].child[1]); 
        i1=merge(i1,i2);
        return i1;
    }
    else {
        return NULL;
    }
}
struct intercode *trans_var_dec(int x,int size,int flag) {
        if (node[x].child_num==4) {
            struct sym_node* n=lookupSym(varTable, node[node[x].child[0]].type);
            struct intercode* i =(struct intercode *) malloc(sizeof(struct intercode));
            sprintf(i->content,"DEC %s %d\n",n->name,size);
            i->next=NULL;
            return i;
        } else {
            if(strcmp(node[node[x].child[0]].s,"ID")==0&&flag!=0){
            struct intercode* i2=(struct intercode *) malloc(sizeof(struct intercode));
            struct sym_node *temp=lookupSym(varTable,node[node[x].child[0]].type);
            sprintf(i2->content,"v%d := t%d\n",temp->number,flag);
            char xx[10];
            char y[10];
            sprintf(xx,"v%d",temp->number);
            sprintf(y,"t%d",flag);
            trans_move(xx,y);
            i2->next=NULL;
            return i2;
        }
            return NULL; 
        }
}
struct intercode* trans_dec(int x, int size) {
        if (node[x].child_num==1) {
            return trans_var_dec(node[x].child[0],size,0);
        } else if (node[x].child_num==3) {
            int t1=new_tmp();
            struct intercode *i1=trans_exp(node[x].child[2],t1);
            struct intercode *i2=trans_var_dec(node[x].child[0],size,t1);
            i1=merge(i1,i2);
            return i1;
        }
}

struct intercode* trans_dec_list(int x,int size) {
        if (node[x].child_num==1) {
            return trans_dec(node[x].child[0],size);
        } else if (node[x].child_num==3) {
            struct intercode* i1=trans_dec(node[x].child[0],size);
            struct intercode* i2=trans_dec_list(node[x].child[2],size);
            return merge(i1,i2);
        }  
}

struct intercode* trans_def(int x) {
    struct sym_node *temp=lookupSym(varTable,node[node[x].child[1]].type);
    return trans_dec_list(node[x].child[1],0);  
}

struct intercode* trans_def_list(int x) {
    if (node[x].child_num==2) {
        struct intercode* i1=trans_def(node[x].child[0]);
        struct intercode* i2=trans_def_list(node[x].child[1]);
        return merge(i1,i2);
    }
    else return NULL;
}

struct intercode* trans_comp_st(int x) {
    struct intercode* i1=trans_def_list(node[x].child[1]);
    struct intercode* i2=trans_stmt_list(node[x].child[2]);
    i1=merge(i1,i2);
    return i1;
}

struct intercode* trans_param(struct sym_node* p,int size){
        if(!p) return NULL;
        struct intercode* i1=(struct intercode *) malloc(sizeof(struct intercode));
        sprintf(i1->content,"PARAM v%d\n",p->number);
        char xx[10];
        sprintf(xx,"v%d",p->number);
        struct temp_var *temp=add_temp_var(xx);
        fprintf(fp,"move $t1, $a0\n");
        fprintf(fp,"sw $t1, %d($sp)\n",temp->offset);
        i1->next;
        size--;
        int cnt=0;
        while(size){
            cnt++;
            p=p->next;
            struct intercode* i=(struct intercode *) malloc(sizeof(struct intercode));
            sprintf(i->content,"PARAM v%d\n",p->number);
            char xx[10];
            sprintf(xx,"v%d",p->number);
            struct temp_var *temp=add_temp_var(xx);
            fprintf(fp,"move $t1, $a%d\n",cnt);
            fprintf(fp,"sw $t1, %d($sp)\n",temp->offset);
            i->next=NULL;
            merge(i1,i);
            size--;
        }
        return i1;
}

struct intercode* trans_fun_dec(int x) {
    struct fun_node *f = lookupFunc(node[node[x].child[0]].type);
    trans_curFunc = f;
    struct intercode* i1=(struct intercode *) malloc(sizeof(struct intercode));
    sprintf(i1->content,"FUNCTION %s : \n", node[node[x].child[0]].type);
    fprintf(fp,"%s : \n", node[node[x].child[0]].type);
    fprintf(fp,"addi $sp, $sp, -500\n");
    i1->next=NULL;
    if(node[x].child_num==4){
        struct intercode* i2=trans_param(trans_curFunc->param,trans_curFunc->size);
        return merge(i1,i2);
    }
    else return i1;
}
struct intercode* trans_ext_dec_list(int x) {
    if(node[x].child_num==1){
        struct sym_node *temp=lookupSym(varTable,node[node[x].child[0]].type);
        return trans_var_dec(node[x].child[0],temp->size,0);
    }
    else{
        struct sym_node *temp=lookupSym(varTable,node[node[x].child[0]].type);
        struct intercode* i1=trans_var_dec(node[x].child[0],temp->size,0);
        struct intercode* i2=trans_ext_dec_list(node[x].child[2]);
        i1=merge(i1,i2);
        return i1;
    }
}
struct intercode* trans_ext_def(int x) {
    if (strcmp(node[node[x].child[1]].s,"ExtDecList")==0) {
        return trans_ext_dec_list(node[x].child[1]);
    } else if (strcmp(node[node[x].child[1]].s,"SEMI")==0) {
        return NULL;
    } else if (strcmp(node[node[x].child[1]].s,"FunDec")==0) {
        struct intercode* i1=trans_fun_dec(node[x].child[1]);
        struct intercode* i2=trans_comp_st(node[x].child[2]);
        trans_curFunc = NULL;
        struct intercode* ret=merge(i1,i2);
        return ret;
    }
}

struct intercode* trans_ext_def_list(int x) {
    if (node[x].child_num == 2) {
        struct intercode* i1=trans_ext_def(node[x].child[0]);
        struct intercode* i2=trans_ext_def_list(node[x].child[1]);
        struct intercode* ret=merge(i1,i2);
        return ret;
    }
    else return NULL;
}

struct intercode* trans_program(int x) {
    print_oject_code();
    return trans_ext_def_list(node[x].child[0]);
}

void trans_move(char* x,char* y){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t0=find_temp_var(y);
        if(t0==NULL){
            t0=add_temp_var(y);
        }
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        fprintf(fp,"move $t1, $t0\n");
        struct temp_var* t1=find_temp_var(x);
        if(t1==NULL){
            t1=add_temp_var(x);
        }
        fprintf(fp,"sw $t1, %d($sp)\n",t1->offset);
        return;
}

void trans_li(char* x,int k){
        char *regx=reg(x);
        fprintf(fp,"li $t1, %d\n",k);
        struct temp_var* t1=find_temp_var(x);
        if(t1==NULL){
            t1=add_temp_var(x);
        }
        fprintf(fp,"sw $t1, %d($sp)\n",t1->offset);
        return;
}

void trans_addi_plus(char* x,char *y,int k){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t0=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        fprintf(fp,"addi $t1, $t0, %d\n",k);
        struct temp_var* t1=find_temp_var(x);
        if(t1==NULL){
            t1=add_temp_var(x);
        }
        fprintf(fp,"sw $t1, %d($sp)\n",t1->offset);
        return;
}

void trans_add(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        char *regz=reg(z);
        struct temp_var* t0=find_temp_var(z);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        struct temp_var* t1=find_temp_var(y);
        fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
        fprintf(fp,"add $t2, $t1, $t0\n");
        struct temp_var* t2=find_temp_var(x);
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        fprintf(fp,"sw $t2, %d($sp)\n",t2->offset);
        return;
}

void trans_addi_minus(char* x,char *y,int k){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t0=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        fprintf(fp,"addi %t1, $t0, -%d\n",k);
        struct temp_var* t1=find_temp_var(x);
        if(t1==NULL){
            t1=add_temp_var(x);
        }
        fprintf(fp,"sw $t1, %d($sp)\n",t1->offset);
        return;
}

void trans_sub(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        char *regz=reg(z);
        struct temp_var* t0=find_temp_var(z);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        struct temp_var* t1=find_temp_var(y);
        fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
        fprintf(fp,"sub $t2, $t1, $t0\n");
        struct temp_var* t2=find_temp_var(x);
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        fprintf(fp,"sw $t2, %d($sp)\n",t2->offset);
        return;
}

void trans_mul(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        char *regz=reg(z);
        struct temp_var* t0=find_temp_var(z);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        struct temp_var* t1=find_temp_var(y);
        fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
        fprintf(fp,"mul $t2, $t1, $t0\n");
        struct temp_var* t2=find_temp_var(x);
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        fprintf(fp,"sw $t2, %d($sp)\n",t2->offset);
        return;
}

void trans_div(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        char *regz=reg(z);
        struct temp_var* t0=find_temp_var(z);
        fprintf(fp,"lw $t0, %d($sp)\n",t0->offset);
        struct temp_var* t1=find_temp_var(y);
        fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
        fprintf(fp,"div $t1, $t0\nmflo $t2\n");
        struct temp_var* t2=find_temp_var(x);
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        fprintf(fp,"sw $t2, %d($sp)\n",t2->offset);
        return;
}

void trans_lw(char* x,char *y){
        char *regx=reg(x);
        char *regy=reg(y);
        fprintf(fp,"lw %s, 0(%s)\n",regx,regy);
        return;
}

void trans_sw(char* x,char *y){
        char *regx=reg(x);
        char *regy=reg(y);
        fprintf(fp,"sw %s, 0(%s)\n",regy,regx);
        return;
}

void trans_goto(char* t1){
        fprintf(fp,"j %s\n",t1);
        return;
}

void trans_jal(char *f,char* x){
        char *regx=reg(x);
        struct temp_var* t2=find_temp_var(x);
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        fprintf(fp,"addi $sp, $sp, -4\nsw $ra, 0($sp)\n");
        fprintf(fp,"jal %s\nlw $ra, 0($sp)\naddi $sp, $sp, 4\nmove $t0, $v0\n",f);
        fprintf(fp,"sw $t0, %d($sp)\n",t2->offset);
        return;
}

void trans_jr(char* x){
        char *regx=reg(x);
        struct temp_var* t2=find_temp_var("v0");
        if(t2==NULL){
            t2=add_temp_var(x);
        }
        struct temp_var* t1=find_temp_var(x);
        fprintf(fp,"lw $t1, %d($sp)\n",t1->offset);
        fprintf(fp,"move $v0, $t1\naddi $sp, $sp, 500\njr $ra\n");
        fprintf(fp,"sw $v0, %d($sp)\n",t2->offset);
        return;
}

void trans_beq(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"beq $t0, $t1, %s\n",z);
        return;
}

void trans_bne(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"bne $t0, $t1, %s\n",z);
        return;
}

void trans_bgt(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"bgt $t0, $t1, %s\n",z);
        return;
}

void trans_blt(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"blt $t0, $t1, %s\n",z);
        return;
}

void trans_bge(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"bge $t0, $t1, %s\n",z);
        return;
}

void trans_ble(char* x,char *y,char *z){
        char *regx=reg(x);
        char *regy=reg(y);
        struct temp_var* t1=find_temp_var(x);
        struct temp_var* t2=find_temp_var(y);
        fprintf(fp,"lw $t0, %d($sp)\n",t1->offset);
        fprintf(fp,"lw $t1, %d($sp)\n",t2->offset);
        fprintf(fp,"ble $t0, $t1, %s\n",z);
        return;
}