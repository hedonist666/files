#include "executable.cxx"

template <typename F, F ins>
void text_end_insert(auto exec, vector<char>& bs) {

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
    }  
    else if (shdr->sh_addr + shdr->sh_size == par_addr) {
      shdr->sh_size += bs.size();
    }  
  } 

  exec._ehdr += exec.page_size();
  ins(exec, bs, par_addr);

}
