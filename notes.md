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


    