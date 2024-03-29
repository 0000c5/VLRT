#include "../xor.h"

class kinterface_t
{
private:
	__int64(__fastcall* function_address)(void*) = nullptr;

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
		request_shellcode = 0x98f,
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
	}shellcode_request, * pshellcode_request;

public:

	inline auto initialize() -> bool
	{
		LoadLibrary("user32.dll");

		const auto win32k = LoadLibrary("win32u.dll");

		if (!win32k)
		{
			return false;
		}

		*(void**)&function_address = GetProcAddress(win32k, "NtUserSetGestureConfig");

		if (!function_address)
		{
			return false;
		}

		return true;
	}

	inline auto send_cmd(void* data, request_codes code) -> bool
	{
		if (!data || !code)
		{
			return false;
		}

		request_data request{ 0 };

		request.unique = request_unique;
		request.data = data;
		request.code = code;

		const auto result = function_address(&request);

		if (result != request_success)
		{
			return false;
		}

		return true;
	}

	inline auto get_module_base(const int pid, const std::string module_name) -> uintptr_t
	{
		base_request data{ 0 };

		data.pid = pid;
		data.handle = 0;

		std::wstring wstr{ std::wstring(module_name.begin(), module_name.end()) };

		memset(data.name, 0, sizeof(WCHAR) * 260);
		wcscpy(data.name, wstr.c_str());

		send_cmd(&data, request_base);

		return data.handle;
	}

	inline auto find_signature(const int pid, const std::uintptr_t base, const std::string signature) -> std::uintptr_t
	{
		pattern_request data{ 0 };

		data.pid = pid;
		data.base = base;
		data.address = 0;

		memset(data.signature, 0, sizeof(char) * 260);
		strcpy(data.signature, signature.c_str());

		send_cmd(&data, request_pattern);

		return data.address;
	}

	inline auto create_shellcode(const int pid, uintptr_t destination, uintptr_t base) -> uintptr_t
	{
		shellcode_request data{ 0 };

		data.pid = pid;
		data.destination = destination;
		data.base = base;
		data.address = 0;

		send_cmd(&data, request_shellcode);

		return data.address;
	}

	inline auto swap_virtual_pointer(const int pid, std::uintptr_t src, std::uintptr_t dst) -> std::uintptr_t
	{
		swap_request data{ 0 };

		data.pid = pid;
		data.src = src;
		data.dst = dst;
		data.old = 0;

		send_cmd(&data, request_swap);

		return data.old;
	}

	inline auto read_virtual_memory(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size) -> bool
	{
		read_request data{ 0 };

		data.pid = pid;
		data.address = address;
		data.buffer = buffer;
		data.size = size;

		return send_cmd(&data, request_read);
	}

	template <typename t>
	inline auto read_virtual_memory(const int pid, const std::uintptr_t address) -> t
	{
		t response{ };
		read_virtual_memory(pid, address, &response, sizeof(t));
		return response;
	}

	inline auto write_virtual_memory(const int pid, const std::uintptr_t address, void* buffer, const std::size_t size, bool force_write = false) -> bool
	{
		write_request data{ 0 };

		data.pid = pid;
		data.address = address;
		data.buffer = buffer;
		data.size = size;
		data.force_write = force_write;

		return send_cmd(&data, request_write);
	}

	template <typename t>
	inline auto write_virtual_memory(const int pid, const std::uintptr_t address, t value) -> bool
	{
		return write_virtual_memory(pid, address, &value, sizeof(t));
	}

	inline auto allocate_kernel_memory(const int pid, size_t size, std::string module_name) -> void*
	{
		_allocate_request data{ 0 };

		data.pid = pid;
		data.size = size;
		data.handle = 0;

		std::wstring wstr{ std::wstring(module_name.begin(), module_name.end()) };

		memset(data.name, 0, sizeof(WCHAR) * 100);
		wcscpy(data.name, wstr.c_str());

		send_cmd(&data, request_allocate);

		return (void*)data.handle;
	}

	inline auto protect_kernel_memory(const int pid, size_t size, void* address, uint32_t flag)
	{
		_protect_request data{ 0 };

		data.pid = pid;
		data.protect = flag;
		data.size = size;
		data.address = address;

		send_cmd(&data, request_protect);
	}
};

static kinterface_t* kinterface = new kinterface_t();