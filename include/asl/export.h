#ifdef _WIN32s
	#ifdef aslzip_EXPORTS
		#define ASLZIP_API __declspec( dllexport )
	#else
		#define ASLZIP_API __declspec( dllimport )
	#endif
#else
	#define ASLZIP_API
#endif
