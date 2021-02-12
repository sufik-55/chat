/* определяет типы данных */
#include <sys/types.h>
/* "Главный" по сокетам */
#include <sys/socket.h>
/*#include <winsock2.h>*/

/* sockaddr_in struct, sin_family, sin_port, in_addr_t, in_port_t, ...*/
#include <netinet/in.h>
/*#include <windows.h>*/
#include <pthread.h>


#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
//#include <CException/CException.h>
#include <stdlib.h>
/*#include <regex.h> */
/*#include <ErrorCode.h>*/
/*#include <TriangleArea.h>*/

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

char* Serialize(struct message_data* this, int * length) {
    int type_l = sizeof(this->type);
    int message_l = strlen(this->message)+1;
    int send_nick_name_l = strlen(this->send_nick_name)+1;
    /*printf("%d\n",type_l);
    printf("%d\n",message_l);
    printf("%d\n",send_nick_name_l);*/

    char* t = (char*)&this->type;

    int l = type_l + message_l + send_nick_name_l + 3;
    *length = l;
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

struct message_data Deserialize(char *message) {
    struct message_data result;
    int c = 0;
    //int j = 0;
    int past_separator = -1;

    for (int i = 0; i < 1024; i++){
        char* ch = &message[i];
        if (*ch == 0x1E) {
            int d = i - (past_separator+1);

            if (c == 0) {
                char *t = (char*)&result.type;
                
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                            
            }

            if (c == 1) {
                char *t = malloc(d);
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                result.message = t;
                
            }
            if (c == 2) {
                char *t = malloc(d);
                for (int j = 0, k = past_separator + 1; j < d; j++, k++) {
                    t[j] = message[k];
                }
                result.send_nick_name = t;

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
};



struct sockaddr_in addr;
int socket_ID;

//struct client_data *clients_data;

struct client_data
{
    int socket_id;
    char* nick_name;
};

struct all_client_data {
    int size;
    int count;
    struct client_data** array;
};

struct all_client_data clients;

struct all_client_data all_client_data_constructor(int size) {
        struct all_client_data object ={
            size, 
            0, 
            malloc(clients.size * sizeof(struct client_data))
        };      
        /*object.size = 10;
        object.count = 0;
        object.array = malloc(clients.size * sizeof(struct client_data));*/
        return object;
};

void all_client_data_add(struct all_client_data* object ,struct client_data *val) {
    /*printf("Count %d\n", object->count);
    printf("Count %d\n", object->size);*/
    if (object->size == object->count) {        
        int newSize = object->size + 10;
        struct client_data** array = malloc(newSize * sizeof(struct client_data));
        for (int i = 0;  i < object->size; i++) {
            array[i] = object->array[i];
        }
        free(object->array);
        object->array = array;
        object->size = newSize;

        
    }

    
    object->array[object->count] = val;
    object->count = object->count + 1;
    /*printf("Count %s\n", "ddd");
    printf("Count %d\n", object->count);
    printf("Count %d\n", object->size);
    printf("Count %p\n", val);
    printf("Count %p\n", object->array[object->count]);*/
    
}

void all_client_data_remove(struct all_client_data* object ,struct client_data *val) {
    int ii = 0;
    for (int i = 0; i < object->count; i++) {
        struct client_data * item = clients.array[i];
        if (item == val) {
            ii = i;
            break;
        }
    }

    clients.array[ii] = clients.array[object->count -1];
    object->count = object->count - 1;

}

void send_message(struct client_data * to, enum message_type type, char * message, char * name);

char* my_fgets (char *__restrict__ __s, int __n, FILE *__restrict__ __stream) {
        char * result = fgets(__s, __n, __stream);
        int i = strlen(__s)-1;
        if(__s[i]=='\n') __s[i] = '\0';
        return result;
}



int communication_client(void* clientDataVoid) {
    printf("communication_client\n");
    struct client_data* clientData = clientDataVoid;
    printf("Connected name: %s\n", clientData->nick_name);

    /*char mess[] = "Введите Nick Name:\n";
    //send(clientData->socket_id, mess, sizeof(mess), 0);
    int i = 0;
    int e;
    char third[512];*/
    int result;

    /*struct message_data mess = {NICK_NAME, "Введите имя:", ""};
    int l;
    char* messSer = Serialize(&mess, &l);

    /*printf("Проверка %s отключен\n", messSer); 
    printf("Проверка %d отключен\n", l); */
    /*result = send( clientData->socket_id, messSer, l, MSG_NOSIGNAL/*, (struct sockaddr *)&addr, sizeof(addr)*//*);
    if (result < 0) {
        printf("Клиент %s отключен\n", clientData->nick_name); 
    }*/
    /*printf("Прова %s отключен\n", messSer); 
    printf("Прока %d отключен\n", l); */
    //free(messSer);
    /*free(mess.message);
    free(mess.send_nick_name);*/

    for(;;)
    {
        // char mess[] = "lslslslslslslslslslsl";
        // send(s1,mess,sizeof(mess), 0);
        char buffer[1024];
        bzero(buffer, 1024);
        //следует помнить, что данные поступают неравномерно
        int rc = recv(clientData->socket_id, buffer, sizeof(buffer)-1, 0);
        //printf("000 %s\n", "dfdfdfdfdf");
        
        /*char mess[] = "lslslslslslslslslslsl";
        send(clientData->socket_id, mess, sizeof(mess), MSG_NOSIGNAL);*/

        //printf("%d\n", rc);
        //printf("%s\n", buffer);
        if( rc < 0 )
        {
                //чтение может быть прервано системным вызовом, это нормально
                if( errno == EINTR )
                        continue;
                perror("Can't receive data.");
                return 0;
        }
        if( rc == 0 )
        {
            break;
        }
        if (rc > 0) {
            //printf("1111 %s\n", buffer);
            struct message_data request = Deserialize(buffer);
            //message_data_show(&mass);
            //printf("1111 %s\n", buffer);
            if (USERS == request.type) {
                printf("Запрос пользователей\n  Кто запрашивает: %s\n"
                        , clientData->nick_name);

                char users[1024];
                bzero(users, 1024);
                char n[8];
                bzero(n, 8);

                for(int i = 0; i < clients.count; i++) {
                    struct client_data * item = clients.array[i];
                    //printf("%s\n", item->nick_name);
                    strcat(users, "\n");
                    sprintf(n, "%d", i+1);
                    strcat(users, n);
                    strcat(users, "->");
                    printf("%s", users);
                    strcat(users, item->nick_name == NULL ? "NULL": item->nick_name);
                    printf("%s", users);
                    if (item == clientData) {
                        strcat(users, "(Это вы)");
                    }

                    /*if (i == clients.count-1) {
                        strcat(users, "\n");
                    }*/
                    //strcat(users, "\n");
                }  

                send_message(clientData, MESSAGE_SEND, users, "system");
                /*struct message_data response = {MESSAGE_SEND, users, "system"};
                int l;
                char* responseSer = Serialize(&response, &l);
                result = send( clientData->socket_id, responseSer, l, MSG_NOSIGNAL);
                if (result < 0) {
                    printf("Клиент %s отключен\n", clientData->nick_name); 
                };
                free(responseSer);*/


            }
            else if (MESSAGE_SEND == request.type) {
                printf(
                    "Отправка сообщения\n    Кто отправляет: %s\n    Кому отправляет: %s\n"
                    , clientData->nick_name
                    , request.send_nick_name);

                    if (NULL == clientData->nick_name || strcmp("", clientData->nick_name) == 0) {
                        send_message(clientData, MESSAGE_SEND, "Введитие имя прежде чем отправить сообщение", "system");                        
                    }
                    else if (strcmp("", request.send_nick_name) == 0) {
                        send_message(clientData, MESSAGE_SEND, "Введите имя получателя", "system");
                    }
                    else if (strcmp("All", request.send_nick_name) == 0
                    || strcmp("all", request.send_nick_name) == 0 
                    || strcmp("ALL", request.send_nick_name) == 0
                    ) {
                        for (int i = 0; i < clients.count; i++) {
                            struct client_data * item = clients.array[i];
                            if (item == clientData) {
                                continue;
                            }

                            send_message(item, MESSAGE_SEND, request.message, clientData->nick_name);

                            /*struct message_data response = {MESSAGE_SEND, request.message, clientData->nick_name };
                            int l;
                            char* responseSer = Serialize(&response, &l);
                            result = send( item->socket_id, responseSer, l, MSG_NOSIGNAL);
                            if (result < 0) {
                                printf("Клиент %s отключен\n", item->nick_name); 
                            };
                            free(responseSer);  */                      
                        }

                    }
                    else
                    {
                        struct client_data * sendUser = NULL;
                        for(int i = 0; i < clients.count; i++){
                            struct client_data * item = clients.array[i];
                            /*if (item == clientData) {
                                continue;
                            }*/
                            if (item->nick_name != NULL && strcmp(item->nick_name, request.send_nick_name) == 0) {
                                sendUser = item;
                            }

                        }

                        if (sendUser == NULL) {
                            send_message(clientData, MESSAGE_SEND, "Пользователь с таким именем не найден", "system");  
                        }
                        else {
                            send_message(sendUser, MESSAGE_SEND, request.message, clientData->nick_name); 
                        }

                    }  
                    /*else if(true) {

                    }*/

            }
            else if (NICK_NAME == request.type) {
                printf("Устоновка имени\n    Было: %s\n    Стало:%s\n"
                        , clientData->nick_name
                        , request.send_nick_name);

                int f = 0;
                //printf("clients.count %d\n", clients.count);
                for(int i = 0; i < clients.count; i++) {
                    //printf("-1Устоновка имени %d\n", i);
                    struct client_data * item = clients.array[i];
                    //printf("0Устоновка имени %d\n", i);
                    /*printf("ffffffff имени %p\n", item);
                    printf("ffffffff имени %p\n", clientData);*/
                    if (item == clientData) {
                        continue;
                    }
                    //printf("1Устоновка имени %d\n", i);

                    //printf("    Устоновка имени %p\n", &item);
                    //printf("    Устоновка имени %p\n", &clientData);
                    //printf("1Устоновка имени %s\n", s);
                    if (NULL != item->nick_name && strcmp(item->nick_name, request.send_nick_name) == 0) {
                        f = 1;
                        printf("1Устоновка имени %s\n", item->nick_name);
                        printf("1Устоновка имени %s\n", request.send_nick_name);
                    }
                    //printf("2Устоновка имени %d\n", i);
                }
                printf("Устоновка имени %d\n", f);
                if (f == 1) {
                    send_message(clientData, MESSAGE_SEND, "Такое имя уже занято", "system");

                    /*struct message_data response = {MESSAGE_SEND, "Такое имя уже занято", "system"};
                    int l;
                    char* responseSer = Serialize(&response, &l);
                    result = send( clientData->socket_id, responseSer, l, MSG_NOSIGNAL);
                    if (result < 0) {
                        printf("Клиент %s отключен\n", clientData->nick_name); 
                    }
                    free(responseSer);*/
                    continue;
                }
                else {
                    for(int i = 0; i < clients.count; i++) {
                        struct client_data* item = clients.array[i];
                        /*printf("dsddsfdsfd имени %p\n", item);
                        printf("dsddsfdsfd имени %p\n", clientData);*/
                        if (item == clientData) {
                            if (NULL != item->nick_name ) {
                                free(item->nick_name);
                            }
                            item->nick_name = strcmp("", request.send_nick_name) == 0 ? NULL : request.send_nick_name ;
                            /*printf("dsddsfdsfd имени %d\n", i);
                            printf("dsddsfdsfd имени %s\n", item->nick_name);
                            printf("dsddsfdsfd имени %s\n", request.send_nick_name);*/
                            break;
                        }
                    }
                    send_message(clientData, MESSAGE_SEND, "Имя установлен", "system");
                    
                    /*struct message_data response = {MESSAGE_SEND, "Имя установлен", "system"};
                    int l;
                    char* responseSer = Serialize(&response, &l);
                    result = send( clientData->socket_id, responseSer, l, MSG_NOSIGNAL);
                    if (result < 0) {
                        printf("Клиент %s отключен\n", clientData->nick_name); 
                    }
                    free(responseSer);*/
                    continue;


                } 
            }
            else if (EXIT == request.type) {
                char response[] = "Hi, dear!";
                if( send( clientData->socket_id, response, sizeof(response), MSG_NOSIGNAL) < 0) {
                    perror("Error sending response");
                }
                all_client_data_remove(&clients, clientData);
                break;               
            }

            free(request.message);
            if (NICK_NAME != request.type) {
                free(request.send_nick_name);
            }
        } 

        
    }



    //Try{
        /*do {/*
            //Try {
                sleep(1);            
                /*snprintf(third, sizeof third, "%s %d\n", mess, i);*/   
                /*printf("result: %d\n", result);*/         
                /*result = send( clientData->socket_id, mess, sizeof(mess), MSG_NOSIGNAL, (struct sockaddr *)&addr, sizeof(addr));
                if (result < 0) {
                   printf("Клиент %s отключен\n", clientData->nick_name); 
                   break;
                }*/
                
            //};
            /*Catch(e) {
                printf("Возникла ошибка: %d\n", e);
            }*/
            
            /*i++;*/
        /*} while (i<5);*/
    /*}
    Catch(e) {
        printf("Возникла ошибка: %d\n", e);
    }*/

    /*
    sendto( clientData->socket_id, mess, sizeof(mess), 0, (struct sockaddr *)&addr, sizeof(addr));
    sendto( clientData->socket_id, mess, sizeof(mess), 0, (struct sockaddr *)&addr, sizeof(addr));
    sendto( clientData->socket_id, mess, sizeof(mess), 0, (struct sockaddr *)&addr, sizeof(addr));
    sendto( clientData->socket_id, mess, sizeof(mess), 0, (struct sockaddr *)&addr, sizeof(addr));
    */

};

int sent(void* var) {
      char read[30];
      //gets(read);
      my_fgets (read, 30, stdin);
}

int my_listen(void *var) {
    struct client_data *clientData;

    for (;;) {
        int s1 = accept(socket_ID, NULL, NULL);
        if( s1 < 0 )
        {
                perror("Error calling accept");
                continue;
                //return 0;
        }
        clientData = malloc(sizeof(struct client_data));
        clientData->socket_id = s1;
        clientData->nick_name = NULL;

        all_client_data_add(&clients, clientData);


        pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));

        pthread_create(thread, NULL, (void*)communication_client,  clientData);

    }
    return 0;

};

void send_message(struct client_data * to, enum message_type type, char * message, char * name) {
    struct message_data response = {type, message, name};
    int l;
    char* responseSer = Serialize(&response, &l);
    int result = send( to->socket_id, responseSer, l, MSG_NOSIGNAL);
    if (result < 0) {
        printf("Клиент %s отключен\n", to->nick_name); 
    };
    free(responseSer);
}



/**@brief Получает от клиента последовательность байт, не длиннее 30 и печатает её на экран по
 * завершении соединения. Клиенту отправляет "Hi, dear!"*/
int main(int argc, char **argv)
{
    //int ss; 
    //printf("my_listen\n");
    socket_ID = socket(AF_INET, SOCK_STREAM, 0);;
    if(socket_ID < 0)
    {
            perror("Error calling socket");
            return 0;
    }

    /*определяем прослушиваемый порт и адрес*/
    //struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(18666);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if( bind(socket_ID, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
    {
            perror("Error calling bind");
            return 0;
    }

    /*помечаем сокет, как пассивный - он будет слушать порт*/
    if( listen(socket_ID, 5) )
    {
            perror("Error calling listen");
            return 0;
    }

    clients = all_client_data_constructor(1);

    //clients_data = malloc()
    pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));
    pthread_t* thread2 = (pthread_t*) malloc(sizeof(pthread_t));
    printf("main %d %p \n", clients.size, clients.array);

    pthread_create(thread, NULL, (void*)my_listen,  NULL);
    pthread_create(thread2, NULL, (void*)sent,  NULL);

    //int status_addr;
    pthread_join(*thread, NULL);
    pthread_join(*thread2, NULL);

    free(thread);
    free(thread2);




    


      /*создаём сокет*/
      /*int ss = socket(AF_INET, SOCK_STREAM, 0);
      int* s = &ss;
      if( *s < 0)
      {
              perror("Error calling socket");
              return 0;
      }*/

      /*определяем прослушиваемый порт и адрес*/
      /*struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(18666);
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      if( bind(*s, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
      {
              perror("Error calling bind");
              return 0;
      }*/

      /*помечаем сокет, как пассивный - он будет слушать порт*/
      /*if( listen(*s, 5) )
      {
              perror("Error calling listen");
              return 0;
      }*/
/*
      char read[30];
      printf("dddddddddddddddd");
      gets(read);

      printf("%s\n", read);*/

        //clients = {10, malloc(10 * sizeof(struct client_data))};

        /*clients.size = 10;
        clients.count = 0;
        clients.array = malloc(clients.size * sizeof(struct client_data));*/
        //10, malloc(10 * sizeof(struct client_data))};

        /*clients = all_client_data_constructor(1);

        //clients_data = malloc()
        pthread_t* thread = (pthread_t*) malloc(sizeof(pthread_t));
        pthread_t* thread2 = (pthread_t*) malloc(sizeof(pthread_t));
        printf("main %d %p \n", clients.size, clients.array);

        pthread_create(thread, NULL, my_listen,  NULL);
        pthread_create(thread2, NULL, sent,  NULL);

        //int status_addr;
        pthread_join(*thread, NULL);
        pthread_join(*thread2, NULL);*/
        /*char read[30];
        gets(read);
        printf("main2\n");*/

        /*начинаем слушать, для соединения создаём другой сокет, в котором можем общаться.*/
        //int s1 = accept(s, NULL, NULL);

        return 0;
/*
        if( s1 < 0 )
        {
             perror("Error calling accept");
             return 0;
        }


        //читаем данные из сокета
        char buffer[31];
        int counter = 0;
        for(;;)
        {
             // char mess[] = "lslslslslslslslslslsl";
             // send(s1,mess,sizeof(mess), 0);
              memset(buffer, 0, sizeof(char)*31);
              //следует помнить, что данные поступают неравномерно
              int rc = recv(s1, buffer, 30, 0);
              printf("%s\n", "dfdfdfdfdf");
              
              char mess[] = "lslslslslslslslslslsl";
              send(s1,mess,sizeof(mess), 0);

              printf("%d\n", rc);
              if( rc < 0 )
              {
                      //чтение может быть прервано системным вызовом, это нормально
                      if( errno == EINTR )
                              continue;
                      perror("Can't receive data.");
                      return 0;
              }
              if( rc == 0 )
                      break;
              printf("%s\n", buffer);
        }
        char response[] = "Hi, dear!";
        if( sendto( s1, response, sizeof(response), 0, (struct sockaddr *)&addr, sizeof(addr) ) < 0 )
                perror("Error sending response");
        printf("Response send\n");
        return 0;*/
};















/*#include <stdio.h>
#include <stdlib.h>

/*using namespace std;*/

/*int main() {
    int *p;
    int *t;
    printf("%d", 6);
    for (int j = 0; j < 1000; j++) {

        for (unsigned long long i = 0; i < 10000; i++ ){
            for (int jj = 0; jj < 1; jj++){
                p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                            p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                p = (int *)malloc(sizeof(int));
                t = (int *)malloc(sizeof(int));
                *p = 0;
                *t = 0;
            }
            
        }
        printf("%d", j);
    }*/

    /*cout << "Hello world" << endl;*/

/*}*/