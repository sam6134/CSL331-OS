#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<time.h>

// Node Structure for Storing Commands
struct PTE{
    unsigned int PFN: 14;
    unsigned int valid : 1;
    unsigned int present : 1;
    unsigned int protected : 3;
    unsigned int dirty : 1;
    unsigned int mode : 1;
};


uint32_t VPN_MASK = 0xFC00;
uint32_t PFN_MASK = 0xFFFC00;
uint32_t SHIFT = 10;
uint32_t OFFSET_MASK = 0x3FF;
uint32_t PFN_SHIFT = 10;

union Page{
    int data;
    struct PTE page_table_entry;
};

struct PageFrame{
    union Page* pages;
};

struct MainMemory
{
   struct PageFrame pf; 
};

struct MainMemory* main_memory;

// Lagged Fibonacci generator for random generation of virtual address spaces
int max(int a,int b){
    if(a>b) return a;
    else return b;
}


union Page AccessMemory(uint32_t Phy_addr,int pte_bit)
{
    usleep(100000);
    if(pte_bit)
    {
        int idx = (Phy_addr/(sizeof(struct PTE)));
        return main_memory[0].pf.pages[idx];
    }
    int pf_no = (Phy_addr & PFN_MASK) >> PFN_SHIFT;
    int offset = (Phy_addr & OFFSET_MASK);
    return main_memory[pf_no].pf.pages[offset];
}

int CanAccess(unsigned int protected_bits)
{
    if((protected_bits >> 2) == 1) return 1;
    return 0;
}

uint32_t convert_to_physical(uint32_t virtual_addr)
{
    uint32_t VPN = (virtual_addr & VPN_MASK) >> SHIFT;
    uint32_t PTE_ADDR = (VPN*sizeof(struct PTE));
    struct PTE Page_table_entry = AccessMemory(PTE_ADDR, 1).page_table_entry;
    if(Page_table_entry.valid == 0)
    {
        printf("\n\n**SEGMENTATION_FAULT**\n\n");
        return -1;
    }
    if(CanAccess(Page_table_entry.protected) == 0)
    {
        printf("\n\n**PROTECTION_FAULT**\n\n");
        return -1;
    }
    uint32_t offset = virtual_addr & OFFSET_MASK;
    uint32_t PhyAddr = (Page_table_entry.PFN << PFN_SHIFT) | offset;
    AccessMemory(PhyAddr,0);
    return PhyAddr;

}

void populate_page_table(int n)
{
    srand(time(NULL));
    // populate the first frame of the memory
    for(int i=0; i<n; i++)
    {
        main_memory[0].pf.pages[i].page_table_entry.PFN = 1 + (rand()%(1<<13)); // won't allocate pf 0 (since it contains TLB)
        main_memory[0].pf.pages[i].page_table_entry.valid = 1;
        main_memory[0].pf.pages[i].page_table_entry.dirty = rand()%2;
        main_memory[0].pf.pages[i].page_table_entry.mode = rand()%2;
        main_memory[0].pf.pages[i].page_table_entry.present = rand()%2;
        main_memory[0].pf.pages[i].page_table_entry.protected = 7;
    }
}

void print_binary(uint32_t address, int virtual)
{
    int* binary = malloc(sizeof(int)*24);
    int i=0;
    while(address)
    {
       binary[23-i] = address%2;
       address = address/2;
       i++;
    }
    if(virtual)
    {
        for(int i=8;i<=13;i++)
        {
            printf("%d",binary[i]);
        }
        printf("|");
    }else{
        for(int i=0;i<=13;i++)
        {
            printf("%d",binary[i]);
        }
        printf("|");
    }
    for(int i=14;i<24;i++) printf("%d", binary[i]);
    printf("\n");
}

int main(int argc, char* argv[])
{
    printf("\n");
    printf("**************************************************************\n\n");
    printf("CSL331 Simulation-1 © Samarth Singh\n\n");
    printf("The page number and offsets are separated by | for better visibility.\n");
    printf("This simulation only provides address translations without buffer. To give realistic delays in memory access a sleep of 1 sec is added on every successful address transaltion from page Table\n");
    printf("**************************************************************\n\n");
    
    clock_t start, end;
    int n; //  number of pages
    main_memory = (struct MainMemory*)malloc(sizeof(struct MainMemory)*(1<<14)); // (2^14) PFN's
    for(int i=0; i<(1<<14);i++){
        main_memory[i].pf.pages = (union Page *)malloc(sizeof(union Page)*(1<<10)); 
    }
    printf("Enter number of virtual addresses to generate\n");
    scanf("%d", &n);
    srand(time(NULL));
    uint32_t* arr = (uint32_t*) malloc(n*sizeof(uint32_t));
    for(int i=0;i<n;i++){
        arr[i] = rand()%(1<<16);
    }
    int s = (1<<6);
    populate_page_table(s);
    double tot_time = 0;
    // printf("Chk1 %d\n",main_memory[0].pf.pages[0].page_table_entry.PFN);
    // printf("Got-%d\n",(convert_to_physical(0)>>PFN_SHIFT) );
    for(int i=0;i<n;i++){
        
        printf("The Virtual Address is ");
        print_binary(arr[i], 1);
        start = time(NULL);
        uint32_t physical_addr = convert_to_physical(arr[i]);
        end = time(NULL);
        if(physical_addr != -1){
            printf("Converted Physical Address is ");
            print_binary(physical_addr, 0);
        }
        tot_time += (end-start);
    }
    printf("Total average access time is - %f seconds\n",(double)(tot_time/n));
    return 0;
}