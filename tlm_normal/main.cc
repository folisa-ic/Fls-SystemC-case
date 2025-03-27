#include "master.hh"
#include "slave.hh"
 
class Top : public sc_core::sc_module {
public:
    Top(sc_core::sc_module_name name) : 
        sc_core::sc_module(name),
        master("test_master"),
        slave("test_slave") {
        slave.targPort.bind(master.initPort);
    }
 
    masterNb master;
    slaveNb slave;
};
 
int sc_main(int argc, char **argv) {
  Top top_module("top_module_nb");
  sc_core::sc_start(100, sc_core::SC_NS);
  return 0;
}
 
 
