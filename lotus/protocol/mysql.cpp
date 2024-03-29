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
    //flag |= CLIENT_PLUGIN_AUTH;
    return flag;
}

uint64_t read_lenenc_int(buff_t *from){
    char *src = from->data();
    uint8_t flag = *(uint8_t*)src;
    int bytes = 0;
    if(flag<0xFC){
        bytes = 0;
    }else if(flag==0xFC){
        bytes = 2;
    }else if(flag==0xFD){
        bytes = 4;
    }else if(flag==0xFE){
        bytes = 8;
    }

    uint64_t len = 0;
    if(flag<0xFC){
        len = flag;
    }
    for(int i=0; i<bytes; ++i){
        len = len + (*(uint8_t*)(src+1))<<(i*8);
    }
    from->release(bytes+1);
    return len;
}

int write_lenenc_int(buff_t *to, uint64_t val){
    int flag = 0;
    int bytes = 0;
    if(val<251){
        flag = val;
        bytes = 0;
    }else if(val<(2<<16)){
        flag = 0xFC;
        bytes = 2;
    }else if(val<(2<<24)){
        flag = 0xFD;
        bytes = 4;
    }else{ //less than 2<<64
        flag = 0xFE;
        bytes = 8;
    }
    to->write_le(&flag, 1);
    to->write_le(&val, bytes);
    return 0;
}

int read_lenenc_str(buff_t *from, char *dst){
    int len = int(read_lenenc_int(from));
    char *src=from->data();
    memcpy(dst, src, len);
    from->release(len);
    return len;
}

int write_lenenc_str(buff_t *to, const char *str, const int len){
    write_lenenc_int(to, len);
    to->append(str, len);
    return 0;
}

