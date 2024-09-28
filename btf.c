#ifndef BTF_H
#define BTF_H

struct btf {
	/* 本机字节序的原始 BTF 数据 */
	void *raw_data;
	/* 非本机字节序的原始 BTF 数据 */
	void *raw_data_swapped;
	__u32 raw_size;
	/* 目标字节序是否与本机不同 */
	bool swapped_endian;

	/*
	 * 当 BTF 从 ELF 或原始内存加载时，它存储在连续的内存块中。
	 * hdr, type_data, 和 strs_data 指向该内存区域中 BTF 表示的各自部分：
	 *
	 * +--------------------------------+
	 * |  Header  |  Types  |  Strings  |
	 * +--------------------------------+
	 * ^          ^          ^
	 * |          |         |
	 * hdr        |         |
	 * types_data-+         |
	 * strs_data-----------+
	 *
	 * 如果 BTF 数据稍后被修改，例如，由于类型被添加或删除，
	 * BTF 去重执行等，这种连续的表示会被分解成三个独立分配的
	 * 内存区域，以便能够独立地修改它们。
	 * 在这一点上，raw_data 被置空，但可以稍后分配并缓存，
	 * 如果用户调用 btf__raw_data()，此时 raw_data 将包含
	 * 头、类型和字符串的连续副本：
	 *
	 * +----------+  +---------+  +-----------+
	 * |  Header  |  |  Types  |  |  Strings  |
	 * +----------+  +---------+  +-----------+
	 * ^             ^             ^
	 * |             |            |
	 * hdr           |            |
	 * types_data----+            |
	 * strset__data(strs_set)-----+
	 *
	 *               +----------+---------+-----------+
	 *               |  Header  |  Types  |  Strings  |
	 * raw_data----->+----------+---------+-----------+
	 */
	struct btf_header *hdr;

	void *types_data;
	size_t types_data_cap; /* hdr->type_len 中存储的使用大小 */

	/* 类型 ID 到 `struct btf_type *` 查找索引
	 * type_offs[0] 对应于第一个非 VOID 类型：
	 *   - 对于基本 BTF，它是类型 [1]；
	 *   - 对于分割 BTF，它是第一个非基础 BTF 类型。
	 */
	__u32 *type_offs;
	size_t type_offs_cap;
	/* 这个 BTF 实例中的类型数量：
	 *   - 不包括特殊的 [0] void 类型；
	 *   - 对于分割 BTF，计算在基础 BTF 上添加的类型数量。
	 */
	__u32 nr_types;
	/* 如果不为 NULL，指向当前分割 BTF 所基于的基础 BTF */
	struct btf *base_btf;
	/* 这个 BTF 实例中的第一个类型的 BTF 类型 ID：
	 *   - 对于基础 BTF，它等于 1；
	 *   - 对于分割 BTF，它等于基础 BTF 的最大类型 ID 加 1。
	 */
	int start_id;
	/* 这个 BTF 实例的逻辑字符串偏移：
	 *   - 对于基础 BTF，它等于 0；
	 *   - 对于分割 BTF，它等于基础 BTF 的字符串节大小总和。
	 */
	int start_str_off;

	/* strs_data 或 strs_set 只有一个可以非 NULL，取决于
	 * BTF 是否处于可修改状态（使用 strs_set）或不是
	 * （strs_data 指向 raw_data 内部）
	 */
	void *strs_data;
	/* 一组唯一的字符串 */
	struct strset *strs_set;
	/* 字符串是否已经去重 */
	bool strs_deduped;

	/* 对于这个实例，base_btf 是否应该在 btf_free 中被释放 */
	bool owns_base;

	/* 如果加载到内核中，BTF 对象的文件描述符 */
	int fd;

	/* 这个 BTF 的目标架构的指针大小（以字节为单位） */
	int ptr_sz;
};
