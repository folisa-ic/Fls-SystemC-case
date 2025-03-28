#include "slave.hh"
#include "ext.hh"
#include <cassert>
#include <cstdint>
#include <iostream>

tlm::tlm_sync_enum 
slaveNb::nb_transport_fw_func(tlm::tlm_generic_payload &payload,
                                        tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    // delay in callback func will block 
    // nb_transport_fw of master, which is not recommended
    wait(delay);
    switch (phase) {
    case tlm::BEGIN_REQ:
        m_req_fifo.write(&payload);
        AXIExtension* t_ext;
        payload.get_extension(t_ext);

        std::cout << "\033[32m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_fw_func recv BEGIN_REQ phase"
            << ", txn_id=" << std::dec << (unsigned)t_ext->attr.id
            << ", addr=0x" << std::hex << payload.get_address()
            << ", wr_data=0x" << *(uint32_t*)(payload.get_data_ptr())
            << "\033[0m" << std::endl;
        break;
    case tlm::END_RESP:
        curReqNum--;
        assert(curReqNum >= 0);
        std::cout << "\033[32m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_fw_func recv END_RESP phase, addr=0x" 
            << std::hex << payload.get_address()
            << ", current req num is " << curReqNum
            << "\033[0m" << std::endl;
        break;
    default:
        assert(false);
    }
    return tlm::TLM_ACCEPTED;
}

void 
slaveNb::sendEndReqThread() {
    tlm::tlm_phase t_phase = tlm::END_REQ;

    sc_core::sc_time t_delay {sc_core::SC_ZERO_TIME};
    while (1) {
        tlm::tlm_generic_payload *t_payload = m_req_fifo.read();
        // if current req num achieve max outstanding,
        // do not handshake with master in req channel
        while (true) {
            if (curReqNum >= curReqOst) {
                std::cout << "\033[90m" << this->name()
                    << " [" << sc_core::sc_time_stamp() << "]"
                    << " current req num has achieved max outstanding, pause receiving req" 
                    << "\033[0m" << std::endl;
                wait(1, sc_core::SC_NS);
            } else {
                // std::cout << "\033[32m" << this->name()
                //     << " [" << sc_core::sc_time_stamp() << "]"
                //     << " current req num is lower than max outstanding, start receiving req" 
                //     << "\033[0m" << std::endl;
                break;
            }
        }
        curReqNum++;

        /***********************************/
        // here can add your block logic, 
        // for back pressure use
        /***********************************/

        std::cout << "\033[32m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_bw, END_REQ phase, addr=0x" 
            << std::hex << t_payload->get_address()
            << "\033[0m" << std::endl;
        targPort->nb_transport_bw(*t_payload, t_phase, t_delay);

        // after END_REQ phase, indicate slave recv req successfully,
        // then handle req, return BEGIN_RESP
        m_resp_fifo.write(t_payload);
        wait(1, sc_core::SC_NS);
    }
}

void 
slaveNb::sendRespThread() {
    tlm::tlm_phase t_phase = tlm::BEGIN_RESP;
    sc_core::sc_time t_delay = sc_core::sc_time(10, sc_core::SC_NS);
    while (1) {
        tlm::tlm_generic_payload *t_payload = m_resp_fifo.read();
        wait(1, sc_core::SC_NS);
        std::cout << "\033[32m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_bw, BEGIN_RESP phase, addr=0x" 
            << std::hex << t_payload->get_address()
            << " delay cycle 10" << "\033[0m" << std::endl;
        targPort->nb_transport_bw(*t_payload, t_phase, t_delay);
    }
}
