#ifndef FALLING_EDGE_H
#define FALLING_EDGE_H

class FallingEdge
{

private:
    bool value;

public:
    FallingEdge();
    ~FallingEdge();

    bool get(bool newValue);
};

#endif