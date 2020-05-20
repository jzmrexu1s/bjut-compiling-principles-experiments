//
// Created by Chengwei Zhang on 5/21/20.
//

struct ast{
    int expressionType;
    int number;
    struct ast *left;
    struct ast *middle;
    struct ast *right;
    char idn[5];
};

struct ast *newastwithidn(int expressionType, struct ast *idn, struct ast *l, struct ast *m, struct ast *r)
{
    struct ast *a;
    a = (struct ast*)malloc(sizeof(struct ast));
    a->expressionType = expressionType;
    a->number = -1;
    strcpy(a -> idn, idn -> idn);
    a->left = l;
    a->middle = m;
    a->right = r;
    return a;

}

//一般建立节点的函数
struct ast *newast(int expressionType, struct ast *l, struct ast *m, struct ast *r)
{
    struct ast *a;
    a = (struct ast*)malloc(sizeof(struct ast));
    a->expressionType = expressionType;
    a->number = -1;
    memset(a->idn, 0, sizeof(a->idn));
    a->left = l;
    a->middle = m;
    a->right = r;
    return a;
}

struct ast *newnum(int expressionType, double number)//建立数据型节点的函数
{
    struct ast *a;
    a = (struct ast*)malloc(sizeof(struct ast));
    a->expressionType = expressionType;
    a->number = number;
    memset(a->idn, 0, sizeof(a->idn));
    a->left = NULL;
    a->middle = NULL;
    a->right = NULL;
    return a;
}

struct ast *newidn(int expressionType, char *idn)//建立idn节点函数
{

    struct ast *a;
    a = (struct ast*)malloc(sizeof(struct ast));
    a->expressionType = expressionType;
    a->number = -1;
    strcpy(a -> idn, idn);
    a->left = NULL;
    a->middle = NULL;
    a->right = NULL;

    return a;
}

char* getPrintPattern(int type) {
    char *s;
    s = (char*)malloc(30);
    if (type == 1) strcpy(s, "P -> L");
    else if (type == 2) strcpy(s, "P -> LP");
    else if (type == 3) strcpy(s, "L -> S;");
    else if (type == 4) strcpy(s, "S -> id = E");
    else if (type == 5) strcpy(s, "S -> if C then S'");
    else if (type == 6) strcpy(s, "S -> while C do S");
    else if (type == 7) strcpy(s, "S -> {P}");
    else if (type == 8) strcpy(s, "S' -> S");
    else if (type == 9) strcpy(s, "S' -> S else S");
    else if (type == 10) strcpy(s, "C -> EC'");
    else if (type == 11) strcpy(s, "C' -> > E");
    else if (type == 12) strcpy(s, "C' -> < E");
    else if (type == 13) strcpy(s, "C' -> = E");
    else if (type == 14) strcpy(s, "E -> T");
    else if (type == 15) strcpy(s, "E -> + T");
    else if (type == 16) strcpy(s, "E -> - T");
    else if (type == 17) strcpy(s, "T -> F");
    else if (type == 18) strcpy(s, "T -> * F");
    else if (type == 19) strcpy(s, "T -> / F");
    else if (type == 20) strcpy(s, "F -> ( E )");
    else if (type == 21) strcpy(s, "F -> id");
    else if (type == 22) strcpy(s, "F -> int8");
    else if (type == 23) strcpy(s, "F -> int10");
    else if (type == 24) strcpy(s, "F -> int16");
    else strcpy(s, "UNDEFINED");
    return s;
}

void displayAST(struct ast* root, FILE* f) //以最左派生顺序输出结果
{
    if (root == NULL)
    {
        printf("root is NULL.\n");
        return;
    }
    char* pattern = getPrintPattern(root -> expressionType);
    fprintf(f, "%s", pattern);
    if (root -> number != -1)
    {
        if (root -> expressionType >= 22 && root -> expressionType <= 24)
            fprintf(f, "\t%d", (int)root -> number);
    }
    if (strlen(root -> idn) > 0)
        fprintf(f, "\t%s", root -> idn);

    fprintf(f, "\n");

    if (root -> left != NULL)
        displayAST(root -> left, f);
    if (root -> middle != NULL)
        displayAST(root -> middle, f);
    if (root -> right != NULL)
        displayAST(root -> right, f);
}
