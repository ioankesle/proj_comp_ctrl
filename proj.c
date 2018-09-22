#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <rtai_lxrt.h>
#include <sched.h>
#include <sys/mman.h>
#include <rtai_shm.h>
#include <rtai_sem.h>
#include <rtai_sched.h>
#include <comedilib.h>
#include "dat.h"
#include <math.h>
#include "dat_fixed.h"
#include "fixpoint.h"
//#include "fixpoint.h"
//#include "fixpifunc.h"


int fl = 0;
int value2 = 1;
int type10 = RES_SEM;
SEM *sem1;
double myarray[10]={ 0 };
double mytime[300]={ -100 };
double myref[300]={ -100 };
int nu10;
char *name10 = "onm222";
int value10 = 1;
float speed =0;
int smth = 1;
double refSkids = 6.2;
void  *start_belt(void *data);
void  *start_skids(void *data1);
void  *start_measure(void *data2);
long long xronos = 0;
int l = 0;
int l2 = 0;
int small = 0;
int medium = 0;
int large = 0;
int too_large = 0;
int too_small = 0;
int choice = 0;


int main()
{
	char *name = "onom";
	int priority = 13;
	int stack_size = 0;
	int max_msg_size = 10;
 	int nu = 0;
	pthread_t thread;
	pthread_t thread1;
	pthread_t thread2;
    pid_t pid = 0;
	int policy=SCHED_FIFO;
	int maxpr = 0; 	
	struct sched_param p;
	nu10 = nam2num(name10);
	RT_TASK *task;	
	//RTIME delay;
	rt_allow_nonroot_hrt();
	nu = nam2num(name);
	mlockall(MCL_CURRENT | MCL_FUTURE);	
	maxpr = sched_get_priority_max(policy);
	p.sched_priority = maxpr;
	sched_setscheduler(pid,policy, &p);	
	task = rt_task_init(nu,priority,stack_size,max_msg_size);
	if (task == NULL){
		printf("task didnt start \n");
		exit(0);
	}
	sem1 = rt_typed_sem_init(nu10,value10,type10);
	if (sem1 == NULL){
		printf("sem1 didnt start \n");
		exit(0);
	}
	pthread_create(&thread,NULL, start_belt, NULL);
	pthread_create(&thread1,NULL, start_skids, NULL);
	pthread_create(&thread2,NULL, start_measure, NULL);
	printf("Entered \n");
	do{
		printf("Enter 1 for speed (m/s) \nEnter 2 for statistics\nEnter 0 to exit\n");
		scanf("%d",&choice);
		if (choice == 1){
			printf("Enter speed (m/s) \n");
			scanf("%f",&speed);
			printf("%f \n",speed);		
		}
		else if (choice == 2){
			printf("Too small: %d \n",too_small);
			printf("Small: %d \n",small);
			printf("Medium: %d \n",medium);
			printf("Large: %d \n",large);
			printf("Too large: %d \n",too_large);	
		}		
	}while(choice!=0);
	fl = 1;
	printf("Terminating the program, please wait...\n");
	pthread_join(thread, NULL);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	rt_task_delete(task);
	return 0;
}
void  *start_belt(void *data)
{	
	double b0=4.703;
	double b1=-4.307;
	double a1=-1;
	double constbelt = 4.138;
	double Tbelt = 0.0088461*1.0e9;
	char *name1 = "onom1";
	int priority1 = 12;
	int stack_size1 = 0;
	int max_msg_size1 = 10;
 	int nu1;	
	RT_TASK *task1;
	RTIME start_time1 = 0*1.0e9;
	RTIME period1 = Tbelt;
	nu1 = nam2num(name1);
	double u =0;
	double e = 0;
	int i=0;
	statetype contr2;

	double ad_data;
	lsampl_t data1_th1;
	lsampl_t data2_th1;
	comedi_t* device1;
	device1=comedi_open("/dev/comedi2");
	task1 = rt_task_init(nu1,priority1,stack_size1,max_msg_size1);
	rt_task_make_periodic_relative_ns(task1,start_time1,period1);
	if (task1 == NULL){
		printf("task1 didnt start \n");
		exit(0);
	}
	regul_init(&contr2);	
     	while (fl== 0)
     	{
		rt_task_wait_period();
		comedi_data_read_delayed(device1,0,0,0,AREF_DIFF,&data1_th1,50000);
		ad_data=data1_th1;
		ad_data=(ad_data-2048)*(0.0048852) -0.127;  // volts
		e = speed*constbelt - ad_data; //  m/s
		u = regul_out(&contr2,e,&b0);
		u = ((4096./20.)*u)+2048;
		if (u>4095){
			u=4095;
		}
		if (u<0){
			u=0;
		}
		data2_th1 = u;
		comedi_data_write(device1,1,0,0,AREF_DIFF,data2_th1);
		regul_update(&contr2,e,&a1,&b1);
		i=i+1;
     	}
	comedi_data_write(device1,1,0,0,AREF_DIFF,2048);	
	rt_task_delete(task1);
	return 0;	
}	
void  *start_skids(void *data1)
{	
	//double b0s=32.7922;
	//double b1s=-26.3526;
	//double a1s=-0.295;
	short b0s =16790;
	short b1s =-26985;
	short a1s =9667;
	double Tskids = 0.0073127*1.0e9;
	char *name2 = "onom2";
	int priority2 = 11;
	int stack_size2 = 0;
	int max_msg_size2 = 10;
 	int nu2;	
	RT_TASK *task2;
	RTIME start_time2 = 0*1.0e9;
	RTIME period2 = Tskids;
	nu2 = nam2num(name2);
	double u2 =0;
	double e2 = 0;
	int i2=0;
	RTIME delay;
	long long metritis = 0;
	statetypef contr1;

	double ad_data2;
	lsampl_t data1_th2;
	lsampl_t data2_th2;
	comedi_t* device2;
	device2=comedi_open("/dev/comedi2");
	task2 = rt_task_init(nu2,priority2,stack_size2,max_msg_size2);
	rt_task_make_periodic_relative_ns(task2,start_time2,period2);
	if (task2 == NULL){
		printf("task2 didnt start \n");
		exit(0);
	}	
	regul_init_fixed(&contr1);
     	while (fl== 0)
     	{
		delay = nano2count(10e3);
		rt_sleep(delay);
		rt_task_wait_period();
		smth = rt_sem_wait(sem1);
		comedi_data_read_delayed(device2,0,1,0,AREF_DIFF,&data1_th2,50000);
		ad_data2=data1_th2;
		ad_data2=(ad_data2-2048)*(0.0048852);
		if (xronos == -1 || myref[l] != -100){
			//printf("Hi");
			xronos = mytime[l];
		}
		metritis = rt_get_time_ns();
		if (metritis >= xronos && xronos != 0){
			refSkids = myref[l];
			xronos = -1;
			l = l + 1;
			//printf("l",l);
		}
		e2 = refSkids - ad_data2;
		u2 = regul_out_fixed(&contr1,e2,b0s);
		u2 = ((4096./20.)*u2)+2048;
		if (u2>4095){
			u2=4095;
		}
		if (u2<0){
			u2=0;
		}
		data2_th2 = u2;
		comedi_data_write(device2,1,1,0,AREF_DIFF,data2_th2);
		regul_update_fixed(&contr1,e2,a1s,b1s);
		i2=i2+1;
		smth = rt_sem_signal(sem1);
     	}
	comedi_data_write(device2,1,1,0,AREF_DIFF,2048);	
	rt_task_delete(task2);
	return 0;	
}
void  *start_measure(void *data2)
{	
	char *name3 = "onom3";
	int priority3 = 7;
	int stack_size3 = 0;
	int max_msg_size3 = 10;
 	int nu3;	
	int ticks = 0;
	RT_TASK *task3;
	RTIME start_time3 = 0*1.0e9;
	RTIME period3 = 0.001*1.0e9;
	nu3 = nam2num(name3);
	int i3=0;
	//float t = 0;
	long long t1=0,t0=0;
	double ad_data3;
	lsampl_t data1_th3;
	comedi_t* device3;
	float length = 0;
	device3=comedi_open("/dev/comedi2");
	task3 = rt_task_init(nu3,priority3,stack_size3,max_msg_size3);
	rt_task_make_periodic_relative_ns(task3,start_time3,period3);
	if (task3 == NULL){
		printf("task3 didnt start \n");
		exit(0);
	}
	t0=rt_get_time_ns();
     	while (fl== 0)
     	{
		rt_task_wait_period();
		rt_sem_wait(sem1);
		//t1=rt_get_time_ns();
		//t =(t1-t0)/10e8;
		//printf("Time %4.4f \n", t);
		comedi_data_read_delayed(device3,0,2,0,AREF_DIFF,&data1_th3,50000);
		ad_data3=data1_th3;
		ad_data3=(ad_data3-2048)*(0.0048852);
		if (ad_data3 > 8){	//there is no brick
			i3=i3+1;
		}
		if (i3 > 0 && ad_data3 < 2){  // there is a brick
			ticks = i3;
			//printf("Ticks %d \n", ticks);
			i3 = 0;
		}
		if (ticks > 0){
			length = ticks*speed;
			printf("%f \n",length);		
			t1=rt_get_time_ns();
			if (length > 42 && length < 57){
				myref[l2] =5;
				mytime[l2] = t1+0.32/speed*1.0e9;
				l2 = l2 + 1;
				small = small + 1;
				printf("Small \n");
			}
			else if (length >= 57 && length <= 90){
				myref[l2] =6.2;
				mytime[l2] = t1+0.32/speed*1.0e9;
				l2 = l2 + 1;
				medium = medium + 1;
				printf("Medium \n");
			}
			else if (length >= 93 && length <= 130){
				myref[l2] =7;
				mytime[l2] = t1+0.32/speed*1.0e9;
				l2 = l2 + 1;
				large = large + 1;
				printf("Large \n");
			}
			else if (length <= 35){
				too_small = too_small + 1;
				printf("Too small \n");
			}
			else if (length >= 135){
				myref[l2] =7;
				mytime[l2] = t1+0.32/speed*1.0e9;
				l2 = l2 + 1;
				too_large = too_large + 1;
				printf("Too large \n");
			}
			ticks = 0;
		}
		rt_sem_signal(sem1);
     	}
	rt_sem_delete(sem1);
	rt_task_delete(task3);
	return 0;	
}
