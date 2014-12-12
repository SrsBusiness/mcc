#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "bot.h"
#include "protocol.h"
#include "json.h"
#include "api.h"

typedef struct bot_globals {
    int status;
} bot_globals_t;

bool next_int_token(int* value, char *string, char **saveptr)
{
    char *token = strtok_r(string, " ", saveptr);
    if (token) {
        // Does not validate range or integer-ness.
        *value = strtol(token, NULL, 0);
        return true;
    }
    return false;
}

void exec(bot_t *bot, char *command, char *strargs)
{
    if (strcmp(command, "dig") == 0) {
        char **saveptr = calloc(1, sizeof(char *));
        bool valid_input = true;
        char *token = NULL;
        int x, y, z;

        valid_input &= next_int_token(&x, strargs, saveptr);
        valid_input &= next_int_token(&y, *saveptr, saveptr);
        valid_input &= next_int_token(&z, *saveptr, saveptr);

        // Ensure there are no trailing tokens.
        token = strtok_r(*saveptr, " ", saveptr);
        if (token) {
            valid_input = false;
        }

        free(saveptr);
        if (valid_input) {
            printf("DIG: (%d, %d, %d)\n", x, y, z);
            //dig(bot, x, y, z);
        } else {
            send_play_serverbound_chat(bot,
                                       "Invalid arguments for DIG command "
                                       "(x, y, z).");
        }
    } else if (strcmp(command, "slot") == 0) {
        char **saveptr = calloc(1, sizeof(char *));
        bool valid_input = true;
        char *token = NULL;
        int slot_no;

        valid_input &= next_int_token(&slot_no, strargs, saveptr);

        // Ensure there are no trailing tokens.
        token = strtok_r(*saveptr, " ", saveptr);
        if (token) {
            valid_input = false;
        }

        free(saveptr);
        if (valid_input) {
            printf("SLOT CHANGE: (%d)\n", slot_no);
            send_play_serverbound_item_change(bot, slot_no);
        } else {
            send_play_serverbound_chat(bot,
                                       "Invalid arguments for SLOT command "
                                       "(0-indexed slot).");
        }
    } else if (strcmp(command, "where") == 0) {
        // Echo current bot location.
        char msg[256] = {0};
        sprintf(msg, "I'm at (%d, %d, %d)\n",
                (int)bot->x, (int)bot->y, (int)bot->z);
        printf(msg);
        send_play_serverbound_chat(bot, msg);
    } else if (strcmp(command, "up") == 0) {
        char msg[256] = {0};
        
        sprintf(msg, "I think I started at (%d, %d, %d) = (%f, %f, %f).\n",
                (int)floor(bot->x - 0.5), (int)bot->y, (int)floor(bot->z - 0.5),
                bot->x, bot->y, bot->z);
        //send_play_serverbound_chat(bot, msg);
        printf(msg);

        pthread_mutex_lock(&bot->bot_mutex);
        int critical_value = (int)floor(bot->y);
        bot->y += 1;
        pthread_mutex_unlock(&bot->bot_mutex);
        send_play_serverbound_player_move(bot, bot->x, bot->y, bot->z, 1);

        sprintf(msg, "I think I'm at (%d, %d, %d) = (%f, %f, %f).\n",
                (int)floor(bot->x - 0.5), (int)bot->y, (int)floor(bot->z - 0.5),
                bot->x, bot->y, bot->z);
        //send_play_serverbound_chat(bot, msg);
        printf(msg);

        place_held(bot,
                   (int)floor(bot->x - 0.5),
                   critical_value,
                   (int)floor(bot->z - 0.5));
        sprintf(msg, "Tried placing block at (%d, %d, %d).\n",
                (int)floor(bot->x - 0.5), critical_value, (int)floor(bot->z - 0.5),
                (int)bot->x, critical_value, (int)bot->z);
        //send_play_serverbound_chat(bot, msg);

        printf("COMMAND <UP>: %s", msg);
    } else if (strcmp(command, "build") == 0) {
        char **saveptr = calloc(1, sizeof(char *));
        bool valid_input = true;
        char *token = NULL;
        int x, y, z;

        valid_input &= next_int_token(&x, strargs, saveptr);
        valid_input &= next_int_token(&y, *saveptr, saveptr);
        valid_input &= next_int_token(&z, *saveptr, saveptr);

        // Ensure there are no trailing tokens.
        token = strtok_r(*saveptr, " ", saveptr);
        if (token) {
            valid_input = false;
        }

        free(saveptr);
        if (valid_input) {
            printf("BUILD: (%d, %d, %d)\n", x, y, z);
            place_held(bot, x, y, z);
        } else {
            send_play_serverbound_chat(bot,
                                       "Invalid arguments for BUILD command "
                                       "(x, y, z).");
        }
    } else {
        printf("Command not implemented: %s\n", command);
        send_play_serverbound_chat(bot,
                                   "Command not implemented.");
    }
}

