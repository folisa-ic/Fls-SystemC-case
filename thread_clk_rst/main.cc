#include "thd.hh"

int sc_main(int argc, char* argv[]) {
    sc_clock clk_sig("clk_sig", 10, SC_NS); 
    sc_signal<bool> rst_n_sig;
    ThdModule thdModule("thd_module", clk_sig, rst_n_sig);
    
    rst_n_sig = false;
    sc_start(15, SC_NS);
    std::cout << "========== disable reset signal at " << sc_time_stamp() << "==========" << std::endl;
    rst_n_sig = true;
    sc_start(40, SC_NS);
    rst_n_sig = false;
    std::cout << "========== enable reset signal at " << sc_time_stamp() << "==========" << std::endl;
    sc_start(25, SC_NS);

    return 0;
}
