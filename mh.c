/*****************************************************************************************************************
 *
 * NAME 		: 	VINU CHARANYA ATHANGUDI PURUSHOTHAMAN
 * PERSON # 	:	37648013
 * PROGRAM 2	:	mh.c
 * HEADER FILE	:	osheaders.h
 *
 * DESCRIPTION	: The program gets one input. The number of days the mother hubbard program should run
 *                            Based on the number of days, the mother wakes up the child does various tasks and once she
 *                            finishes with bathing a child,she wakes up the father to read a book and make the child sleep.
 *
 * EXECUTION	:	./mh <Days>
 *
 * <Days> is the # of days to run the mother Hubbard function on.
 *
 *
 *
 * DATE			:	29 Mar 2011
 *
 ******************************************************************************************************************/
#include "osheaders.h"
int days;
int beg=1,day=1;
sem_t moth;
sem_t fath;
pthread_attr_t attr;

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
	sem_init(&moth,0,1);
	sem_init(&fath,0,0);
	pthread_attr_init(&attr);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
}
/*****************************************************************************************************************
 *
 * Function 	: 	mother
 *
  Parameters	:	nil
 *
 * Description	:	This function performs various tasks on 12 children and when it comes to last task it will wake
 * 							up father and goes back to sleep after finishing 12 children. It sleeps till it is woken up by father
 * 							for the next day. It exits when the number of days are over
 *
 ******************************************************************************************************************/
void *mother (void *args){
	int i;

	while(beg<=days)
	{
		printf("\n\nMother Woke up for day :\t%d\n\n",beg);
		for(i =0; i < 12 ; i++)
					{
						printf(" Mother wakes the child #%d up\n",i+1);
					}
				for(i =0; i < 12 ; i++)
				{
					printf("Mother gives breakfast to child #%d \n",i+1);
				}
				for(i =0; i < 12 ; i++)
					{
						printf("Mother sends the child #%d to school\n",i+1);
					}
				for(i =0; i < 12 ; i++)
					{
						printf("Mother  gives dinner to child #%d\n",i+1);
					}
				for(i =0; i < 12 ; i++)
					{
					printf("Mother gives bath to child #%d\n",i+1);
					usleep(100);
					sem_post(&fath);//Mother wakes the father up
					}
				beg++;
	}
}

/*****************************************************************************************************************
 *
 * Function 	: 	father
 *
  Parameters	:	nil
 *
 * Description	:	This function performs various tasks on 12 children after it is woken up by mother and when he puts
 * 							all the children to sleep he will wake the mother. The father waits on every child to be bathed by mother.
 *  						he sleeps after wakin up the mother after he finishes it with all the 12 children.
 *
 ******************************************************************************************************************/

void *father(void *args){
	int i;

	while(day<=days){
		printf("\n\nFather has woken up for day :\t%d\n\n",day);
		for(i =0; i < 12 ; i++)
				{
					sem_wait(&fath); //Father is waiting to be woken up
					printf("Father reads the child #%d a book\n",i+1);
					usleep(100);
				}

				for(i =0; i < 12 ; i++)
					{

						printf("Father makes the child #%d sleep\n",i+1);
					}

				printf("\n\nFather wakes the Mother and goes to sleep\n");
		day++;
		sem_post(&moth);
	}
}

/*****************************************************************************************************************
 *
 * Function 	: 	Main
 *
  Parameters	:	<# of days>
 *
 *
 * Description	:		The program gets one input. The number of days the mother hubbard program should run
 *                            Based on the number of days, the mother wakes up the child does various tasks and once she
 *                            finishes with bathing a child,she wakes up the father to read a book and make the child sleep.
 *                            The threads are created and they are waited by join to perform the given tasks for the number of days
 *
 ******************************************************************************************************************/


int main(int argc, char **argv) {

	if (argc != 2)//checking for number of arguments
	{
		printf("\nHey argument must be 2!!");
		printf("\nThe correct format is ./mh <# of Days>\n");

	} else {

		printf("\nYou have given correct number of Arguments\n");
		days = atoi(argv[1]);
		printf("\n Number of Days\t =\t%d\n\n", days);

		init();

		pthread_t m;
		pthread_t f;

		pthread_create(&m,&attr,mother,NULL);
		pthread_create(&f,&attr,father,NULL);

		pthread_join(m,NULL);
		pthread_join(f,NULL);

	exit(0);

	}
}



