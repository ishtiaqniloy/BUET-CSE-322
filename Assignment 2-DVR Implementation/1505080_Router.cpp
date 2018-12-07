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
#define SENDING_INTERVAL 1

using namespace std;

class RoutingTableEntry{
public:
    unsigned char *destIP;
    unsigned char *nextHop;
    int cost;
    int hop;
    bool up;

    RoutingTableEntry(unsigned char d[4], unsigned char n[4], int c, int h=1, bool u=true){
        destIP = new  unsigned char[4];
        nextHop =  new unsigned char[4];

        for(int i=0; i<4; i++){
            destIP[i] = d[i];
        }

        for(int i=0; i<4; i++){
            nextHop[i] = n[i];
        }

        cost = c;
        hop = h;
        up = u;
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

    int getHop(){
        return hop;
    }

    void setHop(int hop) {
        RoutingTableEntry::hop = hop;
    }

    bool isUp(){
        return up;
    }

    void setUp(bool up) {
        RoutingTableEntry::up = up;
    }

    void printEntry(){
        printf("%d.%d.%d.%d\t", destIP[0], destIP[1], destIP[2], destIP[3]);
        printf("%d.%d.%d.%d\t", nextHop[0], nextHop[1], nextHop[2], nextHop[3]);

        printf("%d\t%d\n",cost, hop);

    }

    bool equals(RoutingTableEntry e){
    	return (destIP[0]==e.destIP[0] && destIP[1]==e.destIP[1] && destIP[2]==e.destIP[2] && destIP[3]==e.destIP[3]); 
    }


};

unsigned char *selfIP;
vector<unsigned char*> routers;
vector<RoutingTableEntry*> routingTable;

void printIP(unsigned char* IP){
	printf("%d.%d.%d.%d", IP[0], IP[1], IP[2], IP[3]);
}

bool checkIPEqual(unsigned char* IP1, unsigned char* IP2){
	return (IP1[0]==IP2[0] && IP1[1]==IP2[1] && IP1[2]==IP2[2] && IP1[3]==IP2[3]);
}

bool routersContains(unsigned char *IP){
	for(int i=0; i<routers.size(); i++){
		unsigned char *IP1 = routers.at(i);
		if(IP1[0]==IP[0] && IP1[1]==IP[1] && IP1[2]==IP[2] && IP1[3]==IP[3]){
			return true;
		}	
	}
	return false;
}

void updateRoutingTable(RoutingTableEntry* newEntry){
	for(int i=0; i<routingTable.size(); i++){
		RoutingTableEntry *entry = routingTable.at(i);
		if(checkIPEqual(entry->getDestIP(), newEntry->getDestIP()) && entry->getCost() > newEntry->getCost()){
			routingTable[i] = newEntry;
			printf("Updating Routing Table: ");
			newEntry->printEntry();
			return;
		}
	}
	routingTable.push_back(newEntry);

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


int main(int argc, char *argv[]){	//argv[1] = ip, argv[2] = topo.txt

	selfIP = new unsigned char[4];
    sscanf(argv[1], "%hhu.%hhu.%hhu.%hhu", &selfIP[0], &selfIP[1], &selfIP[2], &selfIP[3]);
    printf("Using IP: %d.%d.%d.%d\n", selfIP[0], selfIP[1], selfIP[2], selfIP[3]);
 

	FILE *topo;
    topo= fopen(argv[2], "r");
    if(topo==NULL){
            printf("ERROR OPENING FILE\n");
        exit(0);
    }

       
    char line[100];
    unsigned char* IP1 = new unsigned char[4];
    unsigned char* IP2 = new unsigned char[4];
    int cost;

    while(fgets(line, 80, topo)){
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
		}
		else if(checkIPEqual(selfIP, IP2) && !checkIPEqual(selfIP, IP1)){
			RoutingTableEntry *newEntry = new RoutingTableEntry(IP1, IP1, cost);
			updateRoutingTable(newEntry);
		}


    }
    printRoutingTable();

    clock_t lastSend = clock();
    while(true){
    	if( (clock()-lastSend)/CLOCKS_PER_SEC > SENDING_INTERVAL){
    		lastSend = clock();
    		printf("SENDING...\n");
    	}

    }




	// int sockfd;
	// int bind_flag;
	// char buffer[1024];
	// struct sockaddr_in server_address;
	// struct sockaddr_in client_address;

	// //gets(buffer);

	// if(argc != 2){
	// 	printf("%s <ip address>\n", argv[0]);
	// 	exit(1);
	// }

	// server_address.sin_family = AF_INET;
	// server_address.sin_port = htons(4747);
	// server_address.sin_addr.s_addr = inet_addr("192.168.10.100");

	// client_address.sin_family = AF_INET;
	// client_address.sin_port = htons(4747);
	// client_address.sin_addr.s_addr = inet_addr(argv[1]);

	// sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	// bind_flag = bind(sockfd, (struct sockaddr*) &client_address, sizeof(sockaddr_in));

	// while(true){
	// 	//gets(buffer);
	// 	cin >> buffer;

	// 	if(!strcmp(buffer, "shutdown")) break;
	// 	sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));
	// }

	// close(sockfd);

	return 0;

}
