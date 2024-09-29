#include "program.h"

void signal_handler(int signum) 
{
    set_echo(1); // Re-enable echoing before handling the signal
    exit(signum); // Exit with the received signal code
}

void cleanup() 
{
    set_echo(1); // Re-enable echoing before the program exits
}