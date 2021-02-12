#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* hton, ntoh и проч. */
#include <arpa/inet.h>
#include <memory.h>
#include <pthread.h>
#include <stdio.h>

#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int socet_ID;
pthread_t* thread;
pthread_t* thread2;

enum message_type {
    NICK_NAME,
    MESSAGE_SEND,
    USERS,
    EXIT,
};

struct message_data
{
    enum message_type type;
    char *message;
    char *send_nick_name;
};

char* Serialize(struct message_data* this) {
    int type_l = sizeof(this->type);
    int message_l = strlen(this->message)+1;
    int send_nick_name_l = strlen(this->send_nick_name)+1;
    /*printf("%d\n",type_l);
    printf("%d\n",message_l);
    printf("%d\n",send_nick_name_l);*/

    char* t = (char*)&this->type;

    int l = type_l + message_l + send_nick_name_l + 3;
    /*printf("%d\n",l);*/
    char* result = malloc(l);
    for (int i = 0; i < type_l; i++) {
        //result[i] = 0x41;//t[i];
        result[i] = t[i];
    }
    result[type_l] = 0x1E;
    //result[type_l] = 0x30;
    for (int i = 0; i < message_l; i++) {
        result[type_l+1+i] = this->message[i];
    }
    result[type_l + message_l + 1] = 0x1E;
    //result[type_l + message_l + 1] = 0x30;

    for (int i = 0; i < send_nick_name_l; i++) {
        result[type_l+message_l +2+i] = this->send_nick_name[i];
    }

    result[type_l + message_l + send_nick_name_l +2] = 0x1E;
    //result[type_l + message_l + send_nick_name_l +2] = 0x30;
    
    return result;
};

struct message_data * Deserialize(char *message) {
    struct message_data * result;
    result = malloc(sizeof(struct message_data));
    int c = 0;
    //int j = 0;
    int past_separator = -1;

    for (int i = 0; i < 1024; i++){
        char* ch = &message[i];
        if (*ch == 0x1E) {
            int d = i - (past_separator+1);

            if (c == 0) {
                char *t = (char*)&result->type;
                
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                            
            }

            if (c == 1) {
                char *t = malloc(d);
                bzero(t, d);
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                result->message = t;
                
            }
            if (c == 2) {
                char *t = malloc(d);
                bzero(t, d);
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                result->send_nick_name = t;

            }
            c++;
            past_separator = i;
        }
    }

    return result;
};


void message_data_show(struct message_data* message_data) {
    printf("%d\n", message_data->type); 
    printf("%s\n", message_data->message); 
    printf("%s\n", message_data->send_nick_name); 
}

char* my_fgets (char *__restrict__ __s, int __n, FILE *__restrict__ __stream) {
        char * result = fgets(__s, __n, __stream);
        int i = strlen(__s)-1;
        if(__s[i]=='\n') __s[i] = '\0';
        return result;
}

void send_message(enum message_type type, char * message, char * name);

int my_listen(void *var) {
 
        for(;;)
        {
            //printf("\n");
            char buffer[1024];
            bzero(buffer, 1024);
            //memset(buffer, 0, 50*sizeof(char));
            int result = recv( socet_ID, buffer, sizeof(buffer) - 1, 0);
            if( result < 0 )
            {
                perror("Error calling recv");
                return 0;
            }
            if( result == 0 )
            {
                perror("Server disconnected");
                return 0;
            }
            if(strncmp(buffer, "Hi, dear!", 9) == 0){
                shutdown(socet_ID, 1);
                printf("%s\n","Пока");
                break;
            }
            else {
                /*for (int i = 0; i< 50; i++) {
                    char d[1] = {buffer[i]};
                    printf("%s -> %c -> %u;\n", d, buffer[i], buffer[i] );
                }*/
                struct message_data * response = Deserialize(buffer);

                if (response->type == NICK_NAME) {
                    printf("%s\n",response->message);
                }
                if (response->type == MESSAGE_SEND) {
                    printf("(%s) %s\n",response->send_nick_name, response->message );
                }
                if (response->type == EXIT){
                    printf("%s\n","Пока");
                    break;
                }

                free(response->message);
                free(response->send_nick_name);
                free(response);

                //if (strncmp(buffer, "Введите Nick Name:\n", 9) == 0) {
                   /* printf("%s",buffer);
                    bzero(buffer, 256);
                    char read[256];
                    gets(read);

                    struct message_data m = {NICK_NAME, read, "Sufik" };
                    char* sendData = Serialize(&m);
                    result = send( s, sendData, 255, MSG_NOSIGNAL);
                    if (result < 0) {
                        printf("Ошибка");
                    }
                    free(sendData);

                    message_data_show(&m);*/
                    //printf("%d\n",result);
                    //printf("%s\n",read);

                //}
            }
        }
        return 0;

};

