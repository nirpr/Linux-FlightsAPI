#include <stdio.h>
#include <iostream>
#include <unistd.h>
using namespace std;

int LogicProcess();
int UserInterface();

int main()
{
    pid_t cid = -1;
    cout << "Program is starting" << endl;
    cid = fork();

    if(cid > 0)
        UserInterface();
    else if (cid == 0)
        LogicProcess();
    else
    {
        cerr << "fork creation failed" << endl;
        return EXIT_FAILED;
    }
}

int LogicProcess()
{
    cout << "Fork created [Process id: " <<  getpid() << ", [parent process id: " << getppid() << "]." << std::endl;

}

int UserInterface()
{
    cout << "This is parent section [Process id: " <<  getpid() << "] , [child's id: " << cid << " ]." << std::endl;
}