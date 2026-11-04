#ifndef TOMASULO_H
#define TOMASULO_H

enum class Op
{
    ADD,
    SUB,
    MUL,
    DIV
};

struct RF
{
    double R1 = 1.0;
    double R2 = 1.0;
    double R3 = 1.0;
    double R4 = 1.0;
    double R5 = 1.0;
    double R6 = 1.0;
    double R7 = 1.0;
    double R8 = 1.0;
    double R9 = 1.0;
    double R10 = 1.0;
};

enum class Register
{
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10
};

struct Instruction
{
    Op op;
    Register dest;
    Register source1;
    Register source2;
};

struct ReservationStation
{
    // busy
    bool busy = false;

    // op
    Op op;
    // dest
    Register dest;

    // src1 - ready state
    bool source1_ready = false;

    // src2 - ready state
    bool source2_ready = false;

    // value 1
    double value1 = 0.0;

    // value 2
    double value2 = 0.0;

    // src1 - tag
    int source1_tag = -1;

    // src2 - tag
    int source2_tag = -1;
};

class Tomasulo
{
public:
    void init_instructions();
    bool issue_next();
    double execute(const ReservationStation &station);

private:
    Instruction instructions[4];
    ReservationStation stations[2];

    int next_instruction = 0;
    int register_status[10]{};
    int add_sub_cc{2};
};

#endif