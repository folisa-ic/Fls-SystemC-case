#include "tlm_core/tlm_2/tlm_generic_payload/tlm_gp.h"
#include "tlm_core/tlm_2/tlm_generic_payload/tlm_phase.h"
#include <cstdint>
#include <systemc>

#ifndef __EXTHH__
#define __EXTHH__

struct axi_protocol_types {
    typedef tlm::tlm_generic_payload tlm_payload_type;
    typedef tlm::tlm_phase tlm_phase_type;
};

class AXIAttr {
public:
    uint8_t id {0};
    uint64_t addr {0};    /**< ADDR_WIDTH from 1 to 64 */
    uint8_t len {0};      /**< 8-bit width, burst length = AxLen + 1 */
    uint8_t size;         /**< 3-bit width */
    uint8_t qos;          /**< 4-bit width */
    uint32_t user;        /**< USER_REQ_WIDTH from 0 to 128 */
    uint8_t press;        /**< 3-bit, self-defined */
};

class AXIExtension : public tlm::tlm_extension<AXIExtension> {
public:
    AXIAttr attr;
    AXIExtension() = default;

    /** create a new extension from this ext */
    virtual tlm_extension_base* clone() const override {
        AXIExtension* t = new AXIExtension;
        t->attr = this->attr;
        return t;
    }

    /** copy info from a given extension */
    virtual void copy_from(tlm_extension_base const &ext) override {
        attr = static_cast<AXIExtension const&>(ext).attr;
    }
};

#endif  // __EXTHH__
