#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "marshal.h"
#include "api.h"
#include "protocol.h"
#include "nbt.h"
#include "hexdump.h"

char stone[] = {
    0x5c, 0x08, 0x00, 0x01, 0x0c, 0x00, 0xdc, 0x00,
    0x05, 0x78, 0x01, 0x00, 0x01, 0x40, 0x00, 0x00,
    0x0a, 0x00, 0x00, 0x03, 0x00, 0x0a, 0x52, 0x65,
    0x70, 0x61, 0x69, 0x72, 0x43, 0x6f, 0x73, 0x74,
    0x00, 0x00, 0x00, 0x03, 0x09, 0x00, 0x04, 0x65,
    0x6e, 0x63, 0x68, 0x0a, 0x00, 0x00, 0x00, 0x01,
    0x02, 0x00, 0x02, 0x69, 0x64, 0x00, 0x00, 0x02,
    0x00, 0x03, 0x6c, 0x76, 0x6c, 0x00, 0x01, 0x00,
    0x0a, 0x00, 0x07, 0x64, 0x69, 0x73, 0x70, 0x6c,
    0x61, 0x79, 0x08, 0x00, 0x04, 0x4e, 0x61, 0x6d,
    0x65, 0x00, 0x05, 0x6e, 0x65, 0x67, 0x75, 0x73,
    0x00, 0x00, 0x03, 0x10, 0x06
};

typedef struct _click_window {
    char *format;
    vint32_t packet_id;
    uint8_t window_id;
    int16_t slot;
    int8_t button;
    int16_t action_number;
    int8_t mode;
    slot_t clicked_item;
} click_window;

void test_case(void *raw, click_window *packet_data)
{
    decode_packet(NULL, raw, packet_data);
}

void print_slot(slot_t *slot)
{
    printf("Slot data:\n    block_id: %d\n    count: %d\n    damage: %d\n\n",
           (int)slot->block_id,
           (int)slot->count,
           (int)slot->damage);
    if(slot->tree) {
        printf("NBT data:\n");
        char *nbt_text = nbt_dump_ascii(slot->tree);
        printf("%s\n", nbt_text);
        free(nbt_text);
    }
}

void print_click_window(click_window *data)
{
    printf("Packet data:\n    window_id: %d\n    slot: %d\n    button: %d\n    action_number: %d\n    mode: %d\n\n",
           (int)data->window_id,
           (int)data->slot,
           (int)data->button,
           (int)data->action_number,
           (int)data->mode);
    print_slot(&data->clicked_item);
}

/*
 *typedef struct play_serverbound_player_block_place {
    char*         format;
    vint32_t      packet_id;

    position_t    location;
    int8_t        direction;
    slot_t        item;
    int8_t        x;
    int8_t        y;
    int8_t        z;
} play_serverbound_player_block_place_t;

 **/



void print_block_placement(play_serverbound_player_block_place_t *p)
{
    printf("Packet data:\n    packet_id: %d\n    location: %llu\n    direction: %d\n    x: %d\n    y: %d\n    z:%d\n\n",
           p->packet_id,
           p->location,
           p->direction,
           p->x,
           p->y,
           p->z);
    print_slot(&p->item);
}

int main()
{
    play_serverbound_player_block_place_t foo, bar;
    foo.format =  bar.format = "vlbkbbb";
    decode_packet(NULL, stone, &foo);
    print_block_placement(&foo);

    /* Re-marshal packet with format_packet */

    char remarshalled[sizeof(stone)];

    bot_t b;
    bot_internal i;
    b._data = &i;
    i.packet_threshold = sizeof(stone);

    int check = format_packet(&b, &foo, remarshalled);
    if(memcmp(remarshalled, stone, sizeof(stone)) != 0) {
        printf("%d %d\n", check, sizeof(stone));
        printf("Original and remarshalled packet do not match\n");
        hexdump("Original", stone, sizeof(stone));
        hexdump("Remarshalled", remarshalled, sizeof(stone));
    } else {
        printf("Original and Remarshalled match\n");
    }
    nbt_free(foo.item.tree);
}
