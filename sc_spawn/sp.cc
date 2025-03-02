#include <systemc>
#include "sysc/kernel/sc_dynamic_processes.h"
#include <iostream>
#include <vector>
using namespace sc_core;

class Module : public sc_module {
public:
    SC_HAS_PROCESS(Module);
    std::vector<sc_event> evtVec;

    Module(sc_module_name nm) :
        sc_module(nm),
        evtVec(3)
    {
        SC_THREAD(notifyEvt);
    }

    void end_of_elaboration() override {
        for (int i = 0 ; i < evtVec.size() ; i++) {
            sc_spawn_options opts;
            opts.set_sensitivity(&evtVec.at(i));
            // opts.dont_initialize();
            sc_process_handle handle = sc_spawn(sc_bind(&Module::sp_thread, this, i), 
                    sc_core::sc_gen_unique_name("evtThread"), &opts);
            std::cout << "create process: " << handle.name() << std::endl;
        }
    }

private:
    void notifyEvt() {
        for (int i = 0 ; i < evtVec.size() ; i++) {
            evtVec.at(i).notify(i + 1, SC_NS);
        }
    }

    void sp_thread(uint tag) {
        wait();
        std::cout << "event_id: " << tag << " at " << sc_time_stamp() << std::endl;
    }
};

int sc_main(int, char*[]) {
    Module test_module("test_module");
    sc_start(50, SC_NS);
    return 0;
}
