# Pipes exercises

1. Create **a unidirectional pipe** between a process and it's child.  
The parent should send the numbers from 0 to 10 through the pipe and the child should read and print the received numbers.  
**Solution** is in unidir_pipe.c
   
2. Make the pipe from earlier **bidirectional**.  
The child should increment the value received and send it back to the parent.  
The parent should also read the updated value from the child and print it to the screen.  
**Solution** is bidir_pipe.c  
   
3. Create a **circular pipeline having 3 processes**.  
Process 1 sends numbers between 0 and 5 through a pipe to process 2 (**independent processes**, no parent-child relationship).  
Process 2 squares the received number and sends it to **it's child**, process 3.  
Process 3 adds 10 to the received number and sends to process 1 which will print the result.  
**Hint**: Look at pipeline.png to get a better idea of what you need to do.  
**Solution** is in pipeline_boss.c and pipeline_workers.c