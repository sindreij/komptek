#include "tree.h"
#include "vslc.h"
#include "symtab.h"


#ifdef DUMP_TREES
void
node_print ( FILE *output, node_t *root, uint32_t nesting )
{
    if ( root != NULL )
    {
        fprintf ( output, "%*c%s", nesting, ' ', root->type.text );
        if ( root->type.index == INTEGER )
            fprintf ( output, "(%d)", *((int32_t *)root->data) );
        if ( root->type.index == VARIABLE || root->type.index == EXPRESSION || root->type.index == TEXT)
        {
            if ( root->data != NULL )
                fprintf ( output, "(\"%s\")", (char *)root->data );
            else
                fprintf ( output, "%p", root->data );
        }
        fputc ( '\n', output );
        for ( int32_t i=0; i<root->n_children; i++ )
            node_print ( output, root->children[i], nesting+1 );
    }
    else
        fprintf ( output, "%*c%p\n", nesting, ' ', root );
}
#endif


node_t * node_init ( node_t *nd, nodetype_t type, void *data, uint32_t n_children, ... ) {
    nd->type = type;
    nd->data = data;
    nd->entry = NULL;
    nd->n_children = n_children;

    if (n_children > 0) {
        va_list children;
        va_start(children, n_children);
        nd->children = (node_t**)malloc(sizeof(node_t*)*n_children);
        for (int i = 0; i<n_children; i++) {
            node_t* child = va_arg(children, node_t*);
            nd->children[i] = child;
        }
    } else {
        nd->children = NULL;
    }
    return nd;
}


void node_finalize ( node_t *discard ) {
    free(discard->data);
    //We do not free discard->entry since that does belong to the
    //symbol table, and it is the symbol table that frees those.
    free(discard->children);
    free(discard);
}


void destroy_subtree ( node_t *discard ){
    if (discard == NULL) {
        return;
    }
    for (int i=0; i<discard->n_children; i++) {
        if (discard->children[i] != NULL) {
            destroy_subtree(discard->children[i]);
            //The pointer children[i] no longer point to anything intresting
            discard->children[i] = NULL;
        }
    }
    node_finalize(discard);
}

//Henter alle stringene og kaller strings_add på de
void collect_strings(node_t *node) {
    if (node == NULL) {
        return;
    }

    //Vi har funnet en teks-node
    if (node->type.index == TEXT) {
        //Vi plukker ut teksten fra den
        char* text = node->data;
        int32_t* index = malloc(sizeof(int32_t));
        //Så legger vi til den i listen over strenger
        //og får en index tilbake
        *index = strings_add(text);
        //Nå trenger ikke tekstnoden å holde på strengen
        //lengre, den holder heller en index for hvor
        //strengen er
        node->data = index;
    } else {
        //Er det ikke en tekst node vi har funnet så fortsetter vi å 
        //dukke nedover for å finne en.
        for (uint32_t i=0; i < node->n_children; i++) {
            collect_strings(node->children[i]);
        }
    }
}

//Denne finner alle funskonene i treet vårt
void collect_functions(node_t *node) {
    if (node == NULL) {
        return;
    }

    //Har vi funnet en funksjon så trenger vi ikke å dukke dypere
    if (node->type.index == FUNCTION) {
        //Vi lager et nytt symbol, setter variablene riktig
        symbol_t* symbol = (symbol_t*) malloc(sizeof(symbol_t));
        symbol->stack_offset = 0;
        symbol->label = STRDUP(node->children[0]->data);
        //Så lagrer vi symbolet vårt
        symbol_insert(symbol->label, symbol);
        //Og til slutt lager vi en kobling mellom funksjonen
        //og symbolet så vi kan finne tilbake
        node->entry = symbol;
    } else {
        for (uint32_t i=0; i < node->n_children; i++) {
            collect_functions(node->children[i]);
        }
    }
}

//For å holde rede på hvilken funksjon vi er i. Brukes bare
//for fine feilmeldinger hvis vi finner en feil.
symbol_t* current_function;
//For å holde rede på hva offset for neste lokale variabel er
int32_t local_offset;

