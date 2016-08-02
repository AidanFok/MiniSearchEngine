//
//  main.c
//  mini search engine
//
//  Created by ??? on 16/3/19.
//  Copyright ?2016? ???. All rights reserved.
//

//#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct TrieNode* Trie; //the Trie data structure
typedef struct ListNode* List; //store the IDs
struct ListNode{
    List next, tail;
    int file, line;   //for a particular word found, use a linked-list to store the number of file and line
};
struct TrieNode{
    Trie pointer[26];        //26 pointers point to 26 letter
    List L;                  //the linked-list is embedded in the Trie
};

int tempcnt=0;
int tempans[1000000][2];
int idx[1000];
Trie T;
Trie initial(){
    Trie T=(Trie)malloc(sizeof(struct TrieNode));// alloc space
    T->L=NULL;
    //printf("In initial: %p\n", &(T->L));
    for(int i=0;i<26;i++)T->pointer[i]=NULL;
    return T;
}
Trie insert(char *s,int file_num,int line_num,Trie T) //insert a word s into a trie T
{
    List TempL;
    Trie TempT;
    if (strlen(s) == 0) //if the word is inserted successfully
    {
        TempL = (List)malloc(sizeof(struct ListNode)); //alloc a new node to store the number of file and line
        TempL->next = NULL;
        TempL->tail = TempL; //tail point to itself
        TempL->file = file_num;
        TempL->line = line_num;
        
        //printf("In insert: %p\n", T);
        //T = (Trie)malloc(sizeof(struct TrieNode));

        if(T->L)                      //if this word has been inserted before
        {
            T->L->tail->next = TempL;
            T->L->tail = TempL;
        }else                        //insert this word first time
            T->L = TempL;
    } else
    {
        int ch = s[0];//deal with the first letter
        if(ch>='a') ch-='a';//turn 'a'~'z' to a integer
        else  ch-='A';   //turn 'A'~'Z' to a integer
        TempT = (T->pointer[ch])?T->pointer[ch]:initial(); //if this letter hasn't been inserted before then make a new one
        T->pointer[ch] = insert(s+1,file_num,line_num,TempT);//recursively insert the string and deal with the next letter
    }
    return T;
}

void find(char *s,Trie T)            //find a word s in a trie T
{
    int last = -1;
    if(!T)return;                    //the word does not exist
    if(strlen(s) == 0)                //if the word is found
    {
        List Temp = T->L;
        tempcnt=0;
        while(Temp)                   //deal with the whole linked-list
        {
            tempcnt++;
            tempans[tempcnt][0] = Temp->file;
            tempans[tempcnt][1] = Temp->line;
            if (last != Temp->file)
                idx[Temp->file]++; //update the appearances of file_num
            last = Temp->file;
            Temp = Temp->next;
        }
        for(int i=1;i<=tempcnt;i++){
            printf("file_num:%d line_num:%d\n",tempans[i][0],tempans[i][1]);
        }
        if(tempcnt)printf("total frequency:%d\n",tempcnt);
        else printf("the word does not exist or it's a noisy word\n");

        
        
    } else
    {
        int ch = s[0];                //deal with the current letter
        if (ch>='a') ch-='a'; else    //turn 'a'~'z' to integer
            ch-='A';         //turn 'A'~'Z' to integer
        find(s+1,T->pointer[ch]);    //recursively find the word and deal with the next letter
    }
}

double split(char *s,int file_num,int line_num) //split a sentence into several words and insert
{
    int p = 0, len= strlen(s), word_len=0;
    double word_count=0;
    char word[100];
    while(p <= len)
    {
        if ((s[p]>='A'&&s[p]<='Z')||(s[p]>='a'&&s[p]<='z'))               //if a letter is read
        {
            if (word_len < 100) word[word_len] = s[p];
            word_len++;
        }
        else            //a space is read or it is end of the line
        {//printf("word_len:%d\n",word_len);
            word[word_len] = 0;
            if(word_len!=0){
                word_count++;
                T = insert(word,file_num,line_num,T);}
            word_len = 0;                         //reset the length of word to be 0
        }
        p++;                                      //move to next position
    }
    return word_count;//return the word count in one sentence
}

int word_count_all[1000];

int create_trie(FILE* fp,int j){
    //create the inverted index
    char s[1000];int line_num=0;
    
    long int leng=0;
    if(fp!=NULL){
        while(fgets(s,1000,fp)!=NULL){
            line_num++;
            leng=strlen(s);
            // printf("%s\n%d\n",s,line_num);
            word_count_all[j]+=split(s,j,line_num);
            // printf("%d\n",word_count_all);
            
        }
    }
    else{
        printf("error\n");}
    return word_count_all[j];
}

Trie delete_noisy(Trie T){//delete the noisy word
    List Temp = T->L;
    tempcnt=0;
    while(Temp)                   //deal with the whole linked-list
    {
        tempcnt++;
        Temp = Temp->next;
    }
    if(tempcnt>1000)T->L=NULL;//if frequency>1000, it is a noisy word, delete it
    return T;
}

Trie find_noisy(Trie T){//ergodic the trie recursively
    if(T!=NULL){
        T=delete_noisy(T);
        for(int i=0;i<26;i++)find_noisy(T->pointer[i]);
    }
    return T;
}



int main()
{T=initial();
    for(int k=0;k<1000;k++)word_count_all[k]=0;
    for(int j=0;j<=968;j++){
       
        int flag=0;//for xcode, flag=1;for windows, flag=0;
        
        if(flag){
        char filename[10];
        char entire_add[300]="/Users/aidanfok/Documents/大三下/Data\ Structure/mini\ search\ engine/out/";
        memset(filename,0,sizeof(filename));
        sprintf(filename,"%d",j);
        strcat(filename,".txt");
        strcat(entire_add, filename);
        FILE* fp;
        fp=fopen(entire_add,"r");
        create_trie(fp,j);
        printf("word count of %d file:%d\n",j,word_count_all[j]);
            fclose(fp);}
        else {
            char filename[10];
            memset(filename,0,sizeof(filename));
            sprintf(filename,"%d",j);
            strcat(filename,".txt");
            FILE* fp;
            fp=fopen(filename,"r");
            create_trie(fp,j);
            printf("word count of %d file:%d\n",j,word_count_all[j]);
            fclose(fp);
        }
    }
    T=find_noisy(T);
    
    char a[1000]="pluck";
    find(a, T);
    return 0;
}
