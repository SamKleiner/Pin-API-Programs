This project focused on profiling, instrumentation, and performance analysis to identify bottlenecks in application runtime and memory complexity using perf and Intel's Pin.

Part 1. Profiling with perf  
Measure hardware events (e.g., CPU cycles, cache misses).  
Gather performance statistics for Sum and Fibonacci programs.  

Part 2. Instrumentation with Pin  
Develop tools to:  
Count dynamic instruction execution per function.  
Generate function call graphs.  
Track memory access patterns during execution.

Part 3. Performance Analysis  
Compare execution times and cache performance.  
Analyze runtime complexity and resource utilization.  
Explain performance differences using profiling and instrumentation data.

Programs  
The project analyzes two sets of programs:  
Sum Programs: Compute the sum of the first n natural numbers.  
Fibonacci Programs: Compute the nth Fibonacci number.

Tools and Technologies  
perf: Linux profiling tool for measuring hardware events.  
Intel Pin: A dynamic binary instrumentation framework.

Deliverables  
Profiling data and analysis report.  
Custom Pin tools:  
inst_count.cpp: Counts instructions executed.  
call_graph.cpp: Generates function call graphs.  
mem_trace.cpp: Tracks memory access patterns.  