char *random_seed(int size){
    char *seed = (char*)malloc(size+1);
    for(int i=0 ;i < size ;i++){
        seed[i] = 0xff;
    }
    seed[size] = 0x00;
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

handshake_request_t::handshake_request_t():
    protocol_version(PROTOCOL_VERSION),
    server_version(SERVER_VERSION),
    thread_id(0), //TODO: increment
    capabilities(server_capabilities()),
    charset_index(CHASET_INDEX_UTF8),
    server_status(uint16_t(SERVER_STATUS::AUTOCOMMIT)){
    seed = random_seed(SCRAMBLE_PASSWORD_LEN);
    rest_of_scramble = seed+8;
}

handshake_response_t::handshake_response_t(){
}

command_packet_t::command_packet_t():
    command(COMMAND_TYPE::COM_UNKNOWN){
}

eof_packet_t::eof_packet_t():
    warning_count(0),
    server_status(uint16_t(SERVER_STATUS::AUTOCOMMIT)){
}

error_packet_t::error_packet_t():
    error(0),
    mark('#'),
    sql_state((char*)"HY000"),
    message((char*)""){
}

ok_packet_t::ok_packet_t():
    field_count(0),
    affected_rows(0),
    inserted_id(0),
    warning_count(0),
    server_status(uint16_t(SERVER_STATUS::AUTOCOMMIT)),
    message(nullptr){
}

/////////////////////////////////////////////////////////
int handshake_request_t::encode(buff_t *to){ //for protocol version 10
    to->write_le(&protocol_version, 1);
    to->append(server_version, strlen(server_version)+1);
    to->write_le(&thread_id, 4); //a.k.a. connection id
    to->append(seed, 8);
    uint8_t filter = 0x00;
    to->append(&filter, 1);
    uint16_t capability_flags_1 = capabilities & 0xffff;
    to->write_le(&capability_flags_1, 2); //the lower 2 bytes
    to->write_le(&charset_index, 1);
    to->write_le(&server_status, 2);
    uint16_t capability_flags_2 = (capabilities>>16) & 0xffff;
    to->write_le(&capability_flags_2, 2); //the upper 2 bytes
    if(capabilities & CLIENT_PLUGIN_AUTH){
        uint8_t auth_plugin_data_len = 0;
        to->write_le(&auth_plugin_data_len, 1);
    }
    char reserved[10];
    memset(reserved, 0, sizeof(reserved));
    to->append(reserved, 10);
    to->append(rest_of_scramble, 13);
    if(capabilities & CLIENT_PLUGIN_AUTH){
        const char *auth_plugin_name ="";
        to->append(auth_plugin_name, strlen(auth_plugin_name)+1);
    }
    return to->len();
}

int handshake_request_t::decode(buff_t *from){
    //TODO:
    return 0;
}

int handshake_response_t::encode(buff_t *to){
    memset(extra, 0, sizeof(extra));
    memset(usr, 0, sizeof(usr));
    memset(passwd, 0, sizeof(passwd));
    memset(database, 0, sizeof(database));
    return 0;
}

int handshake_response_t::decode(buff_t *from){
    if(from->len()<32){
        return -1;
    }

    from->read_le(&client_flags, 4);
    from->read_le(&max_packet_size, 4);
    from->read_le(&charset_index, 1);
    from->read_le(extra, 23);

    usr = from->data();
    from->release(strlen(usr));

    int passwd_len = strlen(from->data());
    from->read_str(passwd, passwd_len);
    if(client_flags & CLIENT_CONNECT_WITH_DB){
        database = from->data();
        from->release(strlen(database));
    }
    return 32+strlen(usr)+passwd_len+strlen(database);
}

int command_packet_t::encode(buff_t *to){
    //TODO
    return 0;
}

int command_packet_t::decode(buff_t *from){
    from->read_le(&command, 1);
    return 0;
}

int ok_packet_t::encode(buff_t *to){
    uint8_t flag = 0x00; //header for ok packet
    to->write_le(&flag, 1);
    write_lenenc_int(to, affected_rows);
    write_lenenc_int(to, inserted_id);
    to->write_le(&server_status, 2);
    to->write_le(&warning_count, 2);
    write_lenenc_str(to, message, strlen(message));
    return 0;
}

int ok_packet_t::decode(buff_t *from){
    //TODO
    return 0;
}

int error_packet_t::encode(buff_t *to){
    uint8_t flag = 0xFF; //header for error packet
    to->write_le(&flag, 1);
    to->write_le(&error, 2);
    to->write_le(&mark, 1);
    to->append(sql_state, strlen(sql_state));
    to->append(message, strlen(message));
    return to->len();
}

int error_packet_t::decode(buff_t *from){
    //TODO:
    return 0;
}

int eof_packet_t::encode(buff_t *to){
    uint8_t flag = 0xFE;
    to->write_le(&flag, 1);
    //if capabilities & CLIENT_PROTOCOL_41:
    to->write_le(&warning_count, 2);
    to->write_le(&server_status, 2);
    return to->len();
}

int eof_packet_t::decode(buff_t *from){
    //TODO
    return 0;
}

int mysql_request_t::encode(buff_t *to){
    //TODO
    return 0;
}

int mysql_request_t::decode(buff_t *from){
    header.decode(from);
    cmd.decode(from);
    switch(cmd.command){
        case COMMAND_TYPE::COM_INIDB:
            from->read_str(cmd.query, from->len());
            fprintf(stderr, "cmd_type: COM_INIDB, dbname:%s\n", cmd.query);
        case COMMAND_TYPE::COM_CREATE_DB:
            from->read_str(cmd.query, from->len());
            fprintf(stderr, "cmd_type: COM_CREATE_DB, dbname:%s\n", cmd.query);
        default:
            fprintf(stderr, "cmd_type: UNKNOWN\n");
    }
    return 0;
}

int mysql_response_t::encode(buff_t *to){
    //TODO:
    return 0;
}

int mysql_response_t::decode(buff_t *from){
    //TODO:
    return 0;
}
