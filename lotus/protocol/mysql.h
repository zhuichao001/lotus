#pragma once

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <string>
#include "../buff.h"

#define PROTOCOL_VERSION 0x0A
#define SERVER_VERSION "5.1.1"
#define CHASET_INDEX_UTF8 83
#define SCRAMBLE_PASSWORD_LEN 20

#define OK_PACKET_FIELD_COUNT 0x00
#define ERROR_PACKET_FIELD_COUNT 0xff
#define EOF_PACKET_FIELD_COUNT 0xfe

//SERVER STATUS
enum class SERVER_STATUS {
    IN_TRANS            = 0x0001,
    AUTOCOMMIT          = 0x0002,
    CURSOR_EXISTS       = 0x0040,
    LAST_ROW_SENT       = 0x0080,
    DB_DROPPED          = 0x0100,
    NO_BACKSLASH_ESCAPES= 0x0200,
    METADATA_CHANGED    = 0x0400,
};

//COMMAND TYPE
enum class COMMAND_TYPE {
    COM_UNKNOWN             = 0x01,
    COM_QUIT                = 0x01,
    COM_INIDB               = 0x02,
    COM_QUERY               = 0x03,
    COM_FIELD_LIST          = 0x04,
    COM_CREATE_DB           = 0x05,
    COM_DROP_DB             = 0x06,
    COM_STMT_PREPARE        = 0x16,
    COM_STMT_EXECUTE        = 0x17,
    COM_STMT_SEND_LONG_DATA = 0x18,
    COM_STMT_CLOSE          = 0x19,
    COM_STMT_RESET          = 0x1A,
    COM_SET_OPTION          = 0x1B,
    COM_STMT_FETCH          = 0x1C,
};

enum class FILELD_TYPE{
    DECIMAL     = 0,
    TINY        = 1,
    SHORT       = 2,
    LONG        = 3,
    FLOAT       = 4,
    DOUBLE      = 5,
    NULLER      = 6,
    TIMESTAMP   = 7,
    LONGLONG    = 8,
    INT24       = 9,
    DATE        = 10,
    TIME        = 11,
    DATETIME    = 12,
    YEAR        = 13,
    NEWDATE     = 14,
    VARCHAR     = 15,
    BIT         = 16,
    NEW_DECIMAL = 246,
    ENUM        = 247,
    SET         = 248,
    TINY_BLOB   = 249,
    MEDIUM_BLOB = 250,
    LONG_BLOB   = 251,
    BLOB        = 252,
    VAR_STRING  = 253,
    STRING      = 254,
    GEOMETRY    = 255
};

enum CAPABILITY_FLAG {
    // new more secure passwords
    CLIENT_LONG_PASSWORD = 1,
    // Found instead of affected rows
    CLIENT_FOUND_ROWS = 2,
    // Get all column flags
    CLIENT_LONG_FLAG = 4,
    // One can specify db on connect
    CLIENT_CONNECT_WITH_DB = 8,
    // Don't allow database.table.column
    CLIENT_NO_SCHEMA = 16,
    // Can use compression protocol
    CLIENT_COMPRESS = 32,
    // Odbc client
    CLIENT_ODBC = 64,
    // Can use LOAD DATA LOCAL
    CLIENT_LOCAL_FILES = 128,
    // Ignore spaces before '('
    CLIENT_IGNORE_SPACE = 256,
    // New 4.1 protocol This is an interactive client
    CLIENT_PROTOCOL_41 = 512,
    // This is an interactive client
    CLIENT_INTERACTIVE = 1024,
    // Switch to SSL after handshake
    CLIENT_SSL = 2048,
    // IGNORE sigpipes
    CLIENT_IGNORE_SIGPIPE = 4096,
    // Client knows about transactions
    CLIENT_TRANSACTIONS = 8192,
    // Old flag for 4.1 protocol
    CLIENT_RESERVED = 16384,
    // New 4.1 authentication
    CLIENT_SECURE_CONNECTION = 32768,
    // Enable/disable multi-stmt support
    CLIENT_MULTI_STATEMENTS = 65536,
    // Enable/disable multi-results
    CLIENT_MULTI_RESULTS = 131072,
};

enum class STAGE {
    HANDSHAKE   = 1,
    COMMAND     = 3,
};

enum mysql_rsptype_t {
    RSP_OK          = 101, 
    RSP_ERROR       = 102,
    RSP_EOF         = 103,
    RSP_RESULTSET   = 104,
};

typedef struct mysql_packet_t {
    int packet_len;  //3 bytes
    int sequence_id; //1 bytes
    void *payload;

    mysql_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} mysql_packet_t;

typedef struct handshake_packet_t {
    const uint8_t protocol_version;
    const char *server_version;
    uint32_t thread_id;
    char *seed;
    int capabilities;
    uint8_t charset_index;
    uint16_t server_status;
    char *rest_of_scramble;

    handshake_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} handshake_packet_t;

typedef struct auth_packet_t {
    uint32_t client_flags;
    uint32_t max_packet_size;
    uint8_t charset_index;
    unsigned char extra[23]; //23 bytes

    char *usr;
    char passwd[128];
    char *database;

    auth_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} auth_packet_t;

typedef struct command_packet_t {
    COMMAND_TYPE command;
    uint8_t *args;

    command_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} command_packet_t;

typedef struct eof_packet_t {
    unsigned char field_count;
    unsigned char warning_count;
    unsigned char status;

    eof_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} eof_packet_t;

typedef struct error_packet_t {
    unsigned char field_count;
    int error;
    unsigned char mark;
    char *sql_state;
    char *message;

    error_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} error_packet_t;

typedef struct ok_packet_t {
    uint8_t field_count;
    uint64_t affected_rows; //atmost 8 bytes
    uint64_t inserted_id; //atmost 8 bytes
    uint16_t server_status;
    uint16_t warning_count;
    char *message;

    ok_packet_t();
    int encode(buff_t *to);
    int decode(buff_t *from);
} ok_packet_t;

typedef struct {
    unsigned char* value;
    int length;

    void *next; //used for link list
} _field_value_t;

typedef struct {
    int field_count;
    _field_value_t *value_list;
} row_packet_t;

typedef struct {
    int field_count;
    long extra;
} _result_set_header_t;

struct LIST_HEAD{
        struct LIST_HEAD *prev;
        struct LIST_HEAD *next;
};

typedef struct {
    _result_set_header_t *res_header;
    struct LIST_HEAD fields;
    eof_packet_t *eof;
    row_packet_t *row;
    eof_packet_t *last_eof;
} result_set_t;

class mysql_request_t {
    command_packet_t cmd_pkt;

public:
    int encode(buff_t *to);
    int decode(buff_t *from);
    void print(){
        buff_t buf(512);     
        this->encode(&buf);
        fprintf(stderr, "print mysql_request_t------\n%s\n", buf.data());
    }
};

class mysql_response_t {
    mysql_packet_t pkt;
    mysql_rsptype_t type;
public:
    int encode(buff_t *to);
    int decode(buff_t *from);
    void print(){
        buff_t buf(512);     
        encode(&buf);
        fprintf(stderr, "print mysql_response_t------\n%s\n", buf.data());
    }
};
