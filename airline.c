/*****************************************************************************************************************
 *
 * NAME 				: 	VINU CHARANYA ATHANGUDI PURUSHOTHAMAN
 * PERSON # 		:	37648013
 * PROGRAM 2		:	airline.c
 * HEADER FILE	:	osheaders.h
 *
 * DESCRIPTION	: Program takes four inputs as follows and corresponding threads are created. Each thread waits for the
 * 								corresponding baggage thread or other threads and executes them in the order of baggage,security,attendant
 *   							and exits. After all the thread exits, the main exits.All  passengers arrive at the same time to the terminal
 *	     					  and must wait on a line to be processed, but are processed in no particular order.
 *
 *
 * EXECUTION		:	./airline <# of Passengers> <# of Baggage> <# of Security> <#of Flight Attendants>
 *
 *
 *
 *
 * DATE				:	5 Apr 2011
 *
 ******************************************************************************************************************/
#include "osheaders.h"
int pas;
int bag;
int sec;
int att;
sem_t p2b,b2p,p2s,s2p,p2a,a2p;
sem_t finmut,secmut,attmut;
pthread_mutex_t pasmut;
pthread_attr_t attr;
int in=0;
int counter=0;
struct thrcnt {
	int num;
};

typedef struct{
	int *arr;
}passen;
passen pp;
/*****************************************************************************************************************
 *
 * Function 	: 	init
 *
  Parameters	: nil
 *
 * Description	:	This function initializes the various semaphores and mutex.
 *
 ******************************************************************************************************************/

void init()
{
	sem_init(&p2b, 0, 0);//Semaphore for passenger to baggage
	sem_init(&b2p,0,bag);//Semaphore for baggage to passenger
	sem_init(&p2s,0,0);//Semaphore for passenger to security
	sem_init(&s2p,0,sec);//Semaphore for security to passenger
	sem_init(&p2a,0,0); //Semaphore for passenger to attendant
	sem_init(&a2p,0,att);//Semaphore for attendant to passenger
	sem_init(&finmut,0,0); //Binary Semaphore to say the attendant is done
	sem_init(&secmut,0,0);//Binary Semaphore to say the security can be called
	sem_init(&attmut,0,0);//Binary semaphore to inform attendant can be called
	pthread_mutex_init(&pasmut,NULL);//Mutex to check when passenger number is added to the array
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

}

/*****************************************************************************************************************
 *
 * Function 	: 	passenger
 *
  Parameters	:	<passenger thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function takes the passenger thread value and runs to call the baggage then security
 * 							and then attendant.
 *							This function uses semaphores and one mutex that waits for baggage, security and attendant.
 *							Each function is called after the corresponding binary semaphore is called by the corresp. thread
 *							to process that passenger and prints the passenger id and what it waits for. After all the threads
 *							finishes attendant the thread exits. It uses mutex to add the passenger to the buffer array.
 *
 ******************************************************************************************************************/
void *passenger(void *args)
{
	struct thrcnt tp;
	tp=*(struct thrcnt *)args;
	int pasval =tp.num;
	static int a=0;
	static int b=0;
	static int c=0;

	//Adds the passenger to the array
	pthread_mutex_lock(&pasmut);
	pp.arr[in]=pasval;
	in++;
	pthread_mutex_unlock(&pasmut);

	//Waits for baggage and calls the baggage thread
	sem_wait(&b2p);
	printf("\nPassenger\t%d arrived and waiting for Baggage\n", pp.arr[a++]);
	usleep(1000);
	sem_post(&p2b);

	sem_wait(&secmut);

	//Waits for security and calls the security thread
	sem_wait(&s2p);
	printf("\nPassenger\t%d done with baggage and waiting for Security\n", pp.arr[b++]);
	usleep(1000);
	sem_post(&p2s);

	sem_wait(&attmut);
	//Waits for attendant and calls the attendant thread
	sem_wait(&a2p);
	printf("\nPassenger\t%d done with security and waiting for Attendant\n", pp.arr[c++]);
	usleep(1000);
	sem_post(&p2a);
	sem_wait(&finmut);
	pthread_exit(NULL);

}
/*****************************************************************************************************************
 *
 * Function 	: 	baggage
 *
  Parameters	:	<baggage thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function takes the baggage thread value and keeps running till the program exits
 *							This function uses three semaphores that waits for passengers and posts when it is free
 *							to process next passenger and prints the passenger id and baggage number. after it posts
 *							to passenger thread so that it can call the security thread for that passenger.
 *
 ******************************************************************************************************************/

