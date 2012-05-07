/*****************************************************************************************************************
 *
 * NAME 		: 	VINU CHARANYA ATHANGUDI PURUSHOTHAMAN
 * PERSON # 	:	37648013
 * PROGRAM 1	:	bb.c
 * HEADER FILE	:	osheaders.h
 *
 * DESCRIPTION	: Program gets three user inputs and based on the bounded buffer size the producer produces.
 * 								When the producer is producing the critical resources are locked using mutex similarly for consumers
 * 								uses a mutex to access the resources. Once the producer produces 1000 characters the program exits.
 * 								I've used numbers from 1-1000 instead of characters for easy identification of consumption in order.
 *
 * EXECUTION	:	/bb <P> <C> <BufferSize>
 *
 * <P> is the # of threads to run the producer function on.
 * <C> is the # of threads to run the consumer function on.
 * <BufferSize> is the size of the bounded buffer.
 *
 *
 * DATE			:	23 Mar 2011
 *
 ******************************************************************************************************************/
#include "osheaders.h"

int nop,noc,nob; //Number of producer, Number of Consumer , Number of BufferSize
pthread_attr_t attr;//Initialize the attributes

//Initializing the semaphores and mutex
sem_t bsempty;
sem_t bsfull,final;
pthread_mutex_t mut;
static int counter=0;
int in=0,out=0;


//Thread Struct
struct thrcnt {
	int num;

};

typedef struct {
	int *arr;
}barr;
barr buf;

/*****************************************************************************************************************
 *
 * Function 	: 	init
 *
  Parameters	: nil
 *
 * Description	:	This function initializes the various semaphores
 *
 ******************************************************************************************************************/
void init()
{
	pthread_mutex_init(&mut, NULL);
	//pthread_mutex_init(&cnt,NULL);
	sem_init(&bsfull, 0, 0);
	sem_init(&bsempty, 0, nob);
	sem_init(&final,0,0);
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}

/*****************************************************************************************************************
 *
 * Function 	: 	producer
 *
  Parameters	:	<producer thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function waits if the the buffer is full and produces upto 1000 characters and exits.
 * 							It locks the critical resource while accessing the buffer and and unlocks it once it adds the
 * 							char so that others can use the critical resource.
 *
 ******************************************************************************************************************/

void *producer(void *args)
{
	int end=0;
	static int val=1;
	static int i=0;
	struct thrcnt pp;
	pp=*(struct thrcnt *)args;
	int prodval = pp.num;

	while(1){
					sem_wait(&bsempty);
					pthread_mutex_lock(&mut);
					if(counter<1000){
					usleep(100);
					buf.arr[in]=val;
					printf("Producer %d\tproduced :\t%d\n",prodval,buf.arr[in]);
					++val;
					++in;
					++counter;
					in=in%nob;
					pthread_mutex_unlock(&mut);
					sem_post(&bsfull);}
					else{
						break;
					}
	}

	sem_post(&final);
	pthread_exit(NULL);
}

/*****************************************************************************************************************
 *
 * Function 	: 	Consumer
 *
  Parameters	:	<consumer thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function waits if the the buffer is empty and consumes upto producer produces 1000 characters
 * 							and exits.
 * 							It locks the critical resource while accessing the buffer and and unlocks it once it adds the
 * 							char so that others can use the critical resource.
 *
 ******************************************************************************************************************/

void *consumer(void *args)
{
	int val=0;
	struct thrcnt cc;
	cc=*(struct thrcnt *)args;//getting the arguments
	int consval = cc.num;//storing the thread number
	while(1){
		sem_wait(&bsfull);
		pthread_mutex_lock(&mut);
		usleep(100);
		val=buf.arr[out]; //I am just using a int to check
		printf("Consumer %d\tconsumed :\t%d\n",consval,val);
		++out;
		out=out%nob;// if out>bufsize it is set to 0
		pthread_mutex_unlock(&mut);
		sem_post(&bsempty);
	}

}

/*****************************************************************************************************************
 *
 * Function 	: 	Main
 *
  Parameters	:	<# of Producers> <# of Consumers> <# of buffersize>
 * 					All the filenames are string passed through argv[1] argv[2]  and argv[3] which are
 * 					converted to integer
 *
 * Description	:	Program takes three arguments from the command line <# of Producers> <# of Consumers>
 * 							<# of buffersize> 	It creates thread for each of the values and calls
 *							 the corresponding functions for the threads to run. The main program waits on the producer
 *							 threads and exits when the producer produces 1000 character thread exits.
 *
 ******************************************************************************************************************/


int main(int argc, char **argv) {

	if (argc != 4)//checking for number of arguments
	{
		printf("\nHey argument must be 4!!");
		printf("\nThe correct format is ./bb <# of producers> <# of consumers> <buffersize>\n");

	} else {

		int i=0,j=0,rc=0;

		printf("\nYou have given correct number of Arguments\n");
		nop = atoi(argv[1]);
		noc = atoi(argv[2]);
		nob = atoi(argv[3]);

		printf("\n Producers\t =\t%d", nop);
		printf("\n Consumers\t =\t%d", noc);
		printf("\n Bounded Buffer\t =\t%d\n\n", nob);

		//pthread_t p[nop];
		//pthread_t c[noc];
		pthread_t *p,*c;
		p=(pthread_t *)malloc(sizeof(pthread_t)*nop);
		c=(pthread_t *)malloc(sizeof(pthread_t)*noc);

		int buffer[nob];
		buf.arr=&buffer[0];

		init();
		 //struct thrcnt pthrcnt[nop];
		 //struct thrcnt cthrcnt[noc];
		struct thrcnt *pthrcnt;
		struct thrcnt *cthrcnt;
		pthrcnt = (struct thrcnt *)malloc(sizeof(struct thrcnt)*nop);
		cthrcnt = (struct thrcnt *)malloc(sizeof(struct thrcnt)*noc);

		//Create the threads

		while(i<nop||j<noc)
		{

			if(i<nop)
			{
				pthrcnt[i].num=i+1;
				rc=pthread_create(&p[i],&attr,producer,&pthrcnt[i]);
				if(rc){
					printf("\nError while creating thread : %d",i);
					exit(0);
				}
				i++;
			}
			if(j<noc)
			{
				cthrcnt[j].num=j+1;
				rc=pthread_create(&c[j],&attr,consumer,&cthrcnt[j]);
				if(rc){
					printf("\nError while creating thread : %d",j);
					exit(0);
				}
				j++;
			}
		}
		/*for(i=0;i<nop;i++){
			pthread_join(p[i],NULL);
		}
		/*for(j=0;j<noc;j++){
			pthread_join(c[j],NULL);
		}*/
		sem_wait(&final);
		sem_destroy(&bsfull);
		sem_destroy(&bsempty);
		sem_destroy(&final);
		pthread_mutex_destroy(&mut);
		free(p);
		free(c);
		free(pthrcnt);
		free(cthrcnt);
		return 0;
	}
}