int command(void *var) {

    for (;;) {
        printf("Введите команду\n");

        char comm[256];
        bzero(comm, 256);

        //gets(comm);
        /*fgets(comm, 10, stdin);
        int i = strlen(comm)-1;
        if(comm[i]=='\n') comm[i] = '\0';*/
        my_fgets(comm, 256, stdin);
        if (strcmp("users", comm) == 0 ) {
            send_message(USERS, "", "");

           /* struct message_data request = {USERS, "", ""};
            char* requestChar = Serialize(&request);
            errno = 0;
            int result = send( socet_ID, requestChar, 255, MSG_NOSIGNAL);
            if (result < 0) {
                printf("Ошибка %d\n", errno);
            }
            free(requestChar);*/

            continue;
        } 
        else if (strcmp("send", comm) == 0 ) { 
            printf("Введите имя получателя\n");
            char nick_name[256];
            //gets(nick_name);
            my_fgets(nick_name, 256, stdin);
            printf("Введите сообщение\n");
            char message[256];
            //gets(message);
            my_fgets(message, 256, stdin);
            send_message(MESSAGE_SEND, message, nick_name);
            /*struct message_data request = {MESSAGE_SEND, message, nick_name};
            char* requestChar = Serialize(&request);
            int result = send( socet_ID, requestChar, 255, MSG_NOSIGNAL);
            if (result < 0) {
                printf("Ошибка %d\n", errno);
            }
            free(requestChar);*/
            continue;
        }
        else if (strcmp("name", comm) == 0) {
            printf("Введите имя:\n");
            char nick_name[256];
            //gets(nick_name);
            my_fgets(nick_name, 256, stdin);
            send_message(NICK_NAME, "", nick_name);
            /*struct message_data request = {NICK_NAME, "", nick_name};
            char* requestChar = Serialize(&request);
            int result = send( socet_ID, requestChar, 255, MSG_NOSIGNAL);
            if (result < 0) {
                printf("Ошибка %d\n", errno);
            }
            free(requestChar);*/
            continue;
        }
        else if (strcmp("exit", comm) == 0) {
            send_message(EXIT, "", "");
            //free(s);
            break;
        }
        else if(strcmp("select chat", comm) == 0) {
            send_message(USERS, "", "");
            loop: 
            printf("Введите имя:\n");
            char nick_name[256];
            bzero(nick_name, 256);
            my_fgets(nick_name, 256, stdin);
            if (strcmp("exit", nick_name) == 0) {
                bzero(nick_name, 256);
                continue;
            }
            if(strcmp("", nick_name) == 0) {
                goto loop;
            }
            printf("Вы вошли в чат: %s\n", nick_name);
            for(;;) {
                char mes[256];
                bzero(mes, 256);
                my_fgets(mes, 256, stdin);
                if (strcmp("exit", mes) == 0) {
                    bzero(mes, 256);
                    break;
                }

                send_message(MESSAGE_SEND, mes, nick_name);


            }

            continue;




        }
        else if (strcmp("help", comm) == 0) {
            printf("Команды:\n%s%s%s%s%s%s",
             "name(Установить имя)\n", 
             "send(Отправить сообщение)\n",
             "users(Получить список пользователей)\n",
             "help(Помощь)\n",
             "exit(Выход)\n",
             "select chat(Выбрать диалог общения)\n"
             );
        }
    }

};

void send_message(enum message_type type, char * message, char * name) {
    struct message_data response = {type, message, name};
    int l;
    char* responseSer = Serialize(&response);
    int result = send( socet_ID, responseSer, 512, MSG_NOSIGNAL);
    if (result < 0) {
        printf("Ошибка %d\n", errno);
    };
    free(responseSer);    
}


