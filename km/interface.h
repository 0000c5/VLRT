typedef enum _request_codes
{
	request_free = 0x109,
	request_base = 0x119,
	request_read = 0x129,
	request_write = 0x139,
	request_allocate = 0x149,
	request_expose = 0x159,
	request_swap = 0x169,
	request_pattern = 0x179,
	request_success = 0x91a,
	request_unique = 0x92b,
	request_query = 0x93c,
	request_protect = 0x94e,
	request_unload = 0x98f,
}request_codes, * prequest_codes;

typedef struct _unload_request
{
	bool* buffer;
}unload_request, * punload_request;

typedef struct _read_request {
	uint32_t pid;
	uintptr_t address;
	void* buffer;
	size_t size;
} read_request, * pread_request;

typedef struct _write_request {
	uint32_t pid;
	uintptr_t address;
	void* buffer;
	size_t size;
	bool force_write;
} write_request, * pwrite_request;

typedef struct _query_reqeust {
	uint32_t pid;
	uintptr_t address;
	void* out;
} query_reqeust, * pquery_reqeust;

typedef struct _base_request {
	uint32_t pid;
	uintptr_t handle;
	WCHAR name[260];
} base_request, * pbase_request;

typedef struct _swap_request
{
	uint32_t pid;
	uintptr_t dst;
	uintptr_t src;
	uintptr_t old;
}swap_request, * pswap_request;

typedef struct _free_request
{
	void* address;
	uintptr_t mdl;
}free_request, * pfree_request;

typedef struct _allocate_request
{
	uint32_t pid;
	uintptr_t handle;
	size_t size;
	WCHAR name[100];
}allocate_request, * pallocate_request;

typedef struct _expose_request
{
	void* address;
	size_t size;
	uint32_t pid;
}expose_request, * pexpose_request;

typedef struct _protect_request {
	uint32_t pid;
	size_t size;
	uint32_t protect;
	void* address;
} protect_request, * pprotect_request;

typedef struct _pattern_request
{
	int pid;
	uintptr_t base;
	char signature[260];
	uintptr_t address;
}pattern_request, * ppattern_request;

typedef struct _request_data
{
	uint32_t unique;
	request_codes code;
	void* data;
}request_data, * prequest_data;

typedef struct _request
{
	int pid;
	uintptr_t destination;
	uintptr_t base;
	char remove;
	uintptr_t address;
}shellcode_request, *pshellcode_request;