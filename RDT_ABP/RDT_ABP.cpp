#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>

#include <iostream>
#include <string>
#include <vector>
#include <queue>

#define MSG_SIZE 20
#define TIMEOUT 15

#define A 0
#define B 1

using namespace std;


float time = 0.000;

void bufferCopy(char *s, char *t, int n){
    for(int i=0; i<n; i++){
        s[i] = t[i];
    }
}

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

/* a "Msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */
/* to layer 5 via the students transport level protocol entities.         */
class Msg{
public:
    char *data;

    Msg(){
        data = new char[MSG_SIZE];
    }

    Msg(char *str){
        data = new char[MSG_SIZE];
        if(str!=NULL){
            bufferCopy(data, str, MSG_SIZE);
        }

        delete []str; ///?
        str=NULL;
    }

    Msg(const Msg &msg2){
        if(data!=NULL){
            delete []data;
            data = NULL;
        }
        data = new char[MSG_SIZE];

        if(msg2.data!=NULL){
            bufferCopy(data, msg2.data, MSG_SIZE);
        }

    }

    ~Msg(){
        if(data!=NULL){
            delete []data;
            data = NULL;
        }
    }

};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */
/* students must follow. */
class Pkt{
public:
    int seqNum;
    int ackNum;
    int checksum;
    char *payLoad;

    Pkt(){
        seqNum = -1;
        ackNum = -1;
        checksum = -1;

        payLoad = new char[MSG_SIZE];
    }

    Pkt(int s, int a, int c, char *p){
        seqNum = s;
        ackNum = a;
        checksum = c;

        payLoad = new char[MSG_SIZE];
        if(p!=NULL){
            bufferCopy(payLoad, p, MSG_SIZE);
        }

        delete []p; ///?
        p = NULL;
    }

    Pkt(const Pkt &pkt2){
        if(payLoad!=NULL){
            delete []payLoad;
            payLoad = NULL;
        }
        payLoad = new char[MSG_SIZE];

        if(pkt2.payLoad!=NULL){
            bufferCopy(payLoad, pkt2.payLoad, MSG_SIZE);
        }

        seqNum = pkt2.seqNum;
        ackNum = pkt2.ackNum;
        checksum = pkt2.checksum;

    }

    ~Pkt(){
        if(payLoad!=NULL){
            delete []payLoad;
            payLoad = NULL;
        }
    }

};


/********* Helping variables and functions ******************/
int A_lastSentSeq;
int B_lastCorrectSeq;
bool A_isLastPacketAcked;
Pkt *A_lastPacket;

int calclateCheckSum(int s, int a, char *data){
    int csm = s+a;

    if(data==NULL){
        return csm;
    }
    for(int i=0; i < MSG_SIZE; i++){
        csm = data[i];
    }

    return csm;
}

bool verifyChecksum(Pkt *p){
    return ( calclateCheckSum(p->seqNum, p->ackNum, p->payLoad) == p->checksum );
}

Pkt* A_makePacket(Msg *msg){
    int s = A_lastSentSeq^1;
    int a = -1; //not required in unidirectional transmission
    char *p = msg->data;
    int c = calclateCheckSum(s, a, p);

    return new Pkt(s, a, c, p);

}

Pkt* B_makeAckPacket(int ack){
    int s = -1;
    int a = ack; //not required in unidirectional transmission
    char *p = new char[MSG_SIZE];
    int c = calclateCheckSum(s, a, p);

    return new Pkt(s, a, c, p);
}

Msg *B_extractMsg(Pkt *pkt){
    return new Msg(pkt->payLoad);
}


/********* FUNCTION PROTOTYPES. DEFINED IN THE LATER PART******************/
void startTimer(int AorB, float increment);
void stopTimer(int AorB);
void toLayer3(int AorB, Pkt packet);
void toLayer5(int AorB, char *dataSent);

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/

/* called from layer 5, passed the data to be sent to other side */
void A_output(Msg message){
    printf("*****At %f In A_output()*****\n", time);

    if(!A_isLastPacketAcked){
        printf("***At %f In A_output(): Last packet not acked yet. dropping message : %s***\n", time, message.data);
        return; ///dropped
    }

    Pkt *pkt = A_makePacket(&message);

    printf("***At %f In A_output(): Sending pkt%d : %s***\n", time, pkt->seqNum, pkt->payLoad);

    toLayer3(A, *pkt);
    A_lastSentSeq ^= 1;
    A_isLastPacketAcked = false;

    if(A_lastPacket!=NULL){
        delete A_lastPacket;
    }
    A_lastPacket = pkt;

    ///stopTimer(A);   ///?
    startTimer(A, TIMEOUT);


}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(Pkt packet){
    printf("*****At %f In A_input()*****\n", time);

    if(!verifyChecksum(&packet)){     //corrupt packet received
        printf("***At %f In A_input(): Corrupt packet received***\n", time);
        return;
    }

    int a = packet.ackNum;
    if(a == A_lastSentSeq){ //last packet ACK received
        printf("***At %f In A_input(): ACK received for pkt%d***\n", time, a);
        A_isLastPacketAcked = true;
        stopTimer(A);
    }
    else{   ///NACK received
        printf("***At %f In A_input(): NACK received for pkt%d***\n", time, a^1);

        toLayer3(A, *A_lastPacket);

        stopTimer(A);   ///?
        startTimer(A, TIMEOUT);

    }


}

