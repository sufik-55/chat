#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <limits.h>
 
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include <dir.h>
#include <unistd.h>
#include <uv.h>
#include <conio.h>
#include <Windows.h>
#include <fcntl.h>

struct  ssds {
    int ssfd;
    int sdfsd;
    char sd;
};

char * str() {
    char string[999999];
    return string;
}

char * str2() {
    char *s;
    s = (char *) malloc(999999);
    return s;
}


int main() {
    /*char c[100] = {'s'};
    memset(c, 0, 1024);
    strcat(c, "dfdsfdsfd");
    char* s;*/
    //s = &"sdfdsfds";

    //for (long long i = 0; i < 100000000000; i++) { 
    //char **C = (char **)malloc(10*sizeof(char *)) ;


    //C = getchar();

    //putchar(C);
//"ооооо";
    
    //if (!(i % 100000000))

    /*char **t = (char **) malloc(sizeof(char *));
    t[10] = "fg";*/
    ///t = "dsd";
        /*printf("%p\n", "ftfgfg");
        printf("%p\n", t[10]);
        printf("%d\n", sizeof(char *));
        printf("%d\n", sizeof(char));*/

        /*for ( long long i=0; i < 100000000000; i++) {*/
            //char *t = (char*)malloc(sizeof(char));
             //printf("%d->%c\n", i, i);
             /*free(t);
        } */

    
    //}/*
    /*printf("%d\n", sizeof(struct  ssds));
    printf("%d\n", sizeof(char));
    printf("%d\n", sizeof(int));
    char *d = (char *)"sdfds";
    char *d2 = (char *)"sdfds";
    char *d3 = (char *)"sdfds";
    printf("%s\n", d);
    printf("%s\n", d2);
    printf("%s\n", d3);
    printf("%d", errno);*/
    /*if ((*(d2+2)) = 0) {
        printf("%d", errno);
    };*/
    
    /*printf("%s\n", d);
    printf("%s\n", d2);
    printf("%s\n", d3);
    printf("%s\n", "ddfdfdfdf");*/


//fputs("dsfsdfdsfsdsdfsdfsdfsdfsadf",stdout);
    /*getchar();
    putchar(10);
    putchar(10);
    putchar(10);*/
/*
    typedef union  {
        char char0;
        int int0;
        long long long0;
    } uni;*/
/*
    uni ddd;

    ddd.long0 = 84214154549;*/
/*
    printf("%d\n", ddd.char0);
    printf("%d\n", ddd.int0);
    printf("%d\n", ddd.long0);*/
/*
int len = 1000;
    char str[len];
    FILE * file;
    file = fopen("test.c", "r+a");

    if (file != NULL) {
        int i = 0;
        /*printf("%d\n", );*/
       /* while (fgets(str, len, file)) {
            printf("%s", str);
            i++;
        }*/
        /*printf("%d\n", i);*/
        /*for(int i = 0; i<1000; i++) {
            printf("%c", str[i]);
        }*/
    /*}
    else  {
        printf("%s", "false");
    }*/

    /*DIR *dir = opendir(".");
    if (dir != NULL) {
        struct dirent* ent;
        struct dirent *entryPtr = NULL;
        //int f = readdir_r(dir, &entryPtr, ent);
        while (readdir_r(dir, &entryPtr, &ent)) {
            
        }


    }*/
/*
    struct stat entryInfo;
    lstat()
*/
    //fun("/", 0);
    //fun2(".", 0);
    char *s;
    for (int i = 0; i < 1000000; i++ ) {
        s = str2();
        free(s);
    }

    getchar();
    //free(s);


    return 0;

}
/*

void fun(char * path, int count_t) {
    DIR * dir = opendir(path);
    struct dirent  ent;
    struct dirent *entryPtr = NULL;
    char pathName[PATH_MAX +1];
    if (dir == NULL) {
        printf( "Error opening %s: %s", path, strerror( errno ) );      
        return; 
    }

    int r = readdir_r(dir, &ent, &entryPtr);
    while ( entryPtr != NULL)
    {
        struct stat entryInfo;
        if (strcmp(ent.d_name, ".") == 0 || strcmp(ent.d_name, "..") == 0) {
           r = readdir_r(dir, &ent, &entryPtr);
           continue;
        }

        (void)strncpy( pathName, path, PATH_MAX );
        (void)strncat( pathName, "/", PATH_MAX );
        (void)strncat( pathName, ent.d_name, PATH_MAX );

        if (lstat(pathName, &entryInfo) == 0) {
            if (S_ISDIR( entryInfo.st_mode )){

                for (int i = 1; i <= count_t; i++){
                    printf("  ");
                }
                printf("%s(ПАПКА)\n", pathName);
                fun(pathName, count_t +1);
            }
            else {
                for (int i = 1; i <= count_t; i++){
                    printf("  ");
                }
                //i = 0;

                printf("%s(ФАЙЛ)\n", pathName);
            }
        }
        else {
            printf( "Error statting %s: %s\n", pathName, strerror( errno ) );
        }

        r = readdir_r(dir, &ent, &entryPtr);

    }

}
*/
/*    
    void fun2(char * path, int count_t) {
        //int r;    
        //uv_loop_t* loop = uv_loop_new();
        struct ffblk f;
        int done = findfirst("**", &f, 0);

    }
*/

