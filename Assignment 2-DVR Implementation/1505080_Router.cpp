#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define INFINITY 99999999
#define MAX_DOWN 3

using namespace std;

class RoutingTableEntry{
    unsigned char *destIP;
    unsigned char *nextHop;
    int cost;

public:
    RoutingTableEntry(unsigned char d[4], unsigned char n[4], int c){
        destIP = new  unsigned char[4];
        nextHop =  new unsigned char[4];

        for(int i=0; i<4; i++){
            destIP[i] = d[i];
        }

        for(int i=0; i<4; i++){
            nextHop[i] = n[i];
        }

        cost = c;
    }

    ~RoutingTableEntry(){
        delete []destIP;
        delete []nextHop;
    }

    unsigned char *getDestIP(){
        return destIP;
    }

    void setDestIP(unsigned char *d){
        for(int i=0; i<4; i++){
            destIP[i] = d[i];
        }
    }

    unsigned char *getNextHop(){
        return nextHop;
    }

    void setNextHop(unsigned char *n){
        for(int i=0; i<4; i++){
            nextHop[i] = n[i];
        }
    }

    int getCost(){
        return cost;
    }

    void setCost(int c) {
        RoutingTableEntry::cost = c;
    }

    void printEntry(){
        printf("%d.%d.%d.%d\t", destIP[0], destIP[1], destIP[2], destIP[3]);
        printf("%d.%d.%d.%d\t", nextHop[0], nextHop[1], nextHop[2], nextHop[3]);

        printf("%d\n",cost);

    }

    bool equals(RoutingTableEntry e){
    	return (destIP[0]==e.destIP[0] && destIP[1]==e.destIP[1] && destIP[2]==e.destIP[2] && destIP[3]==e.destIP[3]); 
    }


};

FILE *topo;
unsigned char *selfIP;
char *selfIPStr;

vector<unsigned char*> routers;

vector<unsigned char*> neighbours;
vector<int> linkCost;
vector<bool> linkUp;
vector<int> linkCount;

vector<RoutingTableEntry*> routingTable;

