#include "tomasulo.h"
#include <iostream>
#include <stdexcept>

namespace
{
const char *op_name(Op op)
{
    switch (op)
    {
    case Op::ADD:
        return "ADD";
    case Op::SUB:
        return "SUB";
    case Op::MUL:
        return "MUL";
    case Op::DIV:
        return "DIV";
    }

    return "?";
}
}

Tomasulo::Tomasulo()
    : rat{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
{
}

void Tomasulo::init_instructions()
{
    instructions[0] = {Op::ADD, Register::R1, Register::R2, Register::R3};
    instructions[1] = {Op::SUB, Register::R4, Register::R5, Register::R6};
    instructions[2] = {Op::MUL, Register::R6, Register::R4, Register::R7};
    instructions[3] = {Op::DIV, Register::R8, Register::R9, Register::R10};
}

int Tomasulo::stall(Op op) const
{
    switch (op)
    {
    case Op::ADD:
    case Op::SUB:
        return 2;
    case Op::MUL:
        return 10;
    case Op::DIV:
        return 40;
    }

    throw ::std::logic_error("invalid operation");
}

bool Tomasulo::issue_next()
{
    if (next_instruction >= 4)
    {
        return false;
    }

    int station_id = -1;

    const Instruction &instruction = instructions[next_instruction];
    const bool is_add_sub = instruction.op == Op::ADD ||
                            instruction.op == Op::SUB;

    const int first_station = is_add_sub ? 0 : 2;
    const int last_station = first_station + 2;

    for (int i = first_station; i < last_station; ++i)
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

    ReservationStation &station = stations[station_id];

    station.busy = true;
    station.op = instruction.op;
    station.dest = instruction.dest;
    station.cycles_remaining = stall(instruction.op);

    next_instruction++;

    int source1 = static_cast<int>(instruction.source1);
    int source2 = static_cast<int>(instruction.source2);
    int destination = static_cast<int>(instruction.dest);

    if (rat[source1] == -1)
    {
        station.source1_ready = true;
        station.value1 = read_register(instruction.source1);
    }
    else
    {
        station.source1_tag = rat[source1];
    }

    if (rat[source2] == -1)
    {
        station.source2_ready = true;
        station.value2 = read_register(instruction.source2);
    }
    else
    {
        station.source2_tag = rat[source2];
    }

    rat[destination] = station_id;
    std::cout << "  Issue I" << next_instruction
              << " (" << op_name(instruction.op) << ") -> RS"
              << station_id << " [" << station.cycles_remaining
              << " cycles]\n";

    return true;
}

double Tomasulo::read_register(Register reg) const
{
    switch (reg)
    {
    case Register::R1:
        return rf.R1;
    case Register::R2:
        return rf.R2;
    case Register::R3:
        return rf.R3;
    case Register::R4:
        return rf.R4;
    case Register::R5:
        return rf.R5;
    case Register::R6:
        return rf.R6;
    case Register::R7:
        return rf.R7;
    case Register::R8:
        return rf.R8;
    case Register::R9:
        return rf.R9;
    case Register::R10:
        return rf.R10;
    }

    throw std::logic_error("invalid reg");
}

void Tomasulo::write_register(Register reg, double value)
{
    switch (reg)
    {
    case Register::R1:
        rf.R1 = value;
        return;
    case Register::R2:
        rf.R2 = value;
        return;
    case Register::R3:
        rf.R3 = value;
        return;
    case Register::R4:
        rf.R4 = value;
        return;
    case Register::R5:
        rf.R5 = value;
        return;
    case Register::R6:
        rf.R6 = value;
        return;
    case Register::R7:
        rf.R7 = value;
        return;
    case Register::R8:
        rf.R8 = value;
        return;
    case Register::R9:
        rf.R9 = value;
        return;
    case Register::R10:
        rf.R10 = value;
        return;
    }

    throw std::logic_error("invalid reg");
}

double Tomasulo::execute(const ReservationStation &station) const
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
        if (station.value2 == 0.0)
        {
            throw std::logic_error("Division by zero");
        }
        return station.value1 / station.value2;
    }

    return 0.0;
}

void Tomasulo::broadcast(int station_id, double value)
{
    const ReservationStation &completed = stations[station_id];
    const int destination = static_cast<int>(completed.dest);

    if (rat[destination] == station_id)
    {
        write_register(completed.dest, value);
        rat[destination] = -1;
    }

    for (ReservationStation &station : stations)
    {
        if (station.source1_tag == station_id)
        {
            station.source1_tag = -1;
            station.source1_ready = true;
            station.value1 = value;
        }

        if (station.source2_tag == station_id)
        {
            station.source2_tag = -1;
            station.source2_ready = true;
            station.value2 = value;
        }
    }

    std::cout << "  Write RS" << station_id << " -> "
              << value << "\n";
}

bool Tomasulo::dispatch_next()
{
    bool dispatched = false;

    for (int station_id = 0; station_id < 4; station_id++)
    {
        ReservationStation &station = stations[station_id];

        if (!station.busy || !station.source1_ready || !station.source2_ready)
        {
            continue;
        }

        station.cycles_remaining--;

        if (station.cycles_remaining > 0)
        {
            std::cout << "Execute: station " << station_id
                      << " (" << station.cycles_remaining
                      << " cycles left)\n";
            dispatched = true;
            continue;
        }

        const double result = execute(station);
        std::cout << "  Complete RS" << station_id << "\n";
        broadcast(station_id, result);
        station = ReservationStation{};
        dispatched = true;
    }

    return dispatched;
}

void Tomasulo::simulate(int cycles)
{
    for (int cycle = 1; cycle <= cycles; ++cycle)
    {
        std::cout << "\nCycle " << cycle << '\n';

        issue_next();
        dispatch_next();
    }
}