/* called when A's timer goes off */
void A_timerInterrupt(){
    printf("*****At %f In A_timerInterrupt()*****\n", time);

    printf("***At %f In A_timerInterrupt(): retransmitting pkt%d***\n", time, A_lastSentSeq);

    toLayer3(A, *A_lastPacket);

    ///stopTimer(A);   ///?
    startTimer(A, TIMEOUT);

}

/* the following routine will be called once (only) before any other */
/* entity A routines are called. You can use it to do any initialization */
void A_init(){
    printf("*****At %f In A_init()*****\n", time);

    A_lastSentSeq = 1;
    A_isLastPacketAcked = true;
    A_lastPacket = NULL;


}

/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* need be completed only for extra credit */
void B_output(Msg message){     ///unused for now
    printf("*****At %f In B_output()*****\n", time);

}


/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(Pkt packet){
    printf("*****At %f In B_input()*****\n", time);

    if(!verifyChecksum(&packet)){     //corrupt packet received
        printf("***At %f In B_input(): Corrupt packet received, sending NACK***\n", time);

        Pkt *pkt = B_makeAckPacket(B_lastCorrectSeq);
        toLayer3(B, *pkt);

        return;
    }

    if(packet.seqNum == B_lastCorrectSeq){
        printf("***At %f In B_input(): Duplicate packet received, sending ACK again***\n", time);

        Pkt *pkt = B_makeAckPacket(B_lastCorrectSeq);
        toLayer3(B, *pkt);

        return;
    }
    else{
        printf("***At %f In B_input(): Correct packet received, pkt%d : %s***\n", time, packet.seqNum, packet.payLoad);

        Msg *msg = B_extractMsg(&packet);
        toLayer5(B, msg->data);

        B_lastCorrectSeq ^= 1;

        Pkt *pkt = B_makeAckPacket(B_lastCorrectSeq);
        toLayer3(B, *pkt);

        return;

    }


}

/* called when B's timer goes off */
void B_timerInterrupt(){        ///unused for now
    printf("*****At %f In B_timerInterrupt()*****\n", time);
    printf("***At %f In B_timerInterrupt(): B doesn't have a timer. ignore.***\n", time);
}

/* the following routine will be called once (only) before any other */
/* entity B routines are called. You can use it to do any initialization */
void B_init(){
    printf("*****At %f In B_init()*****\n", time);

    B_lastCorrectSeq = 1;

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

class event{
public:
    float evtime;       /* event time */
    int evtype;         /* event type code */
    int eventity;       /* entity where event occurs */
    Pkt *pktptr; /* ptr to packet (if any) assoc w/ this event */
    event *prev;
    event *next;
};
event *evlist = NULL; /* the event list */

/* possible events: */
#define TIMER_INTERRUPT 3
#define FROM_LAYER5 1
#define FROM_LAYER3 2

#define OFF 0
#define ON 1

int TRACE = 1;     /* for my debugging */
int nsim = 0;      /* number of messages from 5 to 4 so far */
int nsimmax = 0;   /* number of msgs to generate, then stop */
float lossprob;    /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda;      /* arrival rate of messages from layer 5 */
int ntolayer3;     /* number sent into layer 3 */
int nlost;         /* number lost in media */
int ncorrupt;      /* number corrupted by media*/

void init();
void generate_next_arrival();
void insertevent(event *p);

int main(){

    event *eventptr;
    Msg msg2give;
    Pkt pkt2give;

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
                /* fill in Msg to give with string of same letter */
                j = nsim % 26;
                for (i = 0; i < MSG_SIZE; i++)
                    msg2give.data[i] = 'a' + j;
                msg2give.data[MSG_SIZE-1] = 0;
                if (TRACE > 2)
                {
                    printf("          MAINLOOP: data given to student: ");
                    for (i = 0; i < MSG_SIZE; i++)
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
            pkt2give.seqNum = eventptr->pktptr->seqNum;
            pkt2give.ackNum = eventptr->pktptr->ackNum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < MSG_SIZE; i++)
                pkt2give.payLoad[i] = eventptr->pktptr->payLoad[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give); /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        }
        else if (eventptr->evtype == TIMER_INTERRUPT)
        {
            if (eventptr->eventity == A)
                A_timerInterrupt();
            else
                B_timerInterrupt();
        }
        else
        {
            printf("INTERNAL PANIC: unknown event type \n");

            //printf("***At %f In main(): CHECKING EVENT:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, eventptr->evtime, eventptr->evtype,  eventptr->eventity);


        }
        free(eventptr);
    }

    terminate:
    printf(
            " Simulator terminated at time %f\n after sending %d msgs from layer5\n",
            time, nsim);
}

