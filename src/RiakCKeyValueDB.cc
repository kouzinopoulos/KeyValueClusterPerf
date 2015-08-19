// FUNCTIONS TEMPORARILY COMMENTED OUT DUE TO LIBRARY CONFLICT WITH NEWER RIAK !!!

// C C++ inlcudes
#include <map>
#include <string>
#include <sstream>
// External library: Riak-C
//#include "riak.h"
//#include "riak_messages.h"
// KeyValueClusterPerf includes
#include "logger.h"
#include "RiakCKeyValueDB.h"

// Forward declare

/*struct _riak_get_response {
    riak_boolean_t has_vclock;
    riak_binary   *vclock;
    riak_boolean_t has_unchanged;
    riak_boolean_t unchanged;
    riak_int32_t   n_content;
    riak_object  **content; // Array of pointers to allow expansion
 
    void    *internal;
};

struct _riak_object {
    riak_binary   *bucket;

    riak_boolean_t has_bucket_type;
    riak_binary   *bucket_type;

    riak_boolean_t has_key;
    riak_binary   *key;

    riak_binary   *value;

    riak_boolean_t has_charset;
    riak_binary   *charset;

    riak_boolean_t has_last_mod;
    riak_uint32_t  last_mod;

    riak_boolean_t has_last_mod_usecs;
    riak_uint32_t  last_mod_usecs;

    riak_boolean_t has_content_type;
    riak_binary   *content_type;

    riak_boolean_t has_content_encoding;
    riak_binary   *encoding;

    riak_boolean_t has_deleted;
    riak_boolean_t deleted;

    riak_boolean_t has_vtag;
    riak_binary   *vtag;

    riak_int32_t   n_links;
    riak_link    **links;
 
    riak_int32_t   n_usermeta;
    riak_pair    **usermeta;
    riak_int32_t   n_indexes;
    riak_pair    **indexes;
};

struct _riak_binary {
    riak_size_t    len;
    riak_uint8_t  *data;
    riak_boolean_t managed;
};
*/
RiakCKeyValueDB::RiakCKeyValueDB(map<string,string> configuration)
{
	/*LOG_DEBUG("Instance created");
	// use the default configuration
    err = riak_config_new_default(&cfg);
    if (err) {
        exit(1);
    }
    LOG_DEBUG("Configuration generated");

    // create a connection
	cxn  = NULL;
    err = riak_connection_new(cfg, &cxn, "cernvm11", "8087", NULL);
    if (err) {
        exit(1);
    }
    LOG_DEBUG("Connection established");

    riak_print_init(&print_state, output, sizeof(output));

    // Test if connection works
    LOG_DEBUG("Executing ping command");
    err = riak_ping(cxn);
    if (err) {
        fprintf(stderr, "PANG [%s]\n", riak_strerror(err));
        exit(1);
    }
    printf("PONG\n");*/
}

RiakCKeyValueDB::~RiakCKeyValueDB()
{
   /*riak_config_free(&cfg);*/
}

void RiakCKeyValueDB::putValue(string key, string* value)
{
    /*riak_object         *obj;
    obj = riak_object_new(cfg);
    if (obj == NULL) {
        riak_log_critical(cxn, "%s","Could not allocate a Riak Object");
        exit(1);
    }

    riak_object_set_bucket(cfg, obj, riak_binary_copy_from_string(cfg, "testClientBucket"));
    riak_object_set_bucket_type(cfg, obj, riak_binary_copy_from_string(cfg, "default"));
    const char* keychr = key.c_str();
    const char* valuechr = value->c_str();
    riak_object_set_key(cfg, obj, riak_binary_copy_from_string(cfg, keychr));
    riak_object_set_value(cfg, obj, riak_binary_copy_from_string(cfg, valuechr));
    if (riak_object_get_bucket(obj) == NULL ||
        riak_object_get_value(obj) == NULL) {
        fprintf(stderr, "Could not allocate bucket/value\n");
        riak_free(cfg, &obj);
        exit(1);
    }

    riak_put_options *put_options;
    riak_put_response *put_response = NULL;
    put_options = riak_put_options_new(cfg);
    if (put_options == NULL) {
        riak_log_critical(cxn, "%s","Could not allocate a Riak Put Options");
        exit(1);
    }
    riak_put_options_set_return_head(put_options, RIAK_TRUE);
    riak_put_options_set_return_body(put_options, RIAK_TRUE);
    // Put the value into riak cluster
    err = riak_put(cxn, obj, put_options, &put_response);
    if (err == ERIAK_OK) {
        riak_put_response_print(&print_state, put_response);
        printf("%s\n", output);
    }

    riak_put_response_free(cfg, &put_response);
    riak_object_free(cfg, &obj);
    riak_put_options_free(cfg, &put_options);
    if (err) {
        fprintf(stderr, "Put Problems [%s]\n", riak_strerror(err));
        exit(1);
    }*/
}
string RiakCKeyValueDB::getValue(string key)
{
    /*string returnStr;

    riak_get_response *get_response = NULL;
    riak_get_options *get_options;

    get_options = riak_get_options_new(cfg);
    if (get_options == NULL) {
        riak_log_critical(cxn, "%s","Could not allocate a Riak Get Options");
        exit(1);
    }
    riak_get_options_set_basic_quorum(get_options, RIAK_TRUE);
    riak_get_options_set_r(get_options, 2);


    riak_binary *bucket_type_bin = riak_binary_copy_from_string(cfg, "default");
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, "testClientBucket");
    riak_binary *key_bin      = riak_binary_copy_from_string(cfg, key.c_str());


    err = riak_get(cxn, bucket_type_bin, bucket_bin, key_bin, get_options, &get_response);
    if (err == ERIAK_OK) {
        riak_get_response_print(&print_state, get_response);
        printf("%s\n", output);
    }

    riak_get_response* getResponse = get_response;
    int contentSize = get_response->n_content;

    if(contentSize>0)
    {    
        riak_object** responseContent = get_response->content;
        riak_object* responseContentOne = responseContent[0];
        riak_binary* responseValue = responseContentOne->value;
        string tempStr((char*)responseValue->data, (size_t)responseValue->len);
        returnStr = tempStr;
    }

    riak_get_response_free(cfg, &get_response);
    riak_get_options_free(cfg, &get_options);
    if (err) {
        fprintf(stderr, "Get Problems [%s]\n", riak_strerror(err));
        exit(1);
    }
    return returnStr;*/
}
void RiakCKeyValueDB::deleteValue(string key)
{
    /*riak_delete_options *delete_options;

    delete_options = riak_delete_options_new(cfg);
    if (delete_options == NULL) {
        riak_log_critical(cxn, "%s","Could not allocate a Riak Delete Options");
        exit(1);
    }
    riak_delete_options_set_w(delete_options, 1);
    riak_delete_options_set_dw(delete_options, 1);

    riak_binary *bucket_type_bin = riak_binary_copy_from_string(cfg, "default");
    riak_binary *bucket_bin   = riak_binary_copy_from_string(cfg, "testClientBucket");
    riak_binary *key_bin      = riak_binary_copy_from_string(cfg, key.c_str());

    err = riak_delete(cxn, bucket_type_bin, bucket_bin, key_bin, delete_options);
    riak_delete_options_free(cfg, &delete_options);
    if (err) {
        fprintf(stderr, "Delete Problems [%s]\n", riak_strerror(err));
        exit(1);
    }*/
}

void RiakCKeyValueDB::initialise(map<string, string> keyValuePairs)
{

}