void printIP(unsigned char* IP){
	printf("%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
}

char *getIPString(unsigned char* IP){
	char *str = new char[30];
	sprintf(str, "%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);

	return str;
}

bool checkIPEqual(unsigned char* IP1, unsigned char* IP2){
	return (IP1[0]==IP2[0] && IP1[1]==IP2[1] && IP1[2]==IP2[2] && IP1[3]==IP2[3]);
}

bool routersContains(unsigned char *IP){
	for(int i=0; i<routers.size(); i++){
		unsigned char *IP1 = routers.at(i);
		if(checkIPEqual(IP1, IP)){
			return true;
		}	
	}
	return false;
}

bool neighboursContains(unsigned char *IP){
	for(int i=0; i<neighbours.size(); i++){
		unsigned char *IP1 = neighbours.at(i);
		if(checkIPEqual(IP1, IP)){
			return true;
		}	
	}
	return false;
}

int getNeighbourIndex(unsigned char *IP){
	for(int i=0; i<neighbours.size(); i++){
		unsigned char *IP1 = neighbours.at(i);
		if(checkIPEqual(IP1, IP)){
			return i;
		}	
	}
	return -1;
}

int getLinkCost(unsigned char *IP){
	int idx = getNeighbourIndex(IP);
	return linkCost.at(idx);
}

bool getLinkUp(unsigned char *IP){
	int idx = getNeighbourIndex(IP);
	return linkUp.at(idx);
}

void increaseLinkCount(){
	for(int i=0; i<linkCount.size(); i++){
		linkCount[i] = linkCount[i]+1;
	}
}



void printRouters(){
	printf("Routers of this topology : ");
	for(int i=0; i<routers.size(); i++){
		unsigned char *IP1 = routers.at(i);
		printIP(IP1);
		printf(", ");	
	}
	printf("\n");
}

void printNeighbours(){
	printf("Neighbours of %s: ", selfIPStr);
	for(int i=0; i<neighbours.size(); i++){
		unsigned char *IP1 = neighbours.at(i);
		printIP(IP1);
		printf(", ");	
	}
	printf("\n");
}

RoutingTableEntry *findEntry(unsigned char *IP){
	for(int i=0; i<routingTable.size(); i++){
		RoutingTableEntry *entry = routingTable.at(i);
		if(checkIPEqual(IP, entry->getDestIP())){
			return entry;
		}		
	}
	return NULL;
}

void checkDown(){
	for(int i=0; i<linkCount.size(); i++){
		if(linkCount[i]>=MAX_DOWN){
			linkUp[i] = false;

			unsigned char *ip = neighbours[i];
			RoutingTableEntry *entry = findEntry(ip);

			if(checkIPEqual(ip, entry->getNextHop())){
				entry->setCost(INFINITY);
			}

		}
	}
}

void updateRoutingTable(RoutingTableEntry* newEntry){

	RoutingTableEntry *entry = findEntry(newEntry->getDestIP());
	if(entry!=NULL){
		if(entry->getCost() > newEntry->getCost()){
			entry->setNextHop(newEntry->getNextHop());
			entry->setCost(newEntry->getCost());

			printf("Updating Routing Table: ");
			entry->printEntry();

			delete newEntry;
			
		}
	}
	else{
		routingTable.push_back(newEntry);
		printf("New Routing Table entry: ");
		newEntry->printEntry();
	}

}

int findCost(unsigned char *IP){
	RoutingTableEntry *entry = findEntry(IP);
	if(entry==NULL){
		return INFINITY;
	}
	else{
		return entry->getCost();
	}
}

void printRoutingTable(){
	printf("\n***ROUTING TABLE OF IP : ");
	printIP(selfIP);
	printf("***\n");
	for(int i=0; i<routingTable.size(); i++){
		RoutingTableEntry *entry = routingTable.at(i);	
		entry->printEntry();
	}

}

bool buildTopology(){
	char line[100];
    int cost;

    while(fgets(line, 80, topo)){
    	unsigned char* IP1 = new unsigned char[4];
    	unsigned char* IP2 = new unsigned char[4];

    	printf("TOPOLOGY: %s\n", line);
 		sscanf(line, "%hhu.%hhu.%hhu.%hhu %hhu.%hhu.%hhu.%hhu  %d", &IP1[0], &IP1[1], &IP1[2], &IP1[3], &IP2[0], &IP2[1], &IP2[2], &IP2[3], &cost);
		
		if(!routersContains(IP1) && !checkIPEqual(selfIP, IP1)){
			routers.push_back(IP1);
		}
		if(!routersContains(IP2) && !checkIPEqual(selfIP, IP2)){
			routers.push_back(IP2);
		}

		if(checkIPEqual(selfIP, IP1) && !checkIPEqual(selfIP, IP2)){
			RoutingTableEntry *newEntry = new RoutingTableEntry(IP2, IP2, cost);
			updateRoutingTable(newEntry);

			if(!neighboursContains(IP2)){
				neighbours.push_back(IP2);
				linkUp.push_back(true);
				linkCost.push_back(cost);
				linkCount.push_back(0);
			}

		}
		else if(checkIPEqual(selfIP, IP2) && !checkIPEqual(selfIP, IP1)){
			RoutingTableEntry *newEntry = new RoutingTableEntry(IP1, IP1, cost);
			updateRoutingTable(newEntry);

			if(!neighboursContains(IP1)){
				neighbours.push_back(IP1);
				linkUp.push_back(true);
				linkCost.push_back(cost);
				linkCount.push_back(0);
			}

		}


    }
}

void sendRoutingTable(){
	int sockfd;
	int bind_flag;
	char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(4747);

		client_address.sin_family = AF_INET;
		client_address.sin_port = htons(4747);
		client_address.sin_addr.s_addr = inet_addr(selfIPStr);

	for(int i=0; i<neighbours.size(); i++){
		unsigned char *n = neighbours.at(i);

		char *neighbourStr = getIPString(n);

		bool canSend = linkUp[i];
		if(!canSend){
			continue;
		}

		//printf("Sending to : %s\n", getIPString(n));

		server_address.sin_addr.s_addr = inet_addr(neighbourStr);

		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));


		buffer[0] = 'R';
		buffer[1] = 'T';
		buffer[2] = ' ';	

		buffer[3] = selfIP[0];
		buffer[4] = selfIP[1];
		buffer[5] = selfIP[2];
		buffer[6] = selfIP[3];

		for(int j=0; j<routingTable.size(); j++){
			RoutingTableEntry *entry = routingTable.at(j);
			
			//entry->printEntry();

			unsigned char *destIP = entry->getDestIP();

			// if(checkIPEqual(n, destIP)){
			// 	continue;
			// }

			buffer[7] = destIP[0];
			buffer[8] = destIP[1];
			buffer[9] = destIP[2];
			buffer[10] = destIP[3];

			buffer[11] = ' ';
			buffer[12] = 0;

			sprintf(buffer+12, "%d", entry->getCost());

			sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

		}


		close(sockfd);
	}


}



