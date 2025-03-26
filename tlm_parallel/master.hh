#include <cstdint>
#include <systemc>
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/peq_with_get.h"
#include "ext.hh"
 
class masterNb : public sc_core::sc_module {
public:
    SC_HAS_PROCESS(masterNb);

    /** be notified when slave finish request */
    sc_core::sc_event slvEndReqEvt;
    /** initiator port */
    tlm_utils::simple_initiator_socket<masterNb> initPort;

    /** 
     * payload event queue with get. As the container of payload
     * when BEGIN_RESP, the notify process will call SendEndRespThread
     */
    tlm_utils::peq_with_get<tlm::tlm_generic_payload> pldEvtQ;

    masterNb(sc_core::sc_module_name name) : 
        sc_core::sc_module(name),
        initPort("initiator_port"), 
        pldEvtQ("peq") {

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
        
        // send END_RESP when the first BEGIN_RESP payload is ready
        SC_THREAD(sendEndRespThread);
        sensitive << pldEvtQ.get_event();
    }

    /** write data area */
    unsigned char* wrDataArea;
    /** read data area */
    unsigned char* rdDataArea;

    /** 
     * initiate a transaction and call nb_transport_fw to send it, 
     * be scheduled when recv a END_REQ from slave (after 1 cycle)
     */
    void sendReqThread();
 
    /**
     * non-blocking transport backward callback func.
     * as the callback func when initiator need to handle 
     * non-blocking backward transport (recv END_REQ & BEGIN_RESP) 
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
     * be called when initiator recv a BEGIN_RESP.
     *
     * @note sending all avaliable END_RESP each time
     * do not conform to actual cicuit design
     */
    void sendEndRespThread();
};
