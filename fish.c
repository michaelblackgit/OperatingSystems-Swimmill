#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void controlc();    //declaration for function exit on control c
void timerout();    //declaration for function exit on control c

int main() {
        signal(SIGINT, controlc);   //signal for calling an exit because of user control c
        signal(SIGUSR1, timerout);  //signal for calling an exit because of time run out

        const int SIZE = 20;        //constant SIZE of 20 for 20 processes
        int memid;                  //integer for id of shared memory
        int *mem;                   //pointer for shared memory start
        int *s;                     //pointer for accessing shared memory
        int min;                    //integer for holding the location closest to the fish
        int ii;                     //count variable

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

        while(1) {                              //run forever
                sleep(1);                       //sleep to allow other processes to continue
                s = mem;                        //set pointer to beginning of shared memory
                s++;                            //increment to 1st element, since 0th is fish
                min = 100;                      //set min to the MAX
                for(ii = 1; ii < SIZE; ii++) {  //if any of the distances is less than min, set min to
                                                //that distance
                        if(*s < min && *s > 0) min = *s;
                        //if(*s % 10 < min && *s > 0) min = *s;
                        //if((*s - (*s % 10)) / 10 < min && *s > 0) min = *s;
                        s++;
                }

                 s = mem;                       //set pointer to the first element for fish                       
                 if((min % 10) >  *s){          //if the min's x comp is greater than fish's x comp
                        if(*s != 10) *s = *s+1; //move fish right one
                }
                else if((min % 10) < *s){       //else if the min's x comp is less than fish's x comp
                        if(*s != 1) *s = *s-1;  //move fish left one
                }

                
        }  
                    
}

//kill this process if user enters control c
void controlc() {
    printf("PID %d, Fish killed!\n", getpid()); //print exit
    exit(0); //exit
}

//kill this process if time runs out
void timerout() {
    printf("PID %d, Fish Killed! Not enough time!\n", getpid()); //print exit
    exit(0); //exit
}