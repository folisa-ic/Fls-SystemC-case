#include "tlm_core/tlm_2/tlm_generic_payload/tlm_gp.h"
#include "tlm_core/tlm_2/tlm_generic_payload/tlm_phase.h"

#ifndef __FLS_PROTO_TYPES_HH__
#define __FLS_PROTO_TYPES_HH__

namespace fls {

struct fls_protocol_types {
    typedef tlm::tlm_generic_payload tlm_payload_type;
    typedef tlm::tlm_phase tlm_phase_type;
};

DECLARE_EXTENDED_PHASE(REQ_VLD);
DECLARE_EXTENDED_PHASE(REQ_RDY);
DECLARE_EXTENDED_PHASE(RSP_VLD);
DECLARE_EXTENDED_PHASE(RSP_RDY);

}   // namespace fls

#endif // __FLS_PROTO_TYPES_HH__
