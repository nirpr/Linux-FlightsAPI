#include "utility.h"
using namespace std;

void pipeCleanUp(int pipe1, int pipe2)
{
    // Close pipe1 - IO Pipe
    close(pipe1);
    // Close pipe2 - IO Pipe
    close(pipe2);
}

bool sendCodeToPipe(int writePipe, int OpCode) noexcept(false)
{
    ssize_t bytesWritten = write(writePipe, &OpCode, sizeof(OpCode));
    if (bytesWritten == -1)
        throw runtime_error("Failed to write to pipe.");
    return true;
}

int receiveCodeFromPipe(int pipeRead) noexcept(false)
{
    int OpCode = -1; // if -1 returned will be gracefull exit for undefined OpCode
    size_t bytesRead = read(pipeRead, &OpCode, sizeof(OpCode));
    if (bytesRead == -1)
        throw runtime_error("Failed to read task from pipe.");
    return OpCode;
}

string receiveMessage(int pipeRead) noexcept(false)
{
    char buffer[PIPE_BUF];
    int BytesToRead;
    int bytesRead = 0;
    bytesRead = read(pipeRead, &BytesToRead, sizeof(BytesToRead));
    if (bytesRead == -1)
        throw runtime_error("Read message from pipe failed!");

    // Read the strings from the child process
    string stringData;
    char chunk[PIPE_BUF];
    while (BytesToRead > 0)
    {
        int chunkSize = min(PIPE_BUF - 1, BytesToRead);
        int bytesRead = read(pipeRead, chunk, chunkSize);
        if (bytesRead > 0)
        {
            chunk[chunkSize] = '\0';
            stringData.append(chunk); // append string
            BytesToRead -= bytesRead;
        }
        else if (bytesRead == -1)
            throw runtime_error("Read message from pipe failed!");
    }
    return stringData;
}

void sendMessage(int pipeWrite, string message) noexcept(false)
{
    // Sending the size of the message
    int sizesData = message.size();
    if (write(pipeWrite, &sizesData, sizeof(sizesData)) == -1)
        throw runtime_error("Failed to write message into pipe!");

    // Send the string to the pipe
    int remainingSize = message.size();
    int offset = 0;
    // Split the string into smaller chunks if necessary
    while (remainingSize > 0)
    {
        int chunkSize = min(remainingSize, PIPE_BUF);
        if (write(pipeWrite, message.c_str() + offset, chunkSize) == -1)
            throw runtime_error("Failed to write message into pipe!");
        remainingSize -= chunkSize;
        offset += chunkSize;
    }
}
