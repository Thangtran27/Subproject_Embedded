#include <systemc.h>
#include "etc.h"

int etc::car1 = 0;
int etc::car2 = 0;
int etc::car3 = 0;

void etc::check_sensor1_pos() {
    while (true)
    {
        wait(sensor1->posedge_event()); 
        {
            read_sensor1_pos.notify();
            car1++;
            cout << "car1: " << car1 << endl;
            wait(100, SC_MS);
            read_sensor1_pos.cancel();
        }
    }
}

void etc::check_sensor2_pos() {
    while (true)
    {
        wait(sensor2->posedge_event()); 
        {
            read_sensor2_pos.notify();
            car2++;
            cout << "car2: " << car2 << endl;
            wait(100, SC_MS);
            read_sensor2_pos.cancel();
        }
    }
}

void etc::check_sensor3_pos() {
    while (true)
    {
        wait(sensor3->posedge_event()); 
        {
            read_sensor3_pos.notify();
            car3++;
            cout << "car3: " << car3 << endl;
            wait(100, SC_MS);
            read_sensor3_pos.cancel();
        }
    }
}

void etc::check_sensor3_neg() {
    while (true)
    {
        wait(sensor3->negedge_event()); 
        {
            read_sensor3_neg.notify();
            wait(100, SC_MS);
            read_sensor3_neg.cancel();
        }
    }
}

void etc::check_sensor1() {
    while(true) {
        sensor1->read();
        read_sensor1.notify();
        wait(100, SC_MS);
        read_sensor1.cancel();
    }
}

void etc::check_sensor2() {
    while(true) {
        sensor2->read();
        //cout << "Read data from sensor2" << endl;
        read_sensor2.notify();
        wait(100, SC_MS);
        read_sensor2.cancel();
    }
}

void etc::check_sensor3() {
    while(true) {
        sensor3->read();
        //cout << "Read data from sensor3" << endl;
        read_sensor3.notify();
        wait(100, SC_MS);
        read_sensor3.cancel();
    }
}

void etc::check_Epass() {
    while(true) {
        validEpass->read();
        //cout << "Read data from validEpass" << endl;
        read_Epass.notify();
        wait(100, SC_MS);
        read_Epass.cancel();
    }
}

void etc::check_Enable() {
    while(true) {
        enable->read();
        //cout << "Read data from enable" << endl;
        read_Enable.notify();
        wait(100, SC_MS);
        read_Enable.cancel();
    }
}

void etc::down() {
    while (true)
    {
        wait(read_sensor3_neg);
        if (car1 == car3)
        {
            cout << "Turn off Barrier" << endl;
        }
    }
    
}

void etc::cthread() {
        sc_time start(1, SC_MS);
        sc_time stop(1, SC_MS);
        sc_time period(1, SC_MS);
        STATE state = START_ST;
        float time = 0;

        while (true)
        {
            switch (state)
            {
                case START_ST:
                    wait(read_sensor1);
                    if (sensor1->read() & (car1!=car2))
                    {
                        start = sc_time_stamp();
                        cout << "Sensor1 has a passing vehicle at " << start << endl;
                        state = COUNT_TIME_ST;
                    }
                    else
                    {
                        state = START_ST;
                    }
                    
                    break;
                case COUNT_TIME_ST:
                    wait(read_sensor2);
                    if (sensor2->read())
                    {
                        stop = sc_time_stamp();
                        period = stop - start;
                        time = period.to_seconds();
                        cout << "Sensor2 has a passing vehicle at " << stop << endl;
                        float sp = 8*3.6 / time;
                        speed->write(sp);
                        cout << "Speed: " << sp << "km/h" << endl;
                        state = CALC_ST;
                    }
                    else
                    {
                        state = COUNT_TIME_ST;
                    }
                    
                    break;

                case CALC_ST:
                    wait(read_Epass);
                    if (validEpass->read() == 2)
                    {
                        cout << "Valid Epass" << endl;
                        cout << "Turn on Barrier" << endl;
                        barrier->write(1);
                        state = START_ST;
                    }
                    else if (validEpass->read() == 1)
                    {
                        cout << "Illegal ePass" << endl;
                        WAIT_EN:
                        wait(read_Enable);
                        if (enable->read())
                        {
                            cout << "Turn on barrier from Enable Signal" << endl;
                            WAIT_DIS:
                            wait(read_Enable);
                            if (!enable->read())
                            {
                                state = START_ST;
                                cout << "Turn off barrier from Enable Signal" << endl;
                            }
                            else
                            {
                                goto WAIT_DIS;
                            }
                            
                        }
                        else 
                        {
                            goto WAIT_EN;
                        }
                    }
                    else 
                    {
                        state = CALC_ST;
                    }
                    break;

                default:
                    break;
            }
        
        }
}

