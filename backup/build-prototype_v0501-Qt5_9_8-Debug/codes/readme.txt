COMPONENT_NAME	组件全名

COMP_CAPITALIZE	组件大写缩写

COMP_LAST_LOWERCASE	上一个组件的全小写缩写

COMP_THIS_LOWERCASE	当前组件的全小写缩写

MSG_Q_RECEIVE_PRE	一整句，变化较大msgQReceivePre(&gMsgDatQ, (char *)&dat, sizeof(rspl_thread_param), 0, &param_dpc, &param_slc);

COMP_UNDERLINE_ALL_LOWERCASE_ENTRY 一整句，变化较大pRecv = rspl_sidelobe_canceller_entry(&bFirst, dat.pIn, pOut);

INIT_P_ENV	给pEnv赋值

PBUF_OR_LAST_PIN	给pIn赋值

PBUF_OR_RSPL_COMM_GET_BUF	给pOut赋值，有时候是rspl_comm_get_buf(pRecv);，很麻烦

COMP_TITLE_CASE	组件首字符大写的缩写

COMP_DAT_LOWERCASE	可以直接设置成字符串"dat"

RSPL_COMP_UNDERLINE_NAME_FUNCTION_CALL	每个组件接口中参数不一样rspl_sidelobe_canceller(pEnv, slc.pIn, slc.pOut, thread_id);

COMP_NEXT_LOWERCASE	下一个组件的全小写缩写




__THREAD_NUMS__ 线程总数，等于组件的个数(generate.c)

__COMPS_LIST_BY_ORDER__ 组件列表，按照顺序，每个组件用双引号包裹，中间用逗号隔开