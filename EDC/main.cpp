/**
 * 1505080
 * Error Detection using CRC checksum
 * Error Correction using Hamming distance
 **/

/**
 * Code to change consol text color:
 * #include <windows.h>
 * #include <iostream>
 * color = 4 RED
 * color = 10 GREEN
 * color = 11 CYAN
 * color = 15 WHITE
 * SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),colour),	std::cout << "Using colour:" << colour << std::endl;
**/

#include <windows.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cmath>

#include <iostream>
#include <vector>
#include <queue>

#define MAX_MSG_SIZE 80
#define MAX_POLYNOMIAL_SIZE 20

using namespace std;

char *charToBinaryString(char ch){
    char *binaryString = new char[12];

    int mask = 1;

    for(int i=7; i>=0; i--){
        if(ch & mask > 0){
            binaryString[i] = '1';
        }
        else{
            binaryString[i] = '0';
        }
        ch = ch >> 1;
    }
    binaryString[8] = 0;

    return binaryString;

}

void printBooleanArray(int row, int col, bool **array){
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if(array[i][j]){
                printf("1");
            }
            else{
                printf("0");
            }
        }
        printf("\n");
    }
}


int main(){

    //printf("%s %s %s\n", charToBinaryString('A'), charToBinaryString(16), charToBinaryString(7));

    printf("******Error Detection and Correction Simulation******\n");

///=============================================================================
///                             Taking Input
///=============================================================================

    char *inputDataString = new char[MAX_MSG_SIZE];
    printf("Enter Data String:");
    gets(inputDataString);
    printf("Data String:%s\n", inputDataString);

    int m;
    printf("Enter Number of Data Bytes in a Row <m>:");
    scanf("%d", &m);
    printf("m=%d\n", m);

    float p;
    printf("Enter Probability <p>:");
    scanf("%f", &p);
    printf("p=%f\n", p);

    char *generatorPolynomial = new char[MAX_POLYNOMIAL_SIZE];
    printf("Enter Generator Polynomial:");
    gets(generatorPolynomial);  //dummy
    gets(generatorPolynomial);
    printf("Generator Polynomial:%s\n", generatorPolynomial);


///=============================================================================
///                                 Padding
///=============================================================================
    int inputLen = strlen(inputDataString);
    for (int i = inputLen; i%m!=0 ; i++) {
        inputDataString[i] = '~';
        inputDataString[i+1] = 0;
    }
    printf("\nData String after Padding:%s\n", inputDataString);



///=============================================================================
///                         Generating Data Block
///=============================================================================
    inputLen = strlen(inputDataString);

    char *binaryString = new char[MAX_MSG_SIZE*8+1];
    sprintf(binaryString, "");
    for (int i = 0; i < inputLen; i++) {
        sprintf(binaryString, "%s%s", binaryString, charToBinaryString(inputDataString[i]));
    }
    printf("\nBinary String:%s\n", binaryString);


    int numRow = inputLen/m;
    bool **dataBlock = new bool* [numRow];

    int k = 0;
    for (int i = 0; i < numRow; i++) {
        dataBlock[i] = new bool[m*8];

        for (int j = 0; j < m*8; j++) {
            if(binaryString[k] == '0'){
                dataBlock[i][j] = false;
            }
            else{
                dataBlock[i][j] = true;
            }
            k++;
        }

    }

    printf("\nData Block <ASCII Code of m Characters Per Row>:\n");
    printBooleanArray(numRow, m*8, dataBlock);







    return 0;
}
