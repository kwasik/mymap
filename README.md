# mymap
Address space map based on red-black trees

## Compiling and running test program
In order to compile and run test program type:
```
cd test
make
./build/test
```
Output of the test program should look like this:
```
MEMORY LAYOUT:

Nr     vstart       vend        gap
 0       0xc6       0xef        182
 1      0x118      0x17e         41
 2      0x21f      0x230        161
 3      0x344      0x3ae        276
 4      0x45b      0x470        173
 5      0x56b      0x588        251
 6      0x659      0x6d4        209
 7      0x77e      0x799        170
 8      0x861      0x8a5        200
 9      0x9b8      0x9c1        275
10      0xa3e      0xaab        125
11      0xb67      0xb89        188
12      0xc98      0xcaf        271
13      0xdfc      0xe0e        333
14      0xe66      0xe99         88
15      0xf7e      0xf84        229

Last gap =        125

TREE STRUCTURE:

            ┌── r: (vaddr: 0xf7e, vend: 0xf84, gap: 229, max_gap: 229)
        ┌── r: (vaddr: 0xe66, vend: 0xe99, gap: 88, max_gap: 333)
        │   └── r: (vaddr: 0xdfc, vend: 0xe0e, gap: 333, max_gap: 333)
    ┌── r: (vaddr: 0xc98, vend: 0xcaf, gap: 271, max_gap: 333)
    │   │   ┌── r: (vaddr: 0xb67, vend: 0xb89, gap: 188, max_gap: 188)
    │   └── r: (vaddr: 0xa3e, vend: 0xaab, gap: 125, max_gap: 275)
    │       └── r: (vaddr: 0x9b8, vend: 0x9c1, gap: 275, max_gap: 275)
┌── r: (vaddr: 0x861, vend: 0x8a5, gap: 200, max_gap: 333)
│   │       ┌── r: (vaddr: 0x77e, vend: 0x799, gap: 170, max_gap: 170)
│   │   ┌── r: (vaddr: 0x659, vend: 0x6d4, gap: 209, max_gap: 251)
│   │   │   └── r: (vaddr: 0x56b, vend: 0x588, gap: 251, max_gap: 251)
│   └── r: (vaddr: 0x45b, vend: 0x470, gap: 173, max_gap: 276)
│       │   ┌── r: (vaddr: 0x344, vend: 0x3ae, gap: 276, max_gap: 276)
│       └── r: (vaddr: 0x21f, vend: 0x230, gap: 161, max_gap: 276)
│           └── r: (vaddr: 0x118, vend: 0x17e, gap: 41, max_gap: 182)
│               └── r: (vaddr: 0xc6, vend: 0xef, gap: 182, max_gap: 182)

Press any key to run tests...


  nr      vaddr       size                array                 tree
   0      0xde5        133                0xe99                0xe99
   1      0x778       1019   0xffffffffffffffff   0xffffffffffffffff
   2      0x906        602   0xffffffffffffffff   0xffffffffffffffff
   3      0x74f        958   0xffffffffffffffff   0xffffffffffffffff
   4      0xca8        350   0xffffffffffffffff   0xffffffffffffffff
   5       0xaf        164                0x230                0x230
   6      0x7e9        495   0xffffffffffffffff   0xffffffffffffffff
   7      0x45f       1216   0xffffffffffffffff   0xffffffffffffffff
   8      0xddc        256   0xffffffffffffffff   0xffffffffffffffff
   9      0xecf          7                0xecf                0xecf
  10      0x74d        302                0xcaf                0xcaf
  11      0x41a       1514   0xffffffffffffffff   0xffffffffffffffff
  12      0x76c        201                0x8a5                0x8a5
  13      0xfaa         34                0xfaa                0xfaa
  14      0x7cc        466   0xffffffffffffffff   0xffffffffffffffff
  15      0xda7        108                0xe99                0xe99
  16      0xeef         44                0xeef                0xeef
  17      0x4b0        728   0xffffffffffffffff   0xffffffffffffffff
  18      0xfef          6                0xfef                0xfef
  19      0x5cf       1035   0xffffffffffffffff   0xffffffffffffffff
  20      0x915        361   0xffffffffffffffff   0xffffffffffffffff
  21      0xdfd         17                0xe0e                0xe0e
  22      0xe1e         36                0xe1e                0xe1e
  23      0xe15        185                0xe99                0xe99
  24      0x161       1520   0xffffffffffffffff   0xffffffffffffffff
  25      0xcd9        220                0xcd9                0xcd9
  26      0x157        118                0x17e                0x17e
  27      0x896        522   0xffffffffffffffff   0xffffffffffffffff
  28      0x3f0        805   0xffffffffffffffff   0xffffffffffffffff
  29      0x5a3        976   0xffffffffffffffff   0xffffffffffffffff
  30      0xf64         16                0xf64                0xf64
  31      0x18c       1670   0xffffffffffffffff   0xffffffffffffffff
  32      0x888        374   0xffffffffffffffff   0xffffffffffffffff
  ```
  First part shows what layout of the virtual memory looks like, where:
  `vstart` - beginning of the region (first byte)
  `vend` - end of the region (first byte AFTER the region)
  `gap` - size of the gap (unused area) beffore the region
  
  Second part shows the structure of the tree used to store mapped regions. `max_gap` is the size of the biggest gap in the subtree (including the root of the subtree where the information is stored).
  
  Finally, the third part shows the results of the tests. Each part consists of comparing results of two methods of finding unmapped areas - one using array to store region descriptors (function called `_get_unmapped_area`) and one using tree (`mymap_get_unmapped_area`). Results of both methods, suggested virtual address, size and the number of test are displayed in table.
