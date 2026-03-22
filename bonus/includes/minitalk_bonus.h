/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk_bonus.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 12:58:08 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/22 16:07:22 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_BONUS_H
# define MINITALK_BONUS_H

/* ========================================================================== */
/* INCLUDES                                                                   */
/* ========================================================================== */

/* --------------------------- Internal Libraries --------------------------- */
# include <unistd.h>
# include <signal.h>
# include <stdlib.h>
# include <sys/types.h>

/* ========================================================================== */
/* MACROS                                                                     */
/* ========================================================================== */

/**
 * @def CHAR_BITS Total number of bits in a standard character
 */
# define CHAR_BITS	8

/**
 * @def TIMEOUT_MS Maximum time in milliseconds the client waits for an ACK
 */
# define TIMEOUT_MS 3000

/**
 * @def SLEEP_US Microseconds the client sleeps between ACK checks
 */
# define SLEEP_US   1000

/**
 * @def PAUSE Client signal status indicating it is waiting for an ACK
 */
# define PAUSE	0

/**
 * @def ACK Client signal status indicating an acknowledgment was received
 */
# define ACK	1

/* ========================================================================== */
/* TYPEDEFS                                                                   */
/* ========================================================================== */

/**
 * @typedef t_byte
 * @brief Represents an unsigned 8-bit byte.
 * @details Used to prevent sign-extension issues when doing bitwise operations 
 * on characters.
 */
typedef unsigned char	t_byte;

#endif