SC_MODULE(testbench) {
    sc_port<sc_signal_out_if<bool>> sensor1;
    sc_port<sc_signal_out_if<bool>> sensor2;
    sc_port<sc_signal_out_if<bool>> sensor3;
    sc_port<sc_signal_out_if<int>> validEpass;
    sc_port<sc_signal_out_if<bool>> enable;

    SC_CTOR(testbench) {
        SC_THREAD(simulation);
    }

    void simulation() {
        // valid Epass
        cout << "Case 1: Valid Epass" << endl;
        sensor1->write(0);
        sensor2->write(0);
        sensor3->write(0);
        validEpass->write(0);
        enable->write(0);

        wait(2, SC_SEC);

        sensor1->write(1);
        wait(2, SC_SEC);
        sensor2->write(1);
        validEpass->write(2);
        wait(1, SC_SEC);
        sensor1->write(0);
        wait(2, SC_SEC);
        sensor2->write(0);
        sensor3->write(1);
        
        cout << "Case 2: Valid Epass" << endl;
        sensor1->write(1);
        wait(3, SC_SEC);
        sensor3->write(0);

       // valid Epass
        sensor2->write(0);
        sensor3->write(0);
        validEpass->write(0);
        enable->write(0);

        wait(2, SC_SEC);
        sensor2->write(1);
        validEpass->write(2);
        wait(1, SC_SEC);
        sensor1->write(0);
        wait(2, SC_SEC);
        sensor2->write(0);
        sensor3->write(1);
        wait(3, SC_SEC);
        sensor3->write(0);

        wait(4, SC_SEC);
        cout << "Case 3: Illegal Epass" << endl;
        sensor1->write(0);
        sensor2->write(0);
        sensor3->write(0);
        validEpass->write(0);
        enable->write(0);

        wait(2, SC_SEC);

        sensor1->write(1);
        wait(2, SC_SEC);
        sensor2->write(1);
        validEpass->write(1);
        wait(1, SC_SEC);
        sensor1->write(0);
        wait(1, SC_SEC);
        enable->write(1);
        wait(1, SC_SEC);
        sensor2->write(0);
        sensor3->write(1);
        wait(3, SC_SEC);
        enable->write(0);
        sensor3->write(0);

        cout << "END!";
    }

};

int sc_main(int, char*[])
{
    testbench test("test");
    etc DUT("DUT");

    // sc_signal<bool>  clk;
    sc_signal<bool>  sensor1;
    sc_signal<bool>  sensor2;
    sc_signal<bool>  sensor3;
    sc_signal<int>   validEpass;
    sc_signal<bool>  enable;
    sc_signal<bool>  barrier;
    sc_signal<float> speed;    

    // DUT.clk(clk);
    DUT.sensor1(sensor1);
    DUT.sensor2(sensor2);
    DUT.sensor3(sensor3);
    DUT.validEpass(validEpass);
    DUT.enable(enable);
    DUT.barrier(barrier);
    DUT.speed(speed);

    test.sensor1(sensor1);
    test.sensor2(sensor2);
    test.sensor3(sensor3);
    test.validEpass(validEpass);
    test.enable(enable);


    sc_start(60, SC_SEC);
    // sc_start();
    return 0;
}
