#include "pch.h"
#include "DropException.h"


DropException::DropException(Prop* p)
    : prop(p)
{

}


DropException::~DropException(void)
{
}

static const char* errorMessage = "Dropped";

const char* DropException::what() const
{
    return errorMessage;
}
