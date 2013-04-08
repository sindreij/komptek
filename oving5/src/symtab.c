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


//Initates the tables
void symtab_init ( void ){
    scopes = (hash_t**)malloc(sizeof(hash_t*) * scopes_size);
    values = (symbol_t**)malloc(sizeof(symbol_t*) * values_size);
    strings = (char**)malloc(sizeof(char*) * strings_size);
}


//Sletter tabellene og alt innholdet.
void symtab_finalize ( void ) {
    //Sletter symbol-tabellene, dvs hash-tabellene.
    for (int i = 0; i <= scopes_index; i++) {
        ght_finalize(scopes[i]);
        scopes[i] = NULL;
    }
    //Og pekeren vår
    free(scopes);

    //Sletter alle verdiene som var i hash-tabellene.
    for (int i = 0; i <= values_index; i++) {
        //Sletter innholdet og pekeren
        free(values[i]->label);
        free(values[i]);
        values[i] = NULL;
    }
    free(values);

    //Og til slutt alle strengene
    for (int i = 0; i <= strings_index; i++) {
        free(strings[i]);
        strings[i] = NULL;
    }
    free(strings);

}

//Legger til en streng til listen over strenger
int32_t strings_add ( char *str ) {
    strings_index++;
    //Hvis index er lik antall elementer så har vi gått tom for
    //plass og må øke
    if (strings_index == strings_size) {
        //Finner den nye størrelsen. Her finnes det et utall
        //forskjellige måter, men en måte som fungerer er å øke
        //med 16
        strings_size += 16;
        //Så bruker vi realloc til å få en ny peker til et minneområde
        //med plass til dette. realloc flytter også de gamle verdiene
        //over
        strings = (char**) realloc(strings, sizeof(char*) * strings_size);
    }
    //Når vi vet at det er plass så setter vi det nye elementet til å være
    //strengen.
    strings[strings_index] = str;

    return strings_index;
}

//Printer ut alle strengene
void strings_output ( FILE *stream ) {
    //Først en header
    fprintf(stream, ".data\n");
    fprintf(stream, ".INTEGER: .string \"%%d \"\n");
    fprintf(stream, ".NEWLINE: .string \"\\n\"\n");
    //Så alle strengene
    for (int i = 0; i <= strings_index; i++) {
        fprintf(stream, ".STRING%d: .string %s\n", i, strings[i]);
    }
    //og så en footer
    fprintf(stream, ".globl main\n");
}

//Legger til et scoop
void scope_add ( void ) {
    //Starten her er prikk lik strings_add
    scopes_index++;
    if (scopes_index == scopes_size) {
        scopes_size += 16;
        scopes = (hash_t**) realloc(scopes, sizeof(hash_t*) * scopes_size);
    }
    //Her lager vi en ny hash-tabell og legger den til.
    hash_t* scope = ght_create(HASH_BUCKETS);
    scopes[scopes_index] = scope;
}


void scope_remove ( void ) {
    //Vi fjerner et scoop ved å finalize hash-tabellen og dekrementere
    //scopes_index. Merk at vi ikke krymper tabellen vår. Vi kunne gjort
    //det, men det er ikke strengt nødvendig.
    //Merk også at vi ikke fjerner verdiene i hash-tabellen. Det gjøres av
    //symtab_finalize.
    ght_finalize(scopes[scopes_index]);
    scopes_index--;
}

//Legger til et symbol i hashtabellen vår, dvs det gjeldende scopet.
void symbol_insert ( char *key, symbol_t *value ) {
    //Finner det gjeldende scopet
    hash_t* scope = scopes[scopes_index];
    //Legger dybden inn i symbolet.
    value->depth = scopes_index;
    //Og putter det inn i hash-tabellen
    ght_insert(scope, (void*) value, strlen(key), key);

    //Og så må vi putte symbolet i verdi-tabellen vår. Vi har en egen tabell
    //for dette for å letter kunne finalize alle symboler til slutt.
    //Denner er ellers lik som strings_add
    values_index++;
    if (values_index == values_size) {
        values_size += 16;
        values = (symbol_t**) realloc(values, sizeof(symbol_t*) * values_size);
    }
    values[values_index] = value;

    // Keep this for debugging/testing
    #ifdef DUMP_SYMTAB
    fprintf ( stderr, "Inserting (%s,%d)\n", key, value->stack_offset );
    #endif
}

//Henter et symbol fra gjeldende scopes. Gjeldende scope vil si det scopet
//vi er i og alle scop over dette.
symbol_t * symbol_get ( char *key ) {
    symbol_t* result = NULL;

    //Løper igjennom alle gjeldende scope og finner ut hvilket symbolet ligger
    //i. Begynner med det med høyest index (=depth).
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
