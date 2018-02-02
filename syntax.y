%{
    #include<stdio.h>
    int cnt=0;
    extern int yylineno;
    extern int ff;
    int num_error=0;
%}

/*declared types
%union{
    int type_int;
    double type_double;
    tree_node* type_tree_node; //The tree node your design
}
*/
/*declared tokens*/
%token INT
%token PLUS MINUS STAR DIV
%token ONUM HNUM ID TYPE STRUCT RETURN IF ELSE WHILE SEMI COMMA ASSIGNOP AND 
%token OR DOT NOT LP RP LB RB LC RC RELOP ZHUSHI ASB EOL EFLOAT FLOAT

//you should add more tokens here (add all Vt according to your flex file)
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT


%%
Program : ExtDefList {$$=newtree1(cnt,$1,"Program",yylineno,"",1);cnt++; initSymTable();sym_program($$);}
        ;
ExtDefList : ExtDef ExtDefList {$$=newtree2(cnt,$1,$2,"ExtDefList",yylineno,"",1);cnt++;}
           | {$$=newtree0(cnt,"ExtDefList",yylineno,"",1);cnt++;}
           ;
ExtDef : Specifier ExtDecList SEMI {$$=newtree3(cnt,$1,$2,$3,"ExtDef",yylineno,"",1);cnt++;}
       | Specifier SEMI {int i=$2;$$=newtree2(cnt,$1,$2,"ExtDef",yylineno,"",1);cnt++;}
       | Specifier FunDec CompSt {$$=newtree3(cnt,$1,$2,$3,"ExtDef",yylineno,"",1);cnt++;}
       ;
ExtDecList : VarDec {$$=newtree1(cnt,$1,"ExtDecList",yylineno,"",1);cnt++;}
           | VarDec COMMA ExtDecList {$$=newtree3(cnt,$1,$2,$3,"ExtDecList",yylineno,"",1);cnt++;}
           ;
/*Specifiers*/
Specifier : TYPE {$$=newtree1(cnt,$1,"Specifier",yylineno,"",1);cnt++;}
          | StructSpecifier {$$=newtree1(cnt,$1,"Specifier",yylineno,"",1);cnt++;}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=newtree5(cnt,$1,$2,$3,$4,$5,"StructSpecifier",yylineno,"",1);cnt++;}
                | STRUCT Tag {$$=newtree2(cnt,$1,$2,"StructSpecifier",yylineno,"",1);cnt++;}
                ;
OptTag : ID {$$=newtree1(cnt,$1,"OptTag",yylineno,"",1);cnt++;}
       | {$$=newtree0(cnt,"OptTag",yylineno,"",1);cnt++;}
       ;
Tag : ID {$$=newtree1(cnt,$1,"Tag",yylineno,"",1);cnt++;}
    ;
/*Declarators*/
VarDec : ID {$$=newtree1(cnt,$1,"VarDec",yylineno,"",1);cnt++;}
       | VarDec LB INT RB {$$=newtree4(cnt,$1,$2,$3,$4,"VarDec",yylineno,"",1);cnt++;}
       ;
FunDec : ID LP VarList RP {$$=newtree4(cnt,$1,$2,$3,$4,"FunDec",yylineno,"",1);cnt++;}
       | ID LP RP {$$=newtree3(cnt,$1,$2,$3,"FunDec",yylineno,"",1);cnt++;}
       ;
VarList : ParamDec COMMA VarList {$$=newtree3(cnt,$1,$2,$3,"VarList",yylineno,"",1);cnt++;}
        | ParamDec {$$=newtree1(cnt,$1,"VarList",yylineno,"",1);cnt++;}
        ;
ParamDec : Specifier VarDec {$$=newtree2(cnt,$1,$2,"ParamDec",yylineno,"",1);cnt++;}
         ;
/*Statements*/
CompSt : LC DefList StmtList RC {$$=newtree4(cnt,$1,$2,$3,$4,"CompSt",yylineno,"",1);cnt++;}
       ;
StmtList : Stmt StmtList {$$=newtree2(cnt,$1,$2,"StmtList",yylineno,"",1);cnt++;}
         | {$$=newtree0(cnt,"StmtList",yylineno,"",1);cnt++;}
         ;
