#include "tomasulo.h"

int main()
{
    Tomasulo sim;

    sim.init_instructions();
    sim.simulate(8);

    return 0;
}
