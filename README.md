*This project has been created as part of the 42 curriculum by maaugust.*

<div align="center">
  <img src="https://raw.githubusercontent.com/rfs-hybrid/42-Common-Core/main/assets/covers/cover-minitalk-bonus.png" alt="Minitalk Cover" width="100%" />
</div>

<div align="center">
  <h1>💬 Minitalk: Data Exchange Using UNIX Signals</h1>
  <img src="https://img.shields.io/badge/Language-C-blue" />
  <img src="https://img.shields.io/badge/Grade-125%2F100-success" />
  <img src="https://img.shields.io/badge/Norminette-Passing-success" />
</div>

---

## 💡 Description
**Minitalk** is a project in the 42 curriculum. The purpose of this project is to code a small data exchange program using UNIX signals. 

We are tasked with creating a communication program in the form of a client and a server. The client must send a specified string to the server, which must then receive and display the string without delay. To make things interesting, communication between the client and server must exclusively use UNIX signals, specifically only `SIGUSR1` and `SIGUSR2`.

---

## 📑 Features

### 🔹 Mandatory Features
* **Sequential Processing:** The server can receive strings from several clients in a row without needing to restart.
* **Speed:** The program parses and prints bits rapidly, ensuring large strings (100+ characters) are displayed in under a second.
* **Robust Error Handling:** The program handles errors thoroughly and will not quit unexpectedly (no segmentation faults, bus errors, or double frees).

### 🚀 Bonus Features
* **Ping-Pong Acknowledgment:** The server acknowledges every single received message by sending a signal back to the client.
* **Unicode Support:** Natively supports and prints Unicode characters.

---

## 🧠 Algorithm & Data Structure

### The Core Logic: Bitwise Operations
Since we are only allowed to use two signals (`SIGUSR1` and `SIGUSR2`), we cannot send standard `char` data types directly. Instead, the client translates every character of the string into its 8-bit binary representation. 
* It sends `SIGUSR1` to represent a `1`.
* It sends `SIGUSR2` to represent a `0`.

The server catches these signals one by one, shifts the bits back into an 8-bit `unsigned char` (to avoid sign-extension issues), and prints the reconstructed byte to the standard output. Because it prints raw bytes, UTF-8/Unicode characters are implicitly supported in both versions!

### Signal Pacing vs. State Management
* **Mandatory Approach:** The client paces its signal transmission using `usleep()` between each bit to prevent overwhelming the server's signal queue and dropping bits.
* **Bonus Approach (Ping-Pong):** To achieve flawless transmission without relying on arbitrary sleep times, the bonus client utilizes a single global variable as a lock, pausing the client loop until an acknowledgment signal (`SIGUSR1`) is received from the server before dispatching the next bit.

### The Global Variable: `volatile sig_atomic_t`
The project subject permits the use of one global variable per program, provided its use is strictly justified. In the bonus client, `volatile sig_atomic_t g_ack;` is used. Here is the technical justification:
* **`sig_atomic_t`**: This integer type guarantees that reading and writing to the variable happens in a single, atomic instruction. This prevents a scenario where the main program is halfway through reading the variable when a signal interrupts it and changes the value, which would result in corrupted or "half-read" data.
* **`volatile`**: This keyword tells the compiler that the variable's value can change at any time asynchronously (outside the normal flow of the code, such as inside a signal handler). Without `volatile`, an optimizing compiler might look at the `while (g_ack == PAUSE)` loop, assume `g_ack` is never modified inside that loop, and aggressively compile it into an infinite loop. `volatile` forces the program to fetch the actual, most up-to-date value from memory on every single iteration.

---

## 🛠️ Instructions

### 📦 Installation
To compile the project, run the following commands in the root of the repository.

* To build the **mandatory** executables (`client` and `server`):
```bash
make
```

* To build the **bonus** executables with acknowledgment features (`client_bonus` and `server_bonus`):
```bash
make bonus
```

### 🧹 Cleaning
* `make clean`: Removes the compiled object files (`.o`).
* `make fclean`: Removes object files and all executables.
* `make re`: Performs a clean re-build.

### 💻 Usage
**1. Start the Server:**
The server must be started first. Upon launch, it will print its PID.
```bash
./server
# OR for the bonus version:
./server_bonus
```
*Output:*
```text
The process ID is: 12345.
```

**2. Run the Client:**
In a separate terminal window, run the client by passing the server's PID and the string you wish to send.
```bash
./client 12345 "Hello, 42!"
# OR for the bonus version:
./client_bonus 12345 "Hello, 42!"
```
The server terminal will immediately print `Hello, 42!`. If using the bonus version, the client terminal will also print a success message upon full acknowledgment.

---

## ⚠️ The Mandatory `usleep` Dilemma (A Note for Students)
If you are testing the **mandatory** version of this project on a slow Virtual Machine (VM) or Windows Subsystem for Linux (WSL), you might see random garbage characters printed to the terminal. 

