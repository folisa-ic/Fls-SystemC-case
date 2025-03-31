#include "master.hh"
#include <cstdint>

void 
masterNb::sendReqThread() {
    int addr_cnt {0};
    uint8_t txn_id {0};
    int data_area_cnt {0};
    unsigned char* wr_data_cursor {wrDataArea};
    tlm::tlm_phase t_phase = tlm::BEGIN_REQ;
    // sc_core::sc_time t_delay = sc_core::sc_time(1, sc_core::SC_NS);
    sc_core::sc_time t_delay = sc_core::SC_ZERO_TIME;

    while (1) {
        tlm::tlm_generic_payload *t_payload = new tlm::tlm_generic_payload();
        t_payload->set_address(0x10000 + addr_cnt);
        t_payload->set_write();
        unsigned char* wr_data_ptr = new unsigned char[4];
        memcpy(wr_data_ptr, wr_data_cursor, sizeof(uint32_t));
        t_payload->set_data_ptr(wr_data_ptr);
        t_payload->set_data_length(sizeof(uint32_t));

        AXIExtension* t_ext = new AXIExtension;
        t_ext->attr.id = txn_id;
        t_ext->attr.addr = (uint32_t)t_payload->get_address();
        t_payload->set_extension(t_ext);

        std::cout << "\033[35m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_fw, BEGIN_REQ phase, addr=0x" 
            << std::hex << t_payload->get_address()
            << " delay cycle 0" << "\033[0m" << std::endl;
        initPort->nb_transport_fw(*t_payload, t_phase, t_delay);

        // update corresponding cnt and data cursor
        txn_id++;
        addr_cnt++;
        wr_data_cursor += 4;

        // wait the finish of hsk
        wait(slvEndReqEvt);
        wait(1, sc_core::SC_NS);
    }
}

tlm::tlm_sync_enum 
masterNb::nb_transport_bw_func(tlm::tlm_generic_payload &payload, tlm::tlm_phase &phase, sc_core::sc_time &delay) {
    switch (phase) {
    case tlm::END_REQ:
        std::cout << "\033[31m" << this->name()
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_bw_func recv END_REQ phase, addr=0x" 
            << std::hex << payload.get_address() 
            << "\033[0m" << std::endl;
        slvEndReqEvt.notify(delay);
        break;
    case tlm::BEGIN_RESP:
        std::cout << "\033[31m" << this->name() 
            << " [" << sc_core::sc_time_stamp() << "]"
            << " nb_transport_bw_func recv BEGIN_RESP phase, addr=0x" 
            << std::hex << payload.get_address() 
            << "\033[0m" << std::endl;
        pldEvtQ.notify(payload, delay);
        break;
    default:
        assert(false);
    }
    return tlm::TLM_ACCEPTED;
}

void 
masterNb::sendEndRespThread() {
    tlm::tlm_generic_payload *t_get = NULL;
    tlm::tlm_phase t_phase = tlm::END_RESP;
    sc_core::sc_time t_delay = sc_core::SC_ZERO_TIME;
    while (1) {
        wait(); // wait sensitive event list

        // std::cout << "peq size (before get_once): " << pldEvtQ.get_num() << std::endl;
        t_get = pldEvtQ.get_once();
        assert(t_get != nullptr);
        std::cout << "\033[31m" << this->name() 
            << " [" << sc_core::sc_time_stamp() << "]"
            << " call nb_transport_fw, END_RESP phase, addr=0x" 
            << std::hex << t_get->get_address() 
            << "\033[0m" << std::endl;
        // std::cout << "peq size (after get_once): " << pldEvtQ.get_num() << std::endl;
        initPort->nb_transport_fw(*t_get, t_phase, t_delay);

        // release tlm_gp
        if (t_get->get_data_ptr()) {
            delete[] t_get->get_data_ptr();
        }
        t_get->set_data_ptr(nullptr);
        if (t_get->get_byte_enable_ptr()) {
            delete[] t_get->get_byte_enable_ptr();
        }
        t_get->set_byte_enable_ptr(nullptr);
        t_get->reset();     // release all extensions
        delete t_get;
        t_get = NULL;

        // in this block, must can't wait any event or cycle delay
        //  if not, the time of transaction obtained will not accurate
    }
}
