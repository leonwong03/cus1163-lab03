Analysis Questions

After completing the lab, think about these questions:

1. What happens to memory when fork() creates a new process? Does the child get a complete copy of the parent's memory?

When i called fork() in C, the Child process gets a full copy of the parents memory, var, stack, and heap. With more modern OS it uses "copy on write" meaning the Parent and Child initially shares the same memory pages and the OS only creates a real copy if one of the processes changes the data. It's VERY different from Java, in Java we never manually create processes this way. When we create a new thread using "Thread or Runnable" the thread shares the same memory space as the Parent threads. There's no memory duplication at all which is why synchonization is needed in Java. With for(), ea process ends up with its own independent memory. Based off a little research I believe it makes C processes safer but more expensive than Java threads.

2. Why must unused pipe ends be closed? What would happen if you forgot to close them?

Unused pipe ends have to be closed so the OS knows when communication has finished. A pipe only signals EOF when all write ends are closed. If parent or producer forgets to close a write end, the consumer will keep waiting for more data and may block forever. If Parent didn't close its copy of the pipe, the consumers "read()" would never return 0, and the program would hang. 
With Java, streams and sockets in Java are automatically closed by garbage collection eventually and APIs manage EOF more cleanly. In C, I have to manualy close file desc or the OS assumes the pipe is still active. I was able to see how low lvl resource management works in C compared to Java which was interesting.

3. What are zombie processes and how does wait() prevent them? What would happen to your system if zombie processes accumulated?

A zombie process is a child process that finished executing but still has a entry in the process table because the parent hasnt collected its exit status. It's technically "dead" but the OS keeps its info until "wait()" or "waitpid()" is called. The "wait()" sys call prevents zombie processes because it allows Parents to get the Child's exit status and fully removes it from the sys. In terms of this lab, waitpid() made sure that every Child process was cleaned up properly. 
If zombie process accumulated, the sys could eventually run out of process table entries which prevents new processes from being made. 

4. How does the kernel track parent-child relationships? What happens if a parent process dies before its children?

Kernel tracks Parent Child relationships using process IDs (PIDs) and Parent process IDs (PPIDs). Every process knows who its parent is and the kernel uses that info to manage signals and cleanup. If a parent process dies before its Children, the kernel auto assigns the Children to a sys process (usually PID 1, such as init or systemd). That sys process will call "wait()" and clean up the Children when they exit.
So I noticed that in Java this is MOSTLY hidden. When a Java program exits, the JVM terminates ALL its threads automatically. In C, processes are independent and the OS has to step in to stop orphaned processes from yurning into zombies. So it was interesting to see how C gave me more control BUT had more responsibility than Java. 