void collect_names(node_t *node) {
    if (node == NULL) {
        return;
    }
    //For hver node må vi finne ut hva det er.
    //Merk at bare enkelte av disse har et rekursivt kall.
    switch ( node->type.index ) {
        case FUNCTION:{
            //Er det en funksjon vi har funnet, så må vi lage et nytt scope
            scope_add();
            //Så må vi finne alle parameterne til funksjonen. Vi starter
            //med å finne noden som er VARIABLE_LIST. Denne skal ligge som
            //barn nummer 2 (dvs 1 0-indexert).
            node_t* variable_list = node->children[1];
            //En VARIABLE_LIST kan være NULL hvis funksjonen ikke har parametre
            if (variable_list != NULL) {
                //Siste parameter skal ha offset 8. Det skal være 4 i forskjell
                //mellom hvert offset og det synker. Vi må regne ut hva offsettet
                //skal være til den første.
                uint32_t offset = variable_list->n_children*4 + 4;
                //Så løper vi igjennom alle barna til PARAMETER_LIST. Dette skal være
                //noder av type VARIABLE.
                for (uint32_t i=0; i<variable_list->n_children; i++) {
                    node_t* child = variable_list->children[i];
                    //Vi lager et nytt symbol
                    symbol_t* symbol = (symbol_t*) malloc(sizeof(symbol_t));
                    //Setter offset riktig
                    symbol->stack_offset = offset;
                    //Neste offset blir da 4 mindre
                    offset -= 4;
                    //Variabler trenger ikke label
                    symbol->label = NULL;
                    symbol_insert(child->data, symbol);
                    //Vi setter entry så vi vet hva som er symbolet for denne
                    //variabelen.
                    child->entry = symbol;
                }
            }
            //Så setter vi current_function for debugging-formål. Dette
            //gjør at vi vet hvor vi er.
            current_function = node->entry;
            //Resetter local_offset for nytt scope. (Siden det ikke finnes noe
            //variabel-scope over en funksjon trenger vi ikke ta vare på gammel verdi)
            local_offset = -4;
            //Så kaller vi oss selv rekursivt BLOCK-noden som skal være barn nummer
            //tre. Merk at dette lager enda et nytt scope. Det scopet vi nettopp har
            //laget vil dermed bare ha parameterne.
            //(Dette gjelder ikke hvis det ikke er en BLOCK under funksjonen, men
            //da har funskjonen bare et STATEMENT)
            collect_names(node->children[2]);
            //Og der var hele funksjonen gjort. Vi kan nå resette current_function
            //og fjerne scopet.
            current_function = NULL;
            scope_remove();
            break;
        }
        case BLOCK:{
            //En BLOCK ligner veldig på en funksjon, men er litt enklere på noen måter
            //vi starter med å legge til et scope.
            scope_add();
            //Vi må ta vare på det gamle offsettet (Eller kanskje egentlig ikke siden en BLOCK
            //må ha DECLARATION_LIST på starten, men det er nå litt ryddig, siden offsettet
            //fra denne blokken ikke vil havne i blokken over.)
            int32_t old_offset = local_offset;
            //Så kan vi resette local_offset
            local_offset = -4;
            //Så går vi igjennom alle nodene under og rekursivt kaller de
            //(Jeg ser i ettertid at jeg med dette og lagringen av old_offset gjør
            //at man ikke trenger å ha DECKLARATION_LIST på starten av en BLOCK, 
            //men det må man ifølge språkets grammar. Kall det fremadtenkning)
            for (uint32_t i=0; i < node->n_children; i++) {
                node_t* child = node->children[i];
                if (child != NULL) {
                    collect_names(child);
                }
            }
            //Vi er ferdig med å konsumere barna til blokken. På tide å fjerne
            //scopet og sette tilbake local_offset.
            scope_remove();
            local_offset = old_offset;
            break;
        }
        case DECLARATION:{
            //En DECLARATION betyr at vi har en eller flere variabel å deklarere. Denne ligner
            //ganske mye på parameter-delen av en funksjon. Den eneste forskjellen er
            //at offset settes litt anderledes. Hvis man skulle refaktorere dette
            //så burde kanskje denne biten være en egen funksjon.
            //Merk forresten at denne ikke rekurserer videre.
            node_t* variable_list = node->children[0];
            if (variable_list != NULL) {
                for (uint32_t i=0; i<variable_list->n_children; i++) {
                    node_t* child = variable_list->children[i];
                    symbol_t* symbol = (symbol_t*) malloc(sizeof(symbol_t));
                    symbol->stack_offset = local_offset;
                    local_offset -= 4;
                    symbol->label = NULL;
                    symbol_insert(child->data, symbol);
                    child->entry = symbol;
                }
            }
            break;
        }
        //Vi kan vite sikkert at en VARIABLE ikke vil dukke opp som er under
        //en DECLARATION siden rekursjonen stopper på DECLARATION. Derfor er dette
        //greit.
        case VARIABLE:{
            //Dette skjer altså for nodene som er VARIABLE som ikke er en del av 
            //en deklarasjon eller en funksjonsdeklarasjon. Altså en VARIABLE
            //som er en del av en EXPRESSION.

            //Vi starter med å finne ut hvilket symbol som hører til variablene
            symbol_t* symbol = symbol_get(node->data);
            //Hvis vi ikke fant noe symbol så printer vi en error-melding og stopper
            //kjøringen av hele programmet (Litt dristig kanskje, men det er fornuftig
            //nå, i fremtiden når denne kompilatoren blir ferdig burde man kanskje
            //inkrementere en error_number og så si ifra når man har kjørt igjennom
            //slik at man kan plukke opp flere feil).
            if (symbol == NULL) {
                //Her kommer current_function til sitt bruk.
                if (current_function != NULL) {
                    printf("In function %s:\n", current_function->label);
                }
                printf("error: '%s' undeclared\n", node->data);
                exit(0);
            }
            //Setter entry så vi vet hvor variabelen er.
            node->entry = symbol;
        }
        default:{
            //Er dette en annen nodetype så kaller vi alle barn rekursivt.
            for (uint32_t i=0; i < node->n_children; i++) {
                node_t* child = node->children[i];
                if (child != NULL) {
                    collect_names(child);
                }
            }
        } 
    }
}

