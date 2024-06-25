# alloc_hugepage_in_numa_node

#### this project applied two 1G hugepage and set the page in the different numa node in the system, and then calculate the actually physical address for FPGA DMA access

### requirement
#### sudo apt install libnuma-dev
#### sudo apt install numactl

### set the system arrange 2 1G hugepage
#### in the grub configure hugpage support
#### in the /etc/default/grub
#### GRUB_CMDLINE_LINUX_DEFAULT="quiet splash default_hugepagesz=1G hugepagesz=1G hugepages=2"
#### #>grub-update
#### #>reboot

### build 
#### #>build.sh

### run app
#### ./hugepages
#### the app will mmap two 1G memory and the memory will be in different node. 
#### the app will wait until you input "space" "Enter"  


