/*
1. 首先是变量的右值处理:
- `a := 1;`   a是左值, 1是右值
- `a := 1 + 2` a是左值, `1+2`是右值
- `a := "wz"` a是左值, `"wz"`是右值
- `a := pid()` a是左边值  `pid`是右边的值

右值分配策略:
- 如果右边的值是整数、表达式或者返回整数的函数，我们首先会分配一个寄存器, 用这个寄存器存储表达式的结果或者值, 然后把使用这个寄存器移动到栈上面
- 如果右边是字符串或者函数返回字符串, 我们直接编译右值，然后将对应的值放到栈上面去
- 如果右边是变量, 会讲变量的值从一个栈的一个位置移动到另外一个位置上去

2. 函数的参数分配策略, 首先我们默认给函数参数分配到栈上面去, 比如:

比如 `out("%d\n", 1+2);`
- 我们首先会给函数参数分配一个栈地址, 比如`-8`
- 首先将`1+2`的结果分配到寄存器上去
- 最终会把寄存器的值移动到栈上面去
*/

//- 定义类型枚举：定义一个枚举类型来区分不同的右值类型，以及对应的内存位置
typedef enum {
    TYPE_INT,
    TYPE_STR,
    TYPE_VAR,
    TYPE_INT_FUNC,
    TYPE_STR_FUNC,
    TYPE_EXPR,
    TYPE_MAP,
} ValueType;

typedef enum {
    LOC_STACK,
    LOC_REG,
    LOC_SYM,
} Loc;

Loc determine_location(ValueType type) {
    switch (type) {
    case TYPE_INT:
    case TYPE_EXPR:
    case TYPE_INT_FUNC:
        return LOC_REG;
    case TYPE_STR:
    case TYPE_STR_FUNC:
        return LOC_STACK;
    case TYPE_VAR:  //这种情况
        return LOC_SYM;
    default:
        return LOC_REG;
    }
}


void handle_int_or_expr(node_t* src, node_t* dst) {
    reg_t* r1 = gen_expr(src);
    init(dst);
    store(dst, r1);
}

void handle_str_or_rstr(node_t* src, node_t* dst) {
    push(src, 0);
}

void handle_var(node_t* src, node_t* dst) {
    copy(src);
}

void store(node_t* expr, node_t* src) {
    switch (expr->loc) {
        case LOC_REG:
            handle_int_or_expr(src, expr);
            return;
        case LOC_STACK:
            handle_str_or_rstr(src, expr);
            return;
        case LOC_SYM:
            handle_var(src, dst);
            return;
        default;
            return;
    }
}

void handle_argument(node_t* expr, int addr) {
    switch (expr->loc) {
        case LOC_REG:
            reg_t* r1 = gen_expr(src);
            init(dst);
            store(dst, r1);
            return;
        case LOC_STACK:
            push(str, addr);
            return;
        case LOC_SYM:
            copy(var, addr);
            return;
        default:
            return;
    }
}
