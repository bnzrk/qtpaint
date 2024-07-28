#ifndef COMMAND_H
#define COMMAND_H

/*
 * Abstract class for revertable actions.
 */
class Command
{
public:
    virtual ~Command() {};

    virtual void execute() const = 0;
    virtual void revert() const = 0;
};

#endif // COMMAND_H
