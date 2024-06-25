# alloc_hugepage_in_numa_node

#### This project applied two 1G hugepage and set the page in the different numa node in the system, and then calculate the actually physical address for FPGA DMA access

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

### other
#### check numa info
#### #> numactl --hardware
#### available: 2 nodes (0-1)
#### node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 112 113 114 115 116 117 118 119 120 121 122 123 124 125 126 127 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159 160 161 162 163 164 165 166 167
#### node 0 size: 31862 MB
#### node 0 free: 17469 MB
#### node 1 cpus: 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 100 101 102 103 104 105 106 107 108 109 110 111 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 193 194 195 196 197 198 199 200 201 202 203 204 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222 223
####  node 1 size: 32172 MB
####  node 1 free: 19017 MB
#### node distances:
#### node   0   1 
####   0:  10  32 
####   1:  32  10 

#### 

####  #>numastat
####                            node0           node1
#### numa_hit                  726827          612280
#### numa_miss                      0               0
#### numa_foreign                   0               0
#### interleave_hit              4839            5123
#### local_node                724014          470769
#### other_node                  2813          141511

#### numa_hit 在node关联的内存上申请的内存数量
#### numa_miss 不在node关联的内存上申请的内存数量
#### numa_foreign 在其他node关联的内存上申请的内存数量
#### interleave_hit 采用interleave策略申请的内存数量
#### local_node 该node上的进程在该node关联的内存上申请的内存数量
#### other_node 该node上的进程在其他node关联的内存上申请的内存数量

####
#### 在/sys/devices/system/node目录下
#### 系统中每一个Node对应的目录。在每个Node目录下/sys/devices/system/node/node0/hugepages，有关于大页内存的配置信息，一般有两个目录hugepages-1048576kB hugepages-2048kB，这是Linux系统支持的两种大页，一个是1G，一个是2M。
#### 在每个大页内存目录下有三个文件free_hugepages nr_hugepages surplus_hugepages，分别表示当前Node，当前大页内存中空闲的大页内存数、设定的大页内存数，超出使用的大页内存数。
#### cat /sys/devices/system/node/node0/hugepages/hugepages-1048576kB/nr_hugepages
#### cat /sys/devices/system/node/node1/hugepages/hugepages-1048576kB/nr_hugepages

#### 

#### #>grep Huge /proc/meminfo
#### AnonHugePages:         0 kB
#### ShmemHugePages:        0 kB
#### FileHugePages:         0 kB
#### HugePages_Total:       2
#### HugePages_Free:        0
#### HugePages_Rsvd:        0
#### HugePages_Surp:        0
#### Hugepagesize:    1048576 kB
#### Hugetlb:         2097152 kB