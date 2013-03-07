#include <string.h>

#include "symtab.h"

// static does not mean the same as in Java.
// For global variables, it means they are only visible in this file.

// Pointer to stack of hash tables (symbol tables)
static hash_t **scopes;

// Pointer to array of values, to make it easier to free them
static symbol_t **values;

// Pointer to array of strings, should be able to dynamically expand as new strings
// are added.
static char **strings;

// Helper variables for manageing the stacks/arrays
static int32_t scopes_size = 16, scopes_index = -1;
static int32_t values_size = 16, values_index = -1;
static int32_t strings_size = 16, strings_index = -1;


void symtab_init ( void ){
    scopes = (hash_t**)malloc(sizeof(hash_t*) * scopes_size);
    values = (symbol_t**)malloc(sizeof(symbol_t*) * values_size);
    strings = (char**)malloc(sizeof(char*) * strings_size);
}


void symtab_finalize ( void ) {
    for (int i = 0; i <= scopes_index; i++) {
        ght_finalize(scopes[i]);
        scopes[i] = NULL;
    }
    free(scopes);

    for (int i = 0; i <= values_index; i++) {
        free(values[i]->label);
        free(values[i]);
        values[i] = NULL;
    }
    free(values);

    for (int i = 0; i <= strings_index; i++) {
        free(strings[i]);
        strings[i] = NULL;
    }
    free(strings);

}


int32_t strings_add ( char *str ) {
    strings_index++;
    if (strings_index == strings_size) {
        strings_size += 16;
        strings = (char**) realloc(strings, sizeof(char*) * strings_size);
    }
    strings[strings_index] = str;

    return strings_index;
}


void strings_output ( FILE *stream ) {
    fprintf(stream, ".data\n");
    fprintf(stream, ".INTEGER: .string \"%%d \"\n");
    for (int i = 0; i <= strings_index; i++) {
        fprintf(stream, ".STRING%d: .string %s\n", i, strings[i]);
    }
    fprintf(stream, ".globl main\n");
}


void scope_add ( void ) {
    scopes_index++;
    if (scopes_index == scopes_size) {
        scopes_size += 16;
        scopes = (hash_t**) realloc(scopes, sizeof(hash_t*) * scopes_size);
    }
    hash_t* scope = ght_create(HASH_BUCKETS);
    scopes[scopes_index] = scope;
}


void scope_remove ( void ) {
    ght_finalize(scopes[scopes_index]);
    scopes_index--;
}


void symbol_insert ( char *key, symbol_t *value ) {
    hash_t* scope = scopes[scopes_index];
    value->depth = scopes_index;
    ght_insert(scope, (void*) value, strlen(key), key);

    values_index++;
    if (values_index == values_size) {
        values_size += 16;
        values = (hash_t**) realloc(values, sizeof(hash_t*) * values_size);
    }
    values[values_index] = value;

    // Keep this for debugging/testing
    #ifdef DUMP_SYMTAB
    fprintf ( stderr, "Inserting (%s,%d)\n", key, value->stack_offset );
    #endif
}


symbol_t * symbol_get ( char *key ) {
    symbol_t* result = NULL;

    for (int i = scopes_index; i>=0 && result==NULL; i--) {
        hash_t* scope = scopes[i];
        result = (symbol_t*) ght_get(scope, strlen(key), key);
    }


    // Keep this for debugging/testing
    #ifdef DUMP_SYMTAB
        if ( result != NULL )
            fprintf ( stderr, "Retrieving (%s,%d)\n", key, result->stack_offset );
    #endif

    return result;
}
