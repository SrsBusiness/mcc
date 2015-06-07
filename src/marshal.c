#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include <errno.h>
#include "marshal.h"
#include "bot.h"
#include "protocol.h"
#include "nbt.h"

static void *marshal_slot(void *_packet_raw, slot_t *slot_data);

// returns the number of bytes read from data
int varint64(char *data, int64_t *value)
{
    int64_t result = 0;
    int shifts = 0;
    do {
        result |= ((0x7F & *data) << (shifts * 7));
        shifts++;
    } while(expect_more(*data++));
    *value = result;
    return shifts;
}

// returns the number of bytes read from data
int varint32(char *data, int32_t *value)
{
    int32_t result = 0;
    int shifts = 0;
    do {
        result |= ((0x7F & *data) << (shifts * 7));
        shifts++;
    } while(expect_more(*data++));
    *value = result;
    return shifts;
}

int varint64_encode(int64_t value, char *data, int len)
{
    memset(data, 0, len);
    char mask = 0x7F;
    int i = 0;
    do {
        if(i >= len)
            return -1;
        data[i] = (mask & value);
        data[i] |= 0X80;
        value = (uint64_t)value >> 7;
        i++;
    } while(value);
    data[i - 1] &= mask;
    return i;
}

int varint32_encode(int32_t value, char *data, int len)
{
    memset(data, 0, len);
    char mask = 0x7F;
    int i = 0;
    do {
        if(i >= len)
            return -1;
        data[i] = (mask & value);
        data[i] |= 0X80;
        value = (uint32_t)value >> 7;
        i++;
    } while(value);
    data[i - 1] &= mask;
    return i;
}


// a generalized form of the hton functions
// in the sockets library convert numbers to
// their Big-endian format. This function extends to 64 bit
// words as well.
uint64_t reverse(uint64_t number, int len)
{
    if(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) { // we need to fliparoo!
        switch(len) {
        case sizeof(uint8_t):
            break;
        case sizeof(uint16_t):
            number = __builtin_bswap16(number);
            break;
        case sizeof(uint32_t):
            number = __builtin_bswap32(number);
            break;
        case sizeof(uint64_t):
            number = __builtin_bswap64(number);
            break;
        default:
            fprintf(stderr, "Bad word size\n");
            exit(-1);
        }
    }
    return number;
}

size_t format_sizeof(char c)
{
    switch(c) {
    case 'k':
        return sizeof(slot_t);
    case 's':
        return sizeof(char *);
    case 'v':
        return sizeof(vint32_t);
    case '*':
        return sizeof(void *);
    case 'b':
        return sizeof(int8_t);
    case 'h':
        return sizeof(int16_t);
    case 'w':
        return sizeof(int32_t);
    case 'l':
        return sizeof(int64_t);
    case 'q':
        return 2 * sizeof(int64_t);
    default:
        fprintf(stderr, "Bad format specifier\n");
        return -1;
    }
}

// data writes in the same direction as the buffer is growing pushes the data,
// then returns the address where the next datum is to be pushed
void *push(void *buffer, void *data, size_t size)
{
    memcpy(buffer, data, size);
    return (char *)buffer + size;
}

