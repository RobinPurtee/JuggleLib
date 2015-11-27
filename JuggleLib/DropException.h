#pragma once
class Prop;

class DropException :
    public std::exception
{
public:
    DropException(Prop* p);
    virtual ~DropException(void);

    virtual const char* what() const;

    Prop* prop;
};

