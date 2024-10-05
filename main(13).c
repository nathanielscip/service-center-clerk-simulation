/* COP 3502C Assignment 2 
This program is written by: Nathaniel Scipio */

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

#define EMPTY -1

typedef struct customer
{
  char name[50];
  int services;
  int lineNum;
  int ltime;
} customer;

typedef struct node
{
  customer *data;
  struct node *next;
} node;

typedef struct queue
{
  struct node *front;
  struct node *back;
} queue;

int isEmpty(queue* qPtr) 
{
  if (qPtr->front == NULL)
    return 1;

  else
     return 0;
}

customer *peek(queue* qPtr)
{

  if(!isEmpty(qPtr)) // uses isEmpty function to ensure that illegal memory is not being retreived
  {
    customer *retValue = qPtr->front->data;
    return retValue;
  }

  else
    return NULL;
      
}

// initializes every queue by setting front and back to null
queue *init(queue* qPtr) 
{
  qPtr = (queue*)malloc(sizeof(queue));
  qPtr->front = NULL;
  qPtr->back = NULL; 
  
  return qPtr;
}

int enqueue(queue* qPtr, customer *data)
{
  node* temp = malloc(sizeof(node));

  if (temp != NULL)
  {
    temp->data = data;
    temp->next = NULL;
    
    if(qPtr->back != NULL)
     qPtr->back->next = temp;

    qPtr->back = temp;

    if(qPtr->front == NULL)
      qPtr->front = temp;

    return 1;
  }

  return 0;
  
}

customer *dequeue(queue *qPtr)
{
  customer *nextCust; // variable used to retrieve the customer that's next up in the queue

  if(isEmpty(qPtr))
    return NULL;

  nextCust = qPtr->front->data; // saves data from front before freeing it

  node* temp = qPtr->front;

  qPtr->front = qPtr->front->next;

  free(temp);

  if(qPtr->front == NULL)
    qPtr->back = NULL;

  return nextCust;

  
}

// function creates a new customer pointer whenever it is called
customer *makeCustomer()
{
  customer *c = (customer*)malloc(sizeof(customer));

  
  char name[50];
  int services, ltime, line;
  scanf("%s %d %d %d", name, &services, &ltime, &line);
  strcpy(c->name, name);

  c->services = services;
  c->ltime = ltime;
  c->lineNum = line;

  return c;
 
}
// function performs the calculation required to check how long each customer takes
int timeCalc(customer *c)
{
  int services = c->services;
  int result = 50 + (80 * services);

  return result;
  
}
// function frees all pointers from makeCustomer() function and frees all the queues from the customer array
void freeQueues(queue** qList, customer** cList, int nCustomers)
{

  for (int x = 0; x < nCustomers; x++)
  {
      free(cList[x]);
  }

  free(cList);
  
  for (int i = 0; i < 15; i++)
    {
      node *temp = qList[i]->front;
      node *head;

      while(temp != NULL)
      {
        head = temp->next;
        free(temp);
        temp = head;
      }
      free(qList[i]);
    }
    free(qList);
}


int main()
{
  atexit(report_mem_leak);
  int testcases, customerNum;

  // allocates memory for queue

  scanf("%d", &testcases);

  for (int x = 0; x < testcases; x++)
  {
      scanf("%d", &customerNum);
      queue **customerArr =  (queue**)malloc(15 * sizeof(queue*));
      
      for(int i = 0; i < 15; i++)
      {
        customerArr[i] = init(customerArr[i]); // initializes every queue in array
      }

      customer** addedCust = (customer**)malloc(customerNum * sizeof(customer*));
    
      for (int i = 0; i < customerNum; i++)
      {
        addedCust[i] = makeCustomer(); // makeCustomer function fills addedCust array with customers from input
        enqueue(customerArr[addedCust[i]->lineNum - 1], addedCust[i]); // moves data from customer array to the queue array
      }
    
      int minTime = 1999, serviceNum, curTime = 0;

    // sets the current time of the program to the time of when the earliest customer arrives
      if (curTime == 0) 
      {
          for (int i = 0; i < 15; i++) // runs loop to find smallest time
          {
            if(customerArr[i]->front != NULL)
            {
              if(customerArr[i]->front->data->ltime < minTime)
                  minTime = customerArr[i]->front->data->ltime;
            }
          }   
        
          curTime = minTime;
      }

    
        
    customer* nextCust;
    int nextLine = 1999;

    for(int x = 0; x < customerNum; x++)
    {

      int sChecker = 0, services = 999, sChecker2 = 0, tChecker = 0;
      
      for (int i = 0; i < 15; i++)
      {
        customer *frontCust = peek(customerArr[i]); // uses peek function to retreive data from pointer without removing from queue

        
        if(frontCust!=NULL)
        {
          if(frontCust->ltime <= curTime)
          {
              
            tChecker = 1; // tChecker will be assigned a 1 if a customer is found to have arrived earlier than curTime
            //
            if(frontCust->services < services)
            {
              services = frontCust->services;
              nextLine = (frontCust->lineNum - 1);
              sChecker = 1;  //sChecker will be assigned a 1 if a customer is found to have the least amount of services
            }

            if (frontCust->services == services)
            {
              if (frontCust->ltime == minTime) // condition makes sure that the customer with the earliest time goes first even if their line number is larger
              {
                nextLine = (frontCust->lineNum - 1);
                sChecker = 1;
              }
              
              else if ((frontCust->lineNum-1) < nextLine) // checks if customer with same number of services as another customer has a smaller line number
              {
                nextLine = (frontCust->lineNum - 1);
                sChecker = 1;
              }

            }
            else if(frontCust->services > services && sChecker == 0) // if sChecker is assigned with 1 this statement will not be checked since customer does not have the least amount of services.
            {
              services = frontCust->services;
              nextLine = (frontCust->lineNum - 1);
            }
            
          }
          
          else if(frontCust->ltime > curTime && tChecker == 0) // if tChecker is assigned with 1 this statement will not be checked since customers arrived after curTime
          {
            
            if(frontCust->services < services)
            {
              services = frontCust->services;
              nextLine = (frontCust->lineNum - 1);
              sChecker2 = 1;
            }

            if (frontCust->services == services)
            {
              if ((frontCust->lineNum-1) < nextLine)
              {
                nextLine = (frontCust->lineNum - 1);
                sChecker2 = 1;
                
              }
                
            }
            else if(frontCust->services > services && sChecker2 == 0)
            {
              services = frontCust->services;
              nextLine = (frontCust->lineNum - 1);
            }
            
          }
        }

      }
      
      nextCust = dequeue(customerArr[nextLine]); // retrieves and removes data from customerArray. data is now in nextCust variable
      
      int addedTime;
      if(nextCust != NULL)
      {  
        if(nextCust->ltime > curTime) // checks if the customer came after curTime. if so, the curTime gets changed to the time when the customer arrives
        {
            curTime = nextCust->ltime;
            addedTime = timeCalc(nextCust);
        }
        
        else
            addedTime = timeCalc(nextCust);
        
        curTime += addedTime; // result from calculation gets added to curTime
        
        printf("%s checks out at %d from line %d.\n", nextCust->name, curTime, nextCust->lineNum);
      }   
    }
    freeQueues(customerArr, addedCust, customerNum);
  }
}




