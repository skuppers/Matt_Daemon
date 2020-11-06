#include "Cryptograph.hpp"

Cryptograph::Cryptograph(void)
{
    return ;
}

Cryptograph::~Cryptograph(void)
{
    return ;
}

Cryptograph &Cryptograph::operator=(const Cryptograph & rhs)
{
    if (this != &rhs)
        *this = rhs;
    return *this;
}

std::ostream &operator<<(std::ostream &out, Cryptograph const & pm)
{
    out << "Cryptograph" << std::endl;
    return out;
}