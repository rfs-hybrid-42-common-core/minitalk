/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 15:19:21 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/22 16:16:41 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk_bonus.h"

volatile sig_atomic_t	g_ack;

/**
 * @fn static void validate_inputs(pid_t *pid, char **argv)
 * @brief Validates the command-line arguments passed to the client.
 * @details Parses the PID from a string to an integer, accounting for 
 * whitespace and signs. It verifies the PID is valid using kill() and ensures 
 * the message is not empty. Exits the program on failure.
 * @param pid  Pointer to store the extracted server PID.
 * @param argv Command line arguments array.
 */
static void	validate_inputs(pid_t *pid, char **argv)
{
	int		sign;

	*pid = 0;
	sign = 1;
	while (*argv[1] == ' ' || (*argv[1] >= '\t' && *argv[1] <= '\r'))
		argv[1]++;
	if (*argv[1] == '+' || *argv[1] == '-')
		if (*argv[1]++ == '-')
			sign *= -1;
	while (*argv[1] >= '0' && *argv[1] <= '9')
		*pid = *pid * 10 + (*argv[1]++ - '0');
	*pid *= sign;
	while (*argv[1] == ' ' || (*argv[1] >= '\t' && *argv[1] <= '\r'))
		argv[1]++;
	if (!*argv[1] && !kill(*pid, 0) && *argv[2])
		return ;
	if (*argv[1])
		write(STDERR_FILENO, "Server PID must be one integer value!\n", 38);
	else if (kill(*pid, 0))
		write(STDERR_FILENO, "Server PID does not exist!\n", 27);
	if (!*argv[2])
		write(STDERR_FILENO, "The message to send is empty!\n", 30);
	exit(EXIT_FAILURE);
}

/**
 * @fn static void handle_signal(int sig)
 * @brief Handles incoming signals sent back from the server.
 * @details Sets the global acknowledgment flag if SIGUSR1 is received. 
 * If SIGUSR2 is received, it indicates the full message was 
 * successfully processed, and the client exits.
 * @param sig The integer value of the received signal.
 */
static void	handle_signal(int sig)
{
	if (sig == SIGUSR1)
		g_ack = ACK;
	if (sig == SIGUSR2)
	{
		write(STDOUT_FILENO, "Message sent successfully!\n", 27);
		exit(EXIT_SUCCESS);
	}
}

/**
 * @fn static void send_signal(pid_t pid, char c, size_t i)
 * @brief Sends a single bit of a character to the server via UNIX signals.
 * @details Extracts the i-th bit of character 'c' and sends SIGUSR1 if the bit 
 * is 1, or SIGUSR2 if the bit is 0. Waits for the server's acknowledgment.
 * @param pid The process ID of the target server.
 * @param c   The character whose bit is being sent.
 * @param i   The index of the bit to send (0 to 7).
 */
static void	send_signal(pid_t pid, char c, size_t i)
{
	size_t	time_cnt;

	time_cnt = 0;
	if (c & (1 << i) && kill(pid, SIGUSR1) == -1)
	{
		write(STDERR_FILENO, "Failed to send signal!\n", 23);
		exit(EXIT_FAILURE);
	}
	else if (!(c & (1 << i)) && kill(pid, SIGUSR2) == -1)
	{
		write(STDERR_FILENO, "Failed to send signal!\n", 23);
		exit(EXIT_FAILURE);
	}
	while (g_ack == PAUSE && time_cnt < TIMEOUT_MS)
	{
		usleep(SLEEP_US);
		time_cnt++;
	}
	if (g_ack != ACK)
	{
		write(STDERR_FILENO, "Timeout waiting for ack\n", 24);
		exit(EXIT_FAILURE);
	}
	g_ack = PAUSE;
}

/**
 * @fn static void send_message(int pid, char *str)
 * @brief Iterates through a string and sends it bit by bit to the server.
 * @details Sends each character of the string, and finally sends a null 
 * terminator ('\0') to let the server know the message is complete.
 * @param pid The process ID of the target server.
 * @param str The string message to be sent.
 */
static void	send_message(int pid, char *str)
{
	size_t	i;

	while (*str)
	{
		i = CHAR_BITS;
		while (i-- > 0)
			send_signal(pid, *str, i);
		str++;
	}
	i = CHAR_BITS;
	while (i-- > 0)
		send_signal(pid, *str, i);
}

/**
 * @fn int main(int argc, char **argv)
 * @brief Main entry point for the client program.
 * @details Validates inputs, configures the signal handlers for SIGUSR1 
 * and SIGUSR2 using sigaction, and initiates the message transfer.
 * @param argc The number of command-line arguments.
 * @param argv An array of strings representing the command-line arguments.
 * @return     EXIT_SUCCESS on success, or EXIT_FAILURE on error.
 */
int	main(int argc, char **argv)
{
	struct sigaction	sa;
	pid_t				srv_pid;

	if (argc != 3)
	{
		write(STDERR_FILENO, "Invalid input parameters!\n", 26);
		write(STDERR_FILENO, "Usage: ./client <server_pid> <message>\n", 39);
		return (EXIT_FAILURE);
	}
	validate_inputs(&srv_pid, argv);
	g_ack = PAUSE;
	sa.sa_flags = SA_RESTART;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGUSR1);
	sigaddset(&sa.sa_mask, SIGUSR2);
	sa.sa_handler = &handle_signal;
	if (sigaction(SIGUSR1, &sa, NULL) == -1
		|| sigaction(SIGUSR2, &sa, NULL) == -1)
	{
		write(STDERR_FILENO, "sigaction failed to setup handlers!\n", 36);
		return (EXIT_FAILURE);
	}
	send_message(srv_pid, argv[2]);
	return (EXIT_SUCCESS);
}
