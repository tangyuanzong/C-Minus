#include "scan.h"

char * copyString(char *s){
    if (s==NULL) return NULL;
    int n=strlen(s)+1;
    char * t=(char*)malloc(n);
    if (t==NULL) ;
    else
        strcpy(t,s);
    return t;
}

void match(TokenType expected){
    if(token==expected)
        token=GetToken();
    else
        printf("unexpected token");
}
TreeNode * newNode(NodeKind kind){
    TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
    if (t==NULL) ;
    else{
        for (int i=0; i<4; i++){
            t->child[i] = NULL;
        }
        t->sibling = NULL;
        t->nodekind = kind;
        t->lineno = lineno;
        if (kind==OpK||kind==IntK||kind==IdK){
            if(kind==IdK)
                t->attr.name = "";
        }
        if(kind==ConstK)
            t->attr.val = 0;
    }
    return t;
}

//declaration_list->declaration_list declaration | declaration
TreeNode * declaration_list(void) {
    TreeNode * t = declaration();
    TreeNode * p =t;
    while((token!=INT)&&(token!=VOID)&&(token!=ENDFILE)){
        token = GetToken();
        if(token==ENDFILE)
            break;
    }
    while(token==INT||token==VOID){
        TreeNode * q;
        q = declaration();
        if (q!=NULL){
            if (t==NULL){
                t=p=q;
            }
            else{
                p->sibling=q;
                p=q;
            }
        }
    }
    match(ENDFILE);
    return t;
}
TreeNode * declaration(void){ //declaration->var_declaration|fun_declaration
    TreeNode * t = NULL;
    TreeNode * p = NULL;
    TreeNode * q = NULL;
    TreeNode * s = NULL;
    TreeNode * a = NULL;
    if (token==INT){
        p=newNode(IntK);
        match(INT);
    }
    else{
        p=newNode(VoidK);
        match(VOID);
    }
    if(p!=NULL&&token==ID){
        q = newNode(IdK);
        q->attr.name = copyString(tokenString);
        match(ID);
        if (token==LPAREN){ //函数声明
            t = newNode(FuncK);
            t->child[0] = p;
            t->child[1] = q;
            match(LPAREN);
            t->child[2] = params(); //参数列表
            match(RPAREN);
            t->child[3] = compound_stmt(); //函数体
        }
        else if (token==LBRACKET){ //数组声明
            t = newNode(Var_DeclK);
            a = newNode(Arry_DeclK);
            t->child[0] = p;
            t->child[1] = a;
            match(LBRACKET);
            s = newNode(ConstK);
            s->attr.val = atoi(tokenString);
            match(NUM);
            a->child[0]=q;
            a->child[1]=s;
            match(RBRACKET);
            match(SEMI);
        }
        else if (token==SEMI){     //变量声明
            t = newNode(Var_DeclK);
            t->child[0] = p;
            t->child[1] = q;
            match(SEMI);
        }
    }
    return t;
}
TreeNode * params(void)  //params_list | void
{
    TreeNode * t = newNode(ParamsK);
    TreeNode * p = NULL;
    if (token==VOID){
        p = newNode(VoidK);
        match(VOID);
        if (token==RPAREN){
            if(t!=NULL)
                t->child[0] = p;
        }
        else//参数列表为(void id,[……])
        {
            t->child[0] = param_list(p);
        }
    }
    else//(token==INT)
    {
        t->child[0] = param_list(p);
    }
    return t;
}
TreeNode * param_list(TreeNode * k) //params_list->params_list,param | param
{
    TreeNode * t = param(k);
    TreeNode * p = t;
    k = NULL;//没有要传给 param 的 VoidK，所以将 k 设为 NULL
    while (token==COMMA){
        TreeNode * q = NULL;
        match(COMMA);
        q = param(k);
        if (q!=NULL){
            if (t==NULL){
                t=p=q;
            }
            else{
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}
TreeNode * param(TreeNode * k)
{
    TreeNode * t = newNode(ParamK);
    TreeNode * p = NULL;
    TreeNode * q = NULL;
    if(k==NULL&&token==VOID){
        p = newNode(VoidK);
        match(VOID);
    }
    else if(k==NULL&&token==INT){
        p = newNode(IntK);
        match(INT);
    }
    else if (k!=NULL){
        p = k;
    }
    if(p!=NULL){
        t->child[0] = p;
        if (token==ID){
            q = newNode(IdK);
            q->attr.name = copyString(tokenString);
            t->child[1] = q;
            match(ID);
        }
        if (token==LBRACKET&&(t->child[1]!=NULL))//@@@@@@@@@@@@
        {
            match(LBRACKET);
            t->child[2] = newNode(IdK);
            match(RBRACKET);
        }
        else
        {
            return t;
        }
    }
    return t;
}
TreeNode * compound_stmt(void)
{
    TreeNode * t = newNode(CompK);
    match(LBRACE);
    t->child[0] = local_declaration();
    t->child[1] = statement_list();
    match(RBRACE);
    return t;
}
TreeNode * local_declaration(void)
{
    TreeNode * t = NULL;
    TreeNode * q = NULL;
    TreeNode * p = NULL;
    while(token==INT || token==VOID){
        p = newNode(Var_DeclK);
        if(token==INT){
            TreeNode * q1 = newNode(IntK);
            p->child[0] = q1;
            match(INT);
        }
        else if(token==VOID){
            TreeNode * q1 = newNode(VoidK);
            p->child[0] = q1;
            match(INT);
        }
        if((p!=NULL)&&(token==ID)){
            TreeNode * q2 = newNode(IdK);
            q2->attr.name = copyString(tokenString);
            p->child[1] = q2;
            match(ID);
            if(token==LBRACKET){
                TreeNode * q3 = newNode(Var_DeclK);
                p->child[3] = q3;
                match(LBRACKET);
                match(RBRACKET);
                match(SEMI);
            }
            else if(token==SEMI){
                match(SEMI);
            }
            else{
                match(SEMI);
            }
        }
        if(p!=NULL)
        {
            if(t==NULL)
                t = q = p;
            else{
                q->sibling = p;
                q = p;
            }
        }
    }
    return t;
}
TreeNode * statement_list(void)
{
    TreeNode * t = statement();
    TreeNode * p = t;
    while (IF==token || LBRACKET==token || ID==token || WHILE==token ||
            RETURN ==token || SEMI==token || LPAREN==token || NUM==token)
    {
        TreeNode * q;
        q = statement();
        if(q!=NULL){
            if(t==NULL)
                t = p = q;
            else{
                p->sibling = q;
                p = q;
            }
        }
    }
    return t;
}
TreeNode * statement(void)
{
    TreeNode * t = NULL;
    switch(token){
    case IF:
        t = selection_stmt();
        break;
    case WHILE:
        t = iteration_stmt();
        break;
    case RETURN:
        t = return_stmt();
        break;
    case LBRACE:
        t = compound_stmt();
        break;
    case ID:
    case SEMI:
    case LPAREN:
    case NUM:
        t = expression_stmt();
        break;
    default:
        token = GetToken();
        break;
    }
    return t;
}
TreeNode * selection_stmt(void)
{
    TreeNode * t = newNode(Selection_StmtK);
    match(IF);
    match(LPAREN);
    if(t!=NULL)
        t->child[0] = expression();
    match(RPAREN);
    t->child[1] = statement();
    if(token==ELSE){
        match(ELSE);
        if(t!=NULL)
            t->child[2] = statement();
    }
    return t;
}
TreeNode * iteration_stmt(void)
{
    TreeNode * t = newNode(Iteration_StmtK);
    match(WHILE);
    match(LPAREN);
    if(t!=NULL)
        t->child[0] = expression();
    match(RPAREN);
    if(t!=NULL)
        t->child[1] = statement();
    return t;
}
TreeNode * return_stmt(void)
{
    TreeNode * t = newNode(Return_StmtK);
    match(RETURN);
    if (token==SEMI){
        match(SEMI);
        return t;
    }
    else{
        if(t!=NULL)
            t->child[0] = expression();
    }
    match(SEMI);
    return t;
}
TreeNode * expression_stmt(void)
{
    TreeNode * t = NULL;
    if(token==SEMI){
        match(SEMI);
        return t;
    }
    else{
        t = expression();
        match(SEMI);
    }
    return t;
}
TreeNode * expression(void)
{
    TreeNode * t = var();
    if(t==NULL)//不是以 ID 开头，只能是 simple_expression 情况
        t = simple_expression(t);
    else//以 ID 开头，可能是赋值语句，或 simple_expression 中的 var 和 call 类型的情况
        {
            TreeNode * p = NULL;
            if(token==ASSIGN)//赋值语句
    {
        p = newNode(AssignK);
            match(ASSIGN);
            p->child[0] = t;
            p->child[1] = expression();
            return p;
        }
        else //simple_expression 中的 var 和 call 类型的情况
        {
            t = simple_expression(t);
        }
    }
    return t;
}
TreeNode * var(void)
{
    TreeNode * t = NULL;
    TreeNode * p = NULL;
    TreeNode * q = NULL;
    if(token==ID)
    {
        p = newNode(IdK);
        p->attr.name = copyString(tokenString);
        match(ID);
        if(token==LBRACKET)
        {
            match(LBRACKET);
            q = expression();
            match(RBRACKET);
            t = newNode(Arry_ElemK);
            t->child[0] = p;
            t->child[1] = q;
        }
        else
        {
            t = p;
        }
    }
    return t;
}
TreeNode * simple_expression(TreeNode * k)
{
    TreeNode * t = additive_expression(k);
    k = NULL;
    if(EQ==token || GT==token || GEQ==token || LT==token || LEQ==token
            || NEQ==token)
    {
        TreeNode * q = newNode(OpK);
        q->attr.op = token;
        q->child[0] = t;
        t = q;
        match(token);
        t->child[1] = additive_expression(k);
        return t;
    }
    return t;
}
TreeNode * additive_expression(TreeNode * k)
{
    TreeNode * t = term(k);
    k = NULL;
    while((token==PLUS)||(token==MINUS))
    {
        TreeNode * q = newNode(OpK);
        q->attr.op = token;
        q->child[0] = t;
        match(token);
        q->child[1] = term(k);
        t = q;
    }
    return t;
}
TreeNode * term(TreeNode * k)
{
    TreeNode * t = factor(k);
    k = NULL;
    while((token==TIMES)||(token==OVER))
    {
        TreeNode * q = newNode(OpK);
        q->attr.op = token;
        q->child[0] = t;
        t = q;
        match(token);
        q->child[1] = factor(k);
    }
    return t;
}
TreeNode * factor(TreeNode * k)
{
    TreeNode * t = NULL;
    if(k!=NULL)//k 为上面传下来的已经解析出来的以 ID 开头的 var,可能为 call 或var
    {
        if(token==LPAREN && k->nodekind!=Arry_ElemK) //call
        {
            t = call(k);
        }
        else
        {
            t = k;
        }
    }
    else//没有从上面传下来的 var
    {
        switch(token)
        {
        case LPAREN:
            match(LPAREN);
            t = expression();
            match(RPAREN);
            break;
        case ID:
            k = var();
            if(LPAREN==token && k->nodekind!=Arry_ElemK)
            {
                t = call(k);
            }
            else//如果是连续计算，进入这一步
            {
                t=k;
            }
            break;
        case NUM:
            t = newNode(ConstK);
            if((t!=NULL)&&(token==NUM))
            {
                t->attr.val = atoi(tokenString);
            }
            match(NUM);
            break;
        default:
            token = GetToken();
            break;
        }
    }
    return t;
}
TreeNode * call(TreeNode * k)
{
    TreeNode * t = newNode(CallK);
    if(k!=NULL)
        t->child[0] = k;
    match(LPAREN);
    if(token==RPAREN)
    {
        match(RPAREN);
        return t;
    }
    else if(k!=NULL)
    {
        t->child[1] = args();
        match(RPAREN);
    }
    return t;
}

TreeNode * args(void)
{
    TreeNode * t = newNode(ArgsK);
    TreeNode * s = NULL;
    TreeNode * p = NULL;
    if(token!=RPAREN)
    {
        s = expression();
        p = s;
        while(token==COMMA)
        {
            TreeNode * q;
            match(COMMA);
            q = expression();
            if(q!=NULL)
            {
                if(s==NULL)
                {
                    s = p = q;
                }
                else
                {
                    p->sibling = q;
                    p = q;
                }
            }
        }
    }
    if(s!=NULL)
    {
        t->child[0] = s;
    }
    return t;
}

int blank_number=0;
void PreOrder(TreeNode* t)
{
    string blank=" ";
    int i;
    for(i=0; i<blank_number; i++)
    {
        blank+=" ";
    }
    if(t!=NULL)
    {
        if(t->nodekind==OpK)
        {
            cout<<blank<<"Op: "<<OpeLookUp(t->attr.op)<<endl;
            output+=blank+"Op: "+OpeLookUp(t->attr.op)+"\n";
        }
        else if(t->nodekind==IdK)
        {
            cout<<blank<<Change(t->nodekind)<<": "<<t->attr.name<<endl;
            output+=blank+Change(t->nodekind)+": "+t->attr.name+"\n";
        }
        else if(t->nodekind==ConstK)
        {
            cout<<blank<<Change(t->nodekind)<<": "<<t->attr.val<<endl;
            int n=t->attr.val;
            strstream ss;
            string s;
            ss << n;
            ss >> s;
            output+=blank+Change(t->nodekind)+": "+s+"\n";
        }
        else if(t->nodekind==AssignK)
        {
            cout<<blank<<"Assign"<<endl;
            output+=blank+"Assign"+"\n";
        }
        else if(t->nodekind==Selection_StmtK)
        {
            cout<<blank<<"If"<<endl;
            output+=blank+"If"+"\n";
        }
        else if(t->nodekind==Iteration_StmtK)
        {
            cout<<blank<<"While"<<endl;
            output+=blank+"While"+"\n";
        }
        else if(t->nodekind==Return_StmtK)
        {
            cout<<blank<<"Return"<<endl;
            output+=blank+"Return"+"\n";
        }
        else
        {
            cout<<blank<<Change(t->nodekind)<<endl;
            output+=blank+Change(t->nodekind)+"\n";
        }
    }
    for(i=0; i<MAXCHILDREN; i++)
    {
        if(t->child[i]!=NULL)
        {
            blank_number+=2;
            PreOrder(t->child[i]);
            blank_number-=2;
        }
    }
    if(t->sibling!=NULL)
    {
        PreOrder(t->sibling);
    }
}

void parse(void)
{
    TreeNode * t;
    cout<<"Syntax tree:"<<endl;
    token = GetToken();
    t = declaration_list();
    PreOrder(t);
}

int main()
{
    char* file=(char *)malloc(100);
    file = "test.c";
    source=fopen(file,"r");
    ofstream write;//输出文件
    string result;
    result=string(file)+"Result.txt";
    write.open(result);
    write<<"Syntax tree:"<<endl;
    parse();
    write<<output;
    write.close();
}
