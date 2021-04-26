# Test Output

Mon Apr 26 14:16:53 PDT 2021  
Commit 718718de5d7f3e7d2e45e27d65f06b9ed0d8f3db  

Tests: [01](./outputs/01.md) &nbsp;
[02](./outputs/02.md) &nbsp;
[03](./outputs/03.md) &nbsp;
[04](./outputs/04.md) &nbsp;
[05](./outputs/05.md) &nbsp;
[06](./outputs/06.md) &nbsp;
[07](./outputs/07.md) &nbsp;
[08](./outputs/08.md) &nbsp;
[09](./outputs/09.md) &nbsp;
[10](./outputs/10.md) &nbsp;
[11](./outputs/11.md) &nbsp;
[12](./outputs/12.md) &nbsp;
[13](./outputs/13.md) &nbsp;
[14](./outputs/14.md) &nbsp;
[15](./outputs/15.md) &nbsp;
[16](./outputs/16.md) &nbsp;
[17](./outputs/17.md) &nbsp;
[18](./outputs/18.md) &nbsp;

```
Building test programs
cc -Wall -g -pthread -Wno-unused-variable  allocations-1.c -o allocations-1
cc -Wall -g -pthread -Wno-unused-variable  allocations-2.c -o allocations-2
cc -Wall -g -pthread -Wno-unused-variable  allocations-3.c -o allocations-3
cc -Wall -g -pthread -Wno-unused-variable    print-test.c   -o print-test
cc -Wall -g -pthread -Wno-unused-variable    scribble.c   -o scribble
cc -Wall -g -pthread -Wno-unused-variable    thread-safety.c   -o thread-safety
Running Tests: (v22)
 * 01 Basic Allocation     [1 pts]  [  OK  ]
 * 02 ls                   [1 pts]  [  OK  ]
 * 03 Basic First Fit      [1 pts]  [  OK  ]
 * 04 Basic Best Fit       [1 pts]  [  OK  ]
 * 05 Basic Worst Fit      [1 pts]  [  OK  ]
 * 06 First Fit            [1 pts]  [  OK  ]
 * 07 Best Fit             [1 pts]  [  OK  ]
 * 08 Worst Fit            [1 pts]  [  OK  ]
 * 09 Scribbling           [1 pts]  [  OK  ]
 * 10 Thread Safety        [1 pts]  [  OK  ]
 * 11 Unix Utilities       [1 pts]  [  OK  ]
 * 12 Static Analysis      [1 pts]  [  OK  ]
 * 13 Documentation        [1 pts]  [  OK  ]
 * 14 calloc               [1 pts]  [  OK  ]
 * 15 Free                 [1 pts]  [  OK  ]
 * 16 Pandemic Pity Point  [1 pts]  [  OK  ]
 * 17 Penguin Heist        [1 pts]  [  OK  ]
 * 18 The Daniel Test      [0 pts]  [  OK  ]
Execution complete. [17/17 pts] (100.0%)

```
