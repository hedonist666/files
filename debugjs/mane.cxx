#include <iostream>
#include <vector>
#include <utility>
#include <array>
#include <memory>
#include <functional>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <thread>


#define CLI


#ifndef _MSC_VER

#include <elf.h>  
#include <sys/mman.h> 
#include <unistd.h>
#include <fcntl.h>


#endif


using namespace std;
using namespace std::filesystem;

enum class ExecType { Elf, Elf64, PE };

enum class Location {
  EntryStart,
  TextEnd 
};


using OpcodesHolder = vector<char>;

ostream& operator<<(ostream& os, OpcodesHolder buff) {
  for (auto e : buff) {
    os << e;
  }
  return os;
}


istream& operator>>(istream& is, OpcodesHolder& buff) {
  is.read(buff.data(), buff.size());
  return is;
}

void opcodes(auto loc, OpcodesHolder& bytes, auto& fs) {
  OpcodesHolder rbuff(bytes.size()), wbuff(bytes.size());
  fs.seekg(reinterpret_cast<streamoff>(loc));
  fs >> wbuff;
  fs.seekg(reinterpret_cast<streamoff>(loc));
  fs << bytes;
  while (fs >> rbuff) {
    fs << wbuff;
    swap(wbuff, rbuff); 
  }
}

template <typename EHDR_T, typename PHDR_T, typename SHDR_T>
struct ELF {
  EHDR_T* _ehdr;
  PHDR_T* _phdr;
  SHDR_T* _shdr;
  void* mem;

  ELF(void* mem) : mem {mem} {
    stringstream message;
    _ehdr = static_cast<decltype(_ehdr)>(mem);
    message << "{@_@} Parsing ELF Executable on machine " << _ehdr->e_machine << endl;
    _phdr = static_cast<decltype(_phdr)>(mem + _ehdr->e_phoff);
    _shdr = static_cast<decltype(_shdr)>(mem + _ehdr->e_shoff);

    cout << message.str();
  }

  void* get_entry() {
    return reinterpret_cast<void*>(_ehdr->e_entry);
  }


  void insert(void* location, OpcodesHolder& data, fstream& fs) {
    auto phdr{_phdr};
    decltype(phdr->p_vaddr) text;
    for (size_t i{_ehdr->e_phnum}; i-- > 0; ++phdr) {
      if (location - phdr->p_offset > 0) {
        if (reinterpret_cast<decltype(phdr->p_filesz)>(location - phdr->p_offset) < phdr->p_filesz) {
          phdr->p_memsz += data.size();
          phdr->p_filesz += data.size();
        }
        else {
          phdr->p_offset += data.size();
          phdr->p_vaddr += data.size();
        }
      }
    }
    opcodes(location, data, fs);
  }

};

using ELF64 = ELF<Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr>;
using ELF32 = ELF<Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr>;

void insert_code(void* loc, OpcodesHolder& bytes, fstream& fs, void* mem, ExecType et) {
  if (et != ExecType::Elf64) {
    cerr << "not implented yet" << endl;
  } 
}

template <typename Exec>
void* get_location(Location loc, Exec f) {
  switch (loc) {
    case Location::EntryStart:
      return f.get_entry();
      break;
    default:
      cerr << "no such entry in base" << endl;
      exit(0);
  } 
}

#ifdef CLI 
auto parse_args() {
  
  return tuple {
    Location::EntryStart,
    path {"b.out"},
    path {"asm_snips"} / path {"bin"}
  };
}


int main() {

  this_thread::sleep_for(chrono::seconds(20));

  auto [loc_t, p, opp] = parse_args();
  fstream fs {p};

  auto fd = open(p.c_str(), O_RDWR);
  auto mem = mmap(NULL, file_size(p), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

  OpcodesHolder oph(file_size(opp));
  fstream opf { opp , ios::binary | ios::in | ios::out};
  opf >> oph;

  ELF64 exec { mem };
  auto loc = get_location(loc_t, exec);
  exec.insert(loc, oph, fs);

  close(fd);
  return 0;

}
#endif
