# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/01 15:19:49 by maaugust          #+#    #+#              #
#    Updated: 2026/03/22 05:48:15 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
CLIENT         = client
SERVER         = server

# ============================== COMPILER FLAGS ============================== #
CC             = cc
CFLAGS         = -Wall -Wextra -Werror
INCLUDES       = -Iincludes
RM             = rm -rf

# ================================== COLORS ================================== #
GREEN          := \033[32m
RED            := \033[31m
YELLOW         := \033[33m
CYAN           := \033[36m
RESET          := \033[0m
BOLD           := \033[1m

# =============================== SOURCE FILES =============================== #
SRC_PATH       = ./srcs
SRC            = $(shell find $(SRC_PATH) -name '*.c')

OBJ_PATH       = ./objs
OBJ            = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

CLIENT_OBJ     = $(filter %client.o, $(OBJ))
SERVER_OBJ     = $(filter %server.o, $(OBJ))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(CLIENT) $(SERVER)

bonus: all

$(CLIENT): $(CLIENT_OBJ)
	@printf "$(GREEN)✔ Minitalk client objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(CLIENT)$(RESET)\n"

$(SERVER): $(SERVER_OBJ)
	@printf "$(GREEN)✔ Minitalk server objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(SERVER)$(RESET)\n"
	
# =============================== CLEAN TARGETS ============================== #
clean:
	@$(RM) $(OBJ_PATH)
	@rmdir -p --ignore-fail-on-non-empty objs 2>/dev/null || true
	@printf "$(YELLOW)• Cleaned object files.$(RESET)\n"

fclean: clean
	@$(RM) $(CLIENT) $(SERVER)
	@printf "$(RED)• Full clean complete.$(RESET)\n"

# ============================== REBUILD TARGETS ============================= #
re: fclean all

.PHONY: all bonus clean fclean re