// Reads an integer of size bytes from the top of buf
// and returns it as a 64 bit integer
uint64_t value_at(void *buf, size_t size)
{
    uint64_t value = 0;

    if(__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        memcpy(&value, buf, size);
    else
        memcpy(&value + sizeof(value) - size, buf, size);

    return value;
}

// puts the raw packet data from the struct packet_data into packet_raw
// returns the number of bytes written to packet_raw, or -1 if packet_raw
// is not a long enough array
int format_packet(bot_t *bot, void *_packet_data, void *_packet_raw)
{
    char *packet_data = _packet_data;
    char *packet_raw = _packet_raw;

    uint32_t len = bot->_data->packet_threshold;
    uint32_t value = 0;
    uint32_t varlen = 0;
    uint64_t arr_len = 0;
    char varint[5];
    uint64_t size;

    char *fmt = *((char **)packet_data);
    packet_data += sizeof(char *);
    char *save = packet_raw;

    uint64_t network_order;

    while(*fmt) {
        size = format_sizeof(*fmt);
        packet_data = (char *)align(packet_data, size);
        switch(*fmt) {
        case 'k': { /* k is for slot. What, do you have a better idea? */
            slot_t *slot_data = (slot_t *)packet_data;
            packet_raw  = marshal_slot(packet_raw, slot_data);
            break;
        }
        case 'q':
            memcpy(packet_raw, packet_data, size);
            packet_raw += size;
            break;
        case 's': // string (null terminated)
            ;
            char *str = *((char **)packet_data);
            value = strlen(str);
            varlen = varint32_encode(value, varint, 5);
            if(packet_raw - save + varlen + value > len)
                return -1; // TODO: compression
            memmove(packet_raw, varint, varlen);
            packet_raw += varlen;
            memmove(packet_raw, str, value);
            packet_raw += value;
            break;
        case 'v': // varint32_t
            ;
            value = *((uint32_t *)packet_data);
            arr_len = value;
            varlen = varint32_encode(value, varint, 5);
            if(packet_raw - save + varlen > len)
                return -1; // TODO: compression
            memmove(packet_raw, varint, varlen);
            packet_raw += varlen;
            break;
        case '*': // pointer/array
            ;
            fmt++;
            size_t size_elem = format_sizeof(*fmt);
            if(packet_raw - save + size_elem * arr_len > len)
                return -1; // TODO: compression
            void *arr = *(void **)packet_data;
            for(int i = 0; i < arr_len * size_elem; i += size_elem) {
                network_order = value_at(arr + i, size_elem);
                network_order = reverse(network_order, size_elem);
                packet_raw = push(packet_raw, &network_order, size_elem);
            }
            break;
        case 'b':
        case 'h':
        case 'w':
        case 'l':
            ;
            if(packet_raw - save + size > len)
                return -1; // TODO: compression

            network_order = value_at(packet_data, size);
            arr_len = network_order;
            network_order = reverse(network_order, size);
            packet_raw = push(packet_raw, &network_order, size);

            break;
        }
        packet_data += size;
        fmt++;
    }

    varlen = varint32_encode(packet_raw - save, varint, 5);
    if(packet_raw - save + varlen > len) {
        return -1; // TODO: compression
    }
    memmove(save + varlen, save, packet_raw - save);
    memmove(save, varint, varlen);
    return packet_raw - save + varlen;
}

/* TODO: Add bounds checking on packet_raw */
void *marshal_slot(void *_packet_raw, slot_t *slot_data)
{
    int16_t block_id = slot_data->block_id;

    char *packet_raw = _packet_raw;
    if (-1 == block_id) { // Empty slot is 0xffff
        return push(packet_raw, block_id, sizeof(block_id));
    } else {
        int8_t count = slot_data->count;
        int16_t damage = slot_data->damage;
        nbt_node *tree = slot_data->tree;

        /* Convert little to big endian; flip the bytes. */
        block_id = htons(block_id);
        damage = htons(damage);

        /* copy block_id (2 bytes) */
        memcpy(packet_raw, &block_id, sizeof(block_id));
        packet_raw += sizeof(block_id);

        /* copy count (1 byte) */
        memcpy(packet_raw, &count, sizeof(count));
        packet_raw += sizeof(count);

        /* copy damage (2 bytes) */
        memcpy(packet_raw, &damage, sizeof(damage));
        packet_raw += sizeof(damage);

        if (tree) {
            /* convert nbt tree to binary nbt data */
            struct buffer nbt_data = nbt_dump_binary(tree);
            memcpy(packet_raw, nbt_data.data, nbt_data.len);
            packet_raw += nbt_data.len;
            free(nbt_data.data);
        } else {
            memset(packet_raw, 0, sizeof(int8_t));
            packet_raw += sizeof(int8_t);
        }
    }
    return packet_raw;
}

/* Returns the length of the slot data so decode_packet knows how much of
 * the raw packet data is for slot
 **/
uint64_t demarshal_slot(void *_data, slot_t *slot_data, uint64_t space_left)
{
    char *data = _data;

    /* Read 2 bytes (block_id) */
    memcpy(&slot_data->block_id, data, sizeof(slot_data->block_id));
    data += sizeof(slot_data->block_id);
    slot_data->block_id = ntohs(slot_data->block_id);
    if(slot_data->block_id == -1) { /* No slot data */
        return sizeof(int16_t); /* Must return length as 2 */
    }

    /* Read 1 byte (count) */
    slot_data->count = *data;
    data += sizeof(slot_data->count);

    /* Read 2 bytes (damage) */
    memcpy(&slot_data->damage, data, sizeof(slot_data->damage));
    data += sizeof(slot_data->damage);
    slot_data->damage = ntohs(slot_data->damage);

    uint64_t nbt_length;
    if(*data) { /* If next byte is non-zero */
        nbt_node *tree = nbt_parse(data, space_left);
        if (!tree) {/* Error */
            exit(1);
        }
        slot_data->tree = tree;
        struct buffer nbt_bin = nbt_dump_binary(tree);
        nbt_length = nbt_bin.len;
        free(nbt_bin.data);
    } else { /* No nbt data */
        slot_data->tree = NULL;
        nbt_length = 1;
    }
    return nbt_length + 5;
}

int decode_packet(bot_t *bot, void *_packet_raw, void *_packet_data)
{
    // packet_raw = raw packet data
    // packet_data = struct containing packet data
    //
    char *packet_raw = _packet_raw;
    char *packet_data = _packet_data;

    uint64_t bytes_read = 0; /* bytes read from packet_raw */
    uint32_t len;
    vint32_t value;
    uint32_t arr_len = -1;
    size_t size;

    char *fmt = *((char **)packet_data);
    assert(fmt != 0);
    packet_data += sizeof(char *);

    int32_t packet_size;
    int packet_size_len = varint32(packet_raw, &packet_size);
    packet_raw += packet_size_len;
    bytes_read += packet_size_len;

    uint64_t host_order;
    while(*fmt) {
        size = format_sizeof(*fmt);
        packet_data = (char *)align(packet_data, size);
        switch(*fmt) {
        case 'k':
            /* Notch decided it was a good idea not to tell us the length of
             * the nbt binary data. cNBT expects us the know this length. We
             * use the number of bytes left in the packet.
             * cNBT has no function to calculate the length of a nbt tree in
             * its binary form. The only way to do this is to dump the tree,
             * which will give us the length. Thanks.
             **/
        {
            slot_t *slot_data = (slot_t *)packet_data;
            /* bytes_read is the total number of bytes read whereas
             * packet_size is only the number of bytes read starting from
             * the packet_id. Thus, the max number of bytes available for slot
             * is packet_size + packet_size_len - bytes_read*/
            uint64_t slot_length = demarshal_slot(packet_raw, slot_data, packet_size + packet_size_len - bytes_read);
            packet_raw += slot_length;
            bytes_read += slot_length;
            break;
        }
        case 'q':
            memcpy(packet_data, packet_raw, size);
            packet_raw += size;
            bytes_read += size;
            break;
        case 's': // varint followed by string
            len = varint32(packet_raw, &value);
            packet_raw += len;
            bytes_read += size;
            char *str = calloc(value + 1, sizeof(char)); // null terminated
            memmove(str, packet_raw, value);
            *((char **)packet_data) = str;
            packet_raw += value;
            bytes_read += value;
            break;
        case 'v': // varint
            len = varint32(packet_raw, &value);
            arr_len = value;
            memmove(packet_data, &value, sizeof(vint32_t));
            packet_raw += len;
            bytes_read += len;
            break;
        case '*':
            fmt++;
            size_t size_elem = format_sizeof(*fmt);
            assert(arr_len != -1);
            void *arr = calloc(arr_len, size_elem);
            for(int i = 0; i < arr_len * size_elem; i += size_elem) {
                host_order = value_at(packet_raw + i, size_elem);
                host_order = reverse(host_order, size_elem);
                memmove(arr + i, &host_order, size_elem);
            }
            *(void **)packet_data = arr;
            packet_raw += arr_len * size_elem;
            bytes_read += arr_len * size_elem;
            break;
        case 'b':
        case 'h':
        case 'w':
        case 'l':
            ;

            host_order = value_at(packet_raw, size);
            host_order = reverse(host_order, size);
            memmove(packet_data, &host_order, size);

            arr_len = host_order;
            packet_raw += size;
            bytes_read += size;
            break;
        }
        packet_data += size;
        fmt++;
    }
    return packet_size_len + packet_size;
}

vint32_t peek_packet(bot_t *bot, void *packet_raw)
{
    int32_t packet_size;
    int32_t value;
    uint32_t len;
    int packet_size_len = varint32(packet_raw, &packet_size);
    packet_raw += packet_size_len;

    len = varint32(packet_raw, &value);
    return value;
}

void free_packet(void *_packet_data)
{
    char *packet_data = _packet_data;
    char *fmt = *((char **)packet_data);
    void *save = packet_data;
    packet_data += sizeof(char *);
    while (*fmt) {
        size_t size = format_sizeof(*fmt);
        packet_data = (char *)align(packet_data, size);
        if (*fmt == 's' || *fmt == '*') {
            free(*(void **)packet_data);
            if(*fmt == '*')
                fmt++;
        }
        packet_data += size;
        fmt++;
    }
    free(save);
}