bool receiveRoutingEntry(char *buffer){
	 if(buffer[0]!='R' || buffer[1]!='T'){
	 	return false;
	 }

	 unsigned char *n = new unsigned char[5];
	 unsigned char *d = new unsigned char[5];

	 n[0] = buffer[3];
	 n[1] =	buffer[4];
	 n[2] = buffer[5];
	 n[3] = buffer[6];

	 d[0] = buffer[7];
	 d[1] =	buffer[8];
	 d[2] = buffer[9];
	 d[3] = buffer[10];

	 int cost;
	 sscanf(buffer+12, "%d", &cost);

	 char *nStr = getIPString(n);
	 char *dStr = getIPString(d);

	 if(!neighboursContains(n)){
	 	printf("Wrong incoming transmission\n");
	 	return false;
	 }

	 int idx = getNeighbourIndex(n);
	 linkUp[idx] = true;
	 linkCount[idx] = 0;

	 //printf("Received Entry form %s : %s %d\n", nStr, dStr, cost);

	 RoutingTableEntry *destEntry = findEntry(d);
	 RoutingTableEntry *neighbourEntry = findEntry(n);

	 int newCost = cost+getLinkCost(n);
	 if(checkIPEqual(d, selfIP)){	//neighbour
	 	if(neighbourEntry->getCost() >= linkCost[idx]){
	 		neighbourEntry->setCost(linkCost[idx]);
	 		neighbourEntry->setNextHop(n);
	 	}

	 }
	 else if(destEntry == NULL){	//new destination
	 	destEntry = new RoutingTableEntry(d, n, newCost);
	 	routingTable.push_back(destEntry);

	 	//updateRoutingTable(destEntry);

		printf("Found path to new IP : %s\n", dStr);
	 	printRoutingTable();
	 } 
	 else if(checkIPEqual(n, destEntry->getNextHop()) && destEntry->getCost()!=newCost){	//update cost
	 	destEntry->setCost(newCost);

	 	printf("Updated cost to IP : %s through : %s\n", dStr, nStr);
	 	printRoutingTable();
	 }
	 else if(destEntry->getCost() > newCost){	//new path, less cost
	 	destEntry->setNextHop(n);
	 	destEntry->setCost(newCost);

	 	printf("Updated path to IP : %s through : %s\n", dStr, nStr);
	 	printRoutingTable();

	 }

	 return true;



}

bool updateCost(char *buffer){
	 unsigned char *IP1 = new unsigned char[5];
	 unsigned char *IP2 = new unsigned char[5];

	 IP1[0] = buffer[4];
	 IP1[1] = buffer[5];
	 IP1[2] = buffer[6];
	 IP1[3] = buffer[7];

	 IP2[0] = buffer[8];
	 IP2[1] = buffer[9];
	 IP2[2] = buffer[10];
	 IP2[3] = buffer[11];

	 int cost = (int)buffer[12];

	 char *IP1Str = getIPString(IP1);
	 char *IP2Str = getIPString(IP2);

	 printf("Cost change %s %s %d\n", IP1Str, IP2Str, cost);

	 if(checkIPEqual(selfIP, IP1) && !checkIPEqual(selfIP, IP2) && neighboursContains(IP2)){
	 	int idx = getNeighbourIndex(IP2);
	 	linkCost[idx] = cost;	

	 	RoutingTableEntry *entry = findEntry(IP2);

	 	if(checkIPEqual(entry->getNextHop(), IP2)){ //directly
	 		entry->setCost(cost);
	 	}
	 	else if(entry->getCost() >= cost){	//through other path previously
	 		entry->setNextHop(IP2);
	 		entry->setCost(cost);
	 	}


	 }
	 else if(checkIPEqual(selfIP, IP2) && !checkIPEqual(selfIP, IP1) && neighboursContains(IP1)){
		int idx = getNeighbourIndex(IP1);
	 	linkCost[idx] = cost;	

	 	RoutingTableEntry *entry = findEntry(IP1);

	 	if(checkIPEqual(entry->getNextHop(), IP1)){ //directly
	 		entry->setCost(cost);
	 	}
	 	else if(entry->getCost() >= cost){	//through other path previously
	 		entry->setNextHop(IP1);
	 		entry->setCost(cost);
	 	}
	 }
	 else{
	 	printf("Invalid IP\n");
	 }

	 
	 printRoutingTable();
}

