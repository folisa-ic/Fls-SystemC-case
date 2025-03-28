/*****************************************************************************

  Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
  more contributor license agreements.  See the NOTICE file distributed
  with this work for additional information regarding copyright ownership.
  Accellera licenses this file to you under the Apache License, Version 2.0
  (the "License"); you may not use this file except in compliance with the
  License.  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
  implied.  See the License for the specific language governing
  permissions and limitations under the License.

 *****************************************************************************/

// 12-Jan-2009  John Aynsley  Bug fix. sc_time argument to notify should be const
// 20-Mar-2009  John Aynsley  Add cancel_all() method

/**
 * Add by zhanghaohan.zhh@bytedance.com
 * Add get_once for peq_with_get.
 * Require clock cycle of the current module.
 * Support return the num of payloads in peq
 */

#ifndef __PEQ_WITH_GET_ONCE_H__
#define __PEQ_WITH_GET_ONCE_H__

#include "sysc/kernel/sc_time.h"
#include <cassert>
#include <cstdint>
#include <systemc>
//#include <tlm>
#include <map>

template <class PAYLOAD>
class peq_with_get_once : public sc_core::sc_object
{
public:
  typedef PAYLOAD transaction_type;
  typedef std::pair<const sc_core::sc_time, transaction_type*> pair_type;

public:
  peq_with_get_once(const char* name, const sc_core::sc_time& t) : 
    sc_core::sc_object(name),
    clk_period(t) {}

  void notify(transaction_type& trans, const sc_core::sc_time& t)
  {
    m_scheduled_events.insert(pair_type(t + sc_core::sc_time_stamp(), &trans));
    m_event.notify(t);
  }

  void notify(transaction_type& trans)
  {
    m_scheduled_events.insert(pair_type(sc_core::sc_time_stamp(), &trans));
    m_event.notify(); // immediate notification
  }

  // needs to be called until it returns 0
  transaction_type* get_next_transaction()
  {
    if (m_scheduled_events.empty()) {
      return 0;
    }

    sc_core::sc_time now = sc_core::sc_time_stamp();
    if (m_scheduled_events.begin()->first <= now) {
      transaction_type* trans = m_scheduled_events.begin()->second;
      m_scheduled_events.erase(m_scheduled_events.begin());
      return trans;
    }

    m_event.notify(m_scheduled_events.begin()->first - now);

    return 0;
  }

  // get next payload, and notify next
  // don't need to be called until it returns 0
  transaction_type* get_once()
  {
    if (m_scheduled_events.empty()) {
      std::cout << "ERROR! empty but notify" << std::endl;
      assert(false);
    }

    sc_core::sc_time now = sc_core::sc_time_stamp();
    if (m_scheduled_events.begin()->first <= now) {
      transaction_type* trans = m_scheduled_events.begin()->second;
      m_scheduled_events.erase(m_scheduled_events.begin());
    
      // if empty, do not notify new event
      if (m_scheduled_events.empty()) {
        return trans;
      }
      if (m_scheduled_events.begin()->first > now) {
        m_event.notify(m_scheduled_events.begin()->first - now);
      } else {
        m_event.notify(clk_period);
      }
      return trans;
    } else {
      std::cout << "ERROR! notify a un-ready transaction" << std::endl;
      assert(false);
    }
    return nullptr;
  }

  // the num of payload in peq
  uint32_t get_num() {
    return(m_scheduled_events.size());
  }

  sc_core::sc_event& get_event()
  {
    return m_event;
  }

  // Cancel all events from the event queue
  void cancel_all() {
    m_scheduled_events.clear();
    m_event.cancel();
  }

private:
  std::multimap<const sc_core::sc_time, transaction_type*> m_scheduled_events;
  sc_core::sc_event m_event;
  sc_core::sc_time clk_period;
};

#endif
