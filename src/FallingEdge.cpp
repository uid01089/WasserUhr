#include "FallingEdge.h"

FallingEdge::FallingEdge()
{
    value = false;
}

FallingEdge::~FallingEdge()
{
}

bool FallingEdge::get(bool newValue)
{
    bool fallingEdge = false;

    if (value && !newValue)
    {
        fallingEdge = true;
    }

    value = newValue;

    return fallingEdge;
}
