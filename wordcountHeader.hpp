/*
    Name: Robert Choate
*/

#include <string>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <unistd.h> // This is for sleep()

int fileOpen(std::string fileName, std::ifstream& wordDoc);
long wordcount(std::string fileName);
int fileSize(std::ifstream &wordDoc);
extern "C" void * progress_monitor(void* progressMonitor);

static const int LOADBAR = 50;

// Assume that TerminationValue >= Progress Indicator (*CurrentStatus) >= InitialValue
typedef struct {
    long* CurrentStatus;
    long InitialValue;
    long TerminationValue;
} PROGRESS_STATUS;