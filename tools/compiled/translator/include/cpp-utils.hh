#ifndef CPP_UTILS_HH
#define CPP_UTILS_HH

struct cmp_str
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

#endif

