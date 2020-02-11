#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <string_view>
#include <map>

using namespace std::filesystem;
using namespace std;

#ifndef TEST
#ifndef PRODUCTION
#define CLI
#endif
#endif

#define SHOW

#ifdef _MSC_VER

#include <windows.h>

struct File {

  HANDLE f {};
  HANDLE hFileMap {};
  char* mem;
  path p;
  
  static const size_t page_size() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwPageSize;
  }

  File(path p, int hint = FILE_ATTRIBUTE_NORMAL) : p {p} {
    f = CreateFileA(filename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, hint, NULL);
    if (f == INVALID_HANDLE_VALUE) {
      cerr << "cannot read a file" << endl;
      exit(-1);
    }
  }   

  void* createMap() {
    hFileMap = CreateFileMapping(f, NULL, PAGE_READONLY, 0, 0, NULL);
    mem = static_cast<char*>(MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 5));
  }


  size_t gets(vector<char>& bs) {
    size_t n;
    ReadFile(f, bs.data(), bs.size(), &n, NULL);
    return n;
  }

  size_t puts(vector<char>& bs) {
    size_t n;
    WriteFile(f, bs.data(), bs.size(), &n, NULL);
    return n;
  }


  size_t gets(char* data, size_t len) {
    size_t n;
    ReadFile(f, data, len, &n, NULL);
    return n;
  }

  size_t puts(char* data, len) {
    size_t n;
    WriteFile(f, data, len, &n, NULL);
    return n;
  }

  void seek(size_t pos, int whence = FILE_BEGIN) {
    SetFilePointer(f, pos & 0xffff, pos & (0xffff << 8*2), whence);
  }

  ~File() {
    closeHandle(f);
    if (hFileMap) {
      closeHandle(hFileMap);
      UnmapViewOfFile(static_cast<LPVOID>(mem));
    }
  }


}

#else

#include <elf.h>  
#include <sys/mman.h> 
#include <unistd.h>
#include <fcntl.h>

#define abort(mes) perror(mes), exit(0);

struct File {

  static const size_t page_size() {
    return sysconf(_SC_PAGESIZE);
  }

  char* mem {};
  FILE* f {};
  int fd {};
  path p;
  
  File(path p, const char* mode = "r+") : p {p} {
    if (f = fopen(p.c_str(), mode); f == NULL) abort("File():fopen()");
    if (fd = fileno(f); fd == -1) abort("File():fileno()");
  }

  void* createMap(int prot = PROT_READ | PROT_WRITE, int flags = MAP_SHARED) {
    if (mem = static_cast<decltype(mem)>(mmap(NULL, file_size(p), prot, flags, fd, 0)); mem == MAP_FAILED) abort("File():mmap()");
    return mem;
  }

  char* map() {
    if (mem == NULL)
      createMap();
    return mem;
  }
  
  size_t size() {
    return file_size(p);
  }
  size_t gets(vector<char>& bs) {
    return fread(bs.data(), 1, bs.size(), f);
  }

  size_t puts(vector<char>& bs) {
    return fwrite(bs.data(), 1, bs.size(), f);
  }

  size_t gets(char* bs, size_t len) {
    return fread(bs, 1, len, f);
  }

  size_t puts(char* bs, size_t len) {
    return fwrite(bs, 1, len, f);
  }

  void seek(size_t offset, int whence = SEEK_SET) {
    fseek(f, offset, whence);
  }

  ~File() {
    fclose(f);
    if (mem) {
      munmap(mem, file_size(p));
    }
  }
};

#endif


void insert_byte_by_byte(File exec, vector<char>& bs, size_t pos = -1) {
  vector<char> wbuf(bs.size()), rbuf(bs.size());
  
  size_t fin{ exec.size() + bs.size() };

  if (pos != -1) {
    exec.seek(pos);
  }
  else {
    pos = ftell(exec.f);
  }

  if (auto n = exec.gets(wbuf); n < wbuf.size()) {
    wbuf.resize(n);
  }

  exec.seek(pos);
  exec.puts(bs);
  pos += bs.size();

  while (pos < fin) {
    if (auto n = exec.gets(rbuf); n < rbuf.size()) {
      rbuf.resize(n);
    }
    exec.seek(pos);
    exec.puts(wbuf);
    pos += bs.size();
    swap(rbuf, wbuf);
  }

}

void mirror_insert(File exec, vector<char>& bs, size_t pos) {
  const path tmp {{'.', 'l','t'}};
  File copy {tmp, "w+" };
  copy.puts(exec.map(), pos);
  copy.puts(bs);
  copy.puts(exec.map() + pos, exec.size() - pos);   
  rename(tmp, exec.p);
}


#ifdef CLI

int main() {
  ELF64 e {path {"test"} / path{"data"} / path {"a.out"}};
  cout << e.show();
}

#endif