void *baggage(void* args)
{
	struct thrcnt tb;
	tb= *(struct thrcnt *)args ;
	int bagval=tb.num;
	static int c=0;
	while(1)
	{
		sem_wait(&p2b);//Waits for passenger to post
		//printf("\nPassenger is handled by baggage :\t%d",bagval);
		pthread_mutex_lock(&pasmut);
		printf("\nPassenger\t#%d by baggage handler\t\t#%d\n",pp.arr[c],bagval);
		c++;
		pthread_mutex_unlock(&pasmut);
		usleep(1000);
		sem_post(&secmut);// Wakes up secmut to call security thread
		sem_post(&b2p);//Posts to accept next passenger
	}

}
/*****************************************************************************************************************
 *
 * Function 	: 	security
 *
  Parameters	:	<security thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function takes the security thread value and keeps running till the program exits
 *							This function uses three semaphores that waits for passengers and posts when it is free
 *							to process next passenger and prints the passenger id and security number. after it posts
 *							to passenger thread so that it can call the attendant thread for that passenger.
 *
 ******************************************************************************************************************/
void *security(void* args)
{
	struct thrcnt ts;
	ts= *(struct thrcnt *)args ;
	int secval=ts.num;
	static int cs=0;
	while(1)
	{
		sem_wait(&p2s);//Waits for passenger to post
		//printf("\nPassenger is handled by security :\t%d",secval);
		pthread_mutex_lock(&pasmut);
		printf("\nPassenger\t#%d by security screen\t\t#%d\n",pp.arr[cs],secval);
		cs++;
		pthread_mutex_unlock(&pasmut);
		usleep(1000);
		sem_post(&attmut);// Wakes up attmut to call attendant thread
		sem_post(&s2p);//Post when it is free to process next passenger
	}
}
/*****************************************************************************************************************
 *
 * Function 	: 	attendant
 *
  Parameters	:	<attendant thread number>
 * 						Passed as a struct variable from main
 *
 * Description	:	This function takes the attendant thread value and keeps running till the program exits
 *							This function uses three semaphores that waits for passengers and posts when it is free
 *							to process next passenger and prints the passenger id and attendant number. after it posts
 *							to passenger thread so that it can exit.
 *
 ******************************************************************************************************************/
void *attendant(void* args)
{
	struct thrcnt ta;
	ta= *(struct thrcnt *)args ;
	int attval=ta.num;
	static int ca=0;
	while(1)
	{
		sem_wait(&p2a);
		//printf("\nPassenger is handled by attendant :\t%d",attval);
		pthread_mutex_lock(&pasmut);
		printf("\nPassenger\t#%d by flight attendant\t\t#%d\n",pp.arr[ca],attval);
		ca++;
		pthread_mutex_unlock(&pasmut);
		usleep(1000);
		sem_post(&finmut);
		sem_post(&a2p);
	}
}
/*****************************************************************************************************************
 *
 * Function 	: 	Main
 *
  Parameters	:	<# of Passengers> <# of Baggage> <# of Security> <#of Flight Attendants>
 * 					All the filenames are string passed through argv[1] argv[2] argv[3] and argv[4] which are
 * 					converted to integer
 *
 * Description	:	Program takes four arguments from the command line <# of Passengers> <# of Baggage>
 *							 <# of Security> <#of Flight Attendants> 	It creates thread for each of the values and calls
 *							 the corresponding functions for the threads to run. The main program waits on the passenger
 *							 threads and exits when the passenger thread exits.
 *
 ******************************************************************************************************************/