void init() /* initialize the simulator */{
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
float jimsrand(){
    double mmm = RAND_MAX;
    float x;                 /* individual students may need to change mmm */
    x = rand() / mmm;        /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */
/*****************************************************/

void generate_next_arrival(){
    double x, log(), ceil();
    event *evptr;
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand() * 2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = new event();
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(event *p){
    printf("*****At %f In insertevent()*****\n", time);
    //printf("***At %f In insertevent(): CHECKING EVENT:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, p->evtime, p->evtype,  p->eventity);


    event *q, *qold;

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

           // printf("***At %f In insertevent(): CHECKING1***\n", time);
            printf("***At %f In CHECKING21:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, p->evtime, p->evtype,  p->eventity);

        }
        else if (q == evlist)     /* front of list */
        {   ///???????????????????????????????????????????????????

            printf("***At %f In CHECKING2:\nprev evlist=%d ***\n", time, evlist);

            p->next = q;
            p->prev = NULL;
            q->prev = p; ///?
            evlist = p;


           // printf("***At %f In insertevent(): CHECKING2***\n", time);
            printf("***At %f In CHECKING2:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, p->evtime, p->evtype,  p->eventity);
            printf("p->next = %d, evlist = %d, p = %d\n", p->next, evlist, p);

            printf("p->next %d: evtime=%f, evtype=%d, eventity=%d ***\n",p->next, p->next->evtime, p->next->evtype,  p->next->eventity);

            printf("At %f In CHECKING2() End: PRINTING WHOLE Event Queue\n", time);

            for (q = evlist; q != NULL; q = q->next)
                printf("***evtime=%f, evtype=%d, eventity=%d ***\n", q->evtime, q->evtype,  q->eventity);

        }
        else     /* middle of list */
        {
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;

           // printf("***At %f In insertevent(): CHECKING3***\n", time);
            printf("***At %f In CHECKING3:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, p->evtime, p->evtype,  p->eventity);

        }
    }

    printf("***At %f In insertevent() End: CHECKING EVENT:\nevtime=%f, evtype=%d, eventity=%d ***\n", time, p->evtime, p->evtype,  p->eventity);

    printf("At %f In insertevent() End: PRINTING WHOLE Event Queue\n", time);

    for (q = evlist; q != NULL; q = q->next)
        printf("***evtime=%f, evtype=%d, eventity=%d ***\n", q->evtime, q->evtype,  q->eventity);

}

void printevlist(){
    event *q;
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
void stopTimer(int AorB /* A or B is trying to stop timer */){
    printf("*****At %f In stopTimer()*****\n", time);
    printf("***At %f In stopTimer(): A/B=%d***\n", time, AorB);

    event *q, *qold;

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

void startTimer(int AorB /* A or B is trying to start timer */, float increment){
    printf("*****At %f In startTimer()*****\n", time);
    printf("***At %f In startTimer(): A/B=%d***\n", time, AorB);

    event *q;
    event *evptr;

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
    evptr = new event();
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;

    //printf("***At %f In startTimer(): CHECKING EVENT:\n A/B=%d, evtime=%f, evtype=%d, eventity=%d ***\n", time, AorB, evptr->evtime, evptr->evtype,  evptr->eventity);

    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void toLayer3(int AorB, Pkt packet){
    printf("*****At %f In toLayer3()*****\n", time);
    printf("***At %f In toLayer3(): A/B=%d, data = %s***\n", time, AorB, packet.payLoad);

    Pkt *mypktptr;
    event *evptr, *q;
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

    //printf("***At %f In toLayer3(): A/B=%d, Check1***\n", time, AorB);


    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = new Pkt();
    mypktptr->seqNum = packet.seqNum;
    mypktptr->ackNum = packet.ackNum;
    mypktptr->checksum = packet.checksum;

    //printf("***At %f In toLayer3(): A/B=%d, Check1***\n", time, AorB);

    for (i = 0; i < MSG_SIZE; i++)
        mypktptr->payLoad[i] = packet.payLoad[i];

    //printf("***At %f In toLayer3(): A/B=%d, Check2***\n", time, AorB);

    if (TRACE > 2)
    {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqNum,
               mypktptr->ackNum, mypktptr->checksum);
        for (i = 0; i < MSG_SIZE; i++)
            printf("%c", mypktptr->payLoad[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = new event();
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
            mypktptr->payLoad[0] = 'Z'; /* corrupt payLoad */
        else if (x < .875)
            mypktptr->seqNum = 999999;
        else
            mypktptr->ackNum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");

    //printf("***At %f In toLayer3(): A/B=%d, Exiting***\n", time, AorB);
    insertevent(evptr);
}

void toLayer5(int AorB, char *dataSent){
    printf("*****At %f In toLayer5()*****\n", time);
    printf("***At %f In toLayer5(): A/B=%d, data = %s***\n", time, AorB, dataSent);

    int i;
    if (TRACE > 2)
    {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < MSG_SIZE; i++)
            printf("%c", dataSent[i]);
        printf("\n");
    }
}
