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
    string  statusInfo;
} message_buf, status_buf;

int oldStatus[3];
int *status;
key_t key_server, key1, key2, key3;
int msqid_server, msqid1, msqid2, msqid3, shmid_server;
string client[3] = {"Vinh", "Thu", "Nam"};

int checkStatusChanged() {
    for(int i=0; i<3; i++) {
        if (oldStatus[i] != status[i]) {
            return 1;
        }
    }
    return 0;
}

int main() {
    cout << "Server started" << endl;

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

    // shmget returns an identifier in shmid
    shmid_server = shmget(key_server, 1024, 0666 | IPC_CREAT);

    // shmat to attach to shared memory
    status = (int*) shmat(shmid_server,(void*)0,0);

    for(int i=0; i<3; i++) {
        status[i] = 0;
    }

    for(int i=0; i<3; i++) {
        cout << client[i] << ": " << ((status[i] == 0) ? "Offline" : "Online") << endl;
        oldStatus[i] = status[i];
    }

    while (1) {
        msgrcv(msqid_server, &message_buf, sizeof(message_buf), 0, 0);
        // cout << "\nReceived message: " << message_buf.content << endl;
        // cout << "\nReceived message: " << message_buf.imgId << endl;

        if (checkStatusChanged()) {
            cout << endl;
            for (int i=0; i<3; i++) {
                cout << client[i] << ": " << ((status[i] == 0) ? "Offline" : "Online") << endl;
                oldStatus[i] = status[i];
            }

            // status_buf.mtype = SERVER_ID;
            // status_buf.contentType = 1;
            // for (int i=0; i<3; i++) {
            //     status_buf.statusInfo += (client[i] + ": " + ((status[i] == 0) ? "Offline" : "Online") + "\n");
            // }

            // cout << status_buf.statusInfo << endl;

            // msgsnd(msqid1, &status_buf, sizeof(status_buf), 0);
            // msgsnd(msqid2, &status_buf, sizeof(status_buf), 0);
            // msgsnd(msqid3, &status_buf, sizeof(status_buf), 0);
        }

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