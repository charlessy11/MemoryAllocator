# Project 3: Memory Allocator

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-3.html 

**About This Program:**

The Memory Allocator Program is a custom memory allocator.To simplify the allocator, mmap alone is being used, and entire **regions** of memory are allocated at a time. Moreover, this program does not only support allocating memory, but also uses the _Free Space Management_ (FSM) algorithms to split up and reuse empty regions, namely: **First fit, Best fit, and Worst fit**. 

To compile and use the allocator:

```bash
make
LD_PRELOAD=$(pwd)/allocator.so ls /
```

(in this example, the command `ls /` is run with the custom memory allocator instead of the default).

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
