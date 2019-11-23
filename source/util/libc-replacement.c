//--------------------------------------------------------------------------------
// libc-replacement.
//--------------------------------------------------------------------------------
// Provides replacements for some of the functions in libc
//--------------------------------------------------------------------------------

typedef void (*routine)();

extern routine __preinit_array_start[];
extern routine __preinit_array_end[];
extern routine __init_array_start[];
extern routine __init_array_end[];

void* fake_heap_end;

void _init();

void __libc_init_array()
{
    for (routine* func = __preinit_array_start; func != __preinit_array_end; func++) (*func)();
    _init();
    for (routine* func = __init_array_start; func != __init_array_end; func++) (*func)();
}

int __aeabi_atexit (void *arg, void (*func) (void *), void *d)
{
    return 0;
}