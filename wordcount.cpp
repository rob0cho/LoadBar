/*
    Name: Robert Choate
*/

#include "wordcountHeader.hpp"


int main(int argc, char** argv) {
    // Capture the cmd line input for the file name
    std::string fileName = "";
    if(argc > 1) {
        fileName = argv[1];
    }
    wordcount(fileName);
    // Error & success messages
    /*if(wordcount(fileName) == -1)
        std::cout << "Usage: ./wordcount <textFile>" << std::endl;
    else 
        std::cout << "Words successfully counted!" << std::endl;*/
    return 0;
}


// This function spawns the thread to watch the progress of wordcount() on a separate thread
extern "C" void * progress_monitor(void* progressMonitor) {

    // Confirmation we can access the data in the struct
    PROGRESS_STATUS* progress = (PROGRESS_STATUS*)progressMonitor;
    long endVal = progress->TerminationValue;

    float percent;
    bool loading = true;
    int currentPos = 0;
    int prevPos;

    while(loading) {
        prevPos = currentPos; // To hold last value before assigning new one for the loop
        if(*progress->CurrentStatus == endVal) // Exit when we reach TerminationValue
            loading = false;

        // Calculate how much progress we have made
        percent = *progress->CurrentStatus / (float)progress->TerminationValue;
        currentPos = LOADBAR * percent; // Calculate how far the bar should be loaded

        // Loop from where last position ended to the current position
        for(int i = prevPos + 1; i <= currentPos; i++) {
            // Logic for when to print a '-' or '+'
            if(i % 10 == 0) {
                std::cout << "+";
            }
            else {
                std::cout << "-";
            }
        }
        std::cout.flush(); // Force the output to the screen
        //sleep(1); // This is here for testing with small files
    }
    std::cout << std::endl;
    //sleep(3); // Test to confirm wordcount thread waits for exit
    pthread_exit(NULL); // Terminate this thread upon ending function
}



// Function to count the number of words in a file and spawn another thread to watch
// as the count progresses
long wordcount(std::string fileName) {

    float percent;
    char character;
    long initVal = 0;
    long wordCount = 0;
    long currentStat = 0;
    long terminateVal = 0;
    std::ifstream wordDoc;
    long* statusPtr = &currentStat;
    pthread_t thread;

    // Open the file/handle error cases, 1 = success 
    if(fileOpen(fileName, wordDoc) == 1) {
        terminateVal = fileSize(wordDoc); // Count the number of bytes in the file
        PROGRESS_STATUS progressMonitor = { statusPtr, initVal, terminateVal }; // Populating the struct

        // Creating the thread which will call and run progress_monitor()
        pthread_create(&thread, NULL, progress_monitor, (void*)&progressMonitor);
        
        // Iterate through the file
        while(wordDoc) {
            // Collect each byte in the file
            wordDoc.get(character);
            //sleep(1); // This is here for testing with small files

            if(!wordDoc.eof()) {
                currentStat++; // Counting how many bytes we have read

                // Count the amount of words based on if it is not wspace followed by a wspace
                if(!std::iswspace(character) && std::iswspace(wordDoc.peek())) {
                    wordCount++;
                }
                // To count a word at the end of a file
                else if(!std::iswspace(character) && true == wordDoc.eof()) {
                    wordCount++;
                }
            }
        }
    
        pthread_join(thread, NULL); // Tell this thread to wait for progress_monitor() to exit
        std::cout << "There are " << wordCount << " words in " << fileName << "." << std::endl;

        wordDoc.close();
        return wordCount;
    }
    else {
        // The file failed to open
        return -1; // If I want to trigger another error message in main
    }
}


// Function to open our file and handle errors with opening it
int fileOpen(std::string fileName, std::ifstream& wordDoc) {
    int success = 0;
    // For when no arguments are passed to the program
    if(fileName.empty()) {
        std::cout << "no file specified" << std::endl;
    }
    else {
        wordDoc.open(fileName, std::ios::in);
        // For when the specified file is not found
        if(!wordDoc.is_open()) {
            std::cout << "could not open file" << std::endl;
        }
        else {
            success = 1;
        }
    }
    return success;
}


// Function for getting the number of bytes in the file
int fileSize(std::ifstream &wordDoc) {
    int size = 0;
    wordDoc.seekg(0, std::ios::end); // seekg() set position in input seq
    size = wordDoc.tellg(); // tellg() gets position in input seq
    //std::cout << "File size = " << size << " bytes. " << std::endl;
    wordDoc.seekg(0, std::ios::beg);
    return size;
}