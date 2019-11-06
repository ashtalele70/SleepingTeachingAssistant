/**
 *
 *     Sleeping Teaching Assistant
 *     A university computer science department has a teaching assistant (TA) who helps undergraduate students with their
 *     programming assignments during regular office hours. The TA’s office is rather small and has room for only one desk with a chair and computer.
 *     There are three chairs in the hallway outside the office where students can sit and wait if the TA is currently helping another student.
 *     When there are no students who need help during office hours, the TA sits at the desk and takes a nap. If a student arrives during office hours
 *     and finds the TA sleeping, the student must awaken the TA to ask for help. If a student arrives and finds the TA currently helping another student,
 *     the student sits on one of the chairs in the hallway and waits. If no chairs are available, the student will come back at a later time.
 *     Using POSIX threads, mutex locks, and semaphores, implement a solution that coordinates the activities of the TA and the students.
 *     Details for this assignment are provided below.
 *
 *     The Students and the TA
 *
 *     Using Pthreads begin by creating n students. Each will run as a separate thread. The TA will run as a separate thread as well.
 *     Student threads will alternate between programming for a period of time and seeking help from the TA. If the TA is available, they will obtain help.
 *     Otherwise, they will either sit in a chair in the hallway or, if no chairs are available, will resume programming and will seek help at a later time. If a
 *     student arrives and notices that the TA is sleeping, the student must notify the TA using a semaphore. When the TA finishes helping a student, the TA
 *     must check to see if there are students waiting for help in the hallway. If so, the TA must help each of these students in turn. If no students are present,
 *     the TA may return to napping.
 *
 */

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


