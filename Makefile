# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: maaugust <maaugust@student.42porto.com>    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/01 15:19:49 by maaugust          #+#    #+#              #
#    Updated: 2026/03/22 16:23:12 by maaugust         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ============================ PROJECT FILE NAMES ============================ #
CLIENT         = client
B_CLIENT       = client_bonus
SERVER         = server
B_SERVER       = server_bonus

# ============================== COMPILER FLAGS ============================== #
CC             = cc
CFLAGS         = -Wall -Wextra -Werror
INCLUDES       = -Iincludes
B_INCLUDES     = -Ibonus/includes
RM             = rm -rf

# ================================== COLORS ================================== #
GREEN          := \033[32m
RED            := \033[31m
YELLOW         := \033[33m
CYAN           := \033[36m
RESET          := \033[0m
BOLD           := \033[1m

# =============================== SOURCE FILES =============================== #
# Mandatory files
SRC_PATH       = ./srcs
SRC            = $(shell find $(SRC_PATH) -name '*.c')

# Bonus files
B_SRC_PATH     = ./bonus/srcs
B_SRC          = $(shell find $(B_SRC_PATH) -name '*.c')

# Object files
OBJ_PATH       = ./objs
OBJ            = $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRC))

B_OBJ_PATH     = ./bonus/objs
B_OBJ          = $(patsubst $(B_SRC_PATH)/%.c, $(B_OBJ_PATH)/%.o, $(B_SRC))

CLIENT_OBJ     = $(filter %client.o, $(OBJ))
SERVER_OBJ     = $(filter %server.o, $(OBJ))

B_CLIENT_OBJ   = $(filter %client_bonus.o, $(B_OBJ))
B_SERVER_OBJ   = $(filter %server_bonus.o, $(B_OBJ))

# ============================ COMPILATION RULES ============================= #
$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(B_OBJ_PATH)/%.o: $(B_SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@printf "$(CYAN)Compiling:$(RESET) $(YELLOW)$<$(RESET)\n"
	@$(CC) $(CFLAGS) $(B_INCLUDES) -c $< -o $@

# =============================== BUILD TARGETS ============================== #
all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ)
	@printf "$(GREEN)✔ Minitalk client objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(CLIENT_OBJ) -o $(CLIENT)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(CLIENT)$(RESET)\n"

$(SERVER): $(SERVER_OBJ)
	@printf "$(GREEN)✔ Minitalk server objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(SERVER_OBJ) -o $(SERVER)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(SERVER)$(RESET)\n"

bonus: $(B_CLIENT) $(B_SERVER)

$(B_CLIENT): $(B_CLIENT_OBJ)
	@printf "$(GREEN)✔ Minitalk bonus client objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(B_CLIENT_OBJ) -o $(B_CLIENT)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(B_CLIENT)$(RESET)\n"

$(B_SERVER): $(B_SERVER_OBJ)
	@printf "$(GREEN)✔ Minitalk bonus server objects built successfully.$(RESET)\n"
	@$(CC) $(CFLAGS) $(B_SERVER_OBJ) -o $(B_SERVER)
	@printf "$(GREEN)$(BOLD)✔ Build complete → $(B_SERVER)$(RESET)\n"
	
# =============================== CLEAN TARGETS ============================== #
clean:
	@$(RM) $(OBJ_PATH) $(B_OBJ_PATH)
	@rmdir -p --ignore-fail-on-non-empty $(OBJ_PATH) $(B_OBJ_PATH) 2>/dev/null || true
	@printf "$(YELLOW)• Cleaned object files.$(RESET)\n"

fclean: clean
	@$(RM) $(CLIENT) $(SERVER) $(B_CLIENT) $(B_SERVER)
	@printf "$(RED)• Full clean complete.$(RESET)\n"

# ============================== REBUILD TARGETS ============================= #
re: fclean all

.PHONY: all bonus clean fclean re
