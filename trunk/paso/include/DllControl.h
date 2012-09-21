#ifdef __gnu_linux__
#define DLL_API
#define DLL_CLASS
#else
#define DLL_API extern "C" __declspec(dllexport)
#define DLL_CLASS __declspec(dllexport)
#endif
