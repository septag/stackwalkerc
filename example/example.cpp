#define SW_IMPL
#include "../stackwalkerc.h"

#include <stdio.h>

static void symbol_init(const char* search_path, uint32_t sym_opts, void* userptr)
{
    printf("Search path: %s\n", search_path);
}

static void load_module(const char* img, const char* module, uint64_t base_addr, uint32_t size, 
                        uint32_t result, const char* sym_type, const char* pdb_name, uint64_t file_version,
                        void* userptr)
{
    printf("Load module:\n\timage: %s\n\tmodule: %s\n\tpdb: %s\n", img, module, pdb_name);
}

static void callstack_entry(const sw_callstack_entry* entry, void* userptr)
{
    printf("\t%s(%d): %s\n", entry->line_filename, entry->line, entry->und_name);
}

static void callstack_begin(void* userptr)
{
    puts("Callstack:");
}

static void callstack_end(void* userptr)
{
    puts("EndCallstack");
}

static void error_msg(const char* filename, uint32_t gle, uint64_t addr, void* userptr)
{
    printf("error: %s\n", filename);
}

static sw_context* g_stackwalk;

template <typename _T>
class Test
{
    public:
    void SomeFunc(_T c)
    {
        sw_show_callstack(g_stackwalk, NULL);    
    }
};

static void foo2(void) 
{
    Test<int> t;
    t.SomeFunc(1);
}

static void foo(void)
{
    foo2();
}

int main(int argc, char* argv[])
{
    sw_callbacks callbacks;
    callbacks.error_msg = error_msg;
    callbacks.load_module = load_module;
    callbacks.callstack_begin = callstack_begin;
    callbacks.callstack_entry = callstack_entry;
    callbacks.callstack_end = callstack_end;
    callbacks.symbol_init = symbol_init;
    g_stackwalk = sw_create_context_capture(SW_OPTIONS_ALL, callbacks, NULL);
    if (!g_stackwalk) {
        puts("ERROR: stackwalk init");
        return -1;
    }
    foo();  // go through some functions and finally call show_callstack
    sw_destroy_context(g_stackwalk);
    return 1;
}