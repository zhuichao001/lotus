#include "mysql.h"
#include "../buff.h"


int default_server_capabilities(){
    int flag = 0;
    flag |= CLIENT_LONG_PASSWORD;
    flag |= CLIENT_FOUND_ROWS;
    flag |= CLIENT_LONG_FLAG;
    flag |= CLIENT_CONNECT_WITH_DB;
    flag |= CLIENT_ODBC;
    flag |= CLIENT_IGNORE_SPACE;
    flag |= CLIENT_PROTOCOL_41;
    flag |= CLIENT_INTERACTIVE;
    flag |= CLIENT_IGNORE_SIGPIPE;
    flag |= CLIENT_TRANSACTIONS;
    flag |= CLIENT_SECURE_CONNECTION;
    return flag;
}

char* random_seed(int size){
    char* seed = (char*)malloc(size);
    for(int i=0 ;i < size ;i++){
        //seed[i] = rand()%256;
        seed[i] = 0xff;
    }
    return seed;
}
                          
mysql_packet_t::mysql_packet_t():
    packet_len(0),
    sequence_id(0){
}

handshake_packet_t::handshake_packet_t():
    protocol_version(PROTOCOL_VERSION),
    server_version(SERVER_VERSION),
    thread_id(0), //TODO
    capabilities(server_capabilities()),
    charset_index(CHASET_INDEX_UTF8),
    server_status(uint16_t(SERVER_STATUS::AUTOCOMMIT)) {
    seed = random_seed(SCRAMBLE_PASSWORD_LEN);
    rest_of_scramble = seed+8;
}

auth_packet_t::auth_packet_t(){
}

command_packet_t::command_packet_t(){
}

eof_packet_t::eof_packet_t(){
}

error_packet_t::error_packet_t(){
}

ok_packet_t::ok_packet_t():
    field_count(0),
    affected_rows(0),
    inserted_id(0),
    server_status(),
    warning_count(uint16_t(SERVER_STATUS::AUTOCOMMIT)),
    message(nullptr){
}

/////////////////////////////////////////////////
void handshake_packet_t::encode(buff_t *to){
    to->write_le(protocol_version, 1);
    to->append(server_version, strlen(server_version)+1);
    to->write_le(thread_id, 4);
    to->append(seed, 8);
    to->append(0, 1);
    to->write_le(capabilities, 2);
    to->write_le(charset_index, 1);
    to->write_le(server_status, 2);
    char padding[13];
    memset(padding, 0, sizeof(padding));
    to->append(padding, 13);
    to->append(rest_of_scramble, 13);
}

void handshake_packet_t::decode(buff_t *from){
    //TODO:
}

void auth_packet_t::encode(buff_t *to){
    //TODO:
}

void auth_packet_t::decode(buff_t *from){
    char *data = from->data();
    int len = from->len();
    if(len<32){
        return;
    }
    //FIXME
}

void command_packet_t::encode(buff_t *to){
    //TODO:
}

void command_packet_t::decode(buff_t *from){
}

void eof_packet_t::encode(buff_t *to){
    //TODO:
}

void error_packet_t::decode(buff_t *from){
    //TODO:
}

void error_packet_t::encode(buff_t *to){
    //TODO:
}

int mysql_request_t::encode(buff_t *to){
    return 0;
}

int mysql_request_t::decode(buff_t *from){
    return 0;
}

int mysql_response_t::encode(buff_t *to){
    return 0;
}

int mysql_response_t::decode(buff_t *from){
    return 0;
}
