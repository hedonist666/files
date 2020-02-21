#include "file.cxx"
#include <variant>


#ifndef TEST
#ifndef PRODUCTION
#define CLI
#endif
#endif

#define ENABLE_PE

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

  ELF(File&& f) : File {f} {
    _ehdr  = static_cast<decltype(_ehdr)>(createMap());
    _phdr  = reinterpret_cast<decltype(_phdr)>(mem + _ehdr->e_phoff);
    _shdr  = reinterpret_cast<decltype(_shdr)>(mem + _ehdr->e_shoff); 
  }

  void mark() {
    _ehdr->version = 0x1976;
  }

  bool marked() {
    return _ehdr->vecrsion = 0x1976;
  }

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

};

using ELF64 = ELF<Elf64_Ehdr, Elf64_Phdr, Elf64_Shdr>;
using ELF32 = ELF<Elf32_Ehdr, Elf32_Phdr, Elf32_Shdr>;

#ifdef ENABLE_PE
template <typename NT, typename OPT> 
struct PE : File {

  using File::puts;
  using File::gets;
  using File::createMap;
  using File::mem;

  PIMAGE_DOS_HEADER dos {};
  NT nt {};
  PIMAGE_FILE_HEADER fhdr;
  PIMAGE_SECTION_HEADER shdr {};
  OPT opt {};
  PIMAGE_SECTION_HEADER import_section {};
  PIMAGE_IMPORT_DESCRIPTOR import_desc {};
  DWORD import_offset {};
  
  PE(path p) : File{p} {
    dos  = static_cast<decltype(dos)>(createMap());
    nt   = static_cast<decltype(nt)>(mem + dos->e_lfanew);
    fhdr = static_cast<decltype(fhdr)>(&nt->FileHeader);
    opt  = static_cast<decltype(opt)>(&nt->OptionalHeader);
    shdr = static_cast<decltype(shdr)>(mem + dos->e_lfanew + sizeof(nt->Signature) +
        sizeof(IMAGE_FILE_HEADER) + fhdr->SizeOfOptionalHeader);

    auto importRVA = opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    for (size_t i{}; i < fhdr->NumberOfSections; ++i) {
      if (importRVA >= shdr[i].VirtualAddress && importRVA < shdr[i].VirtualAddress + shdr[i].Misc.VirtualSize) {
        import_section = &shdr[i];
      }
    }
    import_offset = mem + import_section->PointerToRawData;
    import_desc = static_cast<decltype(import_desc)>(import_offset + (opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress - import_section->VirtualAddress));
  }

  PE(File&& f) : File(f) {
    dos  = static_cast<decltype(dos)>(createMap());
    nt   = static_cast<decltype(nt)>(mem + dos->e_lfanew);
    fhdr = static_cast<decltype(fhdr)>(&nt->FileHeader);
    opt  = static_cast<decltype(opt)>(&nt->OptionalHeader);
    shdr = static_cast<decltype(shdr)>(mem + dos->e_lfanew + sizeof(nt->Signature) +
        sizeof(IMAGE_FILE_HEADER) + fhdr->SizeOfOptionalHeader);

    auto importRVA = opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

    for (size_t i{}; i < fhdr->NumberOfSections; ++i) {
      if (importRVA >= shdr[i].VirtualAddress && importRVA < shdr[i].VirtualAddress + shdr[i].Misc.VirtualSize) {
        import_section = &shdr[i];
      }
    }
    import_offset = mem + import_section->PointerToRawData;
    import_desc = static_cast<decltype(import_desc)>(import_offset + (opt->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress - import_section->VirtualAddress));
  }


  void mark() {
    dos->e_minalloc = 0x19;
    dos->e_maxalloc = 0x76;
  }

  bool marked() {
    return ((dos->e_minalloc == 0x19) && (dos->e_maxalloc == 0x76));
  }