bool sendMessage(char *buffer){
	 unsigned char *src = new unsigned char[5];
	 unsigned char *dest = new unsigned char[5];

	 src[0] = buffer[4];
	 src[1] = buffer[5];
	 src[2] = buffer[6];
	 src[3] = buffer[7];

	 dest[0] = buffer[8];
	 dest[1] = buffer[9];
	 dest[2] = buffer[10];
	 dest[3] = buffer[11];

	 char *srcStr = getIPString(src);
	 char *destStr = getIPString(dest);

	 char *msg = new char[200];

	 sscanf(buffer+14, "%s", msg);

	 //printf("SEND src = %s, dest = %s, msg = %s\n", srcStr, destStr, msg);


	 buffer[0]='f';
	 buffer[1]='w';
	 buffer[2]='d';
	 buffer[3] = ' ';

	RoutingTableEntry *entry = findEntry(dest);
	if(entry==NULL || entry->getCost() >=INFINITY){
		printf("MSG CANNOT BE SENT\n");
		return false;
	}

	unsigned char *nextHop = entry->getNextHop();
	char *nextHopStr = getIPString(nextHop);

	//printf("HEHEHEHEHEHEHEHHEHEHEHHEHEHEHEHHEHEHEHEHEHEHEHHE\n");


	 int sockfd;
	int bind_flag;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	server_address.sin_addr.s_addr = inet_addr(nextHopStr);

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(4747);
	client_address.sin_addr.s_addr = inet_addr(selfIPStr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

	close(sockfd);

	printf("%s packet forwarded to %s (printed by %s)\n", msg, nextHopStr, selfIPStr);

	return true;


}

bool forwardedMessage(char *buffer){
	 unsigned char *src = new unsigned char[5];
	 unsigned char *dest = new unsigned char[5];

	 src[0] = buffer[4];
	 src[1] = buffer[5];
	 src[2] = buffer[6];
	 src[3] = buffer[7];

	 dest[0] = buffer[8];
	 dest[1] = buffer[9];
	 dest[2] = buffer[10];
	 dest[3] = buffer[11];

	 char *srcStr = getIPString(src);
	 char *destStr = getIPString(dest);

	 char *msg = new char[200];

	 sscanf(buffer+14, "%s", msg);

	 //printf("SEND src = %s, dest = %s, msg = %s\n", srcStr, destStr, msg);

	 if(checkIPEqual(dest, selfIP)){
		printf("%s packet reached destination from src = %s (printed by %s)\n", msg, srcStr, selfIPStr);
		return true;
	 }


	 buffer[0]='f';
	 buffer[1]='w';
	 buffer[2]='d';
	 buffer[3] = ' ';

	RoutingTableEntry *entry = findEntry(dest);
	if(entry==NULL || entry->getCost() >=INFINITY){
		printf("MSG CANNOT BE SENT\n");
		return false;
	}

	unsigned char *nextHop = entry->getNextHop();
	char *nextHopStr = getIPString(nextHop);

	int sockfd;
	int bind_flag;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	server_address.sin_addr.s_addr = inet_addr(nextHopStr);

	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(4747);
	client_address.sin_addr.s_addr = inet_addr(selfIPStr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

	close(sockfd);

	printf("%s packet forwarded to %s (printed by %s)\n", msg, nextHopStr, selfIPStr);

	return true;


}


bool receiveInput(){
	//defining server variables

    int sockfd; 
	int bind_flag;
	int bytes_received;
	socklen_t addrlen;
	char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	server_address.sin_addr.s_addr = inet_addr(selfIPStr);

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

    while(true){
    	//printf("Waiting for next input...\n");
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);

		if(buffer[0]=='c' && buffer[1]=='l' &&buffer[2]=='k'){
			//printf("[%s:%d]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
			//printf("Bytes Received : %d\n", bytes_received);

			printf("CLK\n");

			increaseLinkCount();
			sendRoutingTable();
			checkDown();

		}
		else if(buffer[0]=='R' && buffer[1]=='T'){
			receiveRoutingEntry(buffer);
		}
		else if(buffer[0]=='c' && buffer[1]=='o' && buffer[2]=='s' && buffer[3]=='t'){
			updateCost(buffer);

		}
		else if(buffer[0]=='s' && buffer[1]=='e' && buffer[2]=='n' && buffer[3]=='d'){
			sendMessage(buffer);

		}
		else if(buffer[0]=='f' && buffer[1]=='w' && buffer[2]=='d'){
			forwardedMessage(buffer);

		}
		else if(buffer[0]=='s' && buffer[1]=='h' && buffer[2]=='o' && buffer[3]=='w'){
			printRoutingTable();

		}
		else{
			printf("Unknown Command: %s\n", buffer);
		}


	
    }

}

int main(int argc, char *argv[]){	//argv[1] = ip, argv[2] = topo.txt

	selfIPStr = argv[1];
	selfIP = new unsigned char[4];
    sscanf(argv[1], "%hhu.%hhu.%hhu.%hhu", &selfIP[0], &selfIP[1], &selfIP[2], &selfIP[3]);
    printf("Using IP: %d.%d.%d.%d\n", selfIP[0], selfIP[1], selfIP[2], selfIP[3]);
 

    topo= fopen(argv[2], "r");
    if(topo==NULL){
            printf("ERROR OPENING FILE\n");
        exit(0);
    }

       
    buildTopology();

    printRouters();
    printNeighbours();
    printRoutingTable();


    receiveInput();	//in while true


	return 0;

}