Stmt : Exp SEMI {$$=newtree2(cnt,$1,$2,"Stmt",yylineno,"",1);cnt++;}
     | CompSt {$$=newtree1(cnt,$1,"Stmt",yylineno,"",1);cnt++;}
     | RETURN Exp SEMI {$$=newtree3(cnt,$1,$2,$3,"Stmt",yylineno,"",1);cnt++;}
     | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=newtree5(cnt,$1,$2,$3,$4,$5,"Stmt",yylineno,"",1);cnt++;}
     | IF LP Exp RP Stmt ELSE Stmt {$$=newtree7(cnt,$1,$2,$3,$4,$5,$6,$7,"Stmt",yylineno,"",1);cnt++;}
     | WHILE LP Exp RP Stmt {$$=newtree5(cnt,$1,$2,$3,$4,$5,"Stmt",yylineno,"",1);cnt++;}
     ;
/*Local Definitions*/
DefList : Def DefList {$$=newtree2(cnt,$1,$2,"DefList",yylineno,"",1);cnt++;}
        | {$$=newtree0(cnt,"DefList",yylineno,"",1);cnt++;}
        ;
Def : Specifier DecList SEMI {$$=newtree3(cnt,$1,$2,$3,"Def",yylineno,"",1);cnt++;}
    ;
DecList : Dec {$$=newtree1(cnt,$1,"DecList",yylineno,"",1);cnt++;}
        | Dec COMMA DecList {$$=newtree3(cnt,$1,$2,$3,"DecList",yylineno,"",1);cnt++;}
        ;
Dec : VarDec {$$=newtree1(cnt,$1,"Dec",yylineno,"",1);cnt++;}
    | VarDec ASSIGNOP Exp {$$=newtree3(cnt,$1,$2,$3,"Dec",yylineno,"",1);cnt++;}
    ;
/*Expressions*/
Exp : Exp ASSIGNOP Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp AND Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp OR Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp RELOP Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp PLUS Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp MINUS Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp STAR Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp DIV Exp {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | LP Exp RP {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | MINUS Exp {$$=newtree2(cnt,$1,$2,"Exp",yylineno,"",1);cnt++;}
    | NOT Exp {$$=newtree2(cnt,$1,$2,"Exp",yylineno,"",1);cnt++;}
    | ID LP Args RP {$$=newtree4(cnt,$1,$2,$3,$4,"Exp",yylineno,"",1);cnt++;}
    | ID LP RP {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | Exp LB Exp RB {$$=newtree4(cnt,$1,$2,$3,$4,"Exp",yylineno,"",1);cnt++;}
    | Exp DOT ID {$$=newtree3(cnt,$1,$2,$3,"Exp",yylineno,"",1);cnt++;}
    | ID {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | INT {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | FLOAT {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | EFLOAT {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | ONUM {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | HNUM {$$=newtree1(cnt,$1,"Exp",yylineno,"",1);cnt++;}
    | error {num_error=1;}
    ;
Args : Exp COMMA Args {$$=newtree3(cnt,$1,$2,$3,"Args",yylineno,"",1);cnt++;}
     | Exp {$$=newtree1(cnt,$1,"Args",yylineno,"",1);cnt++;}
     ;
//you can add more rules here (see page.116-page117 for C-- grammar)
%%
#include "lex.yy.c"
#include "symbol.c"
//whatever you like
int main(int argc,char** argv) {
    int res;
    if(argc>1){
        if(!(yyin=fopen(argv[1],"r")))
	{
            perror(argv[1]);
            return 1;
	}
    }
    init();
    res = yyparse();
    if(!ff){
    if(res == 0&&num_error==0){
            //dfs(cnt-1,0);
        printf("SyntaxAnalysis Success!\n");
    }
    else{
        printf("SyntaxAnalysis Failed!\n");
    }
    }
    return 0;
}

int yyerror(char *msg) { 
    //printf("%d\n",xxx);
    fprintf(stderr,"Error type B at Line %d: %s\n",yylineno,msg); 
} 