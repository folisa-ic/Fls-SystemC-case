#include <cstdint>
#include <systemc>
#include "tlm_utils/simple_initiator_socket.h"
#include "peq_with_get_once.h"
#include "ext.hh"
#include "fls_proto_types.hh"
 
class masterNb : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(masterNb);

    /** be notified when slave finish request */
    sc_core::sc_event slvEndReqEvt;
    /** initiator port */
    // tlm_utils::simple_initiator_socket<masterNb> initPort;
    // add self-defined protocol as template parameter to instantiate socket
    tlm_utils::simple_initiator_socket<masterNb, 32, fls::fls_protocol_types> initPort;

    /** 
     * payload event queue with get. As the container of payload
     * when RSP_VLD, the notify process will call SendEndRespThread
     */
    peq_with_get_once<tlm::tlm_generic_payload> pldEvtQ;

    masterNb(sc_core::sc_module_name name) : 
        sc_core::sc_module(name),
        initPort("initiator_port"), 
        pldEvtQ("peq", sc_core::sc_time(10, sc_core::SC_NS)) {

        // initiate write data area with 1000 data beat
        // (uint32_t), as 4000 uint8_t/unsigned char
        wrDataArea = new unsigned char[4000];
        uint32_t* init_ptr = (uint32_t*)wrDataArea;
        for (int i = 0 ; i < 1000 ; i++) {
            *init_ptr = i + 0xabcd0000;
            init_ptr++;
        }

        // register a backward callback func for initPort
        initPort.register_nb_transport_bw(this, &masterNb::nb_transport_bw_func);
        
        SC_THREAD(sendReqThread);
        
        // send RSP_RDY when the first RSP_VLD payload is ready
        SC_THREAD(sendEndRespThread);
        sensitive << pldEvtQ.get_event();
    }

    /** write data area */
    unsigned char* wrDataArea;
    /** read data area */
    unsigned char* rdDataArea;

    /** 
     * initiate a transaction and call nb_transport_fw to send it, 
     * be scheduled when recv a REQ_RDY from slave (after 1 cycle)
     */
    void sendReqThread();
 
    /**
     * non-blocking transport backward callback func.
     * as the callback func when initiator need to handle 
     * non-blocking backward transport (recv REQ_RDY & RSP_VLD) 
     * from the slave (slave call nb_transport_bw).
     *
     * @param payload the payload of this transaction
     * @param phase the phase of this transaction, has been updated by slave
     * @param delay the local time attached by slave
     *
     * @return always return tlm::TLM_ACCEPTED
     */
    tlm::tlm_sync_enum 
    nb_transport_bw_func(tlm::tlm_generic_payload &payload, tlm::tlm_phase &phase, sc_core::sc_time &delay);

    /**
     * be called when initiator recv a RSP_VLD.
     *
     * @note sending all avaliable RSP_RDY each time
     * do not conform to actual cicuit design
     */
    void sendEndRespThread();
};
