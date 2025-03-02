#include <systemc>
#include <iostream>
using namespace sc_core;

class ThdModule : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(ThdModule);
    
    sc_in<bool> clk;
    sc_in<bool> rst_n;

    ThdModule(sc_module_name nm, sc_clock& _clk, sc_signal<bool>& _rst_n) :
        sc_module(nm)
    {
        clk.bind(_clk);
        rst_n.bind(_rst_n);

        SC_THREAD(normThd0);
        sensitive << clk.posedge_event();

        SC_THREAD(normThd1);
        dont_initialize();

        SC_THREAD(normThd2);
        async_reset_signal_is(rst_n, false);
        
        SC_THREAD(normThd3);
        sensitive << normEvt3;
        dont_initialize();

        SC_CTHREAD(cThd0, clk.pos());

        SC_CTHREAD(cThd1, clk.pos());
        dont_initialize();

        SC_CTHREAD(cThd2, clk.pos());
        reset_signal_is(rst_n, false);

    }

    sc_event normEvt3;
    
    void normThd0() {
        std::cout << "normThd0 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait();
            std::cout << "normThd0 after wait at " << sc_time_stamp() << std::endl;
            normEvt3.notify();
        }
    }

    void normThd1() {
        std::cout << "normThd1 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait(clk.posedge_event());
            std::cout << "normThd1 after wait at " << sc_time_stamp() << std::endl;
        }
    }

    void normThd2() {
        std::cout << "normThd2 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait(clk.posedge_event());
            std::cout << "normThd2 after wait at " << sc_time_stamp() << std::endl;
        }
    }

    void normThd3() {
        std::cout << "normThd3 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait();
            std::cout << "normThd3 after wait at " << sc_time_stamp() << std::endl;
        }
    }

    void cThd0() {
        std::cout << "cThd0 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait();
            std::cout << "cThd0 after wait at " << sc_time_stamp() << std::endl;
        }
    }
    
    void cThd1() {
        std::cout << "cThd1 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait();
            std::cout << "cThd1 after wait at " << sc_time_stamp() << std::endl;
        }
    }

    void cThd2() {
        std::cout << "cThd2 before while at " << sc_time_stamp() << std::endl;
        while (true) {
            wait();
            std::cout << "cThd2 after wait at " << sc_time_stamp() << std::endl;
        }
    }
};
