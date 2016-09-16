#include <assert.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <json.h>
#include <openssl/evp.h>

#include "auth.h"
#include "types.h"

static CURL *curl;
static int ssl_init_status = 0;

void ssl_init() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == NULL) {
        fprintf(stderr, "Curl Handle initialization error.\n");
        assert(0);
    }
    
    if (curl_easy_setopt(curl, CURLOPT_SSLENGINE, NULL) != CURLE_OK) {
        fprintf(stderr, "SSL engine initialization error.\n");
        assert(0);
    }
    if (curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L) != CURLE_OK) {
        fprintf(stderr, "Could not set default SSL engine.\n");
        assert(0);
    }
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);  
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1L);

}

static size_t data_cb(char *buffer, size_t size, size_t nitems, void *userdata) {
    char **data_ptr = userdata;
    *data_ptr = malloc(size * nitems);
    memcpy(*data_ptr, buffer, size * nitems);
    return size * nitems;
}

/* Returns length of access token*/
int minecraft_authenticate(const char *username, const char *passwd, char **access_token, char **client_token, char **profile, char **name) {
    if (ssl_init_status == 0) {
        ssl_init();
        ssl_init_status = 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://authserver.mojang.com/authenticate");

    char *data = NULL;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_cb);

    struct curl_slist *header_list = NULL;
    header_list = curl_slist_append(header_list, "Content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    /*
     *{
     *    "agent": {                              // defaults to Minecraft
     *        "name": "Minecraft",                // For Mojang's other game Scrolls, "Scrolls" should be used
     *        "version": 1                        // This number might be increased
     *                                            // by the vanilla client in the future
     *    },
     *    "username": "mojang account name",      // Can be an email address or player name for
     *                                            // unmigrated accounts
     *    "password": "mojang account password",
     *    "clientToken": "client identifier"      // optional
     *}
     */
    
    json_object *auth_data = json_object_new_object();

    json_object *agent = json_object_new_object();
    json_object *agent_name = json_object_new_string("Minecraft");
    json_object *agent_version = json_object_new_int(1);
    json_object_object_add(agent, "name", agent_name);
    json_object_object_add(agent, "version", agent_version);

    json_object *username_obj = json_object_new_string(username);
    json_object *password = json_object_new_string(passwd);
    
    json_object_object_add(auth_data, "agent", agent);
    json_object_object_add(auth_data, "username", username_obj);
    json_object_object_add(auth_data, "password", password);
    
	const char *post_data = json_object_to_json_string(auth_data);

    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Epic fail.\n");
        assert(0);
    }
    
    printf("%s\n", data);
    
    json_object *response = json_tokener_parse(data);
    if (response == NULL) {
        fprintf(stderr, "Authentication failure: JSON parse error in authentication response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }

    json_object *error;
    if (json_object_object_get_ex(response, "error", &error)) {
        fprintf(stderr, "Authentication failure\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }

    /* Extract access token */
    json_object *access_token_obj;
    if (!json_object_object_get_ex(response, "accessToken", &access_token_obj)) {
        fprintf(stderr, "Authentication failure: No access token in response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }

    const char *access_token_const = json_object_get_string(access_token_obj);
    size_t len = strlen(access_token_const) + 1;
    *access_token = malloc(len);
    strncpy(*access_token, access_token_const, len);


    /* Extract client token */
    json_object *client_token_obj;
    if (!json_object_object_get_ex(response, "clientToken", &client_token_obj)) {
        fprintf(stderr, "Authentication failure: No client token in response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }
    const char *client_token_const = json_object_get_string(client_token_obj);
    len = strlen(client_token_const) + 1;
    *client_token = malloc(len);
    strncpy(*client_token, client_token_const, len);



    /* Extract Profile hash and name */
    json_object *selected_profile_obj;
    if (!json_object_object_get_ex(response, "selectedProfile", &selected_profile_obj)) {
        fprintf(stderr, "Authentication failure: No selected profile in response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }

    json_object *profile_id_obj;
    if (!json_object_object_get_ex(selected_profile_obj, "id", &profile_id_obj)) {
        fprintf(stderr, "Authentication failure: No selected profile id in response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }

    json_object *name_obj;
    if (!json_object_object_get_ex(selected_profile_obj, "name", &name_obj)) {
        fprintf(stderr, "Authentication failure: No selected profile name in response\n");
        fprintf(stderr, "%s\n", data);
        assert(0);
    }
    
    const char *profile_const = json_object_get_string(profile_id_obj);
    len = strlen(profile_const) + 1;
    *profile = malloc(len);
    strncpy(*profile, profile_const, len);

    const char *profile_name_const = json_object_get_string(name_obj);
    len = strlen(profile_const) + 1;
    *name = malloc(len);
    strncpy(*name, profile_name_const, len);

    json_object_put(response);
    free(data);
    //curl_easy_cleanup(curl);
    //curl_global_cleanup();
    return 0;
}

void twos_complement(unsigned char *hash, int length) {
    unsigned char carry = 1;
    for (int i = length - 1; i >= 0; i--) {
        hash[i] = ~hash[i];
        hash[i] += carry;
        carry = !hash[i];
    };
}

char *server_hash(char *server_id, int32_t ss_length, unsigned char *ss, int32_t pubkey_length, unsigned char *pubkey) {
    
    printf("Server ID: %s\n", server_id);

    unsigned char hash[EVP_MAX_MD_SIZE];
    uint32_t hash_length;
    EVP_MD_CTX ctx;
    EVP_MD_CTX_init(&ctx);
    const EVP_MD *mode = EVP_sha1();
    EVP_DigestInit_ex(&ctx, mode, NULL);
    printf("Server id: %p, length: %lu\n", server_id, strlen(server_id));
    EVP_DigestUpdate(&ctx, server_id, strlen(server_id));
    EVP_DigestUpdate(&ctx, ss, ss_length);
    EVP_DigestUpdate(&ctx, pubkey, pubkey_length);
    EVP_DigestFinal_ex(&ctx, hash, &hash_length);
   
    /* We need to send the string representation of the hash value in hex. */

    /* Twos Complement (this is so dumb, I hate Mojang) */
    char *hash_str;
    int base;
    if ((hash[0] & 0x80) && 0) {
        hash_str = malloc(hash_length * 2 + 2);
        hash_str[0] = '-';
        base = 1;
        twos_complement(hash, hash_length);
    } else {
        hash_str = malloc(hash_length * 2 + 2);
        base = 0;
    }

    static const char hex[] = "0123456789abcdef";
    /* skip leading 0 bytes */
    unsigned char *hash_head = hash;
    for (;hash_head - hash < hash_length && *hash_head == 0; hash_head++);
    for (int i = 0; i < hash_length; i++) {
        hash_str[2 * i + base]     = hex[(hash_head[i] >> 4) & 0x0f]; 
        hash_str[2 * i + 1 + base] = hex[hash_head[i] & 0x0f];
    }
    hash_str[hash_length * 2 + base] = '\0';

    return hash_str;
}

static const char session_join_url[] = "https://sessionserver.mojang.com/session/minecraft/join";
int session_join(char *access_token, char *client_token, char *profile, char *name, char *hash) {
    if (ssl_init_status == 0) {
        ssl_init();
        ssl_init_status = 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, session_join_url);

    char *data = NULL;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_cb);

    struct curl_slist *header_list = NULL;
    header_list = curl_slist_append(header_list, "Content-type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);


    json_object *payload = json_object_new_object();
    json_object *access_token_object = json_object_new_string(access_token);
    json_object *client_token_object = json_object_new_string(client_token);
    //json_object *access_token_object = json_object_new_string("");
    
    json_object *profile_object = json_object_new_object();
    json_object *profile_id_object = json_object_new_string(profile);
    json_object *profile_name_object = json_object_new_string(name);
    json_object_object_add(profile_object, "id", profile_id_object);
    json_object_object_add(profile_object, "name", profile_name_object);

    json_object *hash_object = json_object_new_string(hash);
    
    json_object_object_add(payload, "accessToken", access_token_object);
    json_object_object_add(payload, "clientToken", client_token_object);
    json_object_object_add(payload, "selectedProfile", profile_object);
    json_object_object_add(payload, "serverId", hash_object);

    const char *post_data = json_object_to_json_string(payload);
    printf("%s\n", post_data);
    
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));

    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "Epic fail.\n");
        assert(0);
    }
     
    printf("Data: %s\n", data);  
    return 0; 
}
