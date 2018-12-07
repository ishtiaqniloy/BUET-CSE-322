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
#define SENDING_INTERVAL 5

using namespace std;

class RoutingTableEntry{
public:
    unsigned char *destIP;
    unsigned char *nextHop;
    int cost;
    bool up;

    RoutingTableEntry(unsigned char d[4], unsigned char n[4], int c, bool u=true){
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

    void printEntry(){
        printf("%d.%d.%d.%d\t", destIP[0], destIP[1], destIP[2], destIP[3]);
        printf("%d.%d.%d.%d\t", nextHop[0], nextHop[1], nextHop[2], nextHop[3]);

        printf("%d\n",cost);

    }

    bool equals(RoutingTableEntry e){
    	return (destIP[0]==e.destIP[0] && destIP[1]==e.destIP[1] && destIP[2]==e.destIP[2] && destIP[3]==e.destIP[3]); 
    }


};

unsigned char *selfIP;
char *selfIPStr;
vector<unsigned char*> routers;
vector<unsigned char*> neighbours;
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

bool neighboursContains(unsigned char *IP){
	for(int i=0; i<neighbours.size(); i++){
		unsigned char *IP1 = neighbours.at(i);
		if(IP1[0]==IP[0] && IP1[1]==IP[1] && IP1[2]==IP[2] && IP1[3]==IP[3]){
			return true;
		}	
	}
	return false;
}

void printRouters(){
	printf("Routers of this topology: ");
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

void updateRoutingTable(RoutingTableEntry* newEntry){
	for(int i=0; i<routingTable.size(); i++){
		RoutingTableEntry *entry = routingTable.at(i);
		if(checkIPEqual(entry->getDestIP(), newEntry->getDestIP())){
			if(entry->getCost() > newEntry->getCost()){
				routingTable[i] = newEntry;
				printf("Updating Routing Table: ");
				newEntry->printEntry();
			
			}
			return;
		}
	}
	routingTable.push_back(newEntry);
	printf("Updating Routing Table: ");
	newEntry->printEntry();

}

int findCost(unsigned char *IP){
	for(int i=0; i<routingTable.size(); i++){
		RoutingTableEntry *entry = routingTable.at(i);
		if(checkIPEqual(IP, entry->getDestIP)){
			return entry->getCost();
		}		
	}
	return INFINITY;
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

void sendRoutingTable(){
	int sockfd;
	int bind_flag;
	char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	for(int i=0; i<neighbours.size(); i++){
		server_address.sin_family = AF_INET;
		server_address.sin_port = htons(4747);
		
		char neighbourStr[30];
		unsigned char *n = neighbours.at(i);

		printf("sending to : ");
		printIP(n);
		printf("\n");

		sprintf(neighbourStr, "%d.%d.%d.%d", n[0], n[1], n[2], n[3]);
		server_address.sin_addr.s_addr = inet_addr(neighbourStr);

		client_address.sin_family = AF_INET;
		client_address.sin_port = htons(4747);
		client_address.sin_addr.s_addr = inet_addr(selfIPStr);

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
			RoutingTableEntry *entry = routingTable.at(i);

			unsigned char *destIP = entry->getDestIP();

			buffer[7] = destIP[0];
			buffer[8] = destIP[1];
			buffer[9] = destIP[2];
			buffer[10] = destIP[3];

			buffer[11] = ' ';
			buffer[12] = 0;

			sprintf(buffer+12, "%d", entry->getCost());

			//char c[5];
			//itoa(entry->getCost(), c, 10);
			//strcat(buffer+12, c);

			sendto(sockfd, buffer, 1024, 0, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

		}


		close(sockfd);
	}


}


int main(int argc, char *argv[]){	//argv[1] = ip, argv[2] = topo.txt

	selfIPStr = argv[1];
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
			}

		}
		else if(checkIPEqual(selfIP, IP2) && !checkIPEqual(selfIP, IP1)){
			RoutingTableEntry *newEntry = new RoutingTableEntry(IP1, IP1, cost);
			updateRoutingTable(newEntry);

			if(!neighboursContains(IP1)){
				neighbours.push_back(IP1);
			}

		}


    }

    printRouters();
    printNeighbours();
    printRoutingTable();


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

	char cmd[20];

    while(true){
    	printf("Waiting for next input...\n");
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);
		printf("[%s:%d]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);

		//sendRoutingTable();

		if(buffer[0]=='c' && buffer[1]=='l' &&buffer[2]=='k'){
			sendRoutingTable();
		}
		else if(buffer[0]=='R' && buffer[1]=='T'){
			
		}
		

    }


	return 0;

}
