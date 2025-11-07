# Shared memory exercises

1. Sum the first n elements(chosen by you) using two processes.  
The parent sums the first half, the child sums the second half. Both print their partial sums and the parent will also print the whole sum.
**Solution** is in sum.c  

2. Given a matrix (you can hardcode it or read it from a file, whatever you prefer) scale it by a number (your choice).  
Each process you create for this should scale a row.
**No solution.**  
**Hint**: In TUTORIAT 3 there is a scale_matrix.c which implements this for threads
