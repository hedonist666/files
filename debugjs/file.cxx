#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstdio>

using namespace std::filesystem;
using namespace std;

#define CLI

#ifdef _MSC_VER

#include <windows.h>

struct File {
  
  static size_t page_size() {
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwPageSize;
  }

}

#else

#include <elf.h>  
#include <sys/mman.h> 
#include <unistd.h>
#include <fcntl.h>

#define abort(mes) perror(mes), exit(0);

struct File {

  static size_t page_size() {
    return sysconf(_SC_PAGESIZE);
  }

  char* mem {};
  FILE* f {};
  int fd {};
  path p;
  
  File(path p, const char* mode = "r+") : p {p} {
    if (f = fopen(p.c_str(), mode); f == NULL) abort("File():");
    if (fd = fileno(f); fd == -1) abort("File():");
  }

  void* createMap(int prot = PROT_READ | PROT_WRITE, int flags = MAP_SHARED) {
    if (mem = static_cast<decltype(mem)>(mmap(NULL, file_size(p), prot, flags, fd, 0)); mem == MAP_FAILED) abort("File():");
    return mem;
  }
  
  void gets(vector<char>& bs) {
    fread(bs.data(), bs.size(), 1, f);
  }

  void puts(vector<char>& bs) {
    fwrite(bs.data(), bs.size(), 1, f);
  }

  void gets(char* bs, size_t len) {
    fread(bs, len, 1, f);
  }

  void puts(char* bs, size_t len) {
    fwrite(bs, len, 1, f);
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


template <typename EHDR_T, typename PHDR_T, typename SHDR_T>
struct ELF : File {

  using File::puts;
  using File::gets;
  using File::createMap;
  using File::mem;

  EHDR_T* _ehdr;
  PHDR_T* _phdr;
  SHDR_T* _shdr;

  ELF(path p) : File {p} {
    _ehdr  = static_cast<decltype(_ehdr)>(createMap());
    _phdr  = reinterpret_cast<decltype(_phdr)>(mem + _ehdr->e_phoff);
    _shdr  = reinterpret_cast<decltype(_shdr)>(mem + _ehdr->e_shoff); 
  }


#ifdef CLI
  void show() { //to check
    stringstream message;

    message << "ELF file on ";

    switch(_ehdr->e_machine) {
      case (EM_NONE): 
        message << "An unknown machine" << endl;
        break;
      case (EM_M32): 
        message << "AT&T WE 32100" << endl;
        break;
      case (EM_386): 
        message << "Intel 80386" << endl;
        break;
      case (EM_860): 
        message << "Intel 80860" << endl;
        break;
      case (EM_X86_64): 
        message << "AMD x86-64" << endl;
        break;
    }

    auto string_table = &mem[_shdr[_ehdr->e_shstrndx].sh_offset];

    message << "with entry point: 0x" << hex << _ehdr->e_entry << endl;
    message << "Section headers:" << endl;

    for (size_t i{1}; i < _ehdr->e_shnum; ++i) {
      message << &string_table[_shdr[i].sh_name] << ": 0x" << _shdr[i].sh_addr << endl;
    }

    message << endl << "Program headers:" << endl; 

    for (size_t i{0}; i < _ehdr->e_phnum; ++i) {
      switch (_phdr[i].p_type) {
        case (PT_LOAD): 
          if (_phdr[i].p_offset == 0) {
            message << "text segment is at 0x";
          }
          else {
            message << "data segment is at 0x";
          }
          message << _phdr[i].p_vaddr << endl;
          break;
        case (PT_INTERP): {
          string interp {&mem[_phdr[i].p_offset]};
          message << "interpreter: " << interp << endl;
          break;
        }
        case (PT_NOTE):
          message << "note is at 0x" << _phdr[i].p_vaddr << endl;
          break;
        case (PT_DYNAMIC):
          message << "dynamic is at 0x" << _phdr[i].p_vaddr << endl;
          break;
        case (PT_PHDR):
          message << "phdr is at 0x" << _phdr[i].p_vaddr << endl;
      }
    }

    cout << message.str();
  }
#endif

};

using ELF64 = ELF<Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr>;
using ELF32 = ELF<Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr>;

void insert_byte_by_byte(auto exec, auto bs, auto pos = 0) {
  decltype(bs) wbuf(bs.size()), rbuf(bs.size());

  exec.seek(pos, SEEK_CUR);
  pos = fteel(exec.f); 

  exec.gets(wbuf);
  exec.seek(pos);
  exec.puts(bs);
  pos += bs.size();

  while (!feof(exec.f)) {
    exec.gets(rbuf);
    exec.seek(pos, SEEK_SET);
    exec.puts(wbuf);
    swap(rbuf, wbuf);
  }

}

void mirror_insert(auto exec, auto bs, auto pos) {
  path tmp {{'.', 'l','t'}};
  File copy { path {tmp} };
  copy.puts(exec.mem, pos);
  copy.puts(bs);
  copy.puts(exec.mem + pos, exec.size() - pos);   
  rename(tmp, exec.p);
}

void text_end_insert(auto exec) {
        
}

#ifdef CLI

int main() {
  File ins { {"insert.test"} };
  File pay { {"payload.test"} };
  vector<char> bs;
  bs.resize(pay.size());
  insert_byte_by_byte(ins, )
}

#endif
