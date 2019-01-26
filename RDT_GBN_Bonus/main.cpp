#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <queue>

#define MSG_SIZE 20
#define TIMEOUT 40
#define TIMEOUT_ACK 25

#define BUFFER_SIZE 8
#define MAX_QUEUE_SIZE 50

#define A 0
#define B 1

using namespace std;

float time = 0.000;

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: SLIGHTLY MODIFIED
 FROM VERSION 1.1 of J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
       are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
       or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
       (although some can be lost).
**********************************************************************/

#define BIDIRECTIONAL 0 /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

/********* Required Classes******************/

/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
struct msg
{
    char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
struct pkt
{
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void starttimer(int AorB, float increment);
void stoptimer(int AorB);
void tolayer3(int AorB, struct pkt packet);
void tolayer5(int AorB, char datasent[20]);

/********* Helping variables and functions ******************/

int A_notInWindow;
bool A_ackedPackets[BUFFER_SIZE];
bool A_sentPackets[BUFFER_SIZE];

float A_sendTime[BUFFER_SIZE];

struct pkt *A_packetBuffer[BUFFER_SIZE];


queue <struct msg *> A_msgQueue;

int B_lastCorrectSeq;
int B_expectedSeq;
bool B_ackTimerSet;

int A_ackReceived;
int A_dropped;
int A_corruptReceived;
int A_timeoutOccurred;
int A_nackReceived;

int B_correctlyReceived;
int B_corruptReceived;
int B_duplicateReceived;


void bufferCopy(char *s, char *t, int n){
    for(int i=0; i<n; i++){
        s[i] = t[i];
    }
}

int calculateCheckSum(int s, int a, char data[]){
    int csm = s+a;

    if(data==NULL){
        return csm;
    }
    for(int i=0; i < MSG_SIZE; i++){
        csm += data[i];
    }

    return csm;
}

bool verifyChecksum(struct pkt *p){
    //printf("In verifyChecksum():  %d %d\n", calculateCheckSum(p->seqnum, p->acknum, p->payload), p->checksum);
    return ( calculateCheckSum(p->seqnum, p->acknum, p->payload) == p->checksum );
}

void A_enqueueMsg(struct msg *message){
    if(A_msgQueue.size()<MAX_QUEUE_SIZE){
        A_msgQueue.push(message);
    }
    else{
        printf("***At %f In A_enqueueMsg(): Max buffer size reached. dropping message : %s***\n", time, message->data);
        A_dropped++;
    }
}

struct pkt* A_makePacket(struct msg *message, int sq){

    struct pkt *tempPacket = new struct pkt;
    tempPacket->seqnum = sq; ///?
    tempPacket->acknum = -1; //not required in unidirectional transmission
    bufferCopy(tempPacket->payload, message->data, MSG_SIZE);
    tempPacket->checksum = calculateCheckSum(tempPacket->seqnum, tempPacket->acknum, tempPacket->payload);


    return tempPacket;

}

void A_addToBuffer(){
    ///base = (A_notInWindow+1)%BUFFER_SIZE
    for (int i = (A_notInWindow+1)%BUFFER_SIZE ; i != A_notInWindow; i = (i+1)%BUFFER_SIZE ) {
        if(A_packetBuffer[i] != NULL){
            continue;
        }
        if(A_msgQueue.empty()){
            break;
        }

        struct msg* message = A_msgQueue.front();
        A_msgQueue.pop();
        struct pkt* newPacket = A_makePacket(message, i);

        A_packetBuffer[i] = newPacket;

    }

}

void A_sendPacket(int idx){
    if(A_packetBuffer[idx]==NULL){
        return;
    }

    printf("***At %f In A_sendPacket(): Sending pkt%d : %s***\n", time, idx, A_packetBuffer[idx]->payload);

    tolayer3(A, *A_packetBuffer[idx]);

    A_sendTime[idx] = time;

    ///base = (A_notInWindow+1)%BUFFER_SIZE
    if((A_notInWindow+1)%BUFFER_SIZE == idx){
        stoptimer(A);
        starttimer(A, TIMEOUT);
    }

    A_ackedPackets[idx] = false;
    A_sentPackets[idx] = true;

}

void A_sendPackets(){
    ///base = (A_notInWindow+1)%BUFFER_SIZE
    for (int i = (A_notInWindow+1)%BUFFER_SIZE ; i != A_notInWindow ; i = (i+1)%BUFFER_SIZE ) {
        if(A_packetBuffer[i] == NULL || A_sentPackets[i]){
            continue;
        }

        A_sendPacket(i);

    }
}

void A_retransmitWindow(){
//    printf("***At %f In A_retransmitWindow(): Retransmitting pkts***\n", time);

    ///base = (A_notInWindow+1)%BUFFER_SIZE
    for (int i = (A_notInWindow+1)%BUFFER_SIZE ; i != A_notInWindow ; i = (i+1)%BUFFER_SIZE ) {
        if(A_packetBuffer[i] == NULL){
            continue;
        }

        A_sendPacket(i);

    }
}

struct pkt* B_makeAckPacket(int ack){

