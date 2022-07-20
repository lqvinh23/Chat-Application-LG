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

#define MSGSZ 128
#define SHMSZ 128

#define SERVER_ID 10
#define SERVER_NAME "LG"

#define CLIENT_ID 2
#define CLIENT_NAME "Thu"

// message queue
struct message {
	long    mtype; //sender
	char    content[MSGSZ]; //content
    int     imgId;
    long    receiverId; //receiver
};

int msqid_server, msqid2, shmid2;
message sbuf, rbuf;

// void sendMsg() {
//     while (1) {
//         sbuf.mtype = CLIENT_ID;
//         char receiver[100];
//         cout << "\nSend to: ";
//         cin >> receiver;

//         if (strcmp(receiver,"Vinh") == 0) {
//             sbuf.receiverId = 1;
//         }
        
//         if (strcmp(receiver,"Nam") == 0) {
//             sbuf.receiverId = 3;
//         }
        
//         cout << "Message: ";
//         cin >> sbuf.content;

//         msgsnd(msqid_server, &sbuf, sizeof(sbuf), 0);
//     }
// }

// void receiveMsg() {
//     while (1) {
//         msgrcv(msqid2, &rbuf, sizeof(rbuf), 0, 0);
//         if (rbuf.mtype == 1) {
//             cout << "\nFrom Vinh: " << rbuf.content << endl;
//             cout << "Send to: ";
//             fflush(stdout);
//         }
//         if (rbuf.mtype == 3) {
//             cout << "\nFrom Nam: " << rbuf.content << endl;
//             cout << "Send to: ";
//             fflush(stdout);
//         }
//     }
// }

void sendImg () {
    while (1) {
        // shmat to attach to shared memory
        char *str = (char*) shmat(shmid2,(void*)0,0);
        sbuf.mtype = CLIENT_ID;
        sbuf.imgId = shmid2;
        char receiver[100];
        cout << "\nSend to: ";
        cin >> receiver;

        if (strcmp(receiver,"Vinh") == 0) {
            sbuf.receiverId = 1;
        }
        
        if (strcmp(receiver,"Nam") == 0) {
            sbuf.receiverId = 3;
        }
        
        char imgLink[100];

        cout << "Image: ";
        cin >> imgLink;

        ifstream image(imgLink, ios::in | ios::binary);
        ofstream binary("bin_img_data.txt", ios::out | ios::app);

        char ch;
        while (!image.eof()) {
            ch = image.get();
            *str += ch;
            binary.put(ch);
        }

        image.close();
        binary.close();

        msgsnd(msqid_server, &sbuf, sizeof(sbuf), 0);
    }
}

void receiveImg () {
    while (1) {
        msgrcv(msqid2, &rbuf, sizeof(rbuf), 0, 0);

        // shmat to attach to shared memory
        char *str = (char*) shmat(rbuf.imgId,(void*)0,0);
        
        ifstream binary("bin_img_data.txt", ios::in | ios::app | ios::binary);
        ofstream image("receivedImg.jpg", ios::out | ios::app);

        char ch;
        while (!binary.eof()) {
            ch = binary.get();
            image.put(ch);
        }

        image.clear();
        binary.close();

        if (rbuf.mtype == 1) {
            cout << "\nFrom Vinh: receivedImg.jpg" << endl;
            cout << "Send to: ";
            fflush(stdout);
        }
        if (rbuf.mtype == 3) {
            cout << "\nFrom Nam: receivedImg.jpg" << endl;
            cout << "Send to: ";
            fflush(stdout);
        }
    }
}

int main() {
    key_t key_server, key2;

    // ftok to generate unique key
    // key_server = ftok(SERVER_NAME, SERVER_ID);
    // key2 = ftok(CLIENT_NAME, CLIENT_ID);
    
    key_server = 10;
    key2 = 12;

    // msgget creates a message queue and returns identifier
    msqid_server = msgget(key_server, 0666 | IPC_CREAT );
    msqid2 = msgget(key2, 0666 | IPC_CREAT );

    // shmget returns an identifier in shmid
    shmid2 = shmget(key2, 1024, 0666 | IPC_CREAT);

    // printf("\nWelcome Vinh to the chat app. Enter 'end' to close the connection.");
    cout << "\nWelcome Thu to the chat app. Enter 'end' to close the connection." << endl;

    thread t1(sendImg);
    thread t2(receiveImg);

    t1.join();
    t2.join();

    return 0;
}