#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

//Michael Black
//013505594
//CECS 326

//This class fixes an index of the shared memory and sets a
//random location to decrement by 10 to simulate moving down
//one on the grid printed in swimmill. If the shared memory
//array is full, the process will not enter and kill itself

void controlc(); //declaration for function exit on control c
void timerout(); //declaration for function to exit when out of time


int main() {
        signal(SIGINT, controlc);   //signal for calling an exit because of user control c
        signal(SIGUSR1, timerout);  //signal for calling an exit because of time run out

        srand(time(NULL));          //reinitialize prng for new random

        const int SIZE = 20;        //constant SIZE of 20 for 20 processes
        int memid;                  //integer for id of shared memory
        int *mem;                   //pointer for shared memory start
        int *s;                     //pointer for accessing shared memory
        int ii;                     //count variable
        int *index;                 //keeps track of the one index this process is accessing
        bool hasspace;              //boolean to indicate if there is already 19 pellet processes

        key_t key;                  //key for shared memory

        key = 7777;                 //set key to 7777


        //set up shared memory using an id, key, and size
        //if there is an error let the user know
        //utilized https://users.cs.cf.ac.uk/Dave.Marshall/C/node23.html
        if((memid = shmget(key, SIZE, 0666)) < 0) {
                perror("shmget");
                exit(1);
        }

        //attatch shared memory using the id
        //if there is an error let the user know
        //utilized from https://users.cs.cf.ac.uk/Dave.Marshall/C/node23.html
        if((mem = shmat(memid, NULL, 0)) == (int *) - 1) {
                perror("shmat");
                exit(1);
        }

        s = mem;                            //set pointer to beginning of shared memory
        s++;                                //increment to 1st element, since 0th is fish
        hasspace = false;                   //initialize hasspace boolean to false
        for(ii = 1; ii < SIZE; ii++) {      //see if there's room for a new process
                if(*s == 0) {               //if element is 0, we can continue process
                        *s = rand() % 100;  //set random distance to this element
                        index = s;          //store this index to manipulate later
                        hasspace = true;    //we have space, so hasspace = true
                        ii = SIZE;          //break from the loop
                }
                s++;                        //otherwise move onto the next element
        }                                   //since this must have a process in place
        if(! hasspace) return 0;            //if there's no space, kill this process
        while(1) {                          //run forever

                sleep(1);                       //sleep to allow other processes to continue
                if(*index - 10 > 0){            //if this pellet has at least 10 units more to decrement before 
                        *index = *index - 10;   //it's negative, decrement it by 10 to simulate moving down 1
                } 
                else {                          //if this pellet's distance will be negative with another move
                        s = mem;                //if this pellet's location matches up with fish, print eaten, otherwise missed
                        if(*index == *s)printf("\nPID: %d, Position: %d, Pellot Eaten!\n", getpid(), *index);
                        else printf("\nPID: %d, Position: %d, Pellet Missed!\n", getpid(), *index);
                        *index = 0;             //set this element in shared memory to 0 since it will be free now
                        return 0;               //kill this process
                }
        }
}

//kill this process if user enters control c
void controlc() {
    printf("\nPID: %d, Pellot Killed!\n", getpid()); //print exit
    exit(0); //exit
}

//kill this process if time runs out
void timerout() {
    printf("\nPID: %d, Pellot Killled! Not enough time!\n", getpid()); //print exit
    exit(0); //exit
}