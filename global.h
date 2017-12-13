#include<fstream>
#include<iostream>
#include<string>
#include<cstring>
#include<strstream>
#include<malloc.h>

using namespace std;
#define BUFLEN 256            //每一行读取的最大字符串长度
#define MAXLEN 256
#define MAXTOKENLEN 40        //词法单元的最大长度
#define MAXCHILDREN 4
static int lineno;
static int linepos = 0;       //读取的字符在 lineBuf 的位置
static int EOF_FLAG = false;  //判断文件是否结束
static int bufsize = 0;       //lineBuf 的长度
static char lineBuf[BUFLEN];
FILE * source;
char tokenString[MAXTOKENLEN+1]; //暂存词法单元的字符串
string output;//输出文件
enum TokenType  //词法单元类型
{
    ENDFILE,ERROR,
    IF,ELSE,INT,RETURN,VOID,WHILE,
    ID,NUM,
    ASSIGN,EQ,LT,PLUS,MINUS,TIMES,OVER,LPAREN,RPAREN,SEMI,LBRACKET,RBRACKET,
    LBRACE,RBRACE,COMMA,
    GT,GEQ,NEQ,LEQ
};
enum StateType  //词法分析中的状态类型
{
    START,INASSIGN,INCOMMENT,INNUM,INID,DONE,PRECOMMENT,AFTERCOMMENT
};
struct //关键字类型
{
    char* str;
    TokenType tok;
} ReserverWords[6]= { {"if",IF},{"else",ELSE},{"int",INT},{"return",RETURN},{"void",VOID},{"while",WHILE}};

struct  //操作符
{
    string str;
    TokenType tk;
} Ope[11]
= { {"=",ASSIGN},{"==",EQ},{"<",LT},{"+",PLUS},{"-",MINUS},{"*",TIMES},{"/",OVER},
    {">",GT},{">=",GEQ},{"!=",NEQ},{"<=",LEQ}
};

enum NodeKind//节点类型
{
    FuncK,IntK,IdK,ParamsK,ParamK,CompK,ConstK,CallK,ArgsK,VoidK,Var_DeclK,Arry_DeclK,
    Arry_ElemK,AssignK,WhileK,OpK,
    Selection_StmtK,Iteration_StmtK,Return_StmtK
};
struct//节点类型和字符串关系
{
    string str;
    NodeKind nk;
} nodekind[19]
= {
    {"Funck",FuncK},{"IntK",IntK},{"IdK",IdK},{"ParamsK",ParamsK},{"ParamK",ParamK},
    {"CompK",CompK},{"ConstK",ConstK},{"CallK",CallK},{"ArgsK",ArgsK},{"VoidK",VoidK},
    {"Var_DeclK",Var_DeclK},{"Arry_DeclK",Arry_DeclK},{"Arry_ElemK",Arry_ElemK},
    {"AssignK",AssignK},{"WhileK",WhileK},{"OpK",OpK},{"Selection_StmtK",Selection_StmtK},
    {"Iteration_StmtK",Iteration_StmtK},{"Return_StmtK",Return_StmtK}
  };

struct TreeNode //树节点
{
    struct TreeNode * child[4];
    struct TreeNode * sibling;
    int lineno;
    NodeKind nodekind;
    union{ TokenType op;int val;const char * name;} attr;
};

TokenType token;

//对于BNF文法求解函数

TreeNode * declaration_list(void);       //declaration_list declaration | declaration
TreeNode * declaration(void);
TreeNode * params(void);
TreeNode * param_list(TreeNode * p);
TreeNode * param(TreeNode * p);
TreeNode * compound_stmt(void);           //{local_declarations statment_list}
TreeNode * local_declaration(void);
TreeNode * statement_list(void);
TreeNode * statement(void);
TreeNode * expression_stmt(void);
TreeNode * selection_stmt(void);
TreeNode * iteration_stmt(void);
TreeNode * return_stmt(void);
TreeNode * expression(void);
TreeNode * var(void);
TreeNode * simple_expression(TreeNode * p);
TreeNode * additive_expression(TreeNode * p);
TreeNode * term(TreeNode * p);
TreeNode * factor(TreeNode * p);
TreeNode * call(TreeNode * p);
TreeNode * args(void);

