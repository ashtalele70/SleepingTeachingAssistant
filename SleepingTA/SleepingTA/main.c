//
//  main.c
//  Sleeping Teaching Assistant
//
//  Created by Ashwini Talele and Samruddhi Patil on 11/6/19.
//  Student ID : 014483456 (Ashwini Talele)
//  Student ID : 014550094 (Samruddhi Patil)
//  Copyright © 2019 Ashwini Talele and Samruddhi Patil. All rights reserved.
//

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>
#include<sys/ipc.h>
#include<semaphore.h>

#define N 3 //number of chairs in waiting room

time_t end_time;/*end time*/
sem_t mutex,students,ta;/*Three semaphors mutex = accessWRSeats  */
int numberOfSeatsWR = N;/*The number of chairs empty*/
void taActivity(void *arg);
void studentActivity(void *arg);

int main(int argc,char *argv[])
{
    pthread_t id1,id2;
    int status=0;
    end_time=time(NULL)+20;/*TA Tutor Hours is 20s*/

    /*Semaphore initialization*/
    sem_init(&mutex,0,1);
    sem_init(&students,0,0);
    sem_init(&ta,0,1);

    
    /*Student_thread initialization*/
    status=pthread_create(&id2,NULL,(void *)studentActivity,NULL);
    if(status!=0)
        perror("create students is failure!\n");
    /*TA_thread initialization*/
    status=pthread_create(&id1,NULL,(void *)taActivity,NULL);
    if(status!=0)
        perror("create ta is failure!\n");


    /*Student_thread first blocked*/
    pthread_join(id2,NULL);
    pthread_join(id1,NULL);

    exit(0);
}

void taActivity(void *arg)/*TA Process*/
{
    while(time(NULL)<end_time || numberOfSeatsWR<N)
    {
        sem_wait(&students);/*P(students)*/
        sem_wait(&mutex);/*P(mutex)*/
        numberOfSeatsWR++;
        printf("TA tutoring, No of empty seats :%d.\n",numberOfSeatsWR);
        sem_post(&mutex);/*V(mutex)*/
        sem_post(&ta);/*V(ta)*/
        sleep(3);
        printf("TA tutoring completed.\n");
        if(numberOfSeatsWR == N)
            printf("TA goes to sleep.\n");
    }
    
}

void studentActivity(void *arg)/*Students Process*/
{
    while(time(NULL)<end_time)
    {
        sem_wait(&mutex);/*P(mutex)*/
        if(numberOfSeatsWR>0)
        {
            numberOfSeatsWR--;
            printf("Student arrived in waiting room, No of empty seats :%d\n",numberOfSeatsWR);
            sem_post(&mutex);/*V(mutex)*/
            sem_post(&students);/*V(students)*/
            sem_wait(&ta);/*P(ta)*/
        }
        else{
            /*V(mutex)*/
            /*If the number is full of students,just put the mutex lock let go*/
            sem_post(&mutex);
            printf("Student sees that there is no empty chair and leaves.\n");
        }
        sleep(1);
    }
}


