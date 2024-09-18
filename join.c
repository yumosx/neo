int main()
{
    node_t *node;
    evhandler_t *evh;

    node = node_new(NODE_CALL);

    evh = vcalloc(1, sizeof(*evh));
    evh->priv = node;
    evh->handle = print_join_string;
    evhandler_register(evh);

    ebpf_t *code = ebpf_new();
    evpipe_init(code->evp, 4 << 10);
    int argnum = 16, i;
    int fd, max_entries = 1, ksize = 4, vsize = 16 + 16 * 1024;

    fd = bpf_map_create(BPF_MAP_TYPE_PERCPU_ARRAY, ksize, vsize, max_entries);

    ebpf_emit(code, MOV(BPF_REG_9, BPF_REG_1));
    ebpf_emit(code, LDXDW(BPF_REG_7, 32, BPF_REG_1));

    ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
    ebpf_emit(code, STXDW(BPF_REG_10, -8, BPF_REG_0));

    //=============================================
    ebpf_emit_map_look(code, fd, -8);

    ebpf_emit(code, MOV(BPF_REG_8, BPF_REG_0));
    ebpf_emit(code, JMP_IMM(BPF_JNE, BPF_REG_8, 0, 2));
    ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
    ebpf_emit(code, EXIT);

    //=================init type============================
    ebpf_emit(code, MOV_IMM(BPF_REG_1, evh->type));
    ebpf_emit(code, STXDW(BPF_REG_8, 8, BPF_REG_1));

    //=================read arg0============================
    ebpf_emit_read_user(code, -8, 8, BPF_REG_7);
    ebpf_emit_read_user_string(code, BPF_REG_8, 16, -8, 1024);

    //=============================================
    ebpf_emit(code, MOV(BPF_REG_6, BPF_REG_10));
    ebpf_emit(code, ALU_IMM(BPF_ADD, BPF_REG_6, -24));

    for (i = 1; i < 16; i++)
    {
        // read user to stack
        ebpf_emit(code, MOV(BPF_REG_1, BPF_REG_6));
        ebpf_emit(code, MOV_IMM(BPF_REG_2, 8));
        ebpf_emit(code, MOV(BPF_REG_3, BPF_REG_7));
        ebpf_emit(code, ALU_IMM(BPF_ADD, BPF_REG_3, i * 8));
        ebpf_emit(code, CALL(BPF_FUNC_probe_read_user));

        // read stack to user
        ebpf_emit(code, LDXDW(BPF_REG_3, -24, BPF_REG_10));
        ebpf_emit(code, MOV(BPF_REG_1, BPF_REG_8));
        ebpf_emit(code, ALU_IMM(BPF_ADD, BPF_REG_1, 16 + i * 1024));
        ebpf_emit(code, MOV_IMM(BPF_REG_2, 1024));
        ebpf_emit(code, CALL(BPF_FUNC_probe_read_user_str));
    }

    // out
    ebpf_emit(code, CALL(BPF_FUNC_get_smp_processor_id));
    ebpf_emit(code, MOV(BPF_REG_3, BPF_REG_0));

    ebpf_emit(code, MOV(BPF_REG_1, BPF_REG_9));
    ebpf_emit_mapld(code, BPF_REG_2, code->evp->mapfd);
    ebpf_emit(code, MOV(BPF_REG_4, BPF_REG_8));

    ebpf_emit(code, MOV_IMM(BPF_REG_5, 4032));
    ebpf_emit(code, CALL(BPF_FUNC_perf_event_output));

    ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
    ebpf_emit(code, EXIT);

    bpf_probe_attach(code, 721);

    siginterrupt(SIGINT, 1);
    signal(SIGINT, term);
    evpipe_loop(code->evp, &term_sig, -1);
}
