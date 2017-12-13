#include "global.h"

void UnGetNextChar()  //回退一个字符
{
    if (!EOF_FLAG)
        linepos--;
}
int GetNextChar()   //获取下一字符
{
    if(!(linepos<bufsize))
    {
        lineno++;
        if(fgets(lineBuf,BUFLEN-1,source)) //读取某一行字符串
        {
            bufsize=strlen(lineBuf);
            linepos=0;
            return lineBuf[linepos++];
        }
        else
        {
            EOF_FLAG=true;
            return EOF;
        }
    }
    else
    {
        return lineBuf[linepos++];
    }
}
TokenType ReservedLookUp(char * s) //判断是否是关键字
{
    int i;
    for (i = 0; i < 6; i++)
    {
        if(!strcmp(s,ReserverWords[i].str))
        {
            return ReserverWords[i].tok;
        }
    }
    return ID;
}
TokenType GetToken()
{
    StateType state = START;//初始状态为 start
    bool save;
    TokenType CurrentToken;
    int tokenStringIndex=0;
    string assign="";
    while(state!=DONE)
    {
        int c=GetNextChar();
        save = true;
        switch (state)
        {
        case START:
            if (isdigit(c))
            {
                state = INNUM;
            }
            else if (isalpha(c))
            {
                state = INID;
            }
            else if ((c == '<')||(c=='>')||(c=='=')||(c=='!'))
            {
                state = INASSIGN;
                assign+=char(c);
            }
            else if ((c == ' ') || (c == '\t') || (c == '\n'))
                save = false;
            else if (c == '/')
            {
                save = false;
                state = PRECOMMENT;
            }
            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = false;
                    CurrentToken = ENDFILE;
                    break;
                case '+':
                    CurrentToken = PLUS;
                    break;
                case '-':
                    CurrentToken = MINUS;
                    break;
                case '*':
                    CurrentToken = TIMES;
                    break;
                case '(':
                    CurrentToken = LPAREN;
                    break;
                case ')':
                    CurrentToken = RPAREN;
                    break;
                case ';':
                    CurrentToken = SEMI;
                    break;
                case '[':
                    CurrentToken = LBRACKET;
                    break;
                case ']':
                    CurrentToken = RBRACKET;
                    break;
                case '{':
                    CurrentToken = LBRACE;
                    break;
                case '}':
                    CurrentToken = RBRACE;
                    break;
                case ',':
                    CurrentToken = COMMA;
                    break;
                default:
                    CurrentToken = ERROR;
                    break;
                }
            }
            break;
        case INCOMMENT:
            save = false;
            if (c == EOF)
            {
                state = DONE;
                CurrentToken = ENDFILE;
            }
            else if (c == '*')
                state = AFTERCOMMENT;
            else
            {
                state=INCOMMENT;
            }
            break;
        case INASSIGN:
            if (c == '=')
            {
                CurrentToken = EQ;
                state=DONE;
            }
            else if(assign=="!")
            {
                UnGetNextChar();
                save = false;
                CurrentToken = ERROR;
                state=DONE;
            }
            else if(assign=="=")
            {
                UnGetNextChar();
                save = false;
                CurrentToken =ASSIGN ;
                state=DONE;
            }
            else if(assign=="<")
            {
                UnGetNextChar();
                save = false;
                CurrentToken =LT ;
                state=DONE;
            }
            else
            {
                UnGetNextChar();
                save = false;
                CurrentToken =GT ;
                state=DONE;
            }
            break;
        case INNUM:
            if (!isdigit(c))
            {
                UnGetNextChar();
                save = false;
                state = DONE;
                CurrentToken = NUM;
            }
            break;
        case INID:
            if (!isalpha(c))
            {
                UnGetNextChar();
                save = false;
                state = DONE;
                CurrentToken = ID;
            }
            break;
        case PRECOMMENT:
            if(c=='*')
            {
                state=INCOMMENT;
                save=false;
            }
            else
            {
                UnGetNextChar();
                CurrentToken=OVER;
                state=DONE;
            }
            break;
        case AFTERCOMMENT:
            save=false;
            if(c=='/')
            {
                state=START;
            }
            else if(c=='*')
            {
                state=AFTERCOMMENT;
            }
            else
            {
                state=INCOMMENT;
            }
            break;
        case DONE:
        default:
            state = DONE;
            CurrentToken = ERROR;
            break;
        }
        if((save)&&(tokenStringIndex<=MAXTOKENLEN))
        {
            tokenString[tokenStringIndex++]=(char)c;
        }
        if(state==DONE)
        {
            tokenString[tokenStringIndex]='\0';
            if(CurrentToken==ID)  //返回关键字类型
            {
                CurrentToken=ReservedLookUp(tokenString);
            }
        }
    }
    return CurrentToken;
}


string OpeLookUp(TokenType tk)//操作符转换为字符串
{
    int i;
    for(i=0; i<11; i++)
    {
        if(tk==Ope[i].tk)
        {
            return Ope[i].str;
        }
    }
}


string Change(NodeKind nk)//节点类型转换为字符串
{
    int i;
    for(i=0; i<19; i++)
    {
        if(nk==nodekind[i].nk)
        {
            return nodekind[i].str;
            break;
        }
    }
}
