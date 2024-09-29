#include "program.h"

void set_echo(int enable) 
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if (!enable) 
    {
        tty.c_lflag &= ~(ECHO | ICANON); // Disable echo and canonical mode
    } 
    else 
    {
        tty.c_lflag |= (ECHO | ICANON); // Enable echo and canonical mode
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void get_hash_prefix(unsigned char *hash, char *prefix) 
{
    for (int i = 0; i < 5; i++) 
    {
        sprintf(prefix + i * 2, "%02x", hash[i]);
    }
    prefix[10] = '\0';  // Null-terminate the prefix
}