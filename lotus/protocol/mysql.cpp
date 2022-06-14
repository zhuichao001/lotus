#include "mysql.h"
#include "../buff.h"


int server_capabilities(){
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

int read_lengthed_str(buff_t *from, char *dst){
    char *src = from->data();
    int len = *(uint8_t*)src;
    int bytes = 0;
    if(len<0xFC){
        memcpy(dst, src+1, len);
        return 1;
    }else if(len==0xFC){
        bytes = 2;
    }else if(len==0xFD){
        bytes = 4;
    }else if(len==0xFE){
        bytes = 8;
    }

    len = 0;
    for(int i=0; i<bytes; ++i){
        len = len + (*(uint8_t*)(src+1))<<(i*8);
    }
    memcpy(dst, src+1+bytes, len);
    return bytes+1;
}

char *random_seed(int size){
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

int mysql_packet_t::encode(buff_t *to){
    //TODO
    return 0;
}

int mysql_packet_t::decode(buff_t *from){
    if(from->read_le(&packet_len, 3)<0){
        fprintf(stderr, "Error: failed to read packet_len\n");
        return -1;
    }
    
    if(from->read_le(&sequence_id, 1)<0){
        fprintf(stderr, "Error: failed to read sequence_id\n");
        return -1;
    }

    payload = (void*)from->data();
    return packet_len;
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

command_packet_t::command_packet_t():
    command(COMMAND_TYPE::COM_UNKNOWN),
    args(nullptr){
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
int handshake_packet_t::encode(buff_t *to){
    to->write_le(&protocol_version, 1);
    to->append(server_version, strlen(server_version)+1);
    to->write_le(&thread_id, 4);
    to->append(seed, 8);
    to->append(0, 1);
    to->write_le(&capabilities, 2);
    to->write_le(&charset_index, 1);
    to->write_le(&server_status, 2);
    char padding[13];
    memset(padding, 0, sizeof(padding));
    to->append(padding, 13);
    to->append(rest_of_scramble, 13);
    return 45 + (strlen(server_version)+1);
}

int handshake_packet_t::decode(buff_t *from){
    //TODO:
    return 0;
}

int auth_packet_t::encode(buff_t *to){
    memset(extra, 0, sizeof(extra));
    memset(usr, 0, sizeof(usr));
    memset(passwd, 0, sizeof(passwd));
    memset(database, 0, sizeof(database));
    return 0;
}

int auth_packet_t::decode(buff_t *from){
    if(from->len()<32){
        return -1;
    }
    from->read_le(&client_flags, 4);
    from->read_le(&max_packet_size, 4);
    from->read_le(&charset_index, 1);
    from->read_le(extra, 23);

    usr = from->data();
    from->release(strlen(usr));

    int passwd_len = read_lengthed_str(from, passwd);

    database = from->data();
    from->release(strlen(database));

    return 32+strlen(usr)+passwd_len+strlen(database);
}

int command_packet_t::encode(buff_t *to){
    //TODO:
    return 0;
}

int command_packet_t::decode(buff_t *from){
    from->read_le(&command, 1);
    return 0;
}

int eof_packet_t::encode(buff_t *to){
    //TODO:
    return 0;
}

int error_packet_t::decode(buff_t *from){
    //TODO:
    return 0;
}

int error_packet_t::encode(buff_t *to){
    //TODO:
    return 0;
}

int mysql_request_t::encode(buff_t *to){
    return 0;
}

int mysql_request_t::decode(buff_t *from){
    cmd_pkt.decode(from);

    switch(cmd_pkt.command){
        //TODO: case
        default:
            fprintf(stderr, "cmd_type:%d\n", cmd_pkt.command);
    }
    return 0;
}

int mysql_response_t::encode(buff_t *to){
    return 0;
}

int mysql_response_t::decode(buff_t *from){
    return 0;
}

