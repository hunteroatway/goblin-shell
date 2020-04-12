/***********************************************************************************************************
** Usage:
**  
**  After compiling with "make" you can run the program with "./raid (foodCount) (troopCount)"
**  With values for foodCount from between 150 and 250.
**  With values for troopCount from between 5 and 10
**  If the last two arguments are left blank the program automatically generates these values.
**
**  The program will execute and then run the simulation of goblins raiding to get food.
**
**  This program is designed to use many features within operating systems such as forks, threads, opening and closing files, mutexes, signals
**
**  Compile on goblin shell: compile raid.c goblin.txt -pthread
**  Alternative compile on general unix system: gcc raid.c -o raid -pthread
**
***********************************************************************************************************/

// note will have to somehow push the goblin.txt file over to the server so can copy photo

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>


int food, troops;
int queen = 0;
int butler = 0;
int gathered = 0;
pthread_mutex_t mutex;

#define MAX 128

struct arg {
    int g_number;
    int sum;
};

void *fetchFood();

// signals used to have the queen and butler communicate across 2 processes to invoke waiting
static void intHandler(int signalNo, siginfo_t *info, void *context) {
    // if signal is sent from butler to queen
    if(signalNo == 12)
        queen = 1;
    // if signal is sent from queen to butler
    if(signalNo == 10)
        butler = 1;
}

