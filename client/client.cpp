#include "Ben_Afk.hpp"

void printUsage(void) {
    std::cout << "Usage: ./Ben_AFK <destination> <port>" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int ac, char **av)
{
    std::string *userInput;

    if (ac < 3)
        printUsage();

    Ben_Afk benny(av[1], atoi(av[2]));
    if (benny.createSocket() && benny.connectToDaemon())
    {
        while (1) {
            userInput = benny.readInput();
            if (benny.communicate(userInput) == false)
                break ;
        }
    }
    return 0;
}