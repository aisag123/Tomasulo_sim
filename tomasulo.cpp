#include "tomasulo.h"
#include <iostream>
#include <stdexcept>

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

    int source1 = static_cast<int>(instruction.source1);
    int source2 = static_cast<int>(instruction.source2);
    int destination = static_cast<int>(instruction.dest);

    if (rat[source1] == -1)
    {
        station.source1_ready = true;
        station.value1 = read_register(instruction.source1);
        std::cout << "read reg for source 1 " << station.value1 << "\n";
    }
    else
    {
        station.source1_tag = rat[source1];
        std::cout << "added tag to rat for source 1 " << station.value1 << "\n";
    }

    if (rat[source2] == -1)
    {
        station.source2_ready = true;
        station.value2 = read_register(instruction.source2);
        std::cout << "read reg for source 2 " << station.value2 << "\n";
    }
    else
    {
        station.source2_tag = rat[source2];
        std::cout << "added tag to rat for source 2 " << station.value2 << "\n";
    }

    rat[destination] = station_id;
    std::cout << "station id: " << station_id << "\n";
    std::cout << "Issue: instruction " << next_instruction
              << " -> station " << station_id << "\n";

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

    std::cout << "Broadcast: station " << station_id
              << " -> result " << value << "\n";
}

bool Tomasulo::dispatch_next()
{
    for (int station_id = 0; station_id < 2; ++station_id)
    {
        ReservationStation &station = stations[station_id];

        if (!station.busy ||
            !station.source1_ready ||
            !station.source2_ready)
        {
            continue;
        }

        const double result = execute(station);
        std::cout << "Dispatch: station " << station_id << "\n";
        broadcast(station_id, result);
        station = ReservationStation{};
        return true;
    }

    return false;
}

void Tomasulo::simulate(int cycles)
{
    for (int cycle = 1; cycle <= cycles; ++cycle)
    {
        std::cout << "\nCycle " << cycle << '\n';

        dispatch_next();
        issue_next();
    }
}