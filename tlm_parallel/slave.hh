#include <cstdint>
#include <systemc>
#include "tlm_utils/simple_target_socket.h"
#include "ext.hh"

class slaveNb : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(slaveNb);

    tlm_utils::simple_target_socket<slaveNb> targPort;
    sc_core::sc_fifo<tlm::tlm_generic_payload *> m_req_fifo;
    sc_core::sc_fifo<tlm::tlm_generic_payload *> m_resp_fifo;

    slaveNb(sc_core::sc_module_name name) : 
        sc_core::sc_module(name), 
        curReqOst(2),
        targPort("target_port_nb") {
        targPort.register_nb_transport_fw(this, &slaveNb::nb_transport_fw_func);
        SC_THREAD(sendEndReqThread);
        SC_THREAD(sendRespThread);
    }

    /**
     * non-blocking transport forward callback func.
     * As the callback func when slave need to handle
     * non-blocking forward transport (recv BEGIN_REQ & END_RESP)
     * from master (master call nb_transport_fw)
     *
     * @param payload the payload of this transaction
     * @param phase the phase of this transaction, has been updated by master
     * @param delay the local time attached by master
     *
     * @return always return tlm::TLM_ACCEPTED
     */
    tlm::tlm_sync_enum 
    nb_transport_fw_func(tlm::tlm_generic_payload &payload, tlm::tlm_phase &phase, sc_core::sc_time &delay);

    void sendEndReqThread();
    void sendRespThread();

    /** current in-flight request num */
    uint32_t curReqNum {0};
    /** current in-flight request oustanding num */
    const uint32_t curReqOst;
 
};
