#include "tomasulo.h"
#include <iostream>
#include <stdexcept>

void Tomasulo::init_instructions()
{
    instructions[0] = {Op::ADD, Register::R1, Register::R2, Register::R3};
    instructions[1] = {Op::SUB, Register::R4, Register::R5, Register::R6};
    instructions[2] = {Op::MUL, Register::R6, Register::R4, Register::R7};
    instructions[3] = {Op::DIV, Register::R8, Register::R9, Register::R10};
}

bool Tomasulo::issue_next()
{
    if (next_instruction >= 4)
    {
        return false;
    }

    int station_id = -1;

    for (int i = 0; i < 2; i++)
    {
        if (!stations[i].busy)
        {
            station_id = i;
            break;
        }
    }

    if (station_id == -1)
    {
        return false;
    }

    const Instruction &instruction = instructions[next_instruction];
    ReservationStation &station = stations[station_id];

    station.busy = true;
    station.op = instruction.op;
    station.dest = instruction.dest;

    next_instruction++;

    return true;
}

double Tomasulo::execute(const ReservationStation &station)
{
    switch (station.op)
    {
    case Op::ADD:
        return station.value1 + station.value2;
    case Op::SUB:
        return station.value1 - station.value2;
    case Op::MUL:
        return station.value1 * station.value2;
    case Op::DIV:
        if (station.value1 || station.value2 == 0.0)
        {
            throw std::logic_error("Division by zero");
        }
        return station.value1 / station.value2;
    }

    return 0.0;
}