int main(int argc, char* argv[]) {

    //sed seed for random generator
    srand(time(0));
    int status; 

    //Variables needed for signals
    sigset_t interruptMask;             
    struct sigaction act;

    int upperFood = 250;
    int lowerFood = 150;
    int lowerTroop = 5;
    int upperTroop = 10;

    // check to make sure number of arguments is correct
    if (argc >= 2) {
        // store the amount of food required
        sscanf(argv[1], "%d", &food);
        sscanf(argv[2], "%d", &troops);

        // check if it is not in range 
        if(food > upperFood || food < lowerFood){
            printf("Value for food is out of range. Pick a number between 150 and 250. \n");
            fflush(stdout);
            return 0;
        }

        // check if it is not in range 
         if(troops > upperTroop || troops < lowerTroop){
            printf("Value for troops is out of range. Pick a number between 5 and 10. \n");
            fflush(stdout);
            return 0;
        }

    } else {
        // randomly pick food count from 150-250
        food = rand() % (upperFood - lowerFood + 1) + lowerFood;

         // randomly pick troops count from 150-250
        troops = rand() % (upperTroop - lowerTroop + 1) + lowerTroop;
    }

    // continue the program
    // set up mutex
    if (pthread_mutex_init(&mutex, NULL)!=0){
        perror("Could not create mutex for output: ");
        return 1;
    }

    // create another process for the butler
    pid_t pid = fork();
    if(pid < 0) {
        perror("fork failue");
        return 0;
    } if (pid == 0) { 
        // the butler

        pid_t queenPid = getppid();

        //set the stage (the bitmask)
            sigemptyset (&interruptMask);
            sigaddset (&interruptMask, SIGUSR1);  

            //Hook up the signalhandler to SIGINT 
            act.sa_sigaction = &intHandler;
            act.sa_flags = SA_SIGINFO;
            sigemptyset (&(act.sa_mask));
            if (sigaction (SIGUSR1, &act, NULL) == -1) {
                    perror("sigaction cannot set SIGUSR1");
                    exit(SIGUSR1);
            }
        while(!butler);

        // have butler speak
        printf("Butler: Why of course your majesty. I will gather %d of our best troops to hunt for %d bits of food \n", troops, food);
        fflush(stdout);
        sleep(1);
        printf("Butler: I am on my way now. I will not let you down! \n");
        fflush(stdout);

        sleep(5);
        printf("Butler: Hello young goblins. I need %d of you to go on an adventure.\n", troops);
        fflush(stdout);
        sleep(1);
        printf("Butler: Get ready! You are heading out now!\n");
        fflush(stdout);

        // set up threads 
        void *p_status;
        pthread_t *thread_ids = malloc(sizeof(pthread_t)*troops);
        struct arg *list;
        list= malloc(sizeof(struct arg) * troops);
        int i;
        for (i = 0; i < troops; i++) {
            list[i].g_number = i +1;
            list[i].sum = 0;
            if( pthread_create(&thread_ids[i], NULL, fetchFood, (void *)&list[i]) > 0){
                    perror("pthread_create failure");
                    exit(1);
            }
        }

        // wait for threads to finish
         // join threads and print their return values
        for (i = 0; i < troops; i++) {
            if (pthread_join(thread_ids[i], &p_status) != 0) {
                perror("trouble joining thread: ");
                exit(1);
            }
        }

        // have goblins tell butler their total once all have returned.
        printf("Butler: Goblins! Since you have returned. Please tell me how much food you each found! \n");
        fflush(stdout);
        sleep(1);
        for(i = 0; i < troops; i++) { // potentially add conditions based off total food found to give different reactions
            printf("Goblin %d: I manged to find a total of %d bits of food!\n", list[i].g_number, list[i].sum);
            fflush(stdout);
            sleep(1);
        }
        //free the array
        free(thread_ids);

        // need to add butler reacting and then passing it to the queen. potentially though a txt file.
        if(gathered < food) {// panic
            printf("Butler: Oh no! I didn't gather enough food! The Queen is going to be so angry with me!\n");
            fflush(stdout);
        } else if(gathered > food) {// panic
            printf("Butler: Oh no! I gathered too much food! The Queen is going to be so angry with me!\n");
            fflush(stdout);
        } else if(gathered == food) {// happy
            printf("Butler: Yay! I gathered enough food. The Queen is going to be happy with me! \n");
            fflush(stdout);
        }
        
        sleep(1);
        printf("Butler: It is time to write my report about these gatherings. \n");
        fflush(stdout);
        // open a txt file to write the report
        FILE *report = NULL;
        if((report = fopen("report.txt", "w")) == NULL){
            perror("Error opening file");
            return -1;
        }
        
        fprintf(report, "We managed to gather a total of: \n");
        fprintf(report, "%d \n \n", gathered);

        if(gathered < food) // panic
            fprintf(report, "We were unable to get enough food.\n");
        else if(gathered > food) // panic
            fprintf(report, "I hope having more food is acceptable.\n");
        else if(gathered == food) // happy
            fprintf(report, "We succesfully manged to get enough food! \n");

        // print how much each goblin found
        for(i = 0; i < troops; i++) {
            fprintf(report, "Goblin %d manged to find %d food. \n", list[i].g_number, list[i].sum);
        }

        // free the list
        free (list);
        //close file
        fclose(report);

        sleep(3);
        // return the list to the queen
        printf("Butler: I now have to deliver this report to the queen \n");
        fflush(stdout);
        sleep(5);
        butler = 0;
        kill(queenPid, SIGUSR2);
        while(!butler);

        // answer queen
        if(gathered < food) {// panic
            printf("Butler: We were unable to get enough food.\n");
            fflush(stdout);
        } else if(gathered > food) { // panic
            printf("Butler: We manged to get extra food. Hopefully that is okay.\n");
            fflush(stdout);
        } else if(gathered == food) {// happy
            printf("Butler: We succesfully manged to get enough food! \n");
            fflush(stdout);
        }
        sleep(1);
        printf("Butler: Here is a report. \n");
        fflush(stdout);
        sleep(1);
        butler = 0;
        kill(queenPid, SIGUSR2);
        while(!butler);

        // respond
        if(gathered == food) {// happy
            printf("Butler: I am glad to be of service!\n");
            fflush(stdout);
        } else if(gathered > food || gathered < food) { // panic
            printf("Butler: I am sorry to have let you down your majesty. I will make sure not to make this mistake on the next raid.\n");
            fflush(stdout);
        }
        
        sleep(1);
        exit(0);

    } else {
        // the queen

        //set the stage (the bitmask)
            sigemptyset (&interruptMask);
            sigaddset (&interruptMask, SIGUSR2);  

            //Hook up the signalhandler to SIGINT 
            act.sa_sigaction = &intHandler;
            act.sa_flags = SA_SIGINFO;
            sigemptyset (&(act.sa_mask));
            if (sigaction (SIGUSR2, &act, NULL) == -1) {
                    perror("sigaction cannot set SIGUSR2");
                    exit(SIGUSR2);
            }

        // allow queen to speak
        queen = 1;
        printf("Queen: Hello butler. I need you to gather troops in order to get some food for the village for me. \n");
        fflush(stdout);
        sleep(1);
        printf("Queen: I am able to allocate %d troops to get a total of %d food. \n", troops, food);
        fflush(stdout);
        sleep(1);
        printf("Queen: I want exactly this amount of food. No more. No less. Got it? \n");
        fflush(stdout);
        sleep(1);
        queen = 0;
        kill (pid, SIGUSR1);
        
        // wait for butler to return
        while(!queen);

        // talk to butler
        printf("Queen: Hello butler, I see you have returned. How did you fair on this mission? \n");
        fflush(stdout);
        sleep(1);
        queen = 0;
        kill (pid, SIGUSR1);
        // wait for butler
        while(!queen);

        printf("Queen: Thank you for the report. I will read that right away.\n");
        fflush(stdout);

        FILE * report; 
        if((report = fopen("report.txt", "r")) == NULL){
            perror("Error opening file");
            return -1;
        }
        fscanf(report, "%*[^\n]"); // ignore first line
        fscanf(report, "%d", &gathered); // get the number gathered

        sleep(2);

        // react to food
        printf("Queen: I see you have gathered a total of %d food. I requested %d food.\n", gathered, food);
        fflush(stdout);
        sleep(1);
        if(gathered == food) {
            printf("Queen: Excellent! Thank you for gathering the correct amount of food! I will require your services again soon! \n");
            fflush(stdout);
            sleep(1);
            kill (pid, SIGUSR1);
        } else if(gathered < food && (food - gathered) < food * 0.1) { // if within 10 %
            printf("Queen: You failed to gather enough food. However, there is enough food to make do. You are dismissed... this time. \n");
            fflush(stdout);
            sleep(1);
            kill (pid, SIGUSR1);
        } else if(gathered < food && !((food - gathered) < food * 0.1)) { // if not within 10 %
            printf("Queen: You failed to gather enough food. Guards get the guillotine ready! \n");
            fflush(stdout);
            sleep(1);
            kill (pid, SIGKILL);
            printf("Crowd: BEGONE USELESS BUTLER! \n");
            fflush(stdout);
            sleep(1);
            printf("Queen: These butlers are dispensable anyway. I will find another.\n");  
            fflush(stdout);
            sleep(1);
            printf("Queen: Goblins you did your best. Be prepared for another raid shortly.\n");
        } else if(gathered > food && (gathered - food) < food * 0.1) { // if within 10 %
            printf("Queen: You mangaged to gather too much food. However, there is enough food for a surplus. You are dismissed... this time. \n");
            fflush(stdout);
            sleep(1);
            kill (pid, SIGUSR1);
        } else if(gathered > food && !((gathered - food) < food * 0.1)) { // if not within 10 %
            printf("Queen: You mangaged to gather too much food. Guards get the guillotine ready! \n");
            fflush(stdout);
            sleep(1);
            kill (pid, SIGKILL);
            printf("Crowd: BEGONE USELESS BUTLER! \n");
            fflush(stdout);
            sleep(1);
            printf("Queen: These butlers are dispensable anyway. I will find another.\n");
            fflush(stdout);
            sleep(1);
            printf("Queen: Goblins you did your best. Be prepared for another raid shortly.\n");
            fflush(stdout);
        }

        wait(&status);

    }
    
    printf("THE END! \n");
    fflush(stdout);

    return 0;
}

