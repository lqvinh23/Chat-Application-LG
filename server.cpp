#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cstdio>

using namespace std;

#define MSGSZ 256
#define SHMSZ 256

#define SERVER_ID 10
#define SERVER_NAME "LG"

// message queue
struct message {
	long    mtype; //sender
	char    content[MSGSZ]; //content
    int     contentType;
    int     imgId;
    long    receiverId; //receiver
} message_buf;

int main() {
    cout << "Server started" << endl;
    key_t key_server, key1, key2, key3;
    int msqid_server, msqid1, msqid2, msqid3, shmid_server;

    // ftok to generate unique key
    // key_server = ftok(SERVER_NAME, SERVER_ID);
    // key1 = ftok("Vinh", 1);
    // key2 = ftok("Thu", 2);
    // key3 = ftok("Nam", 3);

    key_server = 10;
    key1 = 11;
    key2 = 12;
    key3 = 13;

    // cout << key_server << endl << key1 << key2 << key3 << endl;

    // msgget creates a message queue and returns identifier
    msqid_server = msgget(key_server,  0666 | IPC_CREAT );
    msqid1 = msgget(key1, 0666 | IPC_CREAT);
    msqid2 = msgget(key2, 0666 | IPC_CREAT);
    msqid3 = msgget(key3, 0666 | IPC_CREAT);

    // // shmget returns an identifier in shmid
    // shmid_server = shmget(key_server, 1024, 0666 | IPC_CREAT);

    // // shmat to attach to shared memory
    // char *str = (char*) shmat(shmid_server,(void*)0,0);

    while (1) {
        msgrcv(msqid_server, &message_buf, sizeof(message_buf), 0, 0);
        // cout << "\nReceived message: " << message_buf.content << endl;
        // cout << "\nReceived message: " << message_buf.imgId << endl;

        if (message_buf.receiverId == 1) {
            msgsnd(msqid1, &message_buf, sizeof(message_buf), 0);
            cout <<"Sent message to Vinh"<<endl;
        }
        if (message_buf.receiverId == 2) {
            msgsnd(msqid2, &message_buf, sizeof(message_buf), 0);
            cout <<"Sent message to Thu"<<endl;
        }
        if (message_buf.receiverId == 3) {
            msgsnd(msqid3, &message_buf, sizeof(message_buf), 0);
            cout <<"Sent message to Nam"<<endl;
        }
    }

    return 0;
}