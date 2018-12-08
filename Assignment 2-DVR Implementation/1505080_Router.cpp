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
    bool up;

    int downCount;
public:
    RoutingTableEntry(unsigned char d[4], unsigned char n[4], int c, bool u=true, int dCount=0){
        destIP = new  unsigned char[4];
        nextHop =  new unsigned char[4];

        for(int i=0; i<4; i++){
            destIP[i] = d[i];
        }

        for(int i=0; i<4; i++){
            nextHop[i] = n[i];
        }

        cost = c;
        up = u;
        downCount = dCount;
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

    bool isUp(){
        return up;
    }

    void setUp(bool up) {
        RoutingTableEntry::up = up;
    }

    int getDownCount(){
        return downCount;
    }

    void setDownCount(int d) {
        RoutingTableEntry::downCount = d;
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

void updateRoutingTable(RoutingTableEntry* newEntry){

	RoutingTableEntry *entry = findEntry(newEntry->getDestIP());
	if(entry!=NULL){
		if(entry->getCost() > newEntry->getCost() && newEntry->isUp()){
			entry->setNextHop(newEntry->getNextHop());
			entry->setCost(newEntry->getCost());
			entry->setUp(newEntry->isUp());
			entry->setDownCount(newEntry->getDownCount());

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
		if(!entry->isUp()){
			continue;
		}		
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
			}

		}
		else if(checkIPEqual(selfIP, IP2) && !checkIPEqual(selfIP, IP1)){
			RoutingTableEntry *newEntry = new RoutingTableEntry(IP1, IP1, cost);
			updateRoutingTable(newEntry);

			if(!neighboursContains(IP1)){
				neighbours.push_back(IP1);
				linkUp.push_back(true);
				linkCost.push_back(cost);
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

		printf("Sending to : %s\n", getIPString(n));

		server_address.sin_addr.s_addr = inet_addr(neighbourStr);

		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));


		buffer[0] = 'R';
		buffer[1] = 'T';
		buffer[2] = ' ';	//isUp()

		buffer[3] = selfIP[0];
		buffer[4] = selfIP[1];
		buffer[5] = selfIP[2];
		buffer[6] = selfIP[3];

		for(int j=0; j<routingTable.size(); j++){
			RoutingTableEntry *entry = routingTable.at(j);
			
			//entry->printEntry();

			unsigned char *destIP = entry->getDestIP();

			if(checkIPEqual(n, destIP)){
				continue;
			}

			if(entry->isUp()){
				buffer[2] = 1;
			}
			else{
				buffer[2] = 0;
			}

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

	 bool entryUp = false;
	 if(buffer[2]==1){
	 	entryUp = true;
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

	 printf("Received Entry form %s : %s %d %d\n", nStr, dStr, cost, entryUp);

	 if(!neighboursContains(n)){
	 	printf("Wrong incoming transmission\n");
	 	return false;
	 }
	 else if(checkIPEqual(d, selfIP)){
	 	return false;
	 }

	 RoutingTableEntry *destEntry = findEntry(d);
	 RoutingTableEntry *neighbourEntry = findEntry(n);

	 int newCost = cost+getLinkCost(n);

	 if(destEntry == NULL){	//new destination
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
	 else if(destEntry->getCost() > newCost && entryUp){	//new path, less cost
	 	destEntry->setNextHop(n);
	 	destEntry->setCost(newCost);

	 	printf("Updated path to IP : %s through : %s\n", dStr, nStr);
	 	printRoutingTable();

	 }

	 if(!entryUp){
	 	destEntry->setDownCount(destEntry->getDownCount()+1);

	 	if(destEntry->getDownCount()==MAX_DOWN){
			destEntry->setCost(INFINITY);

	 		printf("Path down to IP : %s through : %s\n", dStr, nStr);
	 		printRoutingTable();
	 	}

	 }
	 else{	
		destEntry->setDownCount(0);
	 }

	 return true;



}

bool UpdateCost(char *buffer){

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
    	printf("Waiting for next input...\n");
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);

		if(buffer[0]=='c' && buffer[1]=='l' &&buffer[2]=='k'){
			printf("[%s:%d]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
			//printf("Bytes Received : %d\n", bytes_received);

			sendRoutingTable();		
		}
		else if(buffer[0]=='R' && buffer[1]=='T'){
			receiveRoutingEntry(buffer);
		}
		else if(buffer[0]=='c' && buffer[1]=='o' && buffer[2]=='s' && buffer[3]=='t'){
			UpdateCost(buffer);

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
