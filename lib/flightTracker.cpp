// import libarary
#include "slib/utility.h"
using namespace std;

pid_t pid = -1; // Global so we can access the child's pid whenever we need

// Functions
int main(int argc, char **argv)
{
    int parentToChild[2] = {0, 0}, childToParent[2] = {0, 0}; // Create pipes for communication
    // ---- First Pipe setup ----
    if (pipe(parentToChild) == -1 || pipe(childToParent) == -1)
    {
        cerr << "Failed to create pipe." << endl;
        return EXIT_FAILURE;
    }
    // ---- Pipe setup End ----

    // Fork the process to Parent and Child
    int returnedStatus;
    try
    {
        pid = fork();
        if (pid > 0) // Parent
        {
            // Parent Pipe Setup Continue
            close(parentToChild[READ_END]);  // Parent dont read from this pipe
            close(childToParent[WRITE_END]); // Parent dont write to this pipe

            // Parent Signals Setup
            signal(SIGINT, signalHandlerParent); // Register SIGINT of parent

            // UI - Parent Handling
            returnedStatus = UserInterface(parentToChild[WRITE_END], childToParent[READ_END]); // Handle UI
            pipeCleanUp(parentToChild, childToParent);                                         // Close the pipes between Parent and Child}
            return returnedStatus;
        }
        else if (pid == 0) // Child
        {
            // Child Pipe Setup Continue
            close(parentToChild[WRITE_END]); // Child dont write to this pipe
            close(childToParent[READ_END]);  // Child dont read from this pipe
            // Child Signals Setup
            signal(SIGINT, signalHandlerChild);  // register SIGINT to ignore and handle from the parent
            signal(SIGUSR1, signalHandlerChild); // Register SIGUSR1 of Child

            // Logic Process - Child Handling
            returnedStatus = LogicProcess(parentToChild[READ_END], childToParent[WRITE_END]); // Handle Task and logics from Parent
            pipeCleanUp(parentToChild, childToParent);                                        // Close the pipes between Parent and Child}
            return returnedStatus;
        }
        else // Fork failed handle
        {
            cerr << "Failed to create child process." << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const exception &e) // Exit for critical Error that didn't have any specific handle case
    {
        pipeCleanUp(parentToChild, childToParent);
        cerr << e.what() << endl;
    }
}
