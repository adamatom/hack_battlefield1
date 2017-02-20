# GetEntityList
### Signature Dump
```
00000001447FB580
00000001447E4420 | 48 63 42 28              | movsxd rax,dword ptr ds:[rdx+28]        |
00000001447E4424 | 48 83 C0 06              | add rax,6                               |
00000001447E4428 | 48 01 C0                 | add rax,rax                             |
00000001447E442B | 48 8B 04 C1              | mov rax,qword ptr ds:[rcx+rax*8]        | [rcx+rax*8]:&"ClientVehicleEntity"
```

### Extra Info
If you step through the function in a debugger, m_pGameWorld->Unk0x28 is almost always NULL (sometimes it's 1)

Your external formula is:

1. Read value ( m_pGameWorld->Unk0x28 + 6 ) * 2
2. Take a typeinfo pointer: fb::ClassInfo<fb::ClientVehicleEntity> 0x1427BB6B0
3. Add value from Step 1 & Step 2
4. De-reference that value
5. Keep following pointers at 0x00 until they are == nullptr

```
// rcx = fb::ClassInfo<fb::ClientVehicleEntity>*
// rdx = fb::ClientGameWorld*


.text:0000000140791AF0 ; class fb::EntityList near * fb::getEntityList( fb::ClassInfo*, fb::ClientGameWorld* )

    movsxd  rax, dword ptr [rdx+28h] // *( fb::GameWorld( ) + 0x28 )
    add     rax, 6 // rax = 0x06 (assuming the above pointer is NULL)
    add     rax, rax // rax = 0x0C
    mov     rax, [rcx+rax*8] // fb::ClassInfo* + 0x0C * 8

// return value:
//     rax = 0x1427BB6B0 + 0x0C * 0x08
//     rax = 0x1427BB6B0 + 0x60
//     rax = 0x1427BB710

    retn
.text:0000000140791AFF j_?getEntityList@fb@@YAPAVEntityList@1@XZ endpo
```
# Get ClassInfo Pointer
Find string refs in x64dbg. Search for entity type wanted - `ClientCapturePointEntity`. One of the options is `&"ClientCapturePointEntity"`. This is the beginning of the `MemberInfoData` struct. Now use x64dbg to find pattern using the location of the `&"ClientCapturePointEntity"` above. This is the beginning of the `MemberInfo`/`TypeInfo`/`ClassInfo`.

*HACK* It appears that the only code location where the `&ClientCapturePointEntity` string is used has the `ClassInfo` pointer on the next line.


From below, we know we can just take that pointer address, add `0x60`, and get the head of the list of `ClientCapturePointEntity`'s. The first element of the first dereference is also the pointer to the next.  The list items do not point to beginning of the Entity object though.

To find the vtable/start of the class, start around `-0x50` from where the above process lands you. Then select the 8 bytes that are the candidate vtable, and follow in another dump. If the new dump looks like a vtable list (a bunch of pointers), then follow the first pointer in the disassembler. It should land on a line that contains a pointer to the string found two paragraphs above.

### For ClientCapturePointEntity
Following above procedure, `-0xA8` is vtable.

The `GetTransform` function is the 27th pointer in the vtable. Deref the first pointer of the `ClientCapturePointEntity` and then count out 27 pointers. View the value of that location in the Disassembler.

The code will look like:
```
00000001456765D0 | 0F 10 81 20 03 00 00     | movups xmm0,xmmword ptr ds:[rcx+320]          |
00000001456765D7 | 0F 29 02                 | movaps xmmword ptr ds:[rdx],xmm0              |
00000001456765DA | 0F 10 89 30 03 00 00     | movups xmm1,xmmword ptr ds:[rcx+330]          |
00000001456765E1 | 0F 29 4A 10              | movaps xmmword ptr ds:[rdx+10],xmm1           |
00000001456765E5 | 0F 10 81 40 03 00 00     | movups xmm0,xmmword ptr ds:[rcx+340]          |
00000001456765EC | 0F 29 42 20              | movaps xmmword ptr ds:[rdx+20],xmm0           |
00000001456765F0 | 0F 10 89 50 03 00 00     | movups xmm1,xmmword ptr ds:[rcx+350]          |
00000001456765F7 | 0F 29 4A 30              | movaps xmmword ptr ds:[rdx+30],xmm1           |
00000001456765FB | C3                       | ret                                           |
```
From that we see the Transform matrix is `rcx` (aka this) + 320. The position is held in this.


### For ClientVehicleEntity
Following above procedure, we find the beginning of the `ClientVehicleEntiy` is `-0x50` from the location of the linked
list node.

The `GetTransfor`m for `ClientVehicleEntity`, `ClientSoldierEntity` looks like:
```
0000000145071E90 | 4C 8B 49 38       | mov r9,qword ptr ds:[rcx+38]          | 
0000000145071E94 | 41 0F B6 49 0A    | movzx ecx,byte ptr ds:[r9+A]          | put the byte found at r9+a into ecx, lpad with zeros
0000000145071E99 | 41 0F B6 41 09    | movzx eax,byte ptr ds:[r9+9]          | same, but r9+9 into rax
0000000145071E9E | 4C 8D 04 48       | lea r8,qword ptr ds:[rax+rcx*2]       | just do math: put rax+rcx*2 into r8
0000000145071EA2 | 49 C1 E0 05       | shl r8,5                              | then shiftl by 5. looks like always 740? Might not be always
0000000145071EA6 | 43 0F 10 44 08 10 | movups xmm0,xmmword ptr ds:[r8+r9+10] | so, *(this+38) + 740 + 10 is start of
0000000145071EAC | 0F 29 02          | movaps xmmword ptr ds:[rdx],xmm0      | transform matrix.
0000000145071EAF | 43 0F 10 4C 08 20 | movups xmm1,xmmword ptr ds:[r8+r9+20] |
0000000145071EB5 | 0F 29 4A 10       | movaps xmmword ptr ds:[rdx+10],xmm1   |
0000000145071EB9 | 43 0F 10 44 08 30 | movups xmm0,xmmword ptr ds:[r8+r9+30] |
0000000145071EBF | 0F 29 42 20       | movaps xmmword ptr ds:[rdx+20],xmm0   |
0000000145071EC3 | 43 0F 10 4C 08 40 | movups xmm1,xmmword ptr ds:[r8+r9+40] |
0000000145071EC9 | 0F 29 4A 30       | movaps xmmword ptr ds:[rdx+30],xmm1   |
```
# GetEntityData
This virtual function can be found at vtable entry 10, the 11th pointer. For `ClientCapturePointEntity` we find:
```
0000000144FC7420 | 48 8B 41 30              | mov rax,qword ptr ds:[rcx+30]                 |
0000000144FC7424 | C3                       | ret                                           |j
```

So the ClientCapturePointEntityData is found at +0x30.

# From Feb Patch, finding the ClientCapturePointEntity
Feb: (143CCE7E0)

Find the string we see:
```
0000000143ED5E21 | 48 8D 15 60 80 DF FF | lea rdx,qword ptr ds:[143CCDE88] | 143CCDE88:&"ClientCapturePointEntity"
0000000143ED5E28 | 48 8D 0D B1 89 DF FF | lea rcx,qword ptr ds:[143CCE7E0] |
```

Go to 143CCE7E0 in memory dump.
```
$ ==>            0000000143CCE7E0      88 DE CC 43 01 00 00 00 60 D8 CC 43 01 00 00 00  .ÞÌC....`ØÌC....  
$+10             0000000143CCE7F0      5C 26 00 00 00 00 00 00 B0 41 C3 43 01 00 00 00  \&......°AÃC....  
$+20             0000000143CCE800      00 00 00 00 00 00 00 00 59 03 59 03 00 00 00 00  ........Y.Y.....  
$+30             0000000143CCE810      00 00 00 00 00 00 00 00 A0 38 CC 43 01 00 00 00  ........ 8ÌC....  
$+40             0000000143CCE820      00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00  ................  
$+50             0000000143CCE830      50 AC 65 43 01 00 00 00 00 00 00 00 00 00 00 00  P¬eC............  
$+60             0000000143CCE840      F8 BC 2A E8 01 00 00 00 00 00 00 00 00 00 00 00  ø¼*è............  
```

$+60 contains the pointer to the first item in the list. Dump that in memory dump.
```
$-B0             00000001E82ABC48      91 03 00 00 00 00 00 00 40 DA 9A 42 01 00 00 00  ........@Ú.B....  
$-A0             00000001E82ABC58      E8 BF 2A E8 01 00 00 00 18 B9 2A E8 01 00 00 00  è¿*è.....¹*è....  
$-90             00000001E82ABC68      9E 92 08 00 25 00 00 00 FF FF FF FF 00 00 00 00  ....%...ÿÿÿÿ....  
$-80             00000001E82ABC78      88 42 1C 1F 01 00 00 00 10 A4 7D 6B 01 00 00 00  .B.......¤}k....  
$-70             00000001E82ABC88      20 DB 9A 42 01 00 00 00 00 00 00 00 00 00 00 00   Û.B............  
$-60             00000001E82ABC98      68 A9 F8 1E 01 00 00 00 02 00 00 00 01 00 00 00  h©ø.............  
$-50             00000001E82ABCA8      00 00 80 3F 00 00 00 40 02 00 00 00 00 00 00 00  ...?...@........  
$-40             00000001E82ABCB8      C0 65 37 43 01 00 00 00 D8 BC 2A E8 01 00 00 00  Àe7C....Ø¼*è....  
$-30             00000001E82ABCC8      D8 BC 2A E8 01 00 00 00 00 00 00 00 2E 00 00 00  Ø¼*è............  
$-20             00000001E82ABCD8      78 DB 9A 42 01 00 00 00 00 00 00 00 00 00 00 00  xÛ.B............  
$-10             00000001E82ABCE8      00 00 00 00 00 00 00 00 88 BC 2A E8 01 00 00 00  .........¼*è....  
$ ==>            00000001E82ABCF8      00 00 00 00 00 00 00 00 40 E8 CC 43 01 00 00 00  ........@èÌC....  
```

Let's double check this is a ClientCapturePointEntity. We think the vtable is -0xA8. Dump into memory dump.
```
00000001429ADA40  00 67 67 45 01 00 00 00 50 EB F8 43 01 00 00 00  .ggE....PëøC....  
00000001429ADA50  70 AC FC 44 01 00 00 00 70 AC FC 44 01 00 00 00  p¬üD....p¬üD....  
00000001429ADA60  F0 A5 67 45 01 00 00 00 B0 45 AC 45 01 00 00 00  ð¥gE....°E¬E....  
00000001429ADA70  50 EB F8 43 01 00 00 00 50 EB F8 43 01 00 00 00  PëøC....PëøC....  
00000001429ADA80  50 EB F8 43 01 00 00 00 40 46 67 45 01 00 00 00  PëøC....@FgE....  

