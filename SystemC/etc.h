#ifndef ETC_H
#define ETC_H

#include <systemc.h>

SC_MODULE(etc) {
    // sc_port<sc_signal_in_if<bool>> clk;
    sc_port<sc_signal_in_if<bool>> sensor1;
    sc_port<sc_signal_in_if<bool>> sensor2;
    sc_port<sc_signal_in_if<bool>> sensor3;
    sc_port<sc_signal_in_if<int>>  validEpass;
    sc_port<sc_signal_in_if<bool>> enable;
    sc_port<sc_signal_out_if<bool>>barrier;
    sc_port<sc_signal_out_if<float>> speed;

    sc_event read_sensor1;
    sc_event read_sensor2;
    sc_event read_sensor3;
    sc_event read_sensor1_pos;
    sc_event read_sensor2_pos;
    sc_event read_sensor3_pos;
    sc_event read_sensor3_neg;
    sc_event read_Epass;
    sc_event read_Enable;

    static int car1;
    static int car2;
    static int car3;

    enum STATE {START_ST, COUNT_TIME_ST, CALC_ST};

    SC_CTOR(etc) {
        // SC_CTHREAD(cthread, clk->posedge_event());
        SC_THREAD(cthread);
        SC_THREAD(check_sensor1);
        SC_THREAD(check_sensor2);
        SC_THREAD(check_sensor3);
        SC_THREAD(check_sensor1_pos);
        SC_THREAD(check_sensor2_pos);
        SC_THREAD(check_sensor3_pos);
        SC_THREAD(check_sensor3_neg);
        SC_THREAD(check_Epass);
        SC_THREAD(check_Enable);
        SC_THREAD(down);
        // SC_THREAD(incCar1);
        // SC_THREAD(incCar2);
        // SC_THREAD(incCar3);
    }

    void check_sensor1();
    void check_sensor2();
    void check_sensor3();
    void check_sensor1_pos();
    void check_sensor2_pos();
    void check_sensor3_pos();
    void check_sensor3_neg();
    void check_Epass();
    void check_Enable();
    void cthread();
    void down();
    // void incCar1();
    // void incCar2();
    // void incCar3();

};

#endif