void bind_names ( node_t *root ){
    //Vi henter først alle strenger for å bli ferdig med det.
    collect_strings(root);


    //Vi lager et root-scope. Her vil alle funksjonene havne
    scope_add();
    //Så går vi igjennom og henter funksjonene. Vi gjør dette først så en funksjon kan
    //kalles uansett om den er deklarert før eller etter den brukes
    collect_functions(root);
    //Så tilslutt henter vi opp alle variabler og funksjonskall
    collect_names(root);
    //Vi er ferdig og kan slette root-scopet.
    scope_remove();
}


node_t* simplify_tree ( node_t* node ){
    if ( node != NULL ){

        // Recursively simplify the children of the current node
        for ( uint32_t i=0; i<node->n_children; i++ ){
            node->children[i] = simplify_tree ( node->children[i] );
        }

        // After the children have been simplified, we look at the current node
        // What we do depend upon the type of node
        switch ( node->type.index )
        {
            // These are lists which needs to be flattened. Their structure
            // is about the same, so they can be treated the same way.
            // I also do DECLARATION_LIST here, with a little smartness
            // all this cases can use the same logic
            case FUNCTION_LIST: case STATEMENT_LIST: case PRINT_LIST:
            case EXPRESSION_LIST: case VARIABLE_LIST:
            case DECLARATION_LIST:
                {
                    //After this is runned no child of the node shall be a
                    //list, every FUNCTION_LIST shall be a FUNCTION etc..

                    //By the law of recursion we then know that every child either
                    //is a FUNCTION or a FUNCTION_LIST with only FUNCTION childs. We
                    //want to lift this children up to be ours children, so we need
                    //to make a new child list.

                    //Since we don't have lists that automatically
                    //changes size, we first have to compute the count of 
                    //the new children list.
                    int n_new_children = 0;
                    for ( uint32_t i=0; i<node->n_children; i++ ){
                        node_t* child = node->children[i];
                        //If we have a DECLARATION_LIST it can have child that is
                        //NULL. In that case we just ignore the child (which in effect
                        //is the same as deleting them since we are making a new list)
                        if (child != NULL) {
                            if (child->type.index == node->type.index) {
                                //If this is a new list we add the number of children of
                                //that list
                                n_new_children += child->n_children;
                            } else {
                                //If this is a element we add one
                                n_new_children += 1;
                            }
                        }
                    }
                    //When we know the number of children we add them.
                    //This is just the same loop as before, but instead of
                    //counting the children we add them
                    node_t** new_children = (node_t**)malloc(sizeof(node_t*)*n_new_children);
                    //To append new children we need to know which place to put
                    //them. We need a variable for that
                    int j = 0;
                    for ( uint32_t i=0; i<node->n_children; i++ ){
                        node_t* child = node->children[i];
                        if (child != NULL) {
                            if (child->type.index == node->type.index) {
                                //This is a list. Add all of its childs
                                for ( uint32_t k=0; k<child->n_children; k++ ){
                                    //Add the children
                                    new_children[j] = child->children[k];
                                    //Increment j so we don't use the same
                                    //place twice
                                    j++;
                                }
                                //Now we have copied the childs of this node
                                //and it is of no use anymore.
                                node_finalize(child);
                            } else {
                                //This is a element. Just add it
                                new_children[j] = child;
                                j++;
                            }
                        }
                    }
                    //Now we have a new list. We can 
                    free(node->children);
                    //And set the new list as our list.
                    node->children = new_children;
                    node->n_children = n_new_children;
                }
                break;

            
            // These have only one child, so they are not needed
            case STATEMENT: case PARAMETER_LIST: case ARGUMENT_LIST:
            {
                //We just find the child. Make that the new node in
                //this place (by returning it) and delete the old node
                node_t* child = node->children[0];
                node_finalize(node);
                return child;
            }


            // Expressions where both children are integers can be evaluated (and replaced with
            // integer nodes). Expressions whith just one child can be removed (like statements etc above)
            case EXPRESSION:
                if (node->n_children == 1 && node->data == NULL) {
                    //If it has only one child we delete it (and return the child)
                    node_t* child = node->children[0];
                    node_finalize(node);
                    return child;
                } else if (node->n_children == 2 
                        && node->children[0]->type.index == INTEGER
                        && node->children[1]->type.index == INTEGER) {
                    //If the node has two child which both is INTEGER we add thos
                    //nodes. We use our own data as a rule to how we shall add them.
                    //Since we are working in pointers to ints we need a new place to
                    //put the result.
                    int32_t* result = malloc(sizeof(int32_t));
                    if (strcmp(node->data, "+") == 0) {
                        *result = *(int32_t*) node->children[0]->data + *(int32_t*) node->children[1]->data;
                    } else if (strcmp(node->data, "-") == 0) {
                        *result = *(int32_t*) node->children[0]->data - *(int32_t*) node->children[1]->data;
                    } else if (strcmp(node->data, "*") == 0) {
                        *result = *(int32_t*) node->children[0]->data * *(int32_t*) node->children[1]->data;
                    } else if (strcmp(node->data, "/") == 0) {
                        *result = *(int32_t*) node->children[0]->data / *(int32_t*) node->children[1]->data;
                    } else {
                        return node;
                    }
                    //And then we make this node a INTEGER node with result as the data

                    //First we free the old data
                    free(node->data);
                    //Then we make the new data the result
                    node->data = result;
                    //Then we finalize and NULL out the old children
                    node_finalize(node->children[0]);
                    node_finalize(node->children[1]);
                    node->children[0] = NULL;
                    node->children[1] = NULL;
                    //We set the type to integer
                    node->type = integer_n;
                    //And the number of children to zero
                    node->n_children = 0;
                } else if (node->n_children == 1
                        && node->children[0]->type.index == INTEGER
                        && strcmp(node->data, "-") == 0) {
                    //There is one more case, the unary minus. In that case we have
                    //one INTEGER-node and our data is -. If that is the case we have
                    //to negate the value of the child and return that node

                    node_t* child = node->children[0];
                    //Notice no dereferencing. Points to the same value
                    //but with a different type of pointer. We need to do this
                    //becouse we need to cast to a int-pointer for the next statement
                    //to work
                    int32_t* val = (int32_t*)child->data;
                    //Negate the value. Notice that we do put this value back into the
                    //child since it is already there ;)
                    *val *= -1;
                    //Same deal as we have done before. Finalize this node and return
                    //the child.
                    node_finalize(node);
                    return child;
                }
                break;
        }

    }
    return node;
}
