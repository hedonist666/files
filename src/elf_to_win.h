#include <elf.h>

#define DWORD32 Elf32_Addr
#define DWORD64 Elf64_Addr
#define USHORT Elf32_Half
#define WORD unsigned short
#define LONG long
#define BYTE Elf_Byte
#define IMAGE_SIZEOF_SHORT_NAME 8
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 15

//#define USHORT unsigned short
#define DWORD unsigned int


typedef struct _IMAGE_DOS_HEADER {
    USHORT e_magic;
    USHORT e_cblp;
    USHORT e_cp;
    USHORT e_crlc;
    USHORT e_cparhdr;
    USHORT e_minalloc;
    USHORT e_maxalloc;
    USHORT e_ss;
    USHORT e_sp;
    USHORT e_csum;
    USHORT e_ip;
    USHORT e_cs;
    USHORT e_lfarlc;
    USHORT e_ovno;
    USHORT e_res[4];
    USHORT e_oemid;
    USHORT e_oeminfo;
    USHORT e_res2[10];
    LONG   e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;


typedef struct _IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD32 TimeDateStamp;
  DWORD32 PointerToSymbolTable;
  DWORD32 NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
} IMAGE_FILE_HEADER32, *PIMAGE_FILE_HEADER32;


typedef struct _IMAGE_DATA_DIRECTORY {
  DWORD32 VirtualAddress;
  DWORD32 Size;
} IMAGE_DATA_DIRECTORY32, *PIMAGE_DATA_DIRECTORY32;



typedef struct _IMAGE_OPTIONAL_HEADER {
  WORD                   Magic;
  BYTE                      MajorLinkerVersion;
  BYTE                      MinorLinkerVersion;
  DWORD                SizeOfCode;
  DWORD                SizeOfInitializedData;
  DWORD                SizeOfUninitializedData;
  DWORD                AddressOfEntryPoint;
  DWORD                BaseOfCode;
  DWORD                BaseOfData;
  DWORD                ImageBase;
  DWORD                SectionAlignment;
  DWORD                FileAlignment;
  WORD                   MajorOperatingSystemVersion;
  WORD                   MinorOperatingSystemVersion;
  WORD                   MajorImageVersion;
  WORD                   MinorImageVersion;
  WORD                   MajorSubsystemVersion;
  WORD                   MinorSubsystemVersion;
  DWORD                Win32VersionValue;
  DWORD                SizeOfImage;
  DWORD                SizeOfHeaders;
  DWORD                CheckSum;
  WORD                   Subsystem;
  WORD                   DllCharacteristics;
  DWORD                SizeOfStackReserve;
  DWORD                SizeOfStackCommit;
  DWORD                SizeOfHeapReserve;
  DWORD                SizeOfHeapCommit;
  DWORD                LoaderFlags;
  DWORD                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

/*
 struct IMAGE_OPTIONAL_HEADER64 {
   MagicType Magic;
   byte MajorLinkerVersion;
   byte MinorLinkerVersion;
   uint SizeOfCode;
   uint SizeOfInitializedData;
   uint SizeOfUninitializedData;
   uint AddressOfEntryPoint;
   uint BaseOfCode;
   ulong ImageBase;
   uint SectionAlignment;
   uint FileAlignment;
   ushort MajorOperatingSystemVersion;
   ushort MinorOperatingSystemVersion;
   ushort MajorImageVersion;
   ushort MinorImageVersion;
   ushort MajorSubsystemVersion;
   ushort MinorSubsystemVersion;
   uint Win32VersionValue;
   uint SizeOfImage;
   uint SizeOfHeaders;
   uint CheckSum;
   SubSystemType Subsystem;
   DllCharacteristicsType DllCharacteristics;
   ulong SizeOfStackReserve;
   ulong SizeOfStackCommit;
   ulong SizeOfHeapReserve;
   ulong SizeOfHeapCommit;
   uint LoaderFlags;
   uint NumberOfRvaAndSizes;
   IMAGE_DATA_DIRECTORY ExportTable;
   IMAGE_DATA_DIRECTORY ImportTable;
   IMAGE_DATA_DIRECTORY ResourceTable;
   IMAGE_DATA_DIRECTORY ExceptionTable;
   IMAGE_DATA_DIRECTORY CertificateTable;
   IMAGE_DATA_DIRECTORY BaseRelocationTable;
   IMAGE_DATA_DIRECTORY Debug;
   IMAGE_DATA_DIRECTORY Architecture;
   IMAGE_DATA_DIRECTORY GlobalPtr;
   IMAGE_DATA_DIRECTORY TLSTable;
   IMAGE_DATA_DIRECTORY LoadConfigTable;
   IMAGE_DATA_DIRECTORY BoundImport;
   IMAGE_DATA_DIRECTORY IAT;
   IMAGE_DATA_DIRECTORY DelayImportDescriptor;
   IMAGE_DATA_DIRECTORY CLRRuntimeHeader;
   IMAGE_DATA_DIRECTORY Reserved;
} */


typedef struct _IMAGE_NT_HEADERS {
  DWORD Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;



#define IMAGE_DIRECTORY_ENTRY_EXPORT              0
#define IMAGE_DIRECTORY_ENTRY_IMPORT                      1
#define IMAGE_DIRECTORY_ENTRY_RESOURCE            2
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION           3
#define IMAGE_DIRECTORY_ENTRY_SECURITY            4
#define IMAGE_DIRECTORY_ENTRY_BASERELOC           5
#define IMAGE_DIRECTORY_ENTRY_DEBUG                       6
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT               7
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE        7
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR           8
#define IMAGE_DIRECTORY_ENTRY_TLS                     9
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG        10
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT      11
#define IMAGE_DIRECTORY_ENTRY_IAT                    12
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT       13
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR     14



typedef struct _IMAGE_SECTION_HEADER {
  BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD  NumberOfRelocations;
  WORD  NumberOfLinenumbers;
  DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;


typedef struct _IMAGE_IMPORT_DESCRIPTOR {
        union {
                 DWORD Characteristics; // 0 for terminating null import descriptor
                 DWORD OriginalFirstThunk; // RVA to original unbound IAT
         };
         DWORD TimeDateStamp; // 0 if not bound,
         // -1 if bound, and real date\time stamp
         // in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new)
         // O.W. date/time stamp of DLL bound to (old)
         DWORD ForwarderChain; // -1 if no forwarders
         DWORD Name;
         DWORD FirstThunk; // RVA to IAT
} IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;
