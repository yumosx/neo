
int main()
{
	ebpf_t *code = ebpf_new();
	int argnum = 16, i;
	int fd, max_entries = 1, ksize = 4, vsize = 16 + 16 * 1024;

	fd = bpf_map_create(BPF_MAP_TYPE_PERCPU_ARRAY, ksize, vsize, max_entries);

	ebpf_emit(code, LDXDW(BPF_REG_7, 32, BPF_REG_1));

	// 初始化对应的栈
	ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
	ebpf_emit(code, STXDW(BPF_REG_10, -8, BPF_REG_0));

	// 查找
	ebpf_emit_map_look(code, fd, -8);
	ebpf_emit(code, JMP_IMM(BPF_JNE, BPF_REG_0, 0, 2));
	ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
	ebpf_emit(code, EXIT);

	ebpf_emit(code, MOV(BPF_REG_8, BPF_REG_0));

	ebpf_emit(code, MOV(BPF_REG_1, BPF_REG_10));
	ebpf_emit(code, ALU_IMM(BPF_ADD, BPF_REG_1, -8));
	ebpf_emit(code, MOV_IMM(BPF_REG_2, 8));
	ebpf_emit(code, MOV(BPF_REG_3, BPF_REG_7));
	ebpf_emit(code, CALL(BPF_FUNC_probe_read_user));

	ebpf_emit(code, MOV(BPF_REG_1, BPF_REG_8));
	ebpf_emit(code, ALU_IMM(BPF_ADD, BPF_REG_1, 16));
	ebpf_emit(code, MOV_IMM(BPF_REG_2, 1024));
	ebpf_emit(code, LDXDW(BPF_REG_3, -8, BPF_REG_10));

	ebpf_emit(code, CALL(BPF_FUNC_probe_read_user_str));

	// init stack
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

	ebpf_emit(code, MOV_IMM(BPF_REG_0, 0));
	ebpf_emit(code, EXIT);

	bpf_probe_attach(code, 721);
	while (1)
	{
		;
	}
	return 0;
}
