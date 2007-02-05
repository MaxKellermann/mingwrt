#ifdef __COREDLL__
__declspec(dllimport) unsigned int __lc_codepage;
#endif

static inline
unsigned int get_codepage (void)
{
#ifdef __COREDLL__
  return 0;
#else
  return __lc_codepage;
#endif
}