void * fetchFood(void *num) {

    struct arg *list = (struct arg *)num;

    // calculate number of trips
    int upperTrip = 10;
    int lowerTrip = 1;
    int sum = 0;

    int trips = rand() % (upperTrip - lowerTrip + 1) + lowerTrip;
    printf("Goblin %d: I am going to go on %d adventures to gather this food! \n", list->g_number ,trips);
    fflush(stdout);
    
    // loop through the trips
    int x;
    for(x = 1; x < trips+1; x++) {

        // create a sleep to simulate them going to get the food
        sleep(rand() % (4 - 2 + 1) + 2);
        // get mutex on the gathered
        if(pthread_mutex_lock(&mutex) != 0) {
            perror("Could not lock");
            exit(3);
        }
        // randomly generate number of food from 0 - 5
        int upperFood = 7;
        int lowerFood = 0;
        int foodFound = rand() % (upperFood - lowerFood + 1) + lowerFood;
        gathered = gathered + foodFound;
        
        list->sum = list->sum + foodFound;

        //messge to print how much food was found
        printf("Goblin %d: I have found %d food on trip %d!\n", list->g_number, foodFound, x);
        fflush(stdout);

        // release mutex on the gathered
        if(pthread_mutex_unlock(&mutex) != 0) {
            perror("Could not unlock");
            exit(3);
        }
    }
  return 0;
}