    struct pkt *tempPacket = new struct pkt;
    tempPacket->seqnum = -1;
    tempPacket->acknum = ack; //not required in unidirectional transmission
    for(int i=0; i<MSG_SIZE; i++){
        tempPacket->payload[i] = 0;
    }
    tempPacket->checksum = calculateCheckSum(tempPacket->seqnum, tempPacket->acknum, tempPacket->payload);


    return tempPacket;
}

struct msg *B_extractMsg(struct pkt* packet){
    struct msg *tempMsg = new struct msg;

    bufferCopy(tempMsg->data, packet->payload, MSG_SIZE);

}

void printVariables(void){

    printf("A_ackReceived = %d\n", A_ackReceived);
    printf("A_dropped = %d\n", A_dropped);
    printf("A_corruptReceived = %d\n", A_corruptReceived);
    printf("A_timeoutOccurred = %d\n", A_timeoutOccurred);
    printf("A_nackReceived = %d\n", A_nackReceived);

    printf("B_correctlyReceived = %d\n", B_correctlyReceived);
    printf("B_corruptReceived = %d\n", B_corruptReceived);
    printf("B_duplicateReceived = %d\n", B_duplicateReceived);

}

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(struct msg message){
//    printf("*****At %f In A_output()*****\n", time);

    struct msg* newMessage = new struct msg;
    bufferCopy(newMessage->data, message.data, MSG_SIZE);

    ///if possible enqueue and send message
    A_enqueueMsg(newMessage);
    A_addToBuffer();
    A_sendPackets();


}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(struct pkt packet){
//    printf("*****At %f In A_input()*****\n", time);

    if(!verifyChecksum(&packet)){     ///corrupt packet received. ignore
        printf("***At %f In A_input(): Corrupt packet received***\n", time);
        A_corruptReceived++;
        return;
    }

    int a = packet.acknum;

    if(A_ackedPackets[a]){  ///duplicate ack or NACK
        printf("***At %f In A_input(): NACK received:%d. Doing fast retransmit***\n", time, a);

        A_nackReceived++;
        ///???????
        A_retransmitWindow();

    }
    else if(a != A_notInWindow){ ///ACK in window received
        printf("***At %f In A_input(): ACK received for pkt%d***\n", time, a);


        A_ackReceived++;

        ///base =  (A_notInWindow+1)%BUFFER_SIZE
        for (int i = (A_notInWindow+1)%BUFFER_SIZE ; i != A_notInWindow; i = (i+1)%BUFFER_SIZE ) {
            A_ackedPackets[i] = true;
            A_sentPackets[i] = false;

//            delete  A_packetBuffer[i];
            A_packetBuffer[i] = NULL;

            if(i==a){
                break;
            }

        }

        ///change base
        A_notInWindow = a;

        ///send buffered packets
        A_addToBuffer();
        A_sendPackets();

        stoptimer(A);

        int base = (A_notInWindow+1)%BUFFER_SIZE;
        printf("***At %f In A_input(): BASE at %d***\n", time, base);

        float remTime = TIMEOUT-time+A_sendTime[base];
        if(remTime>0 && A_sentPackets[base] && !A_ackedPackets[base]){
            starttimer(A, remTime);
        }

    }

}

/* called when A's timer goes off */
void A_timerinterrupt(void){
//    printf("*****At %f In A_timerInterrupt()*****\n", time);

    printf("***At %f In A_timerInterrupt(): Timeout occurred. Retransmitting pkts***\n", time);

    //starttimer(A, TIMEOUT); //timeout started in sendPacket() automatically

    A_retransmitWindow();

    A_timeoutOccurred++;


}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(void){
//    printf("*****At %f In A_init()*****\n", time);

    A_notInWindow = BUFFER_SIZE-1;

    for (int i = 0; i < BUFFER_SIZE; i++) {
        A_packetBuffer[i] = NULL;
    }

    for (int i = 0; i < BUFFER_SIZE; i++) {
        A_ackedPackets[i] = true;
    }
    for (int i = 0; i < BUFFER_SIZE; i++) {
        A_sentPackets[i] = false;
    }


    A_ackReceived = 0;
    A_dropped = 0;
    A_corruptReceived = 0;
    A_timeoutOccurred = 0;
    A_nackReceived = 0;

}

/* Note that with simplex transfer from a-to-B, there is no B_output() */
/* need be completed only for extra credit */
void B_output(struct msg message){ ///unused for now
//    printf("*****At %f In B_output()*****\n", time);
}

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(struct pkt packet){
//    printf("*****At %f In B_input()*****\n", time);

    if(!verifyChecksum(&packet)){     ///corrupt packet received. sending nack???
        printf("***At %f In B_input(): Corrupt packet received***\n", time, B_lastCorrectSeq);

//        struct pkt *newPacket = B_makeAckPacket(B_lastCorrectSeq);
//        tolayer3(B, *newPacket);

        if(!B_ackTimerSet){
            starttimer(B, TIMEOUT_ACK);
            B_ackTimerSet = true;
        }

        B_corruptReceived++;

    }
    else if(packet.seqnum == B_expectedSeq){
        printf("***At %f In B_input(): Correct packet received. pkt%d : %s***\n", time, packet.seqnum, packet.payload);

        struct msg *message = B_extractMsg(&packet);
        tolayer5(B, message->data);

//        struct pkt *newPacket = B_makeAckPacket(B_expectedSeq);
//        tolayer3(B, *newPacket);

        if(!B_ackTimerSet){
            starttimer(B, TIMEOUT_ACK);
            B_ackTimerSet = true;
        }


        B_lastCorrectSeq = B_expectedSeq;
        B_expectedSeq = (B_expectedSeq+1)%BUFFER_SIZE;

        B_correctlyReceived++;

    }
    else{
        printf("***At %f In B_input(): Duplicate pkt%d received***\n", time, packet.seqnum, B_lastCorrectSeq);

//        struct pkt *newPacket = B_makeAckPacket(B_lastCorrectSeq);
//        tolayer3(B, *newPacket);
        if(!B_ackTimerSet){
            starttimer(B, TIMEOUT_ACK);
            B_ackTimerSet = true;
        }

        B_duplicateReceived++;
    }


}

/* called when B's timer goes off */
void B_timerinterrupt(void){    ///unused for now
//    printf("*****At %f In B_timerInterrupt()*****\n", time);
//    printf("***At %f In B_timerInterrupt(): B doesn't have a timer. ignore.***\n", time);

    printf("***At %f In B_timerInterrupt(): Sending cumulative ack%d.***\n", time, B_lastCorrectSeq);

    struct pkt *newPacket = B_makeAckPacket(B_lastCorrectSeq);
    tolayer3(B, *newPacket);

    B_ackTimerSet = false;

}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(void){
//    printf("*****At %f In B_init()*****\n", time);

    B_lastCorrectSeq = BUFFER_SIZE-1;
    B_expectedSeq = 0;
    B_ackTimerSet = false;

    B_correctlyReceived = 0;
    B_corruptReceived = 0;
    B_duplicateReceived = 0;
}

/*****************************************************************
***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
    - emulates the tranmission and delivery (possibly with bit-level corruption
        and packet loss) of packets across the layer 3/4 interface
    - handles the starting/stopping of a timer, and generates timer
        interrupts (resulting in calling students timer handler).
    - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
******************************************************************/

struct event
{
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    struct pkt *pktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 0
#define FROM_LAYER5 1
#define FROM_LAYER3 2

#define OFF 0
//#define ON 1
//#define A 0
//#define B 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of messages from 5 to 4 so far */
int nsimmax = 0;   /* number of msgs to generate, then stop */
//float time = 0.000;
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer3;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival(void);
void insertevent(struct event *p);

int main()
{
    struct event *eventptr;
    struct msg msg2give;
    struct pkt pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1)
    {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2)
        {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (eventptr->evtype == FROM_LAYER5)
        {
            if (nsim < nsimmax)
            {
                if (nsim + 1 < nsimmax)
                    generate_next_arrival(); /* set up future arrival */
                /* fill in msg to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < 20; i++)
                    msg2give.data[i] = 97 + j;
                msg2give.data[19] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < 20; i++)
                        printf("%c", msg2give.data[i]);
                    printf("\n");
                }
                nsim++;
                if (eventptr->eventity == A)
                    A_output(msg2give);
                else
                    B_output(msg2give);
            }
        }
        else if (eventptr->evtype == FROM_LAYER3)
        {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < 20; i++)
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give); /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

    terminate:
    printf(
            " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
            time, nsim);

    printVariables();
}

void init() /* initialize the simulator */
{
    int i;
    float sum, avg;
    float jimsrand();

    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d",&nsimmax);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f",&lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f",&corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f",&lambda);
    printf("Enter TRACE:");
    scanf("%d",&TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0;   /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75)
    {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(1);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0;              /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */
/****************************************************************************/
float jimsrand(void)
{
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(void)
{
    double x, log(), ceil();
    struct event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(struct event *p)
{
    struct event *q, *qold;

    if (TRACE > 2)
    {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist;      /* q points to header of list in which p struct inserted */
    if (q == NULL)   /* list is empty */
    {
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    }
    else
    {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL)   /* end of list */
        {
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        }
        else if (q == evlist)     /* front of list */
        {
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist(void)
{
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next)
    {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype,
               q->eventity);
    }
    printf("--------------\n");
}

/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(int AorB /* A or B is trying to stop timer */)
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL;          /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist)   /* front of list - there must be event after */
            {
                q->next->prev = NULL;
                evlist = q->next;
            }
            else     /* middle of list */
            {
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(int AorB /* A or B is trying to start timer */, float increment)
{
    struct event *q;
    struct event *evptr;

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB))
        {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(int AorB, struct pkt packet)
{
    struct pkt *mypktptr;
    struct event *evptr, *q;
    float lastime, x;
    int i;

    ntolayer3++;

    /* simulate losses: */
    if (jimsrand() < lossprob)
    {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (struct pkt *)malloc(sizeof(struct pkt));
    mypktptr->seqnum = packet.seqnum;
    mypktptr->acknum = packet.acknum;
    mypktptr->checksum = packet.checksum;
    for (i = 0; i < 20; i++)
        mypktptr->payload[i] = packet.payload[i];
    if (TRACE > 2)
    {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
               mypktptr->acknum, mypktptr->checksum);
        for (i = 0; i < 20; i++)
            printf("%c", mypktptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *)malloc(sizeof(struct event));
    evptr->evtype = FROM_LAYER3;      /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->pktptr = mypktptr;         /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();

    /* simulate corruption: */
    if (jimsrand() < corruptprob)
    {
        ncorrupt++;
        if ((x = jimsrand()) < .75)
            mypktptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            mypktptr->seqnum = 999999;
        else
            mypktptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer5(int AorB, char datasent[20])
{
    int i;
    if (TRACE > 2)
    {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 20; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }
}
