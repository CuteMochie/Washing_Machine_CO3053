#ifndef FSM_WASHING_MACHINE_H
#define FSM_WASHING_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

#define WASH_TIME_SEC          1800U
#define MIN_COIN_THRESHOLD     50U
#define DOUBLE_CLICK_WINDOW_MS 1500U

typedef enum {
    STATE_STANDBY = 0,
    STATE_READY,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_ERROR
} MachineState_t;

typedef enum {
    EVENT_NONE = 0,
    EVENT_COIN_10,
    EVENT_COIN_20,
    EVENT_COIN_50,
    EVENT_BTN_RUN,
    EVENT_BTN_PAUSE,
    EVENT_BTN_STOP,
    EVENT_FAULT_TRIGGER,
    EVENT_FAULT_CLEAR
} SystemEvent_t;

typedef struct {
    MachineState_t currentState;
    uint16_t balance;
    uint16_t timerSeconds;
    bool motorOn;
    bool rledOn;
    bool rledBlink;
    bool bledOn;
    bool bledBlink;
    uint8_t stopPressCount;
    uint16_t stopTimerMs;
} WashingMachineFSM_t;

void FSM_Init(WashingMachineFSM_t *fsm);
void FSM_ProcessEvent(WashingMachineFSM_t *fsm, SystemEvent_t event);
void FSM_TimerTick_1s(WashingMachineFSM_t *fsm);
void FSM_Tick_100ms(WashingMachineFSM_t *fsm);

#endif /* FSM_WASHING_MACHINE_H */
