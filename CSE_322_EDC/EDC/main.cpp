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
#include <ctime>

#include <iostream>
#include <random>
#include <vector>
#include <queue>

#define MAX_MSG_SIZE 80
#define MAX_POLYNOMIAL_SIZE 20
#define DIV_VAL 100000

#define RED 4
#define GREEN 10
#define CYAN 11
#define WHITE 15

using namespace std;

random_device rand_dev;
default_random_engine generator(rand_dev());
uniform_int_distribution<int> distribution(0,DIV_VAL);

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

char binaryStringToChar(char *binaryString){
    char ch = 0;

    for(int i=0; i<8; i++){
        ch = ch << 1;
        if(binaryString[i]=='1'){
            ch |= 1;
        }
        //printf("%s\n", charToBinaryString(ch));
    }

    return ch;

}

void printCharArray(int row, int col, char **array, int COLOR=WHITE, bool **colorArray=NULL){
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if(colorArray==NULL || !colorArray[i][j]){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  array[i][j];
            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),COLOR),	std::cout <<  array[i][j];
            }
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  endl;
    }
}

int getHammingR(int m){
    for(int r=0; ; r++){
        if(pow(2, r) >= m+r+1){
            return r;
        }
    }
}

char *calculateCRCBits(int crcDataLen, const char *key, const char *dataWithCRC){

//    printf("Calculating CRC Checksum of: \n");
//    printf("%s\n", dataWithCRC);

    int keySize = strlen(key);
    char *crc = new char[keySize+5];

    ///initialize
    for (int i = 0; i < keySize; ++i) {
        crc[i] = dataWithCRC[i];
//        printf("INIT: %c=%d, %c=%d\n", crc[i], crc[i], key[i], key[i]);
    }

    for (int nextBit = keySize; nextBit <= crcDataLen; nextBit++) {

        if(crc[0]=='1'){
            for (int i = 0; i < keySize; ++i) {
//            printf("i=%d, %c=%d, %c=%d\n", i, crc[i], crc[i], key[i], key[i]);
                crc[i] = (crc[i]-'0')^(key[i]-'0')+'0';
            }
        }

        ///for next iteration
        for (int i = 0; i < keySize-1; ++i) {
            crc[i] = crc[i+1];
        }
        crc[keySize-1] = dataWithCRC[nextBit];

        //crc[keySize] = 0;
        //printf("crc = %s\n", crc);
    }

    printf("\ncrc = %s\n", crc);


    return crc;
}


double randomGenerator(){

    int idx = distribution(generator);
    int number = 0;
    for(int i=0; i<idx; i++){   ///random indexed random number
        number = distribution(generator);
    }

    return (1.0*number/DIV_VAL);

}


int main(){

    //printf("%s %s %s\n", charToBinaryString('A'), charToBinaryString(16), charToBinaryString(7));

//    char ch = binaryStringToChar("01100001");
//    printf("%c %d\n", ch, ch);

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

    if(m==0){
        printf("m Must be Greater than 0\n");
        exit(0);
    }

    double p;
    printf("Enter Probability <p>:");
    scanf("%lf", &p);
    printf("p=%f\n", p);

    char *generatorPolynomial = new char[MAX_POLYNOMIAL_SIZE];
    printf("Enter Generator Polynomial:");
    gets(generatorPolynomial);  //dummy
    gets(generatorPolynomial);
    printf("Generator Polynomial:%s\n", generatorPolynomial);


///=============================================================================
///                                 1.Padding
///=============================================================================
    int inputLen = strlen(inputDataString);
    for (int i = inputLen; i%m!=0 ; i++) {
        inputDataString[i] = '~';
        inputDataString[i+1] = 0;
    }
    printf("\nData String after Padding:%s\n", inputDataString);



///=============================================================================
///                         2.Generating Data Block
///=============================================================================
    inputLen = strlen(inputDataString);

    char *binaryString = new char[MAX_MSG_SIZE*8+1];
    sprintf(binaryString, "");
    for (int i = 0; i < inputLen; i++) {
        sprintf(binaryString, "%s%s", binaryString, charToBinaryString(inputDataString[i]));
    }
    //printf("\nBinary String:%s\n", binaryString);


    int numRow = inputLen/m;
    char **dataBlock = new char* [numRow];

    for (int i = 0, k = 0; i < numRow; i++) {
        dataBlock[i] = new char[m*8];
        for (int j = 0; j < m*8; j++, k++) {
            dataBlock[i][j] = binaryString[k];
        }
    }

    printf("\nData Block <ASCII Code of m Characters Per Row>:\n");
    printCharArray(numRow, m * 8, dataBlock);



///=============================================================================
///                     3.Adding Check Bits (EVEN PARITY)
///=============================================================================
    int r = getHammingR(m*8);
    char **dataBlockWithCheckBits = new char* [numRow];

    for (int i = 0; i < numRow; i++) {
        dataBlockWithCheckBits[i] = new char[m*8+r];
        int power=0;
        int dataIdx = 0;

        for (int j = 0; j < m*8+r; j++) {
            if(j == pow(2, power)-1){
                dataBlockWithCheckBits[i][j]= 'X';
                power++;
                continue;
            }
            else{
                dataBlockWithCheckBits[i][j] = dataBlock[i][dataIdx];
                dataIdx++;
            }

        }

    }

//    printf("\nDataBlock With Checkbit Space Created:\n");
//    printCharArray(numRow, m*8+r, dataBlockWithCheckBits);


    printf("\nDataBlock With Checkbits Added:\n");
    for (int i = 0; i < numRow; i++) {
        int level=1;

        for (int j = 0; j < m*8+r; j++) {
            if(dataBlockWithCheckBits[i][j]=='X'){
                int sum = 0;
                for (int base = j; base < m*8+r; base+=level*2) {
                    for (int k = base; k < base+level; k++) {
                        if(k >= m*8+r){
                            break;
                        }
                        else if(dataBlockWithCheckBits[i][k]=='1'){
                            sum++;
                        }
                    }

                }
                dataBlockWithCheckBits[i][j] = (char)(sum%2)+'0';

                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),GREEN),	std::cout <<  dataBlockWithCheckBits[i][j];

                level *= 2;

            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  dataBlockWithCheckBits[i][j];
            }

        }
        cout << endl;

    }



