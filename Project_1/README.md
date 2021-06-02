# Project 1

## Simulating Virtual Memory

The project has multiple specifications. A separate source code submission is required for each specification.

### Specification 1 : Simulating a Page Table.

Look at the following code from Chapter 18 of the textbook.
This code essentially shows how a page-table entry is processed.
Assume that a process

- virtual address space is 16-bits and the
- RAM is 16MB with 1KB page size (10-bits offset).

Assume that a process generates a sequence of virtual addresses, you need to translate them into physical addresses. For this purpose, you need to simulate/implement the following:

a. A random/deterministic sequence of virtual addresses in the valid format. <br />
b Page table entries: assume that we
*implement Valid
*Present
*Protect-bits (3-bits Read-write-execute)
*dirty bit
\*User/kernel mode bit

Hint: Use a struct to implement these

c. Page table functions to extract VPN and so on
*exception functions like SEGMENTATION_FAULT/PROTECTION_FAULT,
*RaiseException and so on.

Hint: Just print the words "SEGMENTATION_FAULT" or whichever and exit the experiment trial

d. A timer function to measure the time taken to access <br />
---a valid page <br />
---an invalid page<br />
---a valid page but protection bits do not allow access<br />

Refer to Section 18.3 (Figure 18.6) to get more help.

- The output should be a sequence of virtual address,<br />
- with the correct physical address and the time taken for translation and the total memory access time (summation).

```
1 //Extract the VPN from the virtual address
2 VPN = (VirtualAddress & VPN_MASK) >> SHIFT
3
4 // Form the address of the page-table entry (PTE)
5 PTEAddr = PTBR + (VPN * sizeof(PTE))
6
7 // Fetch the PTE
8 PTE = AccessMemory(PTEAddr)
9
10 // Check if process can access the page
11 if (PTE.Valid == False)
12 RaiseException(SEGMENTATION_FAULT)
13 else if (CanAccess(PTE.ProtectBits) == False)
14 RaiseException(PROTECTION_FAULT)
15 else
16 // Access is OK: form physical address and fetch it
17 offset = VirtualAddress & OFFSET_MASK
18 PhysAddr = (PTE.PFN << PFN_SHIFT) | offset
19 Register = AccessMemory(PhysAddr)
```

================================================================================================================================

## Specification 2

### Simulating a TLB

The following code is for TLB implementation from Chapter 19, Figure 19.3. You need to repeat the above experiment (meaning run the same sequence of virtual addresses) assuming that we have different TLBs of sizes, 16, 32, 64, 128 and 256. Measure the times and show the difference from the previous scenario for similar access.

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
11 PTEAddr = PTBR + (VPN * sizeof(PTE))
12 PTE = AccessMemory(PTEAddr)
13 if (PTE.Valid == False)
14 RaiseException(SEGMENTATION_FAULT)
15 else if (CanAccess(PTE.ProtectBits) == False)
16 RaiseException(PROTECTION_FAULT)
17 else
18 TLB_Insert(VPN, PTE.PFN, PTE.ProtectBits)
19 RetryInstruction()
```

The output of this program is similar to the previous program except you will also include time taken to access TLB and time taken during TLB miss.

### Required Experimentation

1. Generate sequences of 25, 50, 75, 100 virtual addresses (10 trials each). A trial basically is a distinct set of virtual addresses of the specified size, say 25.

2. For each virtual address, perform the page table lookup and print the corresponding physical address.

3. Both the input and output addresses must be printed in binary, showing clearly the 6-bit VPN and 14-bit PFN, with the 10-bit offset.

4. Compute the average time taken to access memory

### a) Without TLB

Hint: For a trial size 10 of 25 addresses, the average access is given by total access/10. Do not include any sample that generates segmentation faults or protection faults. In such cases, the average will be the total time/total number of successful trials

### b) WIth TLB of size 8, 16, and 32 entries

5. For TLB replacement use the following LRU with CLOCK algorithm.
   a). For each TLB entry associate a USE bit. If the use bit is 0 then the entry was not used recently. <br />
   b). When an entry comes into the TLB its USE bits is set to 1 <br />
   c). Periodically (using some timer), reset all the USE bits to 0. <br />
   d). If a TLB hit occurs, then the USE bit is set to 1 <br />
   e). For replacing an existing TLB entry, scan the array using the clock algorithm, i.e., from the last location where a TLB entry was removed, and keep looking for an entry with USE bit as 0. <br />
   ---If no entries are found, then reset all USE bits to 0 and replace the entry where the clock hand currently sits

### Additional Notes:

1. Using time.h library and functions like timeoftheday, gettime etc can be quite useful for measurement.
2. Functions like, sleep, rand are useful.
3. Use the code you wrote for random number generation to simulate the virtual addresses generated.
4. Use the bit level operations in C to do the shifting and masking operations.
5. Please note that the sizes of the virtual and physical addresses must be conforming to the specification.
6. Refer to Advanced Programming in UNIX Environment by Richard Stevens or C Programming by Keringhan and Ritchie.
