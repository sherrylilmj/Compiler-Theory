%{
  #include<stdio.h>
  int ff=0;
  #include "syntax.tab.h"
  extern int cnt;
%}
%option yylineno
TYPE int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
ONUM 0[0-7]+
HNUM 0(X|x)[A-Fa-f0-9]+
EFLOAT ([0-9]*+"."[0-9]*)(E|e)[+-]?[0-9]+
FLOAT [0-9]+"."[0-9]*
INT ([1-9][0-9]*)|0
SEMI ;
COMMA ,
ASSIGNOP =
PLUS \+
MINUS \-
AND &&
OR \|\|
DOT \.
NOT \!
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
RELOP >|<|==|<=|>=|!=
ZHUSHI (\/\/.*)|(\/\*.*\*\/)
STAR \*
DIV \/
ASB \|
EOL \n
ID [A-Za-z_][A-Za-z_0-9]*
ERROR_TYPEA .
%%
"/*" {
    int cc=0;
    char c;
    char ch;
    int flag=1;
    int f=1;
    int line=yylineno;
    int cou=0;
    char tmp[1000];
    while(flag)
    {
        flag=0;
        while((c=input())!=EOF&&c!='*')
        {
            cc++;
            if(c=='/')f=0;
            else f=1;
        }
        if(c=='*'&&f==0)
        {
            cou++;
        }
        if(c==EOF)
        {
            ff=1;
            printf("Error type A at Line %d: mismatched comments\n",line);
            break;
        }
        else
        {
            if((ch=input())!='/'&&ch!=EOF)
            {
                unput(ch);
                flag=1;
            }
            if(ch=='/')
            {
                if(cou==0)
                {
                    printf("Matched comments at Line %d\n",yylineno);
                    break;
                }
                else
                {
                    int ans=0;
                    int lineno=yylineno;
                    while((ch=input())!=EOF&ch!='*')
                    {
                        tmp[ans++]=ch;
                    }
                    if(ch==EOF)
                    {
                        int i;
                        for(i=ans-1; i>=0; i--)
                        {
                            unput(tmp[i]);
                        }
                        printf("Matched comments at Line %d\n",lineno);
                        break;
                    }
                    if((ch=input())=='/')
                    {
                        ff=1;
                        printf("Error type A at Line %d: nested comments\n",yylineno);
                        break;
                    }
                }
            }
            if(ch==EOF)
            {
                ff=1;
                printf("Error type A at Line %d: mismatched comments\n",line);
                break;
            }
        }
    }
}
{INT} {
   cnt=newtree0(cnt,yytext,yylineno,"INT",0);
   yylval=cnt;
cnt++;
   return INT;
}
{PLUS} {
    cnt=newtree0(cnt,yytext,yylineno,"PLUS",0);
   yylval=cnt;
cnt++;
    return PLUS;
}
{MINUS} {
    cnt=newtree0(cnt,yytext,yylineno,"MINUS",0);
   yylval=cnt;
cnt++;
    return MINUS;
}
{DIV} {
    cnt=newtree0(cnt,yytext,yylineno,"DIV",0);
   yylval=cnt;
cnt++;
    return DIV;
}
{STAR} {
    cnt=newtree0(cnt,yytext,yylineno,"STAR",0);
   yylval=cnt;
cnt++;
    return STAR;
}
{ONUM} {cnt=newtree0(cnt,yytext,yylineno,"ONUM",0);   yylval=cnt;
cnt++;return ONUM;}
{HNUM} {cnt=newtree0(cnt,yytext,yylineno,"HNUM",0);   yylval=cnt;
cnt++;return HNUM;}
{FLOAT} {cnt=newtree0(cnt,yytext,yylineno,"FLOAT",0);   yylval=cnt;
cnt++;return FLOAT;}
{EFLOAT} {cnt=newtree0(cnt,yytext,yylineno,"EFLOAT",0);yylval=cnt;cnt++;return EFLOAT;}
{SEMI} {cnt=newtree0(cnt,yytext,yylineno,"SEMI",0);yylval=cnt;cnt++;return SEMI;}
{COMMA} {cnt=newtree0(cnt,yytext,yylineno,"COMMA",0);yylval=cnt;cnt++;return COMMA;}
{ASSIGNOP} {cnt=newtree0(cnt,yytext,yylineno,"ASSIGNOP",0);yylval=cnt;cnt++;return ASSIGNOP;}
{RELOP} {cnt=newtree0(cnt,yytext,yylineno,"RELOP",0);yylval=cnt;cnt++;return RELOP;}
{AND} {cnt=newtree0(cnt,yytext,yylineno,"AND",0);yylval=cnt;cnt++;return AND;}
{OR} {cnt=newtree0(cnt,yytext,yylineno,"OR",0);yylval=cnt;cnt++;return OR;}
{DOT} {cnt=newtree0(cnt,yytext,yylineno,"DOT",0);yylval=cnt;cnt++;return DOT;}
{NOT} {cnt=newtree0(cnt,yytext,yylineno,"NOT",0);yylval=cnt;cnt++;return NOT;}
{TYPE} {cnt=newtree0(cnt,yytext,yylineno,"TYPE",0);yylval=cnt;cnt++;return TYPE;}
{LP} {cnt=newtree0(cnt,yytext,yylineno,"LP",0);yylval=cnt;cnt++;return LP;}
{RP} {cnt=newtree0(cnt,yytext,yylineno,"RP",0);yylval=cnt;cnt++;return RP;}
{LB} {cnt=newtree0(cnt,yytext,yylineno,"LB",0);yylval=cnt;cnt++;return LB;}
{RB} {cnt=newtree0(cnt,yytext,yylineno,"RB",0);yylval=cnt;cnt++;return RB;}
{LC} {cnt=newtree0(cnt,yytext,yylineno,"LC",0);yylval=cnt;cnt++;return LC;}
{RC} {cnt=newtree0(cnt,yytext,yylineno,"RC",0);yylval=cnt;cnt++;return RC;}
{STRUCT} {cnt=newtree0(cnt,yytext,yylineno,"STRUCT",0);yylval=cnt;cnt++;return STRUCT;}
{RETURN} {cnt=newtree0(cnt,yytext,yylineno,"RETURN",0);yylval=cnt;cnt++;return RETURN;}
{IF} {cnt=newtree0(cnt,yytext,yylineno,"IF",0);yylval=cnt;cnt++;return IF;}
{ELSE} {cnt=newtree0(cnt,yytext,yylineno,"ELSE",0);yylval=cnt;cnt++;return ELSE;}
{WHILE} {cnt=newtree0(cnt,yytext,yylineno,"WHILE",0);yylval=cnt;cnt++;return WHILE;}
"\n" {}
" " {}
{ZHUSHI} {}
{ID} {cnt=newtree0(cnt,yytext,yylineno,"ID",0);yylval=cnt;
cnt++;return ID;}
{ERROR_TYPEA} {printf("Error type A at line %d: Mystirious charachter '%s'\n",yylineno,yytext,yylineno,"",0);ff=1;}
%%