  string show() {
    stringstream message;
    message << "PE file on ";
    switch (nt->Machine) {
      case(IMAGE_FILE_MACHINE_I386):
        message << "Intel 80386" << endl;
        break;
      case(IMAGE_FILE_MACHINE_IA64):
        message << "Intel Itanium" << endl;
        break;
      case(IMAGE_FILE_MACHINE_AMD64):
        message << "AMD64" << endl;
        break;
      default:
        message << "An unknown machine" << endl;
    }
    message << "with entry point: 0x" << hex << opt->AddressOfEntryPoint << endl;
    message << "section headers: " << endl;
    for (size_t i{}; i < nt->NumberOfSections; ++i) {
      message << shdr[i].Name << ": 0x" << shdr[i].VirtualAddress << endl;
    }
    message << endl;
    message << "symbols: " << endl;
    for (size_t i{}; i < fhdr->NumberOfSymbols; ++i) {
      message << fhdr->PointerToSymbolTable[i] << endl;
    }
    message << endl;

    message << "Imports: " << endl;

    for (auto id {import_desc}; id->Name != 0; ++id) {

      message << import_offset + (import_desc->Name - import_section->VirualAddress) << endl;
      auto thunk = import_desc->OriginalFirstThunk == 0 ? import_desc->FirstThunk : import_desc->OriginalFirstThunk;
      auto thunkData = static_cast<PIMAGE_THUNK_DATA>(import_offset  + (thunk - import_section->VirtualAddress));

      for (; thunkData->u1.AddressOfData != 0; ++thunkData) {

        if (thunkData->u1.AddressOfData > 0x80000000) {
          message << "Ordinal: " << static_cast<DWORD>(thunkData->u1.AddressOfData) << endl;
        } 
        else {
          message << import_offset + (thunkData->u1.AddressOfData - import_section->VirualAddress + 2) << endl;
        }

      }
    }

    return message.str();
  }

};

using PE32 = PE<PIMAGE_NT_HEADERS32,PIMAGE_OPTIONAL_HEADER32>;
//using PE64 = PE<PIMAGE_NT_HEADERS64,PIMAGE_OPTIONAL_HEADER64>;

#endif


#ifdef EXECUTABLE
#include "actions.cxx"

struct Executable {
   
  enum exec_t {
    _ELF32,
    _ELF64,
    _PE32,
    _PE64
  }; exec_t type;

  File f;

  variant<ELF32, ELF64, PE32, PE64> core;

  exec_t deduction() {
    if (mem[0] == 0x7f && mem[1] == 'E' && mem[2] == 'L' && mem[3] == 'F') {
      if (mem[4] == ELFCLASS32) return _ELF32;
      else if (mem[4] == ELFCLASS64) return _ELF64; 
      else {
        cerr << "unknown elf file" << endl;
        exit(-1);
      }
    }
    else if (mem[0] == 'D' && mem[1] == 'O' && mem[2] == 'S') {
      if (fhdr->Characteristics & IMAGE_FILE_32BIT_MACHINE) return _PE32;
      else return _PE64;
    }
  }

  Executable(path p) : f {p} {
    type = deduction();
    switch(type) {
      case _ELF32:
        core = ELF32{move(f)};
        break;
      case _ELF64:
        core = ELF64{move(f)};
        break;
      case _PE32:
        core = PE32{move(f)};
        break;
      case _PE64:
        core = PE64{mode(f)};
        break;
    }
  }

  //....

  void show() {
    cout << get<core.index()>(core).show();
  }

  void entry_insert(vector<char>& bs) {
    switch(type) {
      case _ELF32:
        change_entry_elf(get<ELF32>(core), bs);   
        break;
      case _ELF64:
        change_entry_elf(get<ELF64>(core), bs);   
        break;
      case _PE32:
        change_entry_pe(get<PE32>(core), bs); 
        break;
      case _PE64:
        change_entry_pe(get<PE64>(core), bs); 
        break;
    }
  } 

  void hijack_function(vector<char>& bs) {

  }

};

#endif


#ifdef CLI

int main() {
  PE32 e {path{".."} / path {"test"} / path{"data"} / path {"hand_crafted.exe"}};
  cout << e.show();
}

#endif
