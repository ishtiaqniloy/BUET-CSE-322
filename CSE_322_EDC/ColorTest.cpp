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
#include <iostream>

using namespace std;

int main()
{
    for (int colour = 0x00; colour <= 0xff; colour ++)
    {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),colour),	std::cout << "Using colour:" << colour << std::endl;

        // gets();

//		cin.get();
    }
}
