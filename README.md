# Goblin Shell - Remote Execution Shell
Goblin Shell allows users to execute local programs on a remote host and have the program output returned to their local machine. This can be useful for running small programs on differnt operating systems to test compatibility. Users can specify the remote host and port via a configuration file in the config/config.ini directory.

## Usage
Run the shell by executing the following on your terminal. (Note: chmod can also be used to add execution permissions on the shell.py file)

```
python3 shell.py
```

You will then be in the Goblin Shell prompt and can execute the local program that you would like to run on the remote hardware. Note: Goblin Shell supports command line arguments as well, so the following can be run.

```
./example.py arg1 arg2
```
