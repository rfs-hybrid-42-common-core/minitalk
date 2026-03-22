/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:19:28 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/22 04:53:12 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

/**
 * @fn static void putnbr(long nbr)
 * @brief Prints an integer to the standard output.
 * @details Recursively divides the number to print it character by character 
 * using the write() system call.
 * @param nbr The number to be printed.
 */
static void	putnbr(long nbr)
{
	char	c;

	if (nbr < 0)
	{
		nbr *= -1;
		write(STDOUT_FILENO, "-", 1);
	}
	if (nbr >= 10)
		putnbr(nbr / 10);
	c = nbr % 10 + '0';
	write(STDOUT_FILENO, &c, sizeof(char));
}

/**
 * @fn static void send_signal(pid_t pid, int sig)
 * @brief Sends a specified signal to a target process.
 * @details Wraps the kill() function and provides error handling if the 
 * signal fails to send, exiting the program to avoid hanging.
 * @param pid Process ID of the target to send the signal to.
 * @param sig The integer value of the signal to send.
 */
static void	send_signal(pid_t pid, int sig)
{
	if (kill(pid, sig) == -1)
	{
		write(STDERR_FILENO, "Failed to send signal!\n", 23);
		exit(EXIT_FAILURE);
	}
}

/**
 * @fn static void handle_char(t_byte *c, size_t *n_bits, pid_t *pid)
 * @brief Evaluates a fully reconstructed character and prints it.
 * @details If the character is the null terminator, it prints a newline, 
 * signals the client that the transmission is done, and resets the PID. 
 * Otherwise, it prints the character and sends an acknowledgment.
 * @param c      Pointer to the reconstructed byte.
 * @param n_bits Pointer to the bit counter, reset to CHAR_BITS after 
 * processing.
 * @param pid    Pointer to the client's process ID.
 */
static void	handle_char(t_byte *c, size_t *n_bits, pid_t *pid)
{
	pid_t	tmp;

	tmp = *pid;
	if (!*c)
	{
		write(STDOUT_FILENO, "\n", 1);
		send_signal(tmp, SIGUSR2);
		*pid = 0;
	}
	else
		write(STDOUT_FILENO, c, sizeof(t_byte));
	*c = 0;
	*n_bits = CHAR_BITS;
	if (tmp)
		send_signal(tmp, SIGUSR1);
}

/**
 * @fn static void handle_signal(int sig, siginfo_t *info, void *context)
 * @brief Reconstructs a character bit by bit from incoming signals.
 * @details Distinguishes between clients using si_pid. Rebuilds the byte by 
 * shifting bits based on whether SIGUSR1 (1) or SIGUSR2 (0) is received. 
 * Once 8 bits are collected, calls handle_char to process it.
 * @param sig     The received signal (SIGUSR1 or SIGUSR2).
 * @param info    Struct containing additional signal information, like the 
 * sender's PID.
 * @param context Pointer to the execution context (unused).
 */
static void	handle_signal(int sig, siginfo_t *info, void *context)
{
	static t_byte	c = 0;
	static size_t	n_bits = CHAR_BITS;
	static pid_t	cli_pid = 0;

	(void) context;
	if (!cli_pid || info->si_pid != cli_pid)
	{
		cli_pid = info->si_pid;
		c = 0;
		n_bits = CHAR_BITS;
	}
	c |= (sig == SIGUSR1) << --n_bits;
	if (n_bits == 0)
	{
		handle_char(&c, &n_bits, &cli_pid);
		return ;
	}
	send_signal(cli_pid, SIGUSR1);
}

/**
 * @fn int main(void)
 * @brief Main entry point for the server program.
 * @details Fetches and prints the server's PID, configures the signal handling 
 * struct with SA_SIGINFO to track the client PID, and enters an infinite 
 * loop using pause() to wait for signals.
 * @return EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int	main(void)
{
	struct sigaction	sa;
	pid_t				pid;

	pid = getpid();
	write(STDOUT_FILENO, "The process ID is: ", 19);
	putnbr(pid);
	write(STDOUT_FILENO, ".\n", 2);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sa.sa_sigaction = &handle_signal;
	if (sigaction(SIGUSR1, &sa, NULL) == -1
		|| sigaction(SIGUSR2, &sa, NULL) == -1)
	{
		write(STDERR_FILENO, "sigaction failed to setup handlers!\n", 36);
		return (EXIT_FAILURE);
	}
	while (1)
		pause();
	return (EXIT_SUCCESS);
}
