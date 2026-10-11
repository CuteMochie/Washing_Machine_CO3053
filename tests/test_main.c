#include <stdio.h>
#include <assert.h>
#include "fsm_washing_machine.h"

static void assert_state_outputs(const WashingMachineFSM_t *fsm,
                                 MachineState_t expected_state,
                                 bool expected_rled_on,
                                 bool expected_rled_blink,
                                 bool expected_bled_on,
                                 bool expected_bled_blink,
                                 bool expected_motor_on) {
    assert(fsm->currentState == expected_state);
    assert(fsm->rledOn == expected_rled_on);
    assert(fsm->rledBlink == expected_rled_blink);
    assert(fsm->bledOn == expected_bled_on);
    assert(fsm->bledBlink == expected_bled_blink);
    assert(fsm->motorOn == expected_motor_on);
}

int main(void) {
    WashingMachineFSM_t fsm;
    int test_count = 0;

    printf("[EXTENDED UNIT TESTS - 25 CASES]\n");

    /* TC-01: Initial state */
    FSM_Init(&fsm);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-02: Accept 10c in standby */
    FSM_ProcessEvent(&fsm, EVENT_COIN_10);
    assert(fsm.balance == 10U);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-03: Accept 20c then 20c stays below threshold */
    FSM_ProcessEvent(&fsm, EVENT_COIN_20);
    FSM_ProcessEvent(&fsm, EVENT_COIN_20);
    assert(fsm.balance == 50U);
    assert_state_outputs(&fsm, STATE_READY, false, false, true, false, false);
    test_count++;

    /* TC-04: Ready state accepts extra 20c */
    FSM_ProcessEvent(&fsm, EVENT_COIN_20);
    assert(fsm.balance == 70U);
    assert_state_outputs(&fsm, STATE_READY, false, false, true, false, false);
    test_count++;

    /* TC-05: Start wash cycle from ready */
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    assert(fsm.balance == 0U);
    assert(fsm.timerSeconds == 1800U);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-06: Single STOP press does not stop running cycle */
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-07: Double STOP within window forces stop */
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    assert(fsm.balance == 0U);
    assert(fsm.timerSeconds == 1800U);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-08: Pause during running */
    FSM_ProcessEvent(&fsm, EVENT_COIN_50);
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    FSM_ProcessEvent(&fsm, EVENT_BTN_PAUSE);
    assert_state_outputs(&fsm, STATE_PAUSED, false, false, true, false, false);
    test_count++;

    /* TC-09: Timer decrements during pause */
    FSM_TimerTick_1s(&fsm);
    assert(fsm.timerSeconds == 1799U);
    assert_state_outputs(&fsm, STATE_PAUSED, false, false, true, false, false);
    test_count++;

    /* TC-10: Resume from pause */
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-11: Timer decrements during running */
    FSM_TimerTick_1s(&fsm);
    assert(fsm.timerSeconds == 1798U);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-12: Auto-stop at zero timer */
    fsm.timerSeconds = 1U;
    FSM_TimerTick_1s(&fsm);
    assert(fsm.currentState == STATE_STANDBY);
    assert(fsm.balance == 0U);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-13: Fault trigger sets error state */
    FSM_ProcessEvent(&fsm, EVENT_FAULT_TRIGGER);
    assert_state_outputs(&fsm, STATE_ERROR, true, true, false, false, false);
    test_count++;

    /* TC-14: Fault clear resets to standby */
    FSM_ProcessEvent(&fsm, EVENT_FAULT_CLEAR);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-15: Standby ignores invalid event */
    FSM_ProcessEvent(&fsm, EVENT_NONE);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-16: Ready accepts 50c and stays in ready */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_COIN_50);
    assert(fsm.balance == 50U);
    assert_state_outputs(&fsm, STATE_READY, false, false, true, false, false);
    test_count++;

    /* TC-17: Ready transitions to running on RUN */
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-18: Running ignores coin events */
    FSM_ProcessEvent(&fsm, EVENT_COIN_10);
    assert(fsm.balance == 0U);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-19: Paused accepts RUN to resume */
    FSM_ProcessEvent(&fsm, EVENT_BTN_PAUSE);
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-20: Running ignores RUN event */
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    assert_state_outputs(&fsm, STATE_RUNNING, false, false, true, true, true);
    test_count++;

    /* TC-21: STOP double-click in ready resets to standby */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_COIN_50);
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-22: STOP countdown window decrements after 100 ms */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    FSM_Tick_100ms(&fsm);
    assert(fsm.stopPressCount == 1U);
    assert(fsm.stopTimerMs == 1400U);
    test_count++;

    /* TC-23: STOP countdown window expires and resets after final tick */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_BTN_STOP);
    fsm.stopTimerMs = 100U;
    FSM_Tick_100ms(&fsm);
    assert(fsm.stopPressCount == 0U);
    assert(fsm.stopTimerMs == 0U);
    test_count++;

    /* TC-24: Timer resets to 1800s after normal completion */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_COIN_50);
    FSM_ProcessEvent(&fsm, EVENT_BTN_RUN);
    fsm.timerSeconds = 1U;
    FSM_TimerTick_1s(&fsm);
    assert(fsm.timerSeconds == 1800U);
    assert_state_outputs(&fsm, STATE_STANDBY, true, false, false, false, false);
    test_count++;

    /* TC-25: Standby + 50c then extra coins accumulate */
    FSM_Init(&fsm);
    FSM_ProcessEvent(&fsm, EVENT_COIN_20);
    FSM_ProcessEvent(&fsm, EVENT_COIN_20);
    FSM_ProcessEvent(&fsm, EVENT_COIN_10);
    assert(fsm.balance == 50U);
    FSM_ProcessEvent(&fsm, EVENT_COIN_10);
    assert(fsm.balance == 60U);
    assert_state_outputs(&fsm, STATE_READY, false, false, true, false, false);
    test_count++;

    printf("ALL %d TEST CASES PASSED SUCCESSFULLY.\n", test_count);
    return 0;
}
