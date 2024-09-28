int check_assign(node_t* expr) {
	switch (expr->annot.type) {
	case TYPE_INT:
	case TYPE_STR:
	case TYPE_RSTR:
	case TYPE_EXPR:
	case TYPE_ACCESS:
	case TYPE_RINT:
		break;
	default:
		verror("assign bad value");
		break;
	}
}

int check_args(node_t* expr) {
	switch (expr->annot.type) {
	case TYPE_INT:
	case TYPE_STR:
	case TYPE_RINT:
	case TYPE_RSTR:
	case TYPE_VAR:
		break;
	default:
		verror("bad args");
		break;
	}
}
