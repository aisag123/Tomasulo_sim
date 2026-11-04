#include <iostream>
#include "tomasulo.h"

int main(int, char **)
{
    Tomasulo sim;

    sim.init_instructions();

    for (int cycle = 1; cycle <= 4; cycle++)
    {
        if (sim.issue_next())
        {
            std::cout << "Cycle " << cycle << ": Instruction Issued \n";
        }
        else
        {
            std::cout << "Cycle " << cycle << ": No Instruction Issued \n";
        }
    }
    return 0;
}
