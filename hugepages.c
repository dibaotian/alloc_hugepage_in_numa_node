#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <numa.h>
#include <numaif.h>

#include <ncurses.h>

#define GIGABYTE (1UL << 30)

unsigned long long get_physical_address(void *virtual_addr) {
    unsigned long long paddr;
    unsigned long long vaddr = (unsigned long long)virtual_addr;
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    unsigned long long offset = (vaddr / sysconf(_SC_PAGESIZE)) * sizeof(unsigned long long);
    if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        exit(1);
    }

    if (read(fd, &paddr, sizeof(unsigned long long)) != sizeof(unsigned long long)) {
        perror("read");
        exit(1);
    }

    close(fd);

    // Check if page is present
    if (!(paddr & (1ULL << 63))) {
        fprintf(stderr, "Page not present\n");
        exit(1);
    }

    // Extract the physical page number
    paddr = (paddr & ((1ULL << 55) - 1)) * sysconf(_SC_PAGESIZE);

    // Add the page offset
    paddr |= (vaddr & (sysconf(_SC_PAGESIZE) - 1));

    return paddr;
}

int main() {
    void *addr1, *addr2;
    int status;
    char ch;


    // Ensure NUMA is available and nodes exist
    if (numa_available() == -1) {
        fprintf(stderr, "NUMA is not available\n");
        exit(1);
    }
    if (numa_num_configured_nodes() < 2) {
        fprintf(stderr, "At least two NUMA nodes are required\n");
        exit(1);
    }

    // Allocate 1GB page on node 0
    addr1 = mmap(NULL, GIGABYTE, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (addr1 == MAP_FAILED) {
        perror("mmap addr1");
        exit(1);
    }
    status = mbind(addr1, GIGABYTE, MPOL_BIND, (unsigned long[]){1}, sizeof(unsigned long) * 8, 0);
    if (status != 0) {
        perror("mbind addr1");
        munmap(addr1, GIGABYTE);
        exit(1);
    }

    // Access memory to ensure pages are allocated
    memset(addr1, 0, GIGABYTE);

    // Allocate 1GB page on node 1
    addr2 = mmap(NULL, GIGABYTE, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    if (addr2 == MAP_FAILED) {
        perror("mmap addr2");
        munmap(addr1, GIGABYTE);
        exit(1);
    }
    status = mbind(addr2, GIGABYTE, MPOL_BIND, (unsigned long[]){2}, sizeof(unsigned long) * 8, 0);
    if (status != 0) {
        perror("mbind addr2");
        munmap(addr1, GIGABYTE);
        munmap(addr2, GIGABYTE);
        exit(1);
    }

    // Access memory to ensure pages are allocated
    memset(addr2, 0, GIGABYTE);

    printf("Virtual address of addr1: %p\n", addr1);
    printf("Virtual address of addr2: %p\n", addr2);

    unsigned long long paddr1 = get_physical_address(addr1);
    unsigned long long paddr2 = get_physical_address(addr2);

    printf("Physical address of addr1: 0x%llx\n", paddr1);
    printf("Physical address of addr2: 0x%llx\n", paddr2);

    while (1) {
        printf("Enter a character (' ' to exit): ");
        ch = getchar();  // 获取键盘输入

        if (ch == ' ') {
            break;  // 如果按下空格键，退出循环
        } else {
            printf("Key pressed: %c\n", ch);
            // 在这里执行其他操作
        }
    }


    // Clean up
    printf("munmap\n");
    munmap(addr1, GIGABYTE);
    munmap(addr2, GIGABYTE);

    return 0;
}