///=============================================================================
///                             4.Serialize
///=============================================================================

    int serializedBits = numRow*(m*8+r);
    char *serializedData = new char[serializedBits+10]; //10 bit extra for safety

    for (int i = 0, dataIdx = 0; i < m * 8 + r; i++) {
        for (int j = 0; j < numRow; j++, dataIdx++) {
            serializedData[dataIdx] = dataBlockWithCheckBits[j][i];
        }
    }
    serializedData[serializedBits] = 0;

    printf("\nData Bits after Column-wise Serialization:\n");
    printf("%s\n", serializedData);




///=============================================================================
///                             5.CRC Checksum
///=============================================================================
    int crcLen = strlen(generatorPolynomial)-1;

    int crcDataLen = serializedBits+crcLen;
    char *dataWithCRC = new char[crcDataLen+10];

    strcpy(dataWithCRC, serializedData);
    for (int i = serializedBits; i < crcDataLen; ++i) {
        dataWithCRC[i] = '0';
    }
    dataWithCRC[crcDataLen] = 0;

    char *crc = calculateCRCBits(crcDataLen, generatorPolynomial, dataWithCRC);
    for (int i = 0; i < crcLen; ++i) {
        dataWithCRC[serializedBits+i] = crc[i];
    }

    printf("\nData Bits after Appending CRC Checksum <Sent Frame>:\n");
    for (int i = 0; i < serializedBits; ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  dataWithCRC[i];
    }
    for (int i = serializedBits; i < crcDataLen; ++i) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),CYAN),	std::cout <<  dataWithCRC[i];
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  endl;




///=============================================================================
///                             6.Toggle
///=============================================================================
    char *receivedFrame = new char[crcDataLen+10];
    bool *errorArray = new bool[crcDataLen+10];
    strcpy(receivedFrame, dataWithCRC);

    printf("\nReceived Frame:\n");
    for (int i = 0; i < crcDataLen; i++) {
        if(randomGenerator() < p){
            receivedFrame[i] = (char) ((receivedFrame[i]-'0')^1) + '0';    ///toggle
            errorArray[i] = true;
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),RED),	std::cout <<  receivedFrame[i];
        }
        else{
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  receivedFrame[i];
            errorArray[i] = false;
        }

    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  endl;


///=============================================================================
///                             7.Verify CRC Checksum
///=============================================================================
    bool error = false;
    char *tempCRC = calculateCRCBits(crcDataLen, generatorPolynomial, receivedFrame);

    for (int i = 0; i < crcLen; ++i) {
        if(tempCRC[i] != '0' ){
            error = true;
        }
    }

    printf("\nResult of CRC Checksum Matching: ");
    if(error){
        printf("ERROR DETECTED\n");
    }
    else{
        printf("NO ERROR\n");
    }



///=============================================================================
///                 8.Remove CRC Checksum and Deserialize
///=============================================================================
    receivedFrame[serializedBits] = 0;

