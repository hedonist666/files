.arch x86

message:
  push 0
  push 0xAAAAAAAA
  push 0xAAAAAAAA
  push 0

  mov eax, [0xAAAAAAAA]
  call eax

  jmp message
