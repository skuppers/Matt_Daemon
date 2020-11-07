#include "Ben_Afk.hpp"
#include "Cryptograph.hpp"
#include "CryptoWrapper.hpp"

void printUsage(void) {
    std::cout << "Usage: ./Ben_AFK <destination> <port>" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int ac, char **av)
{
    std::string *userInput;

    if (ac < 3)
        printUsage();

    Cryptograph     cg;
    CryptoWrapper   cw(cg);

    Ben_Afk benny(av[1], atoi(av[2]), &cw);

    if (benny.createSocket() && benny.connectToDaemon())
    {
        while (1) {
            if ((userInput = benny.readInput(true)) == nullptr)
                break ;
            if (benny.communicate(userInput) == false)
                break ;
        }
    }
    return 0;
}