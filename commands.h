

#pragma once

#include <slankdev/dpdk_header.h>

void lscpu()
{
    printf("Architecture        : \n");
    printf("CPU op-mode(s)      : \n");

    uint16_t before = 0x1234;
    uint16_t after  = rte_le_to_cpu_16(before);
    printf("Byte Order          : %s \n",before==after?"Little Endian":"Big Endian");

    printf("CPU(s):             : \n");
    printf("On-line CPU(s) list : \n");
    printf("Thread(s) per core  : \n");
    printf("Core(s) per socket  : \n");
    printf("Socket(s)           : \n");
    printf("NUMA node(s)        : \n");
    printf("Vendor ID           : \n");
    printf("CPU family          : \n");
    printf("Model               : \n");
    printf("Model name          : \n");
    printf("Stepping            : \n");
    printf("CPU MHz             : \n");
    printf("CPU max MHz         : \n");
    printf("CPU min MHz         : \n");
    printf("BogoMIPS            : \n");
    printf("Virtualization      : \n");
    printf("L1d cache           : \n");
    printf("L1i cache           : \n");
    printf("L2 cache            : \n");
    printf("L3 cache            : \n");
    printf("NUMA node0 CPU(s)   : \n");

    printf("Flags               : ");
    // for (size_t i=0; i<RTE_CPUFLAG_NUMFLAGS; i++) {
    //     if (sys->cpuflags[i])
    //         printf("%s ", rte_cpu_get_flag_name(rte_cpu_flag_t(i)));
    // }
    printf("\n");
}

