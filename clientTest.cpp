#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <iostream>
#include <cstdio>
#include <fstream>

using namespace std;

#define MSGSZ 256
#define SHMSZ 256

#define SERVER_ID 10
#define SERVER_NAME "LG"

#define CLIENT_ID 1
#define CLIENT_NAME "Vinh"

// message queue
struct message {
	long    mtype; //sender
	char    content[MSGSZ]; //content
    int     contentType;
    int     imgId;
    long    receiverId; //receiver
};

int msqid_server, msqid1, shmid1;
message sbuf, rbuf;

void sendMsg() {
    while (1) {
        sbuf.mtype = CLIENT_ID;
        char receiver[100];
        cout << "\nSend to: ";
        cin >> receiver;

        if (strcmp(receiver,"Thu") == 0) {
            sbuf.receiverId = 2;
        }
        
        if (strcmp(receiver,"Nam") == 0) {
            sbuf.receiverId = 3;
        }

        char type[100];
        cout << "Text or Image: ";
        cin >> type;

        if (strcmp(type, "text") == 0) {
            sbuf.contentType = 1;
            sbuf.imgId = 0;
            cout << "Message: ";
            cin >> sbuf.content;
        }

        if (strcmp(type, "image") == 0) {
            sbuf.contentType = 2;
            // shmat to attach to shared memory
            char *str = (char*) shmat(shmid1,(void*)0,0);
            sbuf.imgId = shmid1;

            cout << "Image: ";
            cin >> sbuf.content;

            ifstream image(sbuf.content, ios::in | ios::binary);
            ofstream binary("bin_img_data.txt", ios::out | ios::app);

            char ch;
            while (!image.eof()) {
                ch = image.get();
                *str += ch;
                binary.put(ch);
            }

            image.close();
            binary.close();
        }

        msgsnd(msqid_server, &sbuf, sizeof(sbuf), 0);
    }
}

void receiveMsg() {
    while (1) {
        msgrcv(msqid1, &rbuf, sizeof(rbuf), 0, 0);

        if (rbuf.contentType == 1) {
            if (rbuf.mtype == 2) {
            cout << "\nFrom Thu: " << rbuf.content << endl;
            cout << "Send to: ";
            fflush(stdout);
            }
            if (rbuf.mtype == 3) {
            cout << "\nFrom Nam: " << rbuf.content << endl;
            cout << "Send to: ";
            fflush(stdout);
            }
        }
        if (rbuf.contentType == 2) {
            // shmat to attach to shared memory
            char *str = (char*) shmat(rbuf.imgId,(void*)0,0);
            
            ifstream binary("bin_img_data.txt", ios::in | ios::app | ios::binary);
            ofstream image(rbuf.content, ios::out | ios::app);

            char ch;
            while (!binary.eof()) {
                ch = binary.get();
                image.put(ch);
            }

            image.clear();
            binary.close();

            if (rbuf.mtype == 2) {
                cout << "\nFrom Thu: " << rbuf.content << endl;
                cout << "Send to: ";
                fflush(stdout);
            }
            if (rbuf.mtype == 3) {
                cout << "\nFrom Nam: " << rbuf.content << endl;
                cout << "Send to: ";
                fflush(stdout);
            }
        }
    }
}

int main() {
    key_t key_server, key1;

    // // ftok to generate unique key
    // key_server = ftok(SERVER_NAME, SERVER_ID);
    // key1 = ftok(CLIENT_NAME, CLIENT_ID);

    key_server = 10;
    key1 = 11;

    // msgget creates a message queue and returns identifier
    msqid_server = msgget(key_server, 0666 | IPC_CREAT );
    msqid1 = msgget(key1, 0666 | IPC_CREAT );

    // shmget returns an identifier in shmid
    shmid1 = shmget(key1, 1024, 0666 | IPC_CREAT);

    // printf("\nWelcome Vinh to the chat app. Enter 'end' to close the connection.");
    cout << "\nWelcome Vinh to the chat app. Enter 'end' to close the connection." << endl;

    thread t1(sendMsg);
    thread t2(receiveMsg);

    t1.join();
    t2.join();

    return 0;
}