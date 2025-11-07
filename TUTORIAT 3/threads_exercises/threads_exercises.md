# Threads exercises

1. Sum the first n (chosen by you) natural numbers using two threads.  
The main thread sums the first half, the secondary sums the second half. Both print their partial sums and the main will also print the whole sum.
**Solution** in sum.c

2. Given a matrix (you can hardcode it or read it from a file, whatever you prefer) scale it by a number (your choice).  
Each thread should scale a row.
**Solution.** in scale_matrix.c

3. Implement a client-server architecture (just like in TUTORIAT 2/sockets_exercises) but with multiple threads now. The server should create a new thread for each new accepted connection. The client should also create multiple connections to the server, each being handled by a separate thread. The data sent between threads can be any message you want.
**No Solution**