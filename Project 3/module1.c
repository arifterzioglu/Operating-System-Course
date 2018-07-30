#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/mm.h>
#include <linux/stat.h>
#include <linux/highmem.h>
#include <linux/sched.h>
#include <asm/page.h>

#define PD_PAGESIZE 4096
MODULE_LICENSE("Dual BSD/GPL");



static int processid = 1;
unsigned long present;
unsigned long writable;
unsigned long userAccesible;
unsigned long writeCache;
unsigned long disableCache;
unsigned long accessed;
unsigned long ignored;
unsigned long huge;
unsigned long global;
unsigned long available;
unsigned long physicalAddress;
unsigned long available2;
unsigned long accessed2;



static void print_mem(struct task_struct *task)
{
    struct mm_struct *mm;
    struct vm_area_struct *vma;
    mm = task->mm;

   /* for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        printk("0x%lx  0x%lx\n", vma->vm_start,vma->vm_end);
    }*/

    //start, end and size of code

    for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        if((vma->vm_start <= mm->start_code) && (vma->vm_end >= mm->start_code))
        {

            printk("Code Segment start = 0x%lx end =  0x%lx size =  %lu\n", vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
            break;
        }
    }

    //Data segment
    for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        if((vma->vm_start <= mm->start_data) && (vma->vm_end >= mm->start_data))
        {

            printk("Data Segment start = 0x%lx end =  0x%lx size =  %lu\n", vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
            break;
        }
    }



    for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        if((vma->vm_start <= mm->start_stack) && (vma->vm_end >= mm->start_stack))
        {

            printk("Stack Segment start = 0x%lx end =  0x%lx size =  %lu\n", vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
            break;
        }
    }

    //start, end and size of the heap
    printk("Heap  Segment start = 0x%lx, end = 0x%lx, size %lu\n"
        ,mm->start_brk, mm->brk, (mm->brk - mm->start_brk));




    //start, end and size of the  main arg
    for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        if((vma->vm_start <= mm->arg_start) && (vma->vm_end >= mm->arg_start))
        {

            printk("Arg Segment start = 0x%lx end =  0x%lx size =  %lu\n", vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
            break;
        }
    }

    //start, end and size of environment var.
    for(vma = mm->mmap ; vma; vma = vma->vm_next)
    {
        if((vma->vm_start <= mm->env_start) && (vma->vm_end >= mm->env_start))
        {

            printk("Env Segment start = 0x%lx end =  0x%lx size =  %lu\n", vma->vm_start, vma->vm_end,(vma->vm_end - vma->vm_start));
            break;
        }
    }

    //number of frames used
    printk("VM Number of Frames = 0x%lu\n", get_mm_rss(mm));


    //total virtual memory used by the process
    printk("Task size = 0x%lu\n", mm->task_size );
    printk("VM size = 0x%lu\n", (mm->total_vm * 4096));






}

static void printTop(struct task_struct *task)
{
    unsigned long  pgd_entry;
    int  i = 0;
    do
    {
        pgd_entry = pgd_val(task->mm->pgd[i]);
        if(pgd_entry !=0 )
        {
            unsigned short mask;

            mask = 1;
            present = (pgd_entry & mask);

            mask = (1 << 1);
            writable= (pgd_entry & mask) >> 1;

            mask = 1 << 2;
            userAccesible = (pgd_entry & mask) >> 2;

            mask = 1 << 3;
            writeCache = (pgd_entry & mask) >> 3;

            mask = 1 << 4;
            disableCache = (pgd_entry & mask) >> 4;

            mask = 1 << 5;
            accessed = (pgd_entry & mask) >> 5;

            mask = 1 << 6;
            ignored = (pgd_entry & mask) >> 6;

            mask = 1 << 7;
            huge = (pgd_entry & mask) >> 7;



            mask = (1 << (3)) - 1;
            global = (pgd_entry >> 8 ) & mask;

            mask = (1 << (38)) - 1;
            available = (pgd_entry >> 12 ) & mask;



            mask =  1;
            physicalAddress = (pgd_entry & mask) >> 51;

            mask = (1 << (10)) - 1;
            available2 = (pgd_entry >> 52 ) & mask;

            mask =  1;
            accessed2 = (pgd_entry & mask) >> 63;

            printk("%lx\n",pgd_entry);
            printk("%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu\n",present,writable,userAccesible,writeCache,disableCache,accessed,ignored,huge,global,available,physicalAddress,available2,accessed2);


        }

        i++;

    }
    while(i<512);

}
int init_module(void)
{
    struct task_struct *task;
    printk("\nGot the process id to look up as %d.\n",processid);
    task = &init_task;

    do
    {
        if (task->pid == processid)
        {
            printk("%s[%d]\n", task->comm, task->pid);
            print_mem(task);
            printTop(task);
        }
    }
    while((task=next_task(task)) != &init_task);

        return 0;

}

void cleanup_module(void)
{
    printk("\nPrint segment information module exiting.\n");
}

module_param(processid, int, 0);