int main(int argc, char * argv[])
{
    /*char* fff;
    printf("%p\n", fff);
    fff = "ggg"; 
    printf("%p\n", fff);
    fff = "ggg"; 
    printf("%p\n", fff);
    
    fff = malloc(3);
    fff[0] = 'g'; 
    fff[1] = 'g'; 
    fff[2] = 'g'; 
    printf("malloc %p\n", fff);
    //fff = "ggg5";
    printf("%p\n", fff);

    free(fff);
    printf("free %p\n", fff);
    //free(fff);
    if (NULL == "") {
        printf("free %s\n", "true");
    }
    else {
        printf("free %s\n", "false");
    }

    if (NULL == NULL) {
        printf("free %s\n", "true");
    }
    else {
        printf("free %s\n", "false");
    }

    printf("free %s\n", NULL);

    return 0;*/

    socet_ID = socket( AF_INET, SOCK_STREAM, 0 );
    if(socet_ID < 0)
    {
            perror( "Error calling socket" );
            return 0;
    }

    /*соединяемся по определённому порту с хостом*/
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons( 18666 );
    peer.sin_addr.s_addr = inet_addr( "127.0.0.1" );
    int result = connect( socet_ID, ( struct sockaddr * )&peer, sizeof( peer ) );
    if( result )
    {
            perror( "Error calling connect" );
            return 0;
    }

    fd_set readmask;
    fd_set allreads;
    FD_ZERO( &allreads );
    FD_SET( 0, &allreads );
    FD_SET( socet_ID, &allreads );

    thread = (pthread_t*) malloc(sizeof(pthread_t));
    thread2 = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, (void*)my_listen, NULL);
    pthread_create(thread2, NULL, (void*)command, NULL);
    pthread_join(*thread, NULL);
    pthread_join(*thread2, NULL);
    free(thread);
    free(thread2);
    return 0;


      /*объявляем сокет*/
      /*int s = socket( AF_INET, SOCK_STREAM, 0 );
      if(s < 0)
      {
              perror( "Error calling socket" );
              return 0;
      }*/

      /*соединяемся по определённому порту с хостом*/
      /*struct sockaddr_in peer;
      peer.sin_family = AF_INET;
      peer.sin_port = htons( 18666 );
      peer.sin_addr.s_addr = inet_addr( "127.0.0.1" );
      int result = connect( s, ( struct sockaddr * )&peer, sizeof( peer ) );
      if( result )
      {
              perror( "Error calling connect" );
              return 0;
      }*/

      /*посылаем данные
       *
       * Если быть точным, данные не посланы, а записаны где-то в стеке, когда и как они будут
       * отправлены реализации стека TCP/IP виднее. Зато мы сразу получаем управление, не
       * дожидаясь у моря погоды.*/
      //char buf[] = "Hello, world!";
      //result = send( s, "Hello, sufik!", 13, 0);
      //printf("%d result\n", result);
      /*if( result <= 0 )
      {
              perror( "Error calling send" );
              return 0;
      }*/
      /* закрываем соединения для посылки данных */
     /* int shutdownResult = shutdown(s, 1);
      printf("%d shutdownResult\n", shutdownResult);
      if( shutdownResult < 0)
      {
              perror("Error calling shutdown");
              return 0;
      }*/

      /* читаем ответ сервера */
      /*fd_set readmask;
      fd_set allreads;
      FD_ZERO( &allreads );
      FD_SET( 0, &allreads );
      FD_SET( s, &allreads );
      for(;;)
      {*/
              //printf("result\n");
              /*readmask = allreads;
              if( select(s + 1, &readmask, NULL, NULL, NULL ) <= 0 )
              {
                      perror("Error calling select");
                      return 0;
              }
              if( FD_ISSET( s, &readmask ) )
              {*/
                    //printf("\n");
                      /*char buffer[256];
                      bzero(buffer, 256);
                      //memset(buffer, 0, 50*sizeof(char));
                      int result = recv( s, buffer, sizeof(buffer) - 1, 0);
                        if( result < 0 )
                        {
                            perror("Error calling recv");
                            return 0;
                        }
                        if( result == 0 )
                        {
                            perror("Server disconnected");
                            return 0;
                        }
                        if(strncmp(buffer, "Hi, dear!", 9) == 0)
                            printf("Got answer. Success.\n");
                        else {
                            
                            if (strncmp(buffer, "Введите Nick Name:\n", 9) == 0) {
                                printf("%s",buffer);
                                bzero(buffer, 256);
                                char read[256];
                                gets(read);

                                struct message_data m = {NICK_NAME, read, "Sufik" };
                                char* sendData = Serialize(&m);
                                result = send( s, sendData, 255, MSG_NOSIGNAL);
                                if (result < 0) {
                                    printf("Ошибка");
                                }
                                free(sendData);

                                message_data_show(&m);
                                //printf("%d\n",result);
                                //printf("%s\n",read);

                            }
                        }
                      //printf("%s\n",buffer);
              /*}
              if( FD_ISSET( 0, &readmask ) )
              {
                      printf( "No server response" );
                      return 0;
              }*/

                /*char buffer[20];
                int result = recv( s, buffer, sizeof(buffer) - 1, 0 );

              printf(buffer);*/
      /*}
      return 0;*/
}