The 42 subject requires that displaying 100 characters must take less than 1 second. However, Linux **does not queue pending signals of the same type**. In the mandatory version, the client relies on a hardcoded `usleep()` delay to pace the signals. 
* If the sleep time is **too low**, the VM's OS scheduler fails to context-switch fast enough, the server drops the incoming signal, and the bits become permanently misaligned (resulting in garbage data).
* If the sleep time is **too high**, the program avoids garbage data but fails the strict speed requirement.

**The Solution:** The `usleep` value in the mandatory `client.c` is optimized for bare-metal campus machines (like the 42 iMacs). If you test this on a slow personal machine and get garbage output, simply increase the `usleep` delay in `client.c` to account for your hypervisor's latency. Alternatively, test the **Bonus version**—its ping-pong acknowledgment system dynamically adapts to the CPU's exact speed, completely eliminating this hardware bottleneck!

---

## ⚠️ The Buffer vs. Stream Debate (Server Output)
A common debate among 42 students is whether the server should print characters one-by-one as they arrive, or buffer the entire string and print it all at once at the very end. **This implementation intentionally streams the output character-by-character.** Here is why:

* **"Without Delay":** The subject requires the server to display the string "without delay". Waiting a full minute to receive a massive string before printing anything looks like a frozen program or a crash. Streaming characters provides real-time, live feedback of the communication.
* **The `malloc` Trap:** The server has no prior knowledge of the incoming message's length. To buffer the string, it would require continuous, dynamic `malloc` and `free` operations inside a signal handler as the buffer expands. This introduces severe performance overhead and massive memory leak risks. By streaming directly via `write()`, this implementation maintains a memory footprint of effectively zero bytes and avoids the dynamic memory trap entirely.

---

### 🧪 Testing & Edge Cases
Minitalk can be deceiving—it might perfectly transmit a small string like "Hello" but completely break, crash, or print random garbage data when handling massive strings or complex characters due to signal dropping or race conditions. **Rigorous testing is essential.**

**1. Unicode & Emoji Stress Test**
Because this implementation passes raw bytes, it natively supports multi-byte Unicode characters. Try sending complex emojis to ensure bits aren't being misaligned:
```bash
./client_bonus 12345 "🔥 👾 42 Porto is awesome! 👾 🔥"
```

**2. Third-Party Testers**
To ensure your Minitalk output is correct *every single time* without dropping signals in the middle of a massive transmission, highly recommend running your code through these community testers:
* [ThibaudM13 / minitalk-Tester](https://github.com/ThibaudM13/minitalk-Tester) - Excellent for automated stress testing.
* [MalwarePup / minitalk_tester](https://github.com/MalwarePup/minitalk_tester) - Great for catching edge cases and checking transmission speeds.

### 🚨 The Norm
Moulinette relies on a program called `norminette` to check if your files comply with the 42 Norm. Every single `.c` and `.h` file must pass this check. If there is a norm error, you will receive a 0.

**The 42 Header:**
Before writing any code, every file must start with the standard 42 header. `norminette` will automatically fail any file missing this specific signature.
```c
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:19:21 by maaugust          #+#    #+#             */
/*   Updated: 2025/07/09 17:18:26 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
```

Run the following command in the root of your repository to check all your files at once:
```bash
norminette -R CheckForbiddenSourceHeader srcs/ bonus/ includes/
```

---

## 📚 Resources & References
* `man 2 kill`
* `man 2 signal`
* `man 2 sigaction`
* `man 2 pause`
* [CodeVault - Unix Signals](https://www.youtube.com/playlist?list=PLfqABt5AS4FkW5mOn2Tn9ZZLLDwA3kZUY) - Exceptional breakdown of signal handling in C.
* [Oceano - Minitalk Explanation](https://www.youtube.com/watch?v=XqhY9WHahJQ) - Great conceptual overview tailored specifically for this 42 project.
* [UNIX Signals Tutorial](https://www.tutorialspoint.com/unix/unix-signals-traps.htm) - A great written introduction to understanding how signals and traps operate under the hood.
* [42 Norm V4](https://cdn.intra.42.fr/pdf/pdf/96987/en.norm.pdf) - The strict coding standard for 42 C projects.
* [Official 42 Norminette Repository](https://github.com/42School/norminette) - The open-source linter enforcing the strict 42 coding standard.

### 🤖 AI Usage Guidelines
*Per the subject requirements:*
* **Tasks:** AI tools were used to generate comprehensive Doxygen comments for the source files, restructure and fix minor variable duplication in the Makefile, and format this `README.md` file to match previous portfolio layouts.
* **Code:** The core bitwise parsing algorithm, signal management via `sigaction`, and the ping-pong acknowledgment loop were manually researched and written to guarantee a true understanding of UNIX process communication.
