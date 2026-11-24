#include "tomasulo.h"

int main()
{
    Tomasulo sim;

    sim.init_instructions();
    sim.simulate(100);

    return 0;
}
