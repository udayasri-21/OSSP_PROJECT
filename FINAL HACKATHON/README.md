<<<<<<< HEAD
# Linux Process Tree Visualizer — C + Linux version

This project combines a browser dashboard with a C program that demonstrates Linux process/system programming. It is designed for Ubuntu 24.04 under WSL2 or a native Linux environment.

## Requirements
- Ubuntu/Linux, including WSL2 Ubuntu
- Python 3
- GCC compiler
- procps (`ps` command)

## Run from Ubuntu
From the extracted project directory, run:
```bash
sudo apt update
sudo apt install build-essential procps python3
python3 server.py
```
Open `http://127.0.0.1:8000` in Windows Chrome while the Ubuntu terminal remains open.

## If the ZIP is in Windows Downloads
If the extracted folder is at `C:\Users\surab\Downloads\Linux_Process_Tree_Visualizer_C`, the path in WSL is normally:
```bash
cd /mnt/c/Users/surab/Downloads/Linux_Process_Tree_Visualizer_C
```
Then run the commands above. If you extract it elsewhere, adjust the path to match.

## C demonstrations
The web interface compiles `os_demo.c` with GCC when a demo is selected:
- `tree`: `execlp()` launches Linux `ps --forest`
- `fork`: `fork()`, `getpid()`, `getppid()`, `waitpid()`
- `exec`: `execlp()` replaces the current process image with `echo`
- `wait`: parent uses `waitpid()` and reads child exit status
- `signal`: `sigaction()`, `kill()` and `SIGUSR1` sent to the same process
- `ipc`: anonymous `pipe()`, `read()`, `write()`, `fork()`, and `waitpid()`

The demos are bounded and intended for local coursework. They do not signal or terminate unrelated processes. The HTTP server binds only to 127.0.0.1, not the network.

## Syllabus relevance
- CO-1: Linux shell, user-space programs, system calls and kernel services.
- CO-2: PID/PPID, process creation with fork, execution with exec, termination and wait status, process states.
- CO-3: POSIX signal handler and anonymous pipe IPC. Process groups/FIFOs are not yet implemented.
- CO-4: Process memory/address-space details are not yet implemented.
- CO-5: File-system internals are not demonstrated in this starter.
- CO-6: No POSIX thread/mutex/semaphore demonstration yet.

## Notes
- Do not open `index.html` directly; use the server so API calls work.
- If port 8000 is busy, change `PORT=8000` in `server.py` and open that port in the browser.
- Output from the live tree is from `ps`, while the system-call demonstrations are compiled C code running in Linux.
=======
# Linux-Process-Tree-Visualizer
>>>>>>> aaed4a5ab67c857bb5096cb8ce3a7388ea9e6a04