int main(int argc, char **argv) {

	if (argc != 5)//checking for number of arguments
	{
		printf("\nHey argument must be 5!!");
		printf("\nThe correct format is ./airline <# of Passengers> <#of Baggage> <# of Security> <# of Flight Attendant>\n");

	} else {
		int i,j,k,l,rc;
		printf("\nYou have given correct number of Arguments\n");
		pas = atoi(argv[1]);
		bag = atoi(argv[2]);
		sec = atoi(argv[3]);
		att = atoi(argv[4]);

		printf("\n Number of Passengers\t =\t%d\n", pas);
		printf("\n Number of Baggage\t =\t%d\n", bag);
		printf("\n Number of Security\t =\t%d\n", sec);
		printf("\n Number of Attendants\t =\t%d\n\n", att);

		// Using malloc for dynamic memory allocation for various thread numbers got as user input
		pthread_t *p,*b,*s,*a;
		p=(pthread_t *)malloc(sizeof(pthread_t) * pas);
		b=(pthread_t *)malloc(sizeof(pthread_t) * bag);
		s=(pthread_t *)malloc(sizeof(pthread_t) * sec);
		a=(pthread_t *)malloc(sizeof(pthread_t) * att);

		//array in struct that will hold the passenger value.
		pp.arr = (int *)malloc(sizeof(int)*pas);

		init(); // Function that initializes the various semaphores and mutex

		struct thrcnt *pc;
		struct thrcnt *bc;
		struct thrcnt *sc;
		struct thrcnt *ac;

		pc = (struct thrcnt *)malloc(sizeof(struct thrcnt)*pas);
		bc = (struct thrcnt *)malloc(sizeof(struct thrcnt)*bag);
		sc = (struct thrcnt *)malloc(sizeof(struct thrcnt)*sec);
		ac = (struct thrcnt *)malloc(sizeof(struct thrcnt)*att);



		// Creation of threads for Baggage handler, Security Screener, Flight Attendants, Passengers
		for(i=0;i<bag;i++)
		{
			bc[i].num=i+1;
			rc=pthread_create(&b[i],&attr,baggage,&bc[i]);
			if(rc){
						printf("\nError while creating thread : %d",i);
						exit(0);
					}
		}

		for(j=0;j<sec;j++)
				{
					sc[j].num=j+1;
					rc=pthread_create(&s[j],&attr,security,&sc[j]);
					if(rc){
								printf("\nError while creating thread : %d",j);
								exit(0);
							}
				}
		for(k=0;k<att;k++)
				{
					ac[k].num=k+1;
					rc=pthread_create(&a[k],&attr,attendant,&ac[k]);
					if(rc){
								printf("\nError while creating thread : %d",k);
								exit(0);
							}
				}
			for(l=0;l<pas;l++)
				{
					pc[l].num=l+1;
					rc=pthread_create(&p[l],&attr,passenger,&pc[l]);
					if(rc){
								printf("\nError while creating thread : %d",l);
								exit(0);
							}
				}

			//Waiting for the Passenger thread to join

			for(l=0;l<pas;l++)
				{
					pthread_join(p[l],NULL);
				}
			printf("\nAll the Passengers are seated and the VC AIRLINES is ready to take off .Happy journey Passengers..YIPEEE\n\n");
			free(p);free(s);free(b);free(a);
			free(pc);free(bc);free(sc);free(ac);
			sem_destroy(&p2b);
			sem_destroy(&p2s);
			sem_destroy(&p2a);
			sem_destroy(&b2p);
			sem_destroy(&s2p);
			sem_destroy(&a2p);
			sem_destroy(&finmut);
			sem_destroy(&secmut);
			sem_destroy(&attmut);
			pthread_mutex_destroy(&pasmut);

exit(0);
	}
}
