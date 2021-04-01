#include <stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>
#include<time.h>
#include<pthread.h>


// Node Structure for Storing Commands
struct PTE{
    unsigned int PFN: 14;
    unsigned int valid : 1;
    unsigned int present : 1;
    unsigned int protected : 3;
    unsigned int dirty : 1;
    unsigned int mode : 1;
};

struct TLB_Entry{
    unsigned int VPN: 6;
    unsigned int PFN: 14;
    unsigned int protected: 3;
    unsigned int valid: 1;
    unsigned int dirty: 1;
    unsigned int ASID: 8;
    unsigned int use: 1;
    unsigned int empty: 1;
};

struct TLB_Entry* TLB;
int TLB_SIZE;
int global_ptr;

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
int done = 0;

// reset function called on each timer
void reset_use_bits()
{
     for(int i = 0; i <TLB_SIZE; i++)
     {
         TLB[i].use = 0;
     }
     return;
}

// thread proccess for timer function ( to be called after every 1ms)
void *threadproc(void *arg)
{
    while(!done)
    {
        usleep(1000);
        reset_use_bits();
    }
    return 0;
}

int max(int a,int b){
    if(a>b) return a;
    else return b;
}


union Page AccessMemory(uint32_t Phy_addr, int pte_bit)
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

struct TLB_Entry TLB_Lookup(uint32_t VPN, int* success){
    for(int i=0;i<TLB_SIZE;i++)
    {
        if(TLB[i].VPN == VPN && TLB[i].empty != 0)
        {
            TLB[i].use = 1;
            return TLB[i];
        }
    }
    struct TLB_Entry dummy;
    *success = -1;
    return dummy;
}

void clock_algorithm()
{
    while(1)
    {
        if(TLB[global_ptr].use == 0)
        {
            // found replacement;
            return;
        }else{
            // reset the bit and carry on
            TLB[global_ptr].use = 0;
            global_ptr = (global_ptr + 1)%TLB_SIZE;
        }
    }
}

void TLB_Insert(uint32_t VPN, uint32_t PFN, unsigned int protected_bits, unsigned int valid)
{
    struct TLB_Entry new_entry;
    new_entry.VPN = VPN;
    new_entry.PFN = PFN;
    new_entry.protected = protected_bits;
    new_entry.valid = valid;
    new_entry.use = 1;
    new_entry.empty = 1;
    if(TLB[global_ptr].empty == 0)
    {
        // empty position
        TLB[global_ptr] = new_entry;
        global_ptr = (global_ptr + 1)%TLB_SIZE;
    }else{
        // apply clock algorithm
        clock_algorithm();
        TLB[global_ptr] = new_entry;
        global_ptr = (global_ptr + 1)%TLB_SIZE;
    }

}

uint32_t convert_to_physical(uint32_t virtual_addr)
{
    uint32_t VPN = (virtual_addr & VPN_MASK) >> SHIFT;
    int success = 1;
    struct TLB_Entry tlb_ret = TLB_Lookup(VPN, &success);
    uint32_t offset, PhyAddr;
    if(success != -1)
    {
        printf("TLB hit!!\n");
        if(CanAccess(tlb_ret.protected) == 1)
        {
            offset = virtual_addr & OFFSET_MASK;
            PhyAddr = (tlb_ret.PFN << PFN_SHIFT) | offset;
            AccessMemory(PhyAddr, 0);
            return PhyAddr;
        }else{
            printf("\n\n**PROTECTION_FAULT**\n\n");
            return -1;
        }
    }
    printf("TLB Miss\n");
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
    TLB_Insert(VPN, Page_table_entry.PFN, Page_table_entry.protected, Page_table_entry.valid);
    return convert_to_physical(virtual_addr);

}

void populate_page_table(int n)
{
    srand(time(NULL));
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
// random number generator LCG
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
    printf("CSL331 Simulation-2 © Samarth Singh\n\n");
    printf("The page number and offsets are separated by | for better visibility.\n");
    printf("This simulation only provides address translations with TLB buffer size chosen by the user.\n To give realistic delays in memory access a sleep of 1 sec is added on every successful address transaltion from page Table\n");
    printf("**************************************************************\n\n");
    
    global_ptr = 0;
    clock_t start, end;
    int n; //  number of pages
    // allocate main_memory
    main_memory = (struct MainMemory*)malloc(sizeof(struct MainMemory)*(1<<14)); // (2^14) PFN's
    // for each page_frame allocate 2^10 pages
    for(int i=0; i<(1<<14);i++){
        main_memory[i].pf.pages = (union Page *)malloc(sizeof(union Page)*(1<<10)); 
    }
    srand(time(NULL));
    
    uint32_t* arr = (uint32_t*) malloc(n*sizeof(uint32_t));
    int s = (1<<6);
    populate_page_table(s);

    // printf("Chk1 %d\n",main_memory[0].pf.pages[0].page_table_entry.PFN);
    // printf("Got-%d\n",(convert_to_physical(0)>>PFN_SHIFT) );

    int num_of_pages [] = {25,50,75,100};
    int tlb_sizes[] = {8,16,32};
    double* avg_time = malloc(12*(sizeof(double)));

    int avg_time_idx = 0;

    for(int j=0;j<4;j++){
        n = num_of_pages[j]; 
        uint32_t* arr = (uint32_t*) malloc(n*sizeof(uint32_t));

        
        for(int k=0;k<3;k++){

            TLB_SIZE = tlb_sizes[k];
            printf("Number of pages to be accessed: %d with TLB size %d\n\n\n",n, tlb_sizes[k]);

            for(int i=0;i<10;i++){

                TLB = (struct TLB_Entry *) malloc(TLB_SIZE*sizeof(struct TLB_Entry));
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
                avg_time[avg_time_idx] += tot_time;
                printf("Trial %d ended\n", i+1);
                printf("===============================================\n\n");
                free(TLB);
            }

            printf("Total access time for %d pages with TLB size %d, (avgd for 10 trials) - %f ms\n", n,TLB_SIZE,avg_time[avg_time_idx]/10);
            printf("===============================================\n\n\n\n");
            avg_time_idx++;
        }
        free(arr);
    }
    printf("----FINAL RESULT SUMMARY---\n\n");
    avg_time_idx = 0;
    for(int i=0;i<4;i++)
    {
        printf("Total access time for %d pages, (avgd for 10 trials)\n",num_of_pages[i]);
        for(int j=0;j<3;j++)
        {
            printf("TLB size %d - %f ms\n", tlb_sizes[j],avg_time[avg_time_idx]/10);
            avg_time_idx ++;
        }
        printf("\n");   
    }
    printf("Simualtion ended !! \n\n");
    return 0;
}