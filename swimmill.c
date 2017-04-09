#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

//Michael Black
//013505594
//CECS 326

//This process creates child fish and pellet processes and prints the swimmill simulation
//using shared memory array to read their locations. It handles the user entering 
//control c during the simulation and the simulation running out of time.

void controlc();        //declaration for function exit on control c
void timerout();        //declaration for function exit on control c   

const int SIZE = 20;    //constant SIZE of 20 for 20 processes
const int TIME = 30;    //constant TIME of 30 for 30 seconds

int memid;              //integer for id of shared memory
int *mem;               //pointer for shared memory start
pid_t fish;             //pid type fish for fish process
pid_t pellet[19];       //pellet type array for 19 processes 
                        //(hate hardccoding numbers but it avoided wardnings)

int main() {

        signal(SIGINT, controlc);           //signal for calling an exit because of user control c
        int ii, jj;                         //counter variables
        int timer;                          //integer for keeping time

        bool ispellet;                      //is this location a pellet? boolean
        bool isfish;                        //is this location a fish boolean

        
        int *s;                             //pointer for shared memory
        char *fishargv[] = {"","",NULL};    //arguments for fish process
        char *pelletargv[] = {"","",NULL};  //arguments for pellet processes

        key_t key;                          //key type key for setting up shared memory
        
        int index = 0;                      //inedex for pellet process array
        timer = TIME;                       //initialize timer to constant TIME


        key = 7777;                         //initialize key to 7777 for shared memory allocation


        //set up shared memory using an id, key, and size
        //if there is an error let the user know
        //utilized https://users.cs.cf.ac.uk/Dave.Marshall/C/node23.html
        if((memid = shmget(key, SIZE, IPC_CREAT | 0666)) <0) {
                perror("shmget error\n");
                exit(1);
        }


        //attatch shared memory using the id
        //if there is an error let the user know
        //utilized from https://users.cs.cf.ac.uk/Dave.Marshall/C/node23.html
        if((mem = shmat(memid, NULL, 0)) ==(int *)-1) {
                perror("shmat error\n");
                exit(1);
        }


        s = mem;                            //set pointer to start of memory for fish
        *s = 5;                             //set fish's location to the middle to start
        s++;                                //icnrement pointer for initializing pellet locations

        for(ii = 0; ii < SIZE; ii++) {      //set all pellet's locations to 0 to start
                *s = 0;                     //0 location indicates that a pellet doesn't exist
                s++;                        //increment pointer to go to the next element
        }


        fish = fork();                      //fork for fish process
        if(fish == 0) {                     //if fish pid returns 0 execute fish file
                execv("/home/michael/Documents/cecs326/fish", fishargv);
        } else {
                while(timer > 0) {          //run as long as timer is not out
                    pellet[index] = fork(); //fork a new pellet process and add it to pellet process array
                    if(pellet[index] == 0) {//if this pellet pid 
                        execv("/home/michael/Documents/cecs326/pellet", pelletargv);
                        if(index == 18) index = 0;  //if index is 18, circle around to 0
                        else index++;               //otherwise increment the index
                    }
                    else{
                        sleep(1);                   //sleep to give other processes a chance to run
                        for(ii = 100; ii > 0; ii--) {//for the size of the visual swimmill (10x10)
                            isfish = false;         //initialize isfish to false
                            ispellet = false;       //initialize ispellet to false
                            s = mem;                //set pointer to beginning of memory
                            if(*s == ii){           //if this location is the location of the fish
                                putchar('F');       //print fish
                                isfish = true;      //this location is fish's location, set true
                            }
                            s++;                    //increment pointer


                            for(jj = 1; jj < SIZE; jj++) {      //for each pellet location

                                if(*s == ii){                   //if this location is a pellet's location
                                        
                                    if(!isfish) putchar('P');   //print pellet
                                        ispellet = true;        //this location is the pellet's location, set true
                                        break;                  //break from loop since this is a pellets location
                                }
                                    else s++;                   //increment pointer otherwise
                            }

                            if(ispellet == false && isfish == false) putchar('~');  //if this isn't the location of
                                                                                    //of the fish or the pellet then
                                                                                    //it's water

                            if(((ii - 1) % 10) == 0) {                              //if it's the end of a row
                                putchar('\n');                                      //print a new line
                            }
                               
                        }
                        putchar('\n');

                    }
                    printf("\nTimer: %d\n", timer);                                 //print time left
                    timer--;                                                        //decrement counter
                }                                                                   //sleeping for 1 second with
        }                                                                           //iterative counter acts as
                                                                                    //timer
            timerout();         //times up, time to kill children
}

//this function kills all children before killing itself
//as the user has entered control c in the terminal
//SIGINT signal is to handle control c
//utilized ftp://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html
void controlc() {
    kill(fish, SIGINT);                     //send signal to kill fish
    int ii;
    for(ii = 0; ii < SIZE - 1; ii++) {      //send signals to kill each pellet
        kill(pellet[ii], SIGINT);          
    }   
    shmdt(mem);                             //detach memory
    shmctl(memid, IPC_RMID, 0);             //dealocate memory
    
    printf("\nPID %d, Exiting\n", getpid());    //print exit
    sleep(3);                                   //sleep to allow the signals to be sent
    exit(0);                                    //exit
}

//this funciton kills all children before killing itself
//as the user the entered control c in the terminal
//SIGUSR1 signal is user defined handling
//utilized ftp://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html
void timerout() {                               
    sleep(1);                                   //sleep to slow printing/killing 
    kill(fish, SIGUSR1);                        //send signal to kill fish
    int ii;                                 
    for(ii = 0; ii < SIZE - 1; ii++) {          //send signals to kill each pellet
        sleep(1);                               //sleep to slow printing/killing
        kill(pellet[ii], SIGUSR1);        
    }
   
    shmdt(mem);                                 //detach memory
    shmctl(memid, IPC_RMID, 0);                 //dealocate memory

    printf("\nPID %d, Exiting\n", getpid());        //print exit
    sleep(3);                                       //sleep to allow the signals to be sent
    exit(0);                                        //exit
}