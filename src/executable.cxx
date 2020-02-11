#include "file.cxx"


#ifndef TEST
#ifndef PRODUCTION
#define CLI
#endif
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


#ifdef SHOW
  string show() { //to check
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
          if (_phdr[i].p_offset == 0 && _phdr->p_flags == PF_R | PF_X) {
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

    return message.str();
  }
#endif

};

using ELF64 = ELF<Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr>;
using ELF32 = ELF<Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr>;

#ifdef PE
struct PE : File {

  using File::puts;
  using File::gets;
  using File::createMap;
  using File::mem;

  PIMAGE_DOS_HEADER dos {};
  PIMAGE_NT_HEADERS nt {};
  PIMAGE_SECTION_HEADER shdr {};
  PIMAGE_SECION_HEADER import_section {};
  PIMAGE_THUNK_DATA thunkData {};
  
  PE(path p) : File{p} {
    dos = static_cast<decltype(dos)>(mem);
    nt  = static_cast<decltype(nt)>(mem + dos->e_lfanew);

  }
};
#endif


struct Executable : File {

   
  enum exec_t {
    ELF,
    ELF64,
    PE 
  } type;


  Executable(path p) : File{p} {

  }

};


#ifdef CLI

int main() {
  ELF64 e {path {"test"} / path{"data"} / path {"a.out"}};
  cout << e.show();
}

#endif
