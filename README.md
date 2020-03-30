# Goblin Shell - Remote Execution Shell
Goblin Shell allows users to execute local programs on a remote host and have the program output returned to their local machine. This can be useful for running small programs on different operating systems to test compatibility.

## Setup
Download the necessary files using the following git command from your terminal:

```
git clone https://github.com/hunteroatway/goblin-shell.git
```

Compile the shell using the Makefile included in the repository. Running 'make' via your terminal will build the necessary files required for operation of the execution environment. 

## Usage
Run the shell by executing the following on your terminal. 

```
./goblin-shell <remote-host>
```

Note: The <remote-host> field needs to be included when running the shell. This specifies the IP address/DNS of the remote computer you wish to run your program on.

Goblin Shell supports the compilation of various files on the remote host. It is often necessary to check compatibility at compile-time as opposed to run-time. Compiling a file on the remote host can be done via the **compile** command. 

```
compile example.c
``` 

Running the program on the remote host is just as simple as compiling. By using the **run** command, the executable file will be ran on the specified remote computer and the output will be return to the local machine.

```
run example
```

Goblin Shell needs to transfer files to the remote host to compile and execute the program, therefore it is necessary to clean the files so that they don't build up over time. This can be easily accomplished using the **clean** command.

```
clean
```

Exiting the shell can be done with many different commands such as **exit**, **quit**, **lo**, and **shutdown**.

```
exit
```

## Example

An example program has been provided for testing purposes. The example program is contained within raid.c and can be run on a remote host to make sure everything is working as intended.
