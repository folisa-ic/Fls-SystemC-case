#include "master.hh"
#include "sysc/datatypes/fx/sc_fxdefs.h"
#include <cstdint>

void 
masterNb::sendReqThread() {
    int addr_cnt {0};
    uint8_t txn_id {0};
    int data_area_cnt {0};
    unsigned char* wr_data {wrDataArea};
    tlm::tlm_phase t_phase = fls::REQ_VLD;
    // sc_core::sc_time t_delay = sc_core::sc_time(1, sc_core::SC_NS);
    sc_core::sc_time t_delay = sc_core::SC_ZERO_TIME;

    while (1) {
        tlm::tlm_generic_payload *t_payload = new tlm::tlm_generic_payload();
        t_payload->set_address(0x10000 + addr_cnt);
        t_payload->set_write();
        t_payload->set_data_ptr(wr_data);

        AXIExtension* t_ext = new AXIExtension;
        t_ext->attr.id = txn_id;
        t_ext->attr.addr = (uint32_t)t_payload->get_address();
        t_payload->set_extension(t_ext);

        std::cout << "\033[35m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_fw, REQ_VLD phase, addr=0x" 
            << std::hex << t_payload->get_address()
            << " delay cycle 0" << "\033[0m" << std::endl;
        initPort->nb_transport_fw(*t_payload, t_phase, t_delay);

        // update corresponding cnt and data
        txn_id++;
        addr_cnt++;
        wr_data += 4;

        // wait the finish of hsk
        wait(slvEndReqEvt);
        wait(1, sc_core::SC_NS);
    }
}

tlm::tlm_sync_enum 
masterNb::nb_transport_bw_func(tlm::tlm_generic_payload &payload, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    // case value must be a constant expression,
    // do not use switch-case here
    if (phase == fls::REQ_RDY) {
        std::cout << "\033[31m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_bw_func recv REQ_RDY phase, addr=0x" 
            << std::hex << payload.get_address() 
            << "\033[0m" << std::endl;
        slvEndReqEvt.notify(delay);
    } else {
        assert(phase == fls::RSP_VLD);
        std::cout << "\033[31m" << this->name() 
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_bw_func recv RSP_VLD phase, addr=0x" 
            << std::hex << payload.get_address() 
            << "\033[0m" << std::endl;
        pldEvtQ.notify(payload, delay);
    }
    return tlm::TLM_ACCEPTED;
}

void 
masterNb::sendEndRespThread() {
    tlm::tlm_generic_payload *t_get = NULL;
    tlm::tlm_phase t_phase = fls::RSP_RDY;
    sc_core::sc_time t_delay = sc_core::SC_ZERO_TIME;
    while (1) {
        wait(); // wait sensitive event list

        // std::cout << "peq size (before get_once): " << pldEvtQ.get_num() << std::endl;
        t_get = pldEvtQ.get_once();
        assert(t_get != nullptr);
        std::cout << "\033[31m" << this->name() 
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_fw, RSP_RDY phase, addr=0x" 
            << std::hex << t_get->get_address() 
            << "\033[0m" << std::endl;
        // std::cout << "peq size (after get_once): " << pldEvtQ.get_num() << std::endl;
        initPort->nb_transport_fw(*t_get, t_phase, t_delay);
        t_get = NULL;

        // in this block, must can't wait any event or cycle delay
        //  if not, the time of transaction obtained will not accurate
    }
}
