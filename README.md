# Washing Machine Control Unit (CO3053 Assignment 2)

## Overview
This project implements the coin-operated washing machine control unit as an extended finite state machine (EFSM), following the course materials in `Source` and the assignment specification.

## Hardware Configuration
- MCU: ARM Cortex-M class microcontroller
- Inputs: STOP, RUN, PAUSE buttons and 10c/20c/50c coin sensor
- Outputs: RLED, BLED, motor drive, and 30-minute countdown timer

## Project Structure
- `include/fsm_washing_machine.h` — public interface and state definitions
- `src/fsm_washing_machine.c` — transition logic, outputs, and timer behavior
- `tests/test_main.c` — unit tests verifying the state machine behavior
- `web/index.html` — browser simulator for the washing machine controller
- `docs/diagrams.md` — FSM and flowchart diagrams

## Build and Run
```bash
gcc -Wall -Wextra -Iinclude src/fsm_washing_machine.c tests/test_main.c -o fsm_test
./fsm_test
```

Open `web/index.html` in a browser to use the virtual prototype.
