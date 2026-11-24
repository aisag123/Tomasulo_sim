Instruction
    |
    v
Reservation station
    |
    +--> Check RAT for source 1
    |       +--> No producer: read value from RF
    |       +--> Producer exists: store producer tag
    |
    +--> Check RAT for source 2
    |       +--> No producer: read value from RF
    |       +--> Producer exists: store producer tag
    |
    +--> RAT[ destination ] = this reservation station

    i need to add the offsets for instructions

    add/sub - 2cc
    mul - 10cc
    div - 40cc

    need to add the 4 RS 
        - need 2 RS for add/sub
        - need 2 for mul/div

