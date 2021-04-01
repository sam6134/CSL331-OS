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
    for(int i=0;i<1000000;i++); // extra time to give real time delays in memory access
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
    // accessed if any of the three read/ write and execute are  not protected
    // other protection fault is raised
    if(protected_bits  == 7) return 0;
    return 1;
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
        // won't allocate pf 0 (since it contains TLB)
        main_memory[0].pf.pages[i].page_table_entry.PFN = 1 + (rand()%(1<<13)); 

        // setting up valid bit 0 with 0.005 probability
        if(rand()%500 == 0){
            main_memory[0].pf.pages[i].page_table_entry.valid = 0;
        }
        else{
           main_memory[0].pf.pages[i].page_table_entry.valid = 1; 
        }
        // initially bits are not dirty
        main_memory[0].pf.pages[i].page_table_entry.dirty = 0;

        // assuming all are in user mode
        main_memory[0].pf.pages[i].page_table_entry.mode = 0;

        // assuming enough memmory for this particular process
        main_memory[0].pf.pages[i].page_table_entry.present = 1;

        main_memory[0].pf.pages[i].page_table_entry.protected = 0;
        // adding read/ write and execute protected with probability 0.01
        if(rand()%100 == 0){
             main_memory[0].pf.pages[i].page_table_entry.protected += 1;
        }
        if(rand()%100 == 0){
             main_memory[0].pf.pages[i].page_table_entry.protected += (1<<1);
        }
        if(rand()%100 == 0){
             main_memory[0].pf.pages[i].page_table_entry.protected += (1<<2);
        }
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

// the random function from Lab-1
uint32_t startState = 0xACEu;

uint32_t genLCG()
{
    uint32_t a = 13;
    uint32_t c = 1;
    uint32_t m = (1<<16);
    startState = (a*startState + c)%m;
    return startState;
}

int main(int argc, char* argv[])
{
    printf("\n");
    printf("**************************************************************\n\n");
    printf("CSL331 Simulation-1 © Samarth Singh\n\n");
    printf("The page number and offsets are separated by | for better visibility.\n");
    printf("This simulation only provides address translations without buffer. To give realistic delays in memory access a sleep of 1ms is added on every memory access\n");
    printf("**************************************************************\n\n");
    
    srand(time(NULL));
    clock_t start, end;
    int n; //  number of pages
    main_memory = (struct MainMemory*)malloc(sizeof(struct MainMemory)*(1<<14)); // (2^14) PFN's
    
    // each page frame allocate 2^10 pages
    for(int i=0; i<(1<<14);i++){
        main_memory[i].pf.pages = (union Page *)malloc(sizeof(union Page)*(1<<10)); 
    }
    int s = (1<<6);
    populate_page_table(s);
    int num_of_pages [] = {25,50,75,100};
    double* avg_time = malloc(4*(sizeof(double)));
    for(int j=0;j<4;j++){
        n = num_of_pages[j]; 
        printf("Number of pages to be accessed: %d\n\n\n",n);
        uint32_t* arr = (uint32_t*) malloc(n*sizeof(uint32_t));
        for(int i=0;i<10;i++){

            printf("Trial %d\n\n",i+1);

            for(int i=0;i<n;i++){
                arr[i] = genLCG();
            }

            double tot_time = 0;
            // printf("Chk1 %d\n",main_memory[0].pf.pages[0].page_table_entry.PFN);
            // printf("Got-%d\n",(convert_to_physical(0)>>PFN_SHIFT) );
            for(int i=0;i<n;i++){
                
                printf("The Virtual Address is ");
                print_binary(arr[i], 1);
                start = clock();
                uint32_t physical_addr = convert_to_physical(arr[i]);
                end = clock();
                if(physical_addr != -1){
                    printf("Converted Physical Address is ");
                    print_binary(physical_addr, 0);
                }else{
                    break;
                }
                tot_time += ((end-start)*1000)/CLOCKS_PER_SEC;
            }
            avg_time[j] += tot_time;
            printf("Trial %d ended\n", i+1);
            printf("===============================================\n\n");
        }

        printf("Total access time for %d pages, (avgd for 10 trials) - %f ms\n", n,avg_time[j]/10);
        printf("===============================================\n\n\n\n");
    }
    printf("----FINAL RESULT SUMMARY---\n\n");
    for(int i=0;i<4;i++)
    {
        printf("Total access time for %d pages, (avgd for 10 trials) - %f ms\n", num_of_pages[i],avg_time[i]/10);   
    }
    printf("Simualtion ended !! \n\n");
    return 0;
}