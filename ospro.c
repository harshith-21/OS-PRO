#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<wait.h>
#include<sys/types.h>
#include<math.h>
#include<stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

pthread_cond_t cond1=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2=PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3=PTHREAD_COND_INITIALIZER;

pthread_mutex_t lock1=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock3=PTHREAD_MUTEX_INITIALIZER;

bool finished_1=false;
bool finished_2=false;
bool finished_3=false;

int to_do=1;

int run1=0;
int run2=0;
int run3=0;
double t1,t2,t3 = 0;
time_t e1,s2,e2,s3,e3;
static time_t s1;

int shmid;
int* shmPtr;

typedef struct thread_data {
   int n;
   long long int result;
   bool finished;
   double waiting_time;
   time_t start_time;
}thread_data;

typedef struct thread_data_fcfs {
   int n;
   long long int result;
   bool finished;
   double waiting_time;
   unsigned long start_time;
}thread_data_fcfs;

void *process_c2(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;
    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    for(int i=0;i<n;i++){
        pthread_mutex_lock(&lock2);
        while(!run2) { // paused
            gettimeofday(&tv,NULL);
            start=1000000 * tv.tv_sec + tv.tv_usec;
            // start=time(0);
            printf("paused,waiting,c2\n");
            pthread_cond_wait(&cond2, &lock2); // Wait for signal
            gettimeofday(&tv,NULL);
            end=1000000 * tv.tv_sec + tv.tv_usec;
            waiting_time=waiting_time+(double)(end-start);
        }
        pthread_mutex_unlock(&lock2);
        fscanf(fp,"%d\n",&num);
        printf("number is : %d\n",num);
    }
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c2_fcfs(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;
    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    gettimeofday(&tv,NULL);
    start=1000000 * tv.tv_sec + tv.tv_usec;
    while(!run2){}
    gettimeofday(&tv,NULL);
    end=1000000 * tv.tv_sec + tv.tv_usec;
    waiting_time=(double)(end-start);

    for(int i=0;i<n;i++){
        fscanf(fp,"%d\n",&num);
        printf("number is : %d\n",num);
    }
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c1(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;
    long long int sum=0;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    for(int j=1;j<=n;j++){
        pthread_mutex_lock(&lock1);
        while(!run1) { // paused
            gettimeofday(&tv,NULL);
            start=1000000 * tv.tv_sec + tv.tv_usec;
            printf("paused,waiting,c1\n");
            pthread_cond_wait(&cond1, &lock1); // Wait for signal
            gettimeofday(&tv,NULL);
            end=1000000 * tv.tv_sec + tv.tv_usec;
            waiting_time=waiting_time+(double)(end-start);
        }
        pthread_mutex_unlock(&lock1);
        sum+=j;
    }
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c1_fcfs(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;
    long long int sum=0;
    int n=tdata->n;
    double start,end;
    double waiting_time=0;
    gettimeofday(&tv,NULL);
    start=1000000 * tv.tv_sec + tv.tv_usec;
    while(!run1){}
    gettimeofday(&tv,NULL);
    end=1000000 * tv.tv_sec + tv.tv_usec;
    waiting_time=(double)(end-start);

    for(int j=1;j<=n;j++){
        sum+=j;
    }
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c3(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;

    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    long long int sum=0;
    double start,end;
    double waiting_time=0;
    for(int i=0;i<n;i++){
        pthread_mutex_lock(&lock3);
        while(!run3) { // paused
            gettimeofday(&tv,NULL);
            start=1000000 * tv.tv_sec + tv.tv_usec;
            printf("paused,waiting,c3\n");
            pthread_cond_wait(&cond3, &lock3); // Wait for signal 
            gettimeofday(&tv,NULL);
            end=1000000 * tv.tv_sec + tv.tv_usec;
            waiting_time=waiting_time+(double)(end-start);
        }
        pthread_mutex_unlock(&lock3);
        fscanf(fp,"%d\n",&num);
        sum=sum+num;
    }
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void *process_c3_fcfs(void *data){
    thread_data_fcfs *tdata=(thread_data_fcfs*)data;
    struct timeval tv;
    gettimeofday(&tv,NULL);
    unsigned long start_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
    tdata->start_time=start_in_micros;

    FILE *fp;
    fp=fopen("Number.txt","r");
    int num;
    int n=tdata->n;
    long long int sum=0;
    double start,end;
    double waiting_time=0;

    gettimeofday(&tv,NULL);
    start=1000000 * tv.tv_sec + tv.tv_usec;
    while(!run3){}
    gettimeofday(&tv,NULL);
    end=1000000 * tv.tv_sec + tv.tv_usec;
    waiting_time=(double)(end-start);

    for(int i=0;i<n;i++){
        fscanf(fp,"%d\n",&num);
        sum=sum+num;
    }
    tdata->result=sum;
    tdata->finished=true;
    tdata->waiting_time=waiting_time;
    pthread_exit(NULL);
}

void RR(int n1,int n2,int n3)
{
    int c1_pid,c2_pid,c3_pid;
    int time_quantum;
    printf("Enter time quantum: ");
    scanf("%d",&time_quantum);
    int fds_1[2],fds_2[2],fds_3[3],fds_1_time[2],fds_2_time[2],fds_3_time[2],fds_1_wtime[2],fds_2_wtime[2],fds_3_wtime[2],fds_1_stime[2],fds_2_stime[2],fds_3_stime[2],fds_2_parent_to_child[2];
    pipe(fds_1);
    pipe(fds_1_time);
    pipe(fds_1_wtime);
    pipe(fds_1_stime);


    char buf_1[30],buf_3[30],buf_2[30],buf_3_2[30],buf_1_2[30],buf_2_2[30];

    shmid=shmget(2045,32,0666 | IPC_CREAT);
    shmPtr=shmat(shmid,0,0);
    *shmPtr=-1;

    *(shmPtr+1)=0;
    *(shmPtr+2)=0;
    *(shmPtr+3)=0;

    c1_pid=fork();
    if(c1_pid!=0){
        // sleep(1);
        pipe(fds_2);
        pipe(fds_2_time);
        pipe(fds_2_wtime);
        pipe(fds_2_stime);
        pipe(fds_2_parent_to_child);

        c2_pid=fork();
        if(c2_pid!=0){
            // sleep(1);
            pipe(fds_3);
            pipe(fds_3_time);
            pipe(fds_3_wtime);
            pipe(fds_3_stime);

            c3_pid=fork();
            if(c3_pid!=0){
                sleep(4);
                struct timeval tv;
                gettimeofday(&tv,NULL);
                unsigned long start_of_all = 1000000 * tv.tv_sec + tv.tv_usec;

                char str[30];
                int len=snprintf(NULL,0,"%lu",start_of_all)+1;
                snprintf(str,len,"%lu",start_of_all);
                close(fds_2_parent_to_child[0]);
                write(fds_2_parent_to_child[1],str,len);

                printf("---------------STARTTING ALL PROCESSES-----------\n");
                *shmPtr=1;

                while((*(shmPtr+1)==0) || (*(shmPtr+2)==0) || (*(shmPtr+3)==0)){
                    *shmPtr=to_do;
                    if(to_do==1 && *(shmPtr+1)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C1 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        sleep(time_quantum);
                    }
                    else if(to_do==2 && *(shmPtr+2)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C2 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        sleep(time_quantum);
                    }
                    else if(to_do==3 && *(shmPtr+3)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C3 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        sleep(time_quantum);
                    }
                    to_do=to_do+1;
                    if(to_do==4){
                        to_do=1;
                    }
                }


                printf("----------------------\n");
                printf("Printing in Parent process\n");
                printf("----------------------\n");

                char* eptr;
                close(fds_1[1]);
                read(fds_1[0],buf_1,25);
                printf("C1 has sent %lld to parent via pipe\n",strtoll(buf_1,&eptr,10));

                close(fds_1_time[1]);
                read(fds_1_time[0],buf_1_2,25);
                char* eptr3;
                double ta_c1=(double)(strtoul(buf_1_2,&eptr3,10)-start_of_all)/1000000;
                printf("Turnaround time of C1 is %lf sec\n",ta_c1);

                close(fds_1_wtime[1]);
                read(fds_1_wtime[0],buf_1_2,25);
                double wt=strtod(buf_1_2,&eptr3);

                close(fds_1_stime[1]);
                read(fds_1_stime[0],buf_1_2,25);
                unsigned long s_ll=strtoul(buf_1_2,&eptr3,10);
                double wt_c1=(wt-(double)(start_of_all-s_ll))/1000000;
                printf("Waiting time of C1 is %lf sec\n",wt_c1);
                printf("Execution time of C1 is %lf sec\n",ta_c1-wt_c1);

                printf("----------------------\n");

                close(fds_2[1]);
                read(fds_2[0],buf_2,14);
                printf("C2 has sent '%s' to parent via pipe\n",buf_2);

                close(fds_2_time[1]);
                read(fds_2_time[0],buf_2_2,25);
                double ta_c2=(double)(strtoul(buf_2_2,&eptr3,10)-start_of_all)/1000000;
                printf("Turnaround time of C2 is %lf sec\n",ta_c2);

                close(fds_2_wtime[1]);
                read(fds_2_wtime[0],buf_2_2,25);
                char* eptr4;

                wt=strtod(buf_2_2,&eptr4);

                close(fds_2_stime[1]);
                read(fds_2_stime[0],buf_2_2,25);
                s_ll=strtoul(buf_2_2,&eptr4,10);
                double wt_c2=(wt-(double)(start_of_all-s_ll))/1000000;
                printf("Waiting time of C2 is %lf sec\n",wt_c2);
                printf("Execution time of C2 is %lf sec\n",ta_c2-wt_c2);

                printf("----------------------\n");

                close(fds_3[1]);
                read(fds_3[0],buf_3,25);
                printf("C3 sent %lld to parent via pipe\n",strtoll(buf_3,&eptr,10));

                close(fds_3_time[1]);
                read(fds_3_time[0],buf_3_2,25);
                char* eptr2;
                double ta_c3=(double)(strtoul(buf_3_2,&eptr2,10)-start_of_all)/1000000;
                printf("Turnaround time of C3 is %lf sec\n",ta_c3);

                close(fds_3_wtime[1]);
                read(fds_3_wtime[0],buf_3_2,25);

                wt=strtod(buf_3_2,&eptr2);

                close(fds_3_stime[1]);
                read(fds_3_stime[0],buf_3_2,25);
                s_ll=strtoul(buf_3_2,&eptr2,10);
                double wt_c3=(wt-(double)(start_of_all-s_ll))/1000000;

                printf("Waiting time of C3 is %lf sec\n",wt_c3);
                printf("Execution time of C3 is %lf sec\n",ta_c3-wt_c3);
            }
            else{
                struct timeval tv;
                gettimeofday(&tv,NULL);
                unsigned long s3_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
                printf("entered n3 is %d\n",n3);
                pthread_t thread_id_3;
                thread_data_fcfs data;
                data.n=n3;
                data.finished=false;
                int rc=pthread_create(&thread_id_3,NULL,process_c3,(void*)&data);    
                if(rc){
                    printf("error %d in pthread_create()\n",rc);
			        exit(1);
                }

                //shared mem
                shmid=shmget(2045,32,0);
                shmPtr=shmat(shmid,0,0);

                while(!data.finished){
//The below statement checks if the parent process has told C3 to start
//We have implemented using shared memory because we can constantly look at the shared data
//and can decide which process to run
                    if((*shmPtr)==3){
                        pthread_mutex_lock(&lock3);
                        run3 = 1;
                        // printf("signalling c3\n");
                        pthread_cond_signal(&cond3);
//the above line signals the thread which is waiting 
                        pthread_mutex_unlock(&lock3);
                        while((*shmPtr)==3){

                        }
                        pthread_mutex_lock(&lock3);
                        run3 = 0;
                        pthread_mutex_unlock(&lock3);
                    }
                }
                *(shmPtr+3)=1;
                // printf("sum in c3 is %lld\n",data.result);
                char str[30];
                int len=snprintf(NULL,0,"%lld",data.result)+1;
                snprintf(str,len,"%lld",data.result);
                close(fds_3[0]);
                write(fds_3[1],str,len);

                gettimeofday(&tv,NULL);
                unsigned long e3_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

                char str2[25];
                int len2=snprintf(NULL,0,"%lu",e3_in_micros)+1;
                snprintf(str2,len2,"%lu",e3_in_micros);
                close(fds_3_time[0]);
                write(fds_3_time[1],str2,len2);

                char str3[25];
                int len3=snprintf(NULL,0,"%lf",data.waiting_time)+1;
                snprintf(str3,len3,"%lf",data.waiting_time);
                close(fds_3_wtime[0]);
                write(fds_3_wtime[1],str3,len3);

                char str4[25];
                int len4=snprintf(NULL,0,"%lu",data.start_time)+1;
                snprintf(str4,len4,"%lu",data.start_time);
                close(fds_3_stime[0]);
                write(fds_3_stime[1],str4,len4);
            }
        }
        else{
            //c2 process here
            struct timeval tv;
            gettimeofday(&tv,NULL);
            unsigned long s2_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
            printf("entered n2 is %d\n",n2);
            pthread_t thread_id_2;
            thread_data_fcfs data;
            data.n=n2;
            data.finished=false;
            // sleep(3);
            int rc=pthread_create(&thread_id_2,NULL,process_c2,(void*)&data);    
            if(rc){
                printf("error %d in pthread_create()\n",rc);
			    exit(1);
            }
            //shared mem
            shmid=shmget(2045,32,0);
            shmPtr=shmat(shmid,0,0);

            while(!data.finished){
                if((*shmPtr)==2){
                    pthread_mutex_lock(&lock2);
                    run2 = 1;
                    // printf("signalling c2\n");
                    pthread_cond_signal(&cond2);
                    pthread_mutex_unlock(&lock2);
                    while((*shmPtr)==2){}
                    pthread_mutex_lock(&lock2);
                    run2 = 0;
                    pthread_mutex_unlock(&lock2);
                }
            }
            *(shmPtr+2)=1;
            printf("Done printing\n");
            close(fds_2[0]);
            write(fds_2[1],"Done Printing",14);
            gettimeofday(&tv,NULL);
            unsigned long e2_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

            char buf[30];
            close(fds_2_parent_to_child[1]);
            read(fds_2_parent_to_child[0],buf,25);

            char* eptr;
            printf("Turnaround time of C2 is %lf sec\n",(double)(e2_in_micros-strtoul(buf,&eptr,10))/1000000);

            char str2[25];
            int len2=snprintf(NULL,0,"%lu",e2_in_micros)+1;
            snprintf(str2,len2,"%lu",e2_in_micros);
            close(fds_2_time[0]);
            write(fds_2_time[1],str2,len2);

            char str3[25];
            int len3=snprintf(NULL,0,"%lu",data.start_time)+1;
            snprintf(str3,len3,"%lu",data.start_time);
            close(fds_2_stime[0]);
            write(fds_2_stime[1],str3,len3);

            char str4[25];
            int len4=snprintf(NULL,0,"%lf",data.waiting_time)+1;
            snprintf(str4,len4,"%lf",data.waiting_time);
            close(fds_2_wtime[0]);
            write(fds_2_wtime[1],str4,len4);
        }
    }
    else{
        //c1 process here
        printf("entered n1 is %d\n",n1);
        pthread_t thread_id_1;
        thread_data data;
        data.n=n1;
        data.finished=false;
        int rc=pthread_create(&thread_id_1,NULL,process_c1,(void*)&data);    
        if(rc){
            printf("error %d in pthread_create()\n",rc);
		    exit(1);
        }
        //shared mem
        shmid=shmget(2045,32,0);
        shmPtr=shmat(shmid,0,0);

        struct timeval tv;
        gettimeofday(&tv,NULL);
        unsigned long s1_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
        while(!data.finished){
            if((*shmPtr)==1){
                pthread_mutex_lock(&lock1);
                run1 = 1;
                // printf("signalling c1\n");
                pthread_cond_signal(&cond1);
                pthread_mutex_unlock(&lock1);
                while((*shmPtr)==1){}

                pthread_mutex_lock(&lock1);
                run1 = 0;
                pthread_mutex_unlock(&lock1);
            }
        }
        *(shmPtr+1)=1;
        // printf("sum in c1 is %lld\n",data.result);
        char str[30];
        int len=snprintf(NULL,0,"%lld",data.result)+1;
        snprintf(str,len,"%lld",data.result);
        close(fds_1[0]);
        write(fds_1[1],str,len);
        
        gettimeofday(&tv,NULL);
        unsigned long e1_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

        char str2[25];
        int len2=snprintf(NULL,0,"%lu",e1_in_micros)+1;
        snprintf(str2,len2,"%lu",e1_in_micros);
        close(fds_1_time[0]);
        write(fds_1_time[1],str2,len2);

        char str3[25];
        int len3=snprintf(NULL,0,"%lu",data.start_time)+1;
        snprintf(str3,len3,"%lu",data.start_time);
        close(fds_1_stime[0]);
        write(fds_1_stime[1],str3,len3);

        char str4[25];
        int len4=snprintf(NULL,0,"%lf",data.waiting_time)+1;
        snprintf(str4,len4,"%lf",data.waiting_time);
        close(fds_1_wtime[0]);
        write(fds_1_wtime[1],str4,len4);
    }
}


void FCFS(int n1,int n2,int n3)
{
    int c1_pid,c2_pid,c3_pid;
    int fds_1[2],fds_2[2],fds_3[3],fds_1_time[2],fds_2_time[2],fds_3_time[2],fds_1_wtime[2],fds_2_wtime[2],fds_3_wtime[2],fds_1_stime[2],fds_2_stime[2],fds_3_stime[2],fds_2_parent_to_child[2];
    pipe(fds_1);
    pipe(fds_1_time);
    pipe(fds_1_wtime);
    pipe(fds_1_stime);


    char buf_1[30],buf_3[30],buf_2[30],buf_3_2[30],buf_1_2[30],buf_2_2[30];

    shmid=shmget(2045,32,0666 | IPC_CREAT);
    shmPtr=shmat(shmid,0,0);
    *shmPtr=-1;

    *(shmPtr+1)=0;
    *(shmPtr+2)=0;
    *(shmPtr+3)=0;

    c1_pid=fork();
    if(c1_pid!=0){
        // sleep(1);
        pipe(fds_2);
        pipe(fds_2_time);
        pipe(fds_2_wtime);
        pipe(fds_2_stime);
        pipe(fds_2_parent_to_child);

        c2_pid=fork();
        if(c2_pid!=0){
            // sleep(1);
            pipe(fds_3);
            pipe(fds_3_time);
            pipe(fds_3_wtime);
            pipe(fds_3_stime);

            c3_pid=fork();
            if(c3_pid!=0){
                //scheduling here
                sleep(4);
                struct timeval tv;
                gettimeofday(&tv,NULL);
                unsigned long start_of_all = 1000000 * tv.tv_sec + tv.tv_usec;

                printf("---------------STARTTING ALL PROCESSES-----------\n");
                *shmPtr=1;

                while((*(shmPtr+1)==0) || (*(shmPtr+2)==0) || (*(shmPtr+3)==0)){
                    *shmPtr=to_do;
                    if(to_do==1 && *(shmPtr+1)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C1 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        while(*(shmPtr+1)==0){}
                    }
                    else if(to_do==2 && *(shmPtr+2)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C2 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        while(*(shmPtr+2)==0){}
                    }
                    else if(to_do==3 && *(shmPtr+3)==0){
                        gettimeofday(&tv,NULL);
                        printf("Executing C3 at %lf\n",(double)(1000000 * tv.tv_sec + tv.tv_usec)/1000000);
                        while(*(shmPtr+3)==0){}
                    }
                    to_do=to_do+1;
                    if(to_do==4){
                        to_do=1;
                    }
                }

                char str[30];
                int len=snprintf(NULL,0,"%lu",start_of_all)+1;
                snprintf(str,len,"%lu",start_of_all);
                close(fds_2_parent_to_child[0]);
                write(fds_2_parent_to_child[1],str,len);

                printf("----------------------\n");
                printf("Printing in Parent process\n");
                printf("----------------------\n");
                char* eptr;
                close(fds_1[1]);
                read(fds_1[0],buf_1,25);
                printf("C1 has sent %lld to parent via pipe\n",strtoll(buf_1,&eptr,10));

                close(fds_1_time[1]);
                read(fds_1_time[0],buf_1_2,25);
                char* eptr3;
                double ta_c1=(double)(strtoul(buf_1_2,&eptr3,10)-start_of_all)/1000000;
                printf("Turnaround time of C1 is %lf sec\n",ta_c1);

                close(fds_1_wtime[1]);
                read(fds_1_wtime[0],buf_1_2,25);
                double wt=strtod(buf_1_2,&eptr3);

                close(fds_1_stime[1]);
                read(fds_1_stime[0],buf_1_2,25);
                unsigned long s_ll=strtoul(buf_1_2,&eptr3,10);
                double wt_c1=(wt-(double)(start_of_all-s_ll))/1000000;

                printf("Waiting time of C1 is %lf sec\n",wt_c1);
                printf("Execution time of C1 is %lf sec\n",ta_c1-wt_c1);

                printf("----------------------\n");

                close(fds_2[1]);
                read(fds_2[0],buf_2,14);
                printf("C2 has sent '%s' to parent via pipe\n",buf_2);

                close(fds_2_time[1]);
                read(fds_2_time[0],buf_2_2,25);
                double ta_c2=(double)(strtoul(buf_2_2,&eptr3,10)-start_of_all)/1000000;
                printf("Turnaround time of C2 is %lf sec\n",ta_c2);

                close(fds_2_wtime[1]);
                read(fds_2_wtime[0],buf_2_2,25);
                char* eptr4;

                wt=strtod(buf_2_2,&eptr4);

                close(fds_2_stime[1]);
                read(fds_2_stime[0],buf_2_2,25);
                s_ll=strtoul(buf_2_2,&eptr4,10);
                double wt_c2=(wt-(double)(start_of_all-s_ll))/1000000;

                printf("Waiting time of C2 is %lf sec\n",wt_c2);
                printf("Execution time of C2 is %lf sec\n",ta_c2-wt_c2);

                printf("----------------------\n");

                close(fds_3[1]);
                read(fds_3[0],buf_3,25);
                printf("C3 sent %lld to parent via pipe\n",strtoll(buf_3,&eptr,10));

                close(fds_3_time[1]);
                read(fds_3_time[0],buf_3_2,25);
                char* eptr2;
                double ta_c3=(double)(strtoul(buf_3_2,&eptr2,10)-start_of_all)/1000000;
                printf("Turnaround time of C3 is %lf sec\n",ta_c3);

                close(fds_3_wtime[1]);
                read(fds_3_wtime[0],buf_3_2,25);

                wt=strtod(buf_3_2,&eptr2);

                close(fds_3_stime[1]);
                read(fds_3_stime[0],buf_3_2,25);
                s_ll=strtoul(buf_3_2,&eptr2,10);
                double wt_c3=(wt-(double)(start_of_all-s_ll))/1000000;

                printf("Waiting time of C3 is %lf sec\n",wt_c3);
                printf("Execution time of C3 is %lf sec\n",ta_c3-wt_c3);
            }
            else{
                //c3 process here
               
                struct timeval tv;
                gettimeofday(&tv,NULL);
                unsigned long s3_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
                printf("entered n3 is %d\n",n3);
                pthread_t thread_id_3;
                thread_data_fcfs data;
                data.n=n3;
                data.finished=false;
                int rc=pthread_create(&thread_id_3,NULL,process_c3_fcfs,(void*)&data);    
                if(rc){
                    printf("error %d in pthread_create()\n",rc);
			        exit(1);
                }

                //shared mem
                shmid=shmget(2045,32,0);
                shmPtr=shmat(shmid,0,0);

                while(*shmPtr!=3){}
                run3=1;
                while(!data.finished){}
                *(shmPtr+3)=1;
                // printf("sum in c3 is %lld\n",data.result);
                char str[30];
                int len=snprintf(NULL,0,"%lld",data.result)+1;
                snprintf(str,len,"%lld",data.result);
                close(fds_3[0]);
                write(fds_3[1],str,len);

                gettimeofday(&tv,NULL);
                unsigned long e3_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

                char str2[25];
                int len2=snprintf(NULL,0,"%lu",e3_in_micros)+1;
                snprintf(str2,len2,"%lu",e3_in_micros);
                close(fds_3_time[0]);
                write(fds_3_time[1],str2,len2);

                char str3[25];
                int len3=snprintf(NULL,0,"%lf",data.waiting_time)+1;
                snprintf(str3,len3,"%lf",data.waiting_time);
                close(fds_3_wtime[0]);
                write(fds_3_wtime[1],str3,len3);

                char str4[25];
                int len4=snprintf(NULL,0,"%lu",data.start_time)+1;
                snprintf(str4,len4,"%lu",data.start_time);
                close(fds_3_stime[0]);
                write(fds_3_stime[1],str4,len4);
            }
        }
        else{
            //c2 process here
            struct timeval tv;
            gettimeofday(&tv,NULL);
            unsigned long s2_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
            printf("entered n2 is %d\n",n2);
            pthread_t thread_id_2;
            thread_data_fcfs data;
            data.n=n2;
            data.finished=false;
            // sleep(4);
            int rc=pthread_create(&thread_id_2,NULL,process_c2_fcfs,(void*)&data);    
            if(rc){
                printf("error %d in pthread_create()\n",rc);
			    exit(1);
            }
            //shared mem
            shmid=shmget(2045,32,0);
            shmPtr=shmat(shmid,0,0);

            while(*shmPtr!=2){}
            run2=1;
            while(!data.finished){}
            *(shmPtr+2)=1;
            printf("Done printing\n");
        
            close(fds_2[0]);
            write(fds_2[1],"Done Printing",14);
            gettimeofday(&tv,NULL);
            unsigned long e2_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

            char buf[30];
            close(fds_2_parent_to_child[1]);
            read(fds_2_parent_to_child[0],buf,25);

            char* eptr;
            printf("Turnaround time of C2 is %lf sec\n",(double)(e2_in_micros-strtoul(buf,&eptr,10))/1000000);

            char str2[25];
            int len2=snprintf(NULL,0,"%lu",e2_in_micros)+1;
            snprintf(str2,len2,"%lu",e2_in_micros);
            close(fds_2_time[0]);
            write(fds_2_time[1],str2,len2);

            char str3[25];
            int len3=snprintf(NULL,0,"%lu",data.start_time)+1;
            snprintf(str3,len3,"%lu",data.start_time);
            close(fds_2_stime[0]);
            write(fds_2_stime[1],str3,len3);

            char str4[25];
            int len4=snprintf(NULL,0,"%lf",data.waiting_time)+1;
            snprintf(str4,len4,"%lf",data.waiting_time);
            close(fds_2_wtime[0]);
            write(fds_2_wtime[1],str4,len4);
        }
    }
    else{
        //c1 process here
        printf("entered n1 is %d\n",n1);
        pthread_t thread_id_1;
        thread_data_fcfs data;
        data.n=n1;
        data.finished=false;
        int rc=pthread_create(&thread_id_1,NULL,process_c1_fcfs,(void*)&data);    
        if(rc){
            printf("error %d in pthread_create()\n",rc);
		    exit(1);
        }
        //shared mem
        shmid=shmget(2045,32,0);
        shmPtr=shmat(shmid,0,0);

        struct timeval tv;
        gettimeofday(&tv,NULL);
        unsigned long s1_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
        while(*shmPtr!=1){}
        run1=1;
        while(!data.finished){}
        *(shmPtr+1)=1;
        // printf("sum in c1 is %lld\n",data.result);
        char str[30];
        int len=snprintf(NULL,0,"%lld",data.result)+1;
        snprintf(str,len,"%lld",data.result);
        close(fds_1[0]);
        write(fds_1[1],str,len);

        gettimeofday(&tv,NULL);
        unsigned long e1_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;

        char str2[25];
        int len2=snprintf(NULL,0,"%lu",e1_in_micros)+1;
        snprintf(str2,len2,"%lu",e1_in_micros);
        close(fds_1_time[0]);
        write(fds_1_time[1],str2,len2);

        char str3[25];
        int len3=snprintf(NULL,0,"%lu",data.start_time)+1;
        snprintf(str3,len3,"%lu",data.start_time);
        close(fds_1_stime[0]);
        write(fds_1_stime[1],str3,len3);

        char str4[25];
        int len4=snprintf(NULL,0,"%lf",data.waiting_time)+1;
        snprintf(str4,len4,"%lf",data.waiting_time);
        close(fds_1_wtime[0]);
        write(fds_1_wtime[1],str4,len4);
    }
}



int main(){
    int n1,n2,n3;
    printf("Enter n1 n2 n3 (in format n1 n2 n3): ");
    scanf("%d %d %d",&n1,&n2,&n3);
    int algo;
    printf("enter the scheduling algo (0 for fcfs and 1 for RR) : ");
    scanf("%d",&algo);
    if(algo==0){
        FCFS(n1,n2,n3);
    }
    else{
        RR(n1,n2,n3);
    }
}