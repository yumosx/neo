void free_node(node_t *node) {    
    switch (node->type) {
	case NODE_TEST:
    case NODE_PROBE:
	case NODE_KPROBE:
        free(node->probe.name);
        free_nodes(node->probe.stmts);
        free(node);
    case NODE_CALL:
        break;
    case NODE_REC:
        break;
    case NODE_STR:
        break;
    default:
		break;
	}
}
