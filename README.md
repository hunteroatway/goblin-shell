# Goblin Shell - Remote Execution Shell
Goblin Shell allows users to execute local programs on a remote host and have the program output returned to their local machine. This can be useful for running small programs on differnt operating systems to test compatibility. Users can specify the remote host and port via a configuration file in the config/config.ini directory.

## Setup
Compile the shell using the Makefile including the repository. Running 'make' via your terminal will build the necessary files required for operation of the execution environment. 

Make sure to configure your desired host and port information in the config.ini file located in the config directory of the repository.

## Usage
Run the shell by executing the following on your terminal. (Note: chmod can also be used to add execution permissions on the shell.c file)

```
./goblin-shell
```

You will then be in the Goblin Shell prompt and can execute the local program that you would like to run on the remote hardware. Note: Goblin Shell supports command line arguments as well, so the following can be run.

```
./example.c arg1 arg2
```
