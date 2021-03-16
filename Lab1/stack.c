#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

struct Node{
    int data;
    struct Node* next;
};


struct Node* top = NULL;
void push(int data){
    struct Node* newNode = malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = top;
    top = newNode;
}

void pop(){
    if(top == NULL){
        printf("Stack is Empty !! Aborted \n");
    }else{
        struct Node* delNode = top;
        top = top->next;
        free(delNode);
    }
}

void display(){
    if(top == NULL){
        printf("Stack is Empty!!\n");
    }else{
        struct Node* currNode = top;
        while(currNode != NULL){
            printf("%d\n",currNode->data);
            currNode = currNode->next;
        }
    }
}

int main(int argc, char* argv[]){
    push(1);
    push(2);
    push(3);
    pop();
    display();
    return 0;
}