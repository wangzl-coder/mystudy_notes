#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


struct book_st{
    char *name;
    char *type;
    int price;
    int pages;
};

static void printbook(struct book_st *book)
{
    if(book == NULL) {
        printf("invaild book param \r\n");
        return;
    }
    printf("book address:%p, name: %s, type: %s, price: %d, pages: %d \r\n", book, book->name, book->type, book->price, book->pages);
}

static void *thread_func1(void *arg)
{
    struct book_st book;
    book.name = "UNIX Env Program";
    book.type = "study";
    book.price = 89;
    book.pages = 700;
    //printbook(arg);    
    printbook(&book);
    pthread_exit(&book);
}

static void *thread_func2(void *arg)
{
    struct book_st *book = malloc(sizeof(struct book_st));
    if(book == NULL) {
        fprintf(stderr, "malloc failed \r\n");
        pthread_exit(NULL);
    }
    book->name = "UNIX Env Program";
    book->type = "study";
    book->price = 89;
    book->pages = 700;
    //printbook(arg);
    printbook(book);
    pthread_exit(book);
}

int main()
{
    int err;
    pthread_t tid;
    struct book_st engbook;
    struct book_st *retbook;
    
    engbook.name = "english";
    engbook.type = "study";
    engbook.price = 20;
    engbook.pages = 200;
    printf("parent start 1 thread \r\n");
    err = pthread_create(&tid, NULL, thread_func1, &engbook);
    if(err) {
        fprintf(stderr, "create thread 1'd failed \r\n");
        exit(-1);
    }
    pthread_join(tid, (void **)&retbook);
    printbook(retbook);
    printf("parent start 2 thread \r\n");
    err = pthread_create(&tid, NULL, thread_func2, &engbook);
    if(err) {
        fprintf(stderr, "failed to create 2'd thread \r\n");
        exit(-1);
    }
    pthread_join(tid, (void **)&retbook);
    printbook(retbook);
    free(retbook);
    exit(0);
}