void decode_chat_json(char *raw_json, char **msg, char **sender_name)
{
    json_value *json_data = json_parse((json_char *)raw_json, strlen(raw_json));

    for (int i = 0; i < json_data->u.object.length; i++) {
        char * name = json_data->u.object.values[i].name;
        if (strcmp(name, "with") == 0) {
            json_value * arr = json_data->u.object.values[i].value;
            // Iterate over array data['with'].
            for (int j = 0; j < arr->u.array.length; j++) {
                json_value * curr = arr->u.array.values[j];
                switch (curr->type) {
                // The sender is in the object data.
                case json_object:
                    ;
                    for (int k = 0; k < curr->u.object.length; k++) {
                        char * name = curr->u.object.values[k].name;
                        if (strcmp(name, "text") == 0) {
                            // sender_name = data['with'][0]['text']
                            json_value * sender = curr->u.object.values[k].value;
                            *sender_name = malloc(sender->u.string.length + 1);
                            memcpy(*sender_name, sender->u.string.ptr,
                                   sender->u.string.length + 1);
                            break;
                        }
                    }
                    break;
                // The actual message is the string.
                case json_string:
                    ;
                    // msg = data['with'][1]
                    *msg = malloc(curr->u.string.length + 1);
                    memcpy(*msg, curr->u.string.ptr, curr->u.string.length + 1);
                    break;
                default:
                    printf("Invalid JSON message error.");
                    break;
                }

            }
            // Short circuit on key == 'with'.
            break;
        }
    }
    json_value_free(json_data);
}

void chat_handler(bot_t *bot, void *vp)
{
    play_clientbound_chat_t *p =
        (play_clientbound_chat_t *)vp;
    char *msg = NULL;
    char *sender = NULL;
    decode_chat_json(p->json, &msg, &sender);
    // Ensure that we do not echo ourselves,
    // and that we received a chat message (not a server message).
    if (msg && sender && strcmp(sender, bot->name)) {
        // Commands to bots start with a backslash.
        // Only operate on non-empty commands.
        if (msg[0] == '\\' && msg[1]) {
            // Parse space delimited tokens.
            char **saveptr = calloc(1, sizeof(char *));
            char *command = strtok_r(msg+1, " ", saveptr);
            exec(bot, command, *saveptr);
            free(saveptr);
        } else {
            printf("CHAT: <%s> %s\n", sender, msg);
        }
    }
    free(sender);
    free(msg);
}

void builder_main(void *vbot)
{
    bot_t *bot = (bot_t *)vbot;
    msleep(1000);
    send_play_serverbound_player_move(bot, bot->x, bot->y, bot->z, 1);
    //2 = upwards
    // Want to send:
    // 13 (len)
    // 08 (packet id)
    // 00 00 01 00 f4 00 00 02 (position)
    // 02 (dir)
    // 00 04 40 00 00 00 (slot, '00 04 + <no. of blocks> + 00 00 00')
    // 00 00 00 (mouse x y z)

    // Timed function calls
    /*
    int x = (int)floor(bot->x - 0.5);
    int y = (int)bot->y;
    int z = (int)floor(bot->z - 0.5);
    printf("STARTING FROM: (%d, %d, %d)\n", x, y, z);
    place_held(bot, x, y-1, z);
    msleep(300);
    place_held(bot, x, y+2, z);
    msleep(300);
    place_held(bot, x, y+3, z);
    msleep(300);
    place_held(bot, x+1, y, z);
    msleep(300);
    place_held(bot, x-1, y, z);
    msleep(300);
    place_held(bot, x, y, z+1);
    msleep(300);
    place_held(bot, x, y, z-1);
    msleep(300);
    TESTING ONLY
    */
        
        
    while(1) {
        msleep(500);
        send_play_serverbound_player_status(bot, 0);
        send_play_serverbound_player_move(bot, bot->x, bot->y, bot->z, 1);
    }
}

bot_t *init_builder(char *name, char *server_name, int port)
{
    bot_t *bot = init_bot(name, *builder_main);
    bot->state = calloc(1, sizeof(bot_globals_t));

    register_defaults(bot);
    register_event(bot, PLAY, 0x02, chat_handler);

    login(bot, server_name, port);

    return bot;
}
