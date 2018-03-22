#pragma once

#include <string>
#include <fstream>

#ifdef __PRETTY_FUNCTION__
#define info() \
  printf("%s:%d in %s", __FILE__, __LINE__, __PRETTY_FUNCTION__);
#else
#define info() \
  printf("%s:%d in %s", __FILE__, __LINE__, __func__);
#endif

#define assertf(X) \
  do { \
    if (!(X)) { \
      printf("assertion `%s' failed at ", #X); \
      info() \
      puts(""); \
      exit(1); \
    } \
  } while (0)

#define die(...) do { printf(__VA_ARGS__); puts(""); exit(1); } while (0)

inline std::string read_file(const std::string &filename) {
  std::ifstream ifs(filename, std::ios::binary);
  if (!ifs)
    die("Failed to open file \"%s\"", filename.c_str());
  std::string buffer { std::istreambuf_iterator<char>(ifs)
    , std::istreambuf_iterator<char>() };
  return buffer;
}

