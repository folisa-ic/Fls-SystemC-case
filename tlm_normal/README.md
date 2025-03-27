* 不建议使用 peq_with_get，因为其 get_next_transaction 要求一次性 paralle 地取出全部 ready 的 payload，这与正常的电路设计不相符；
* slave 侧在 nb_transport_fw_func 中执行了 wait(delay) 操作，这会 block master 侧的 nb_transport_fw，不符合 non-blocking 传输的理念；

