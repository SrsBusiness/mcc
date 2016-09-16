#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "types.h"

/* This function should probably be split up into 2 functions, one for auth and one without
 * If the name does not match the name discovered during authentication, it will be overwritten.
 * This gotcha may catch some off guard.
 **/
void init_bot(struct bot_agent *bot, char *name, int auth, char *username, char *passwd);

void join_server_ipaddr(struct bot_agent *bot, char* server_host, int server_port);
void join_server_hostname(struct bot_agent *bot, char *server_hostname, char *service);

void disconnect(struct bot_agent *bot);
