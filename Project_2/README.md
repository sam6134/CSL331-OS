# Project 2

## Simulating Virtual Memory

## Specification

### Multi-level Page tables

Assume that a process virtual address space is 20-bits and the RAM is 16MB and page size is 1KB. <br/>
Assume that a process generates a sequence of virtual address, you need to translate them into physical addresses.  
Implement a two-level page table by extending the previous code for Specification 2 (Project 2) that you have written.  
For this purpose, you must assume that the virtual address space is not completely utilized and the some of the page table entries contain invalid or unallocated pages.

Assume that only 512 pages are valid in the entire space of pages and remaining pages are unallocated.
Using this notion, develop a page-directory that will only store the relevant parts of the page table that are allocated.
The page directory should be an array that will point to a relevant portion of the page table and so on. <br />
The experiments are similar to previous scenario except that you need to consider the access time of the two-level page table during simulations.<br />
Refer to Figure 20.6 in Chapter 20 of the text for this purpose.
The rest of the chapter will also be useful for implementing this specification.

## Additional Notes:

Number of pages in the process: 2^20/2^10=2^10 pages

Number of page table entries: 1024 (2^10)

VPN size: 10-bits

PFN size: 14-bits

Number of physical frames: 2^24/2^10=2^14 frames

Size of each PTE: 14 (PFN) + 7 (mode-bits) : 21 bits (3 Bytes).

Page Table Size: 1024\*4 = 4096 B (doesn't fit in one page of 1024)

PT1: 256 = (00)

PT2: 256 = (01)

PT3: 256 = (10)

PT4: 256 = (11)

VPN : PDE= 2-bits and PTIndex= 8-bits

Since page table is implemented as an array, the VPN number is only used an index and need not be stored explicitly.

```
1 VPN = (VirtualAddress & VPN_MASK) >> SHIFT

2 (Success, TlbEntry) = TLB_Lookup(VPN)

3 if (Success == True) // TLB Hit

4 if (CanAccess(TlbEntry.ProtectBits) == True)

5 Offset = VirtualAddress & OFFSET_MASK

6 PhysAddr = (TlbEntry.PFN << SHIFT) | Offset

7 Register = AccessMemory(PhysAddr)

8 else

9 RaiseException(PROTECTION_FAULT)

10 else // TLB Miss

11 // first, get page directory entry

12 PDIndex = (VPN & PD_MASK) >> PD_SHIFT

13 PDEAddr = PDBR + (PDIndex * sizeof(PDE))

14 PDE = AccessMemory(PDEAddr)

15 if (PDE.Valid == False)

16 RaiseException(SEGMENTATION_FAULT)

17 else

18 // PDE is valid: now fetch PTE from page table

19 PTIndex = (VPN & PT_MASK) >> PT_SHIFT

20 PTEAddr = (PDE.PFN << SHIFT) + (PTIndex * sizeof(PTE))

21 PTE = AccessMemory(PTEAddr)

22 if (PTE.Valid == False)

23 RaiseException(SEGMENTATION_FAULT)

24 else if (CanAccess(PTE.ProtectBits) == False)

25 RaiseException(PROTECTION_FAULT)

26 else

27 TLB_Insert(VPN, PTE.PFN, PTE.ProtectBits)

28 RetryInstruction()
```

## Output:

1. All output specifications in Project 2

2. Average Access Time on Memory Traces similar to Project 2

## Additional Notes:

1. Using time.h library and functions like timeoftheday, gettime etc can be quite useful for measurement.

2. Functions like, sleep, rand are useful.

3. Use the code you wrote for random number generation to simulate the virtual addresses generated.

4. Use the bit level operations in C to do the shifting and masking operations.

5. Please note that the sizes of the virtual and physical addresses must be conforming to the specification.

6. Refer to Advanced Programming in UNIX Environment by Richard Stevens or C Programming by Keringhan and Ritchie.