```
Looks like a vtable, follow the first entry in the disassembler
```
0000000145676700 | 48 8D 05 D9 80 65 FE     | lea rax,qword ptr ds:[143CCE7E0]              |
0000000145676707 | C3                       | ret                                           |
```

Bingo, 143CCE7E0 matches what we started with in step 1. jump back to `00000001E82ABC50`. From the dump of the
GetTransform for this class, we know the Transform Matrix is at +320, grab 64 bytes from that.

# GameClientContext
### Value
original: 14341B650
feb patch: 143436980

## Signatures
### Used for Feb Patch
Get into bf1 code, then right click on "follow in dump" then search the following `56 48 83 EC 20 83 79 58 00`
```
000000014439E970 | 56                       | push rsi                                                 |
000000014439E971 | 48 83 EC 20              | sub rsp,20                                               |
000000014439E975 | 83 79 58 00              | cmp dword ptr ds:[rcx+58],0                              |
000000014439E979 | 48 89 CE                 | mov rsi,rcx                                              |
000000014439E97C | 0F 84 97 00 00 00        | je bf1.14439EA19                                         |
000000014439E982 | 48 8B 05 C7 CC 07 FF     | mov rax,qword ptr ds:[14341B650]                         |
```

### Alternatives
```
000000014437C589 | 48 89 9F 50 09 00 00     | mov qword ptr ds:[rdi+950],rbx                           |
000000014437C590 | 31 DB                    | xor ebx,ebx                                              |
000000014437C592 | 48 89 9F 68 09 00 00     | mov qword ptr ds:[rdi+968],rbx                           |
000000014437C599 | 89 9F 70 09 00 00        | mov dword ptr ds:[rdi+970],ebx                           |
000000014437C59F | 66 89 9F 74 09 00 00     | mov word ptr ds:[rdi+974],bx                             |
000000014437C5A6 | 48 89 9F 78 09 00 00     | mov qword ptr ds:[rdi+978],rbx                           |
000000014437C5AD | 83 7E 10 01              | cmp dword ptr ds:[rsi+10],1                              |
000000014437C5B1 | 0F 94 D0                 | sete al                                                  |
000000014437C5B4 | 88 87 76 09 00 00        | mov byte ptr ds:[rdi+976],al                             |
000000014437C5BA | 39 5E 10                 | cmp dword ptr ds:[rsi+10],ebx                            |
000000014437C5BD | 75 0D                    | jne bf1.14437C5CC                                        |
000000014437C5BF | 48 8B 05 8A F0 09 FF     | mov rax,qword ptr ds:[14341B650]                         |
```
```
0000000144385141 | 48 8B 86 A0 00 00 00     | mov rax,qword ptr ds:[rsi+A0]                            |
0000000144385148 | 8B 50 20                 | mov edx,dword ptr ds:[rax+20]                            |
000000014438514B | 48 89 F1                 | mov rcx,rsi                                              |
000000014438514E | E8 3D 39 00 00           | call bf1.144388A90                                       |
0000000144385153 | 48 8B 05 F6 64 09 FF     | mov rax,qword ptr ds:[14341B650]                         |
```

# Interesting Looking Entities

```
String=&"ClientTripwireEntity"
0000000143EB71F8 | 48 8D 0D 61 39 DE FF     | lea rcx,qword ptr ds:[143C9AB60]              |
Address=0000000143EB71F1
Disassembly=lea rdx,qword ptr ds:[143C9A750]
```

# *Not* Interesting Entities
None of these get instantiated:
* ClientCheckpointFlagEntity
* ClientObjectiveEntity
* ClientSpawnEntity
