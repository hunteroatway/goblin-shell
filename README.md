# Goblin Shell - Remote Execution Shell
Goblin Shell allows users to compile and execute local programs on a remote host and have the program output returned to their local machine. This can be useful for running small programs on different operating systems to test compatibility.

Name(s): Hunter Oatway & Justin Ottenbreit

Student ID(s): 200378986 & 200251932

Topic(s): 
  - Sockets
  - Processes
  - Signals
  - Pipes
  - Shell Scripting

## Setup
Download the necessary files using the following git command from your terminal:

```
git clone https://github.com/hunteroatway/goblin-shell.git
```

Build the necessary files using the Makefile included with the repository. Running **make** via your terminal will build the necessary files required for operation of the execution environment. 

## Usage
The server must be started on the remote machine before a socket connection can be established. You will need to copy the **server.c** file over to the remote host and then compile using **gcc**. 

```
scp server.c <user-name>@<server-name>:server.c
gcc server.c -o server
./server
```

**Note: A script to start the server (startServer.sh) can be run. The username and servername must be provided as arguments. You may be prompted for a password multiple times.**

```
chmod +x startServer.sh
./startServer.sh <user-name> <server-name> 
``` 

Running the server will provide the port number for the socket that will be needed to start the shell.

Run the shell by executing the following on your terminal.

```
./goblin-shell -u <user-name> -s <server-name> -p <port>
```

**Note: All of the fields must be specified when running the shell**. They specify the SSH username, IP address/DNS and port of the remote computer you wish to run your program on.

Goblin Shell supports the compilation of C/C++ files (e.g. .c, .cc, .cpp) on the remote host. It is often necessary to check compatibility at compile-time as opposed to run-time. Compiling a file on the remote host can be done via the **compile** command. 

**Note: At compile time, any necessary files included in the command are copied to the remote host (e.g .h, .txt)**

```
compile <code-files> <link-files> <flags>
``` 

Running the program on the remote host is just as simple as compiling. By using the **run** command, the executable file will be ran on the specified remote computer and the output will be return to the local machine.

**Note: Command line arguments are also supported on the shell. They must be specified after the program name.**

```
run <program-name> <args>
```

Exiting the shell can be done with many different commands such as **exit**, **quit**, **lo**, and **shutdown**.

```
exit
```

## Example

An example program has been provided for testing purposes. The example program is contained within **raid.c** and can be run on a remote host to make sure everything is working as intended.

```
compile raid.c goblin.txt -pthread
run raid 
```
