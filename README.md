# OS_EX3
Operating Systems - Assignment 3    
By Maor Saadon and Matan Adar   
ID 318532421 & ID 209321552  

## Introduction  
In this assignment we implemented a tool called STNC (Stands for Student Network Communication) tool, that has two following main functions.

### Chat  
A chat cmd tool that can send messages over the network, to the same tool, listening on the other side, and get the response,   
so there will be 2 sides communication. The communication is based of IPv4 TCP protocol.  

## Network Performance Tool
Network performance tool that checks the internal and external network between two processes (that can be in two different machines). 
The tool generates a 100MB of random data and sends it to the other side via one of the 8 different protocols:
    1.IPv4 – TCP or UDP (local or remote machine).
    2.IPv6 – TCP or UDP (local or remote machine).
    3.Unix Domain Socket (UDS) – Stream (equivalent to TCP) or Datagram (equivalent to UDP), local machine only.
    4.Memory Mapped File – Share the generated data via memory mapping (local machine only).
    5.Piping – Transfer the generated data via piping (local machine only).

## Requirements
* Linux machine (Ubuntu 22.04 LTS recommended)
* GNU C Compiler (GCC)
* Make (For Makefile)

## Building
In order to run this project follow this steps:    
  1. Download zip from our reposetory : push on code -> Download zip. 
     Or Cloning the reposetory to local machine: git clone https://github.com/maorsaadon/OS_Ex3.git
  2. Extract all on yor computer.   
  3. Open the file in the workspace - Ubuntu 20.04 LTS.  
  4. Export shared libraries: export LD_LIBRARY_PATH="."  
  5. Execute "make all"  
  6. Run any of the following tools 

## Running

1.Open STNC in client normal chat mode.  
./stnc -c <IP> <PORT>  

2.Open STNC in server normal chat mode.  
./stnc -s <PORT>  

3.Open STNC in client performance mode.  
./stnc -c <IP> <PORT> -p <TYPE> <PARAM>  

4.Open STNC in server performance mode.  
./stnc -s <PORT> -p  

5.Open STNC in server performance and quiet mode.  
./stnc -s <PORT> -p -q  

Options for TYPE and PARAM: 
1.IPv4 & TCP mode - ipv4 tcp  
2.IPv4 & UDP mode - ipv4 udp  
3.IPv6 & TCP mode - ipv6 tcp  
4.IPv6 & UDP mode - ipv6 udp  
5.UDS & Datagram mode - uds dgram  
6.UDS & Stream mode - uds stream  
7.Memory map mode with specific file - mmap filename  
8.Pipe mode with specific file - pipe filename  
