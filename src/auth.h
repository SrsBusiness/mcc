#pragma once

void ssl_init();

int minecraft_authenticate(const char *username, const char *passwd, char **access_token, char **client_token, char **profile, char **name);
char *server_hash(char *server_id, int32_t ss_length, unsigned char *ss, int32_t pubkey_length, unsigned char *pubkey);
int session_join(char *access_token, char *client_token, char *profile, char *name, char *hash);
