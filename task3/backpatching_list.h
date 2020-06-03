///
/// Created by Zehua Liu on 5/30/20.
///
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
struct listNode {
    struct listNode *next;
    int quad;// the line number of a quad
};

struct listNode* makelist(int quad){
    struct listNode* node;
    node = (struct listNode*)malloc(sizeof(struct listNode));
    node -> quad = quad;
    return node;
}

struct listNode* merge(struct listNode* l1,struct listNode* l2){
    struct listNode* l=(struct listNode*)malloc(sizeof(struct listNode));
    struct listNode* now=l;
    while(l1){
        struct listNode* tmp = (struct listNode*)malloc(sizeof(struct listNode));
        tmp->quad=l1->quad;
        now->next=tmp;
        now=tmp;
        l1=l1->next;
    }
    while(l2){
        struct listNode* tmp = (struct listNode*)malloc(sizeof(struct listNode));
        tmp->quad=l2->quad;
        now->next=tmp;
        now=tmp;
        l2=l2->next;
    }
    return l->next;
}