//    printf("\nAfter Removing CRC Checksum:\n");
//    for (int i = 0; i < serializedBits; i++) {
//        if(errorArray[i]){
//            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),RED),	std::cout <<  receivedFrame[i];
//        }
//        else{
//            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  receivedFrame[i];
//        }
//
//    }
//    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),WHITE),	std::cout <<  endl;

    char **receivedBlockWithCheckBits = new char* [numRow];
    bool **errorBlock = new bool* [numRow];
    for (int i = 0; i < numRow; i++) {
        receivedBlockWithCheckBits[i] = new char[m*8+r];
        errorBlock[i] = new bool[m*8+r];
    }

    for (int i = 0, dataIdx = 0; i < m * 8 + r; i++) {
        for (int j = 0; j < numRow; j++, dataIdx++) {
            receivedBlockWithCheckBits[j][i] = receivedFrame[dataIdx];
            errorBlock[j][i] = errorArray[dataIdx];

        }
    }

    printf("\nData Bits after Removing CRC Checksum <Deserialized>:\n");
    printCharArray(numRow, m*8+r, receivedBlockWithCheckBits, RED, errorBlock);




///=============================================================================
///                 9.Correct Data and Remove Checkbits
///=============================================================================

    printf("\nAttempting to Correct Error Bits:\n");
    for (int i = 0; i < numRow; i++) {
        int level = 1;
        int power = 0;
        int correctionIdx = 0;

        for (int j = 0; j < m*8+r; j++) {
            if(j == pow(2, power)-1){
                power++;
                int sum = 0;
                for (int base = j; base < m*8+r; base+=level*2) {
                    for (int k = base; k < base+level; k++) {
                        if(k >= m*8+r){
                            break;
                        }
                        else if(receivedBlockWithCheckBits[i][k]=='1'){
                            sum++;
                        }
                    }

                }
                if(sum%2>0){    ///error found
                    correctionIdx += j+1;
                }

                level *= 2;

            }

        }

        correctionIdx--;
        if(correctionIdx>=0 && correctionIdx< m*8+r){
            receivedBlockWithCheckBits[i][correctionIdx] = (char) ((receivedBlockWithCheckBits[i][correctionIdx]-'0')^1)+'0';
            printf("Correcting (%d,%d)\n", i, correctionIdx);
        }

    }

    printf("\nAfter Attempt to Correct Error Bits:\n");
    printCharArray(numRow, m*8+r, receivedBlockWithCheckBits, RED, errorBlock);


    for (int i = 0; i < numRow; i++) {
        int power=0;
        int dataIdx = 0;

        for (int j = 0; j < m*8+r; j++) {
            if(j == pow(2, power)-1){
                receivedBlockWithCheckBits[i][j]= 'X';
                power++;
                continue;
            }

        }
    }

//    printf("\nRemoving Checkbits:\n");
//    printCharArray(numRow, m*8+r, receivedBlockWithCheckBits);

    char **extractedDataBlock = new char*[numRow];
    for (int i = 0; i < numRow; i++) {
        extractedDataBlock[i] = new char[m*8];
        for (int j = 0, dataIdx=0; j < m*8+r; j++) {
            if(receivedBlockWithCheckBits[i][j]=='X'){
                continue;
            }
            else{
                extractedDataBlock[i][dataIdx] = receivedBlockWithCheckBits[i][j];
                dataIdx++;
            }
        }

    }

    printf("\nData Block After Removing Checkbits:\n");
    printCharArray(numRow, m*8, extractedDataBlock);


///=============================================================================
///                 10.Correct Data and Remove Checkbits
///=============================================================================

    printf("\nOutput Frame: ");
    char *msg = new char[MAX_MSG_SIZE];
    strcpy(msg, "");

    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < m*8; j+=8) {
            char *str = new char[10];

            strncpy(str, extractedDataBlock[i]+j, 8);
            char ch = binaryStringToChar(str);
            printf("%c", ch);
            sprintf(msg, "%s%c", msg, ch);

            delete []str;
        }

    }
    printf("\n");

    for(int i = strlen(msg)-1; i>=0; i--){
        if(msg[i] != '~'){
            break;
        }
        msg[i]=0;
    }

    printf("\nExtracted Message: ");
    printf("%s\n", msg);


///=============================================================================
///                         Free Taken Memory
///=============================================================================



    delete []inputDataString;
    delete []generatorPolynomial;
    delete []binaryString;
    delete []serializedData;
    delete []dataWithCRC;
    delete []crc;
    delete []receivedFrame;
    delete []errorArray;
    delete []tempCRC;
    delete []msg;


    for (int i = 0; i < numRow; ++i) {
        delete [] dataBlock[i];
        delete [] dataBlockWithCheckBits[i];
        delete [] receivedBlockWithCheckBits[i];
        delete [] errorBlock[i];
        delete [] extractedDataBlock[i];
    }
    delete []dataBlock;
    delete []dataBlockWithCheckBits;
    delete []receivedBlockWithCheckBits;
    delete []errorBlock;
    delete []extractedDataBlock;


    return 0;
}
