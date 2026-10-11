#include "fsm_washing_machine.h"

static void UpdateOutputs(WashingMachineFSM_t *fsm) {
    if (!fsm) {
        return;
    }

    switch (fsm->currentState) {
        case STATE_STANDBY:
            fsm->rledOn = true;
            fsm->rledBlink = false;
            fsm->bledOn = false;
            fsm->bledBlink = false;
            fsm->motorOn = false;
            break;

        case STATE_READY:
            fsm->rledOn = false;
            fsm->rledBlink = false;
            fsm->bledOn = true;
            fsm->bledBlink = false;
            fsm->motorOn = false;
            break;

        case STATE_RUNNING:
            fsm->rledOn = false;
            fsm->rledBlink = false;
            fsm->bledOn = true;
            fsm->bledBlink = true;
            fsm->motorOn = true;
            break;

        case STATE_PAUSED:
            fsm->rledOn = false;
            fsm->rledBlink = false;
            fsm->bledOn = true;
            fsm->bledBlink = false;
            fsm->motorOn = false;
            break;

        case STATE_ERROR:
        default:
            fsm->rledOn = true;
            fsm->rledBlink = true;
            fsm->bledOn = false;
            fsm->bledBlink = false;
            fsm->motorOn = false;
            break;
    }
}

void FSM_Init(WashingMachineFSM_t *fsm) {
    if (!fsm) {
        return;
    }

    fsm->currentState = STATE_STANDBY;
    fsm->balance = 0U;
    fsm->timerSeconds = WASH_TIME_SEC;
    fsm->stopPressCount = 0U;
    fsm->stopTimerMs = 0U;
    UpdateOutputs(fsm);
}

void FSM_ProcessEvent(WashingMachineFSM_t *fsm, SystemEvent_t event) {
    if (!fsm) {
        return;
    }

    if (event == EVENT_FAULT_TRIGGER) {
        fsm->currentState = STATE_ERROR;
        UpdateOutputs(fsm);
        return;
    }

    if (event == EVENT_BTN_STOP) {
        fsm->stopPressCount++;
        if (fsm->stopPressCount == 1U) {
            fsm->stopTimerMs = DOUBLE_CLICK_WINDOW_MS;
        } else if (fsm->stopPressCount >= 2U) {
            fsm->stopPressCount = 0U;
            fsm->stopTimerMs = 0U;
            fsm->balance = 0U;
            fsm->timerSeconds = WASH_TIME_SEC;
            fsm->currentState = STATE_STANDBY;
            UpdateOutputs(fsm);
            return;
        }
    }

    switch (fsm->currentState) {
        case STATE_STANDBY:
            if (event == EVENT_COIN_10) {
                fsm->balance += 10U;
            } else if (event == EVENT_COIN_20) {
                fsm->balance += 20U;
            } else if (event == EVENT_COIN_50) {
                fsm->balance += 50U;
            }

            if (fsm->balance >= MIN_COIN_THRESHOLD) {
                fsm->currentState = STATE_READY;
            }
            break;

        case STATE_READY:
            if (event == EVENT_COIN_10) {
                fsm->balance += 10U;
            } else if (event == EVENT_COIN_20) {
                fsm->balance += 20U;
            } else if (event == EVENT_COIN_50) {
                fsm->balance += 50U;
            } else if (event == EVENT_BTN_RUN) {
                fsm->balance = 0U;
                fsm->timerSeconds = WASH_TIME_SEC;
                fsm->currentState = STATE_RUNNING;
            }
            break;

        case STATE_RUNNING:
            if (event == EVENT_BTN_PAUSE) {
                fsm->currentState = STATE_PAUSED;
            }
            break;

        case STATE_PAUSED:
            if (event == EVENT_BTN_RUN) {
                fsm->currentState = STATE_RUNNING;
            }
            break;

        case STATE_ERROR:
            if ((event == EVENT_FAULT_CLEAR) || (event == EVENT_BTN_STOP)) {
                fsm->currentState = STATE_STANDBY;
            }
            break;

        default:
            fsm->currentState = STATE_STANDBY;
            break;
    }

    UpdateOutputs(fsm);
}

void FSM_TimerTick_1s(WashingMachineFSM_t *fsm) {
    if (!fsm) {
        return;
    }

    if ((fsm->currentState == STATE_RUNNING) || (fsm->currentState == STATE_PAUSED)) {
        if (fsm->timerSeconds > 0U) {
            fsm->timerSeconds--;
        }

        if (fsm->timerSeconds == 0U) {
            fsm->balance = 0U;
            fsm->timerSeconds = WASH_TIME_SEC;
            fsm->currentState = STATE_STANDBY;
            UpdateOutputs(fsm);
        }
    }
}

void FSM_Tick_100ms(WashingMachineFSM_t *fsm) {
    if (!fsm) {
        return;
    }

    if (fsm->stopTimerMs > 0U) {
        if (fsm->stopTimerMs <= 100U) {
            fsm->stopTimerMs = 0U;
            fsm->stopPressCount = 0U;
        } else {
            fsm->stopTimerMs -= 100U;
        }
    }
}
