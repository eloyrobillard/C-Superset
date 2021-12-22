Dump of assembler code for function main:
   0x0000000000001141 <+0>:	endbr64 
   0x0000000000001145 <+4>:	push   %rbp
   0x0000000000001146 <+5>:	mov    %rsp,%rbp
   0x0000000000001149 <+8>:	mov    $0x4,%esi
   0x000000000000114e <+13>:	mov    $0x3,%edi
   0x0000000000001153 <+18>:	callq  0x1129 <plus>
   0x0000000000001158 <+23>:	pop    %rbp
   0x0000000000001159 <+24>:	retq   
End of assembler dump.
Dump of assembler code for function plus:
   0x0000000000001129 <+0>:	endbr64 
   0x000000000000112d <+4>:	push   %rbp
   0x000000000000112e <+5>:	mov    %rsp,%rbp
   0x0000000000001131 <+8>:	mov    %edi,-0x4(%rbp)
   0x0000000000001134 <+11>:	mov    %esi,-0x8(%rbp)
   0x0000000000001137 <+14>:	mov    -0x4(%rbp),%edx
   0x000000000000113a <+17>:	mov    -0x8(%rbp),%eax
   0x000000000000113d <+20>:	add    %edx,%eax
   0x000000000000113f <+22>:	pop    %rbp
   0x0000000000001140 <+23>:	retq   
End of assembler dump.
