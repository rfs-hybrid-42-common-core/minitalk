/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/02 12:58:08 by maaugust          #+#    #+#             */
/*   Updated: 2026/03/25 16:24:22 by maaugust         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

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
 * @def CHAR_BITS
 * @brief Total number of bits in a standard character
 */
# define CHAR_BITS	8

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