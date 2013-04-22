#include <tree.h>
#include <generator.h>
#include <assert.h>

bool peephole = false;


/* Elements of the low-level intermediate representation */

/* Instructions */
typedef enum {
    STRING, LABEL, PUSH, POP, MOVE, CALL, SYSCALL, LEAVE, RET,
    ADD, SUB, MUL, DIV, JUMP, JUMPZERO, JUMPNONZ, DECL, CLTD, NEG, CMPZERO, NIL,
    CMP, SETL, SETG, SETLE, SETGE, SETE, SETNE, CBW, CWDE,JUMPEQ
} opcode_t;

/* Registers */
static char
*eax = "%eax", *ebx = "%ebx", *ecx = "%ecx", *edx = "%edx",
    *ebp = "%ebp", *esp = "%esp", *esi = "%esi", *edi = "%edi",
    *al = "%al", *bl = "%bl";

/* A struct to make linked lists from instructions */
typedef struct instr {
    opcode_t opcode;
    char *operands[2];
    int32_t offsets[2];
    struct instr *next;
} instruction_t;

/* Start and last element for emitting/appending instructions */
static instruction_t *start = NULL, *last = NULL;

/*
 * Track the scope depth when traversing the tree - init. value may depend on
 * how the symtab was built
 */ 
static int32_t depth = 0;

/* Prototypes for auxiliaries (implemented at the end of this file) */
static void instruction_add ( opcode_t op, char *arg1, char *arg2, int32_t off1, int32_t off2 );
static void instructions_print ( FILE *stream );
static void instructions_finalize ( void );


/*
 * Convenience macro to continue the journey through the tree - just to save
 * on duplicate code, not really necessary
 */
#define RECUR() do {\
    for ( int32_t i=0; i<root->n_children; i++ )\
    generate ( stream, root->children[i] );\
} while(false)

/*
 * These macros set implement a function to start/stop the program, with
 * the only purpose of making the call on the first defined function appear
 * exactly as all other function calls.
 */
#define TEXT_HEAD() do {\
    instruction_add ( STRING,       STRDUP("main:"), NULL, 0, 0 );      \
    instruction_add ( PUSH,         ebp, NULL, 0, 0 );                  \
    instruction_add ( MOVE,         esp, ebp, 0, 0 );                   \
    instruction_add ( MOVE,         esp, esi, 8, 0 );                   \
    instruction_add ( DECL,         esi, NULL, 0, 0 );                  \
    instruction_add ( JUMPZERO,     STRDUP("noargs"), NULL, 0, 0 );     \
    instruction_add ( MOVE,         ebp, ebx, 12, 0 );                  \
    instruction_add ( STRING,       STRDUP("pusharg:"), NULL, 0, 0 );   \
    instruction_add ( ADD,          STRDUP("$4"), ebx, 0, 0 );          \
    instruction_add ( PUSH,         STRDUP("$10"), NULL, 0, 0 );        \
    instruction_add ( PUSH,         STRDUP("$0"), NULL, 0, 0 );         \
    instruction_add ( PUSH,         STRDUP("(%ebx)"), NULL, 0, 0 );     \
    instruction_add ( SYSCALL,      STRDUP("strtol"), NULL, 0, 0 );     \
    instruction_add ( ADD,          STRDUP("$12"), esp, 0, 0 );         \
    instruction_add ( PUSH,         eax, NULL, 0, 0 );                  \
    instruction_add ( DECL,         esi, NULL, 0, 0 );                  \
    instruction_add ( JUMPNONZ,     STRDUP("pusharg"), NULL, 0, 0 );    \
    instruction_add ( STRING,       STRDUP("noargs:"), NULL, 0, 0 );    \
} while ( false )

#define TEXT_TAIL() do {\
    instruction_add ( LEAVE, NULL, NULL, 0, 0 );            \
    instruction_add ( PUSH, eax, NULL, 0, 0 );              \
    instruction_add ( SYSCALL, STRDUP("exit"), NULL, 0, 0 );\
} while ( false )

/*
* Finds the variable and creates instructions to make OFFSET(eax)
* point to it. Returns OFFSET
*/
int32_t find_variable(node_t* variable) {
    symbol_t* symbol = variable->entry;
    //Add instruction to make eax be the same as ebp
    instruction_add(MOVE, STRDUP("%ebp"), STRDUP("%eax"), 0, 0);
    if (symbol->depth != depth) {
        int32_t depth_offset = depth-symbol->depth;
        assert(depth_offset > 0);
        for (int32_t i=0; i<depth_offset; i++) {
            //Move one stackframe up. Eax will now be the same as the ebp for
            //that stackframe
            instruction_add(MOVE, STRDUP("(%eax)"), STRDUP("%eax"), 0, 0);
        }
    }
    //Returns the offset from eax the variable is at
    return symbol->stack_offset;
}

int label_id = 0;

char* get_next_label_id() {
    int32_t size = label_id/10+1 + 3;
    char* toprint = malloc(size + 1);
    sprintf(toprint, ".L%d", label_id);
    label_id++;
    return toprint;
}

char* add_colon(char* s) {
    char* new = (char*)malloc(strlen(s)+2);
    strncpy(new, s, strlen(s)+2);
    strncat(new, ":", strlen(s)+2);
    return new;
}

void generate ( FILE *stream, node_t *root ) {
    int elegant_solution;
    if ( root == NULL )
        return;

    switch ( root->type.index )
    {
        case PROGRAM:{
            /* Output the data segment */
            strings_output ( stream );
            instruction_add ( STRING, STRDUP( ".text" ), NULL, 0, 0 );

            RECUR();
            TEXT_HEAD();

            //Finds the label for the first function since we want to call it
            char* first_function = root->children[0]->children[0]->entry->label;

            instruction_add(CALL, STRDUP(first_function), NULL, 0, 0);

            TEXT_TAIL();

            instructions_print ( stream );
            instructions_finalize ();
            break;
        }

        case FUNCTION:{
            /*
            * Function definitions:
            * Set up/take down activation record for the function, return value
            */

            char* label = root->entry->label;

            //Get the label-text
            int32_t size = strlen(label)+3;
            char* s = malloc(size);
            sprintf(s, "_%s:", label);

            instruction_add(STRING, s, NULL, 0, 0 );
            instruction_add(PUSH, ebp, NULL, 0, 0);
            instruction_add(MOVE, esp, ebp, 0, 0);
            depth++;

            //children[2] is the block. We don't want to call generate on the variable that is
            //the function name or the parameter
            generate(stream, root->children[2]);

            depth--;
            //Both FUNCTION and BLOCK add these instructions, in most cases that is not
            //Neceserry, but we need it when the function has no block, that is when there are
            //no curly brackets.
            instruction_add(LEAVE, NULL, NULL, 0, 0);
            instruction_add(RET, NULL, NULL, 0, 0);

            break;
        }

        case BLOCK:
            /*
            * Blocks:
            * Set up/take down activation record, no return value
            */

            //Move down a stack frame
            instruction_add(PUSH, ebp, NULL, 0, 0);
            instruction_add(MOVE, esp, ebp, 0, 0);
            depth++;

            RECUR();

            depth--;
            //Move up a stack frame
            instruction_add(LEAVE, NULL, NULL, 0, 0);

            break;

        case DECLARATION:{
            /*
            * Declarations:
            * Add space on local stack
            */

            node_t* variable_list = root->children[0];
            if (variable_list != NULL) {
                for (uint32_t i=0; i<variable_list->n_children; i++) {
                    //We just push a 0 to the stack for every variable. We also could
                    //increment esp by 4 but this way every variable start out as 0 
                    instruction_add(PUSH, STRDUP("$0"), NULL, 0, 0);
                }
            }

            break;
        }

        case IF_STATEMENT:{
            node_t* expression = root->children[0];
            node_t* statement = root->children[1];
            node_t* else_statement = NULL;
            char* else_label = NULL;
            char* end_label = get_next_label_id();
            if (root->n_children > 2) {
                else_statement = root->children[2];
                else_label = get_next_label_id();
            }
            generate(stream, expression);
           
            instruction_add(POP, STRDUP("%eax"), NULL, 0, 0);
            instruction_add(CMP, STRDUP("$0"), STRDUP("%eax"), 0, 0);
            if (else_statement != NULL) {
                instruction_add(JUMPEQ, else_label, NULL, 0, 0);    
            } else {
                instruction_add(JUMPEQ, end_label, NULL, 0, 0);
            }
            generate(stream, statement);
            if (else_statement != NULL) {
                instruction_add(JUMP, end_label, NULL, 0, 0);    
                instruction_add(STRING, add_colon(else_label), NULL, 0, 0);
                generate(stream, else_statement);
            }
            instruction_add(STRING, add_colon(end_label), NULL, 0, 0);
            break;
        }

        case FOR_STATEMENT:{
            node_t* assignment = root->children[0];
            node_t* expression = root->children[1];
            node_t* statement = root->children[2];
            generate(stream, assignment);

            char* top_label = get_next_label_id();
            char* end_label = get_next_label_id();

            instruction_add(STRING, add_colon(top_label), NULL, 0, 0);
            generate(stream, expression);
            instruction_add(POP, STRDUP("%ebx"), NULL, 0, 0);
            int32_t offset = find_variable(assignment->children[0]);
            instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, offset);
            instruction_add(JUMPEQ, end_label, NULL, 0, 0);

            generate(stream, statement);
            offset = find_variable(assignment->children[0]);
            instruction_add(ADD, STRDUP("$1"), eax, 0, offset);
            instruction_add(JUMP, top_label, NULL, 0, 0);   
            instruction_add(STRING, add_colon(end_label), NULL, 0, 0);
            break;
        }

        case WHILE_STATEMENT:{
            node_t* expression = root->children[0];
            node_t* statement = root->children[1];
            char* top_label = get_next_label_id();
            char* end_label = get_next_label_id();
            instruction_add(STRING, add_colon(top_label), NULL, 0, 0);
            generate(stream, expression);
            instruction_add(POP, STRDUP("%eax"), NULL, 0, 0);
            instruction_add(CMP, STRDUP("$0"), STRDUP("%eax"), 0, 0);
            instruction_add(JUMPEQ, end_label, NULL, 0, 0);

            generate(stream, statement);
            instruction_add(JUMP, top_label, NULL, 0, 0);   
            instruction_add(STRING, add_colon(end_label), NULL, 0, 0);
            break;
        }

        case PRINT_LIST:
            /*
            * Print lists:
            * Emit the list of print items, followed by newline (0x0A)
            */

            RECUR();

            //I had to add a line to symtab.c to add a NEWLINE to the strings. 
            //.NEWLINE is just "\n"
            instruction_add(PUSH, STRDUP("$.NEWLINE"), NULL, 0, 0);
            instruction_add(SYSCALL, STRDUP("printf"), NULL, 0, 0);
            instruction_add(ADD, STRDUP("$4"), esp, 0, 0);


            break;

        case PRINT_ITEM: {
            /*
            * Items in print lists:
            * Determine what kind of value (string literal or expression)
            * and set up a suitable call to printf
            */

            if (root->children[0]->type.index == TEXT) {
                int32_t* index = root->children[0]->data;
                //C er ikke akkurat laget for string-handeling...
                //Lengden av "$STRING%d" + 1. We divide by 10 to get the length of
                //the string that is needed to hold index.
                int32_t size = *index/10+2 + 7;
                //Mallocs a variable to hold the new string
                char* toprint = malloc(size + 1);
                //And finally call sprintf
                sprintf(toprint, "$.STRING%d", *index);

                //And add the instructions to call printf
                instruction_add(PUSH, toprint, NULL, 0, 0);
                instruction_add(SYSCALL, STRDUP("printf"), NULL, 0, 0);
                instruction_add(ADD, STRDUP("$4"), esp, 0, 0);
            } else {
                RECUR();
                //Nå burde riktig tall ligge på stacken
                instruction_add(PUSH, STRDUP("$.INTEGER"), NULL, 0, 0);
                instruction_add(SYSCALL, STRDUP("printf"), NULL, 0, 0);
                //We need to remove the $.INTEGER and the integer that was
                //pushed to the stack
                instruction_add(ADD, STRDUP("$8"), esp, 0, 0);
            }

            

            break;
        }

        case EXPRESSION:{
            /*
            * Expressions:
            * Handle any nested expressions first, then deal with the
            * top of the stack according to the kind of expression
            * (single variables/integers handled in separate switch/cases)
            */

            RECUR();

            if (root->n_children == 2) {
                //The two children have pushed the numbers that we shall 
                //do something with to the stack. We put them in the eax and ebx registers.
                instruction_add(POP, STRDUP("%ebx"), NULL, 0, 0);
                instruction_add(POP, STRDUP("%eax"), NULL, 0, 0);
                //Check the operator and do the correct thing
                if (strcmp(root->data, "+") == 0) {
                    instruction_add(ADD, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                } else if (strcmp(root->data, "-") == 0) {
                    instruction_add(SUB, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                } else if (strcmp(root->data, "*") == 0) {
                    instruction_add(MUL, STRDUP("%ebx"), NULL, 0, 0);
                } else if (strcmp(root->data, "/") == 0) {
                    instruction_add(STRING, STRDUP("\tcdq"), NULL, 0, 0);
                    instruction_add(DIV, STRDUP("%ebx"), NULL, 0, 0);
                } else if (strcmp(root->data, "<") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETL, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, ">") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETG, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, "==") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETE, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, "!=") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETNE, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, "<=") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETLE, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, ">=") == 0) {
                    instruction_add(CMP, STRDUP("%ebx"), STRDUP("%eax"), 0, 0);
                    instruction_add(SETGE, STRDUP("%al"), NULL, 0, 0);
                    instruction_add(CBW, NULL, NULL, 0, 0);
                    instruction_add(CWDE, NULL, NULL, 0, 0);
                } else if (strcmp(root->data, "F") == 0) {
                    //Operator like F means we have a function call
                    //Push the parameters to the stack, that is we just
                    //call generate and that will push the parameters to the stack
                    node_t* expression_list = root->children[1];
                    int32_t count;
                    if (expression_list != NULL) {
                        for (int32_t i=0; i<expression_list->n_children; i++) {
                            generate(stream, expression_list->children[i]);
                        }
                        count = expression_list->n_children*4;
                    } else {
                        count = 0;
                    }
                    char* label = root->children[0]->entry->label;
                    instruction_add(CALL, STRDUP(label), NULL, 0, 0);

                    //We need the number of bytes which is in the count variable.
                    //Time to do string-manipulation in c again
                    int32_t size = count/10+1 + 2;
                    char* s = malloc(size + 1);
                    sprintf(s, "$%d", count);
                    instruction_add(ADD, s, STRDUP("%esp"), 0, 0);
                }
                //In some way all the operators will make the result be in
                //eax. We need to push this to the stack since we are working with
                //the stack.
                instruction_add(PUSH, STRDUP("%eax"), NULL, 0, 0);
            } else if (strcmp(root->data, "-") == 0) {
                //Unary minus. Uses NEG to take the twos complement.
                instruction_add(POP, STRDUP("%eax"), NULL, 0, 0);
                instruction_add(NEG, STRDUP("%eax"), NULL, 0, 0);
                instruction_add(PUSH, STRDUP("%eax"), NULL, 0, 0);
            }



            break;
        }

        case VARIABLE:{
            /*
            * Occurrences of variables: (declarations have their own case)
            * - Find the variable's stack offset
            * - If var is not local, unwind the stack to its correct base
            */

            //We just uses find_variable to find the variable
            int32_t offset = find_variable(root);
            //Now eax is such that the variable is offset from eax.
            instruction_add(PUSH, STRDUP("%eax"), NULL, offset, 0);

            break;
        }

        case INTEGER:{
            /*
            * Integers: constants which can just be put on stack
            */
            //A little string manipulation to get a dollarsign in front of the 
            //number and we can put it on the stack.
            int32_t* data = root->data;
            int32_t size = *data/10+1 + 2;
            char* s = malloc(size + 1);
            sprintf(s, "$%d", *data);
            instruction_add(PUSH, s, NULL, 0, 0);

            break;
        }

        case ASSIGNMENT_STATEMENT:{
            /*
            * Assignments:
            * Right hand side is an expression, find left hand side on stack
            * (unwinding if necessary)
            */

            //Finds the two nodes that makes this assignent. Variable is the left hand
            //side and expression is the right hand side
            node_t* variable = root->children[0];
            node_t* expression = root->children[1];

            //We want right hand side on the stack. Generate does that.
            generate(stream, expression);

            //Finds the variable on a stack frame
            int32_t offset = find_variable(variable);
            //and now we can put the pushed integer in the correct position on
            //the stack
            instruction_add(POP, STRDUP("%eax"), NULL, offset, 0);



            break;
        }

        case RETURN_STATEMENT:
            /*
            * Return statements:
            * Evaluate the expression and put it in EAX
            */
            RECUR();
            instruction_add(POP, STRDUP("%eax"), NULL, 0, 0);
            //Maybe there is a nicer way to do this. We basically need to get out of
            //all blocks we are in and the function and each of these have its own stack-frame
            for (int32_t i = 0; i < depth; i++) {
                instruction_add(LEAVE, NULL, NULL, 0, 0);
            }
            instruction_add(RET, NULL, NULL, 0, 0);
            break;

        default:
            /* Everything else can just continue through the tree */
            RECUR();
            break;
    }
}


/* Provided auxiliaries... */


    static void
instruction_append ( instruction_t *next )
{
    if ( start != NULL )
    {
        last->next = next;
        last = next;
    }
    else
        start = last = next;
}


    static void
instruction_add (
        opcode_t op, char *arg1, char *arg2, int32_t off1, int32_t off2 
        )
{
    instruction_t *i = (instruction_t *) malloc ( sizeof(instruction_t) );
    *i = (instruction_t) { op, {arg1, arg2}, {off1, off2}, NULL };
    instruction_append ( i );
}


    static void
instructions_print ( FILE *stream )
{
    instruction_t *this = start;
    while ( this != NULL )
    {
        switch ( this->opcode )
        {
            case PUSH:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tpushl\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tpushl\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case POP:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tpopl\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tpopl\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case MOVE:
                if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tmovl\t%s,%s\n",
                            this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tmovl\t%d(%s),%s\n",
                            this->offsets[0], this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
                    fprintf ( stream, "\tmovl\t%s,%d(%s)\n",
                            this->operands[0], this->offsets[1], this->operands[1]
                            );
                break;

            case ADD:
                if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\taddl\t%s,%s\n",
                            this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\taddl\t%d(%s),%s\n",
                            this->offsets[0], this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
                    fprintf ( stream, "\taddl\t%s,%d(%s)\n",
                            this->operands[0], this->offsets[1], this->operands[1]
                            );
                break;
            case SUB:
                if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tsubl\t%s,%s\n",
                            this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tsubl\t%d(%s),%s\n",
                            this->offsets[0], this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
                    fprintf ( stream, "\tsubl\t%s,%d(%s)\n",
                            this->operands[0], this->offsets[1], this->operands[1]
                            );
                break;
            case MUL:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\timull\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\timull\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case DIV:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tidivl\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tidivl\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case NEG:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tnegl\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tnegl\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;

            case DECL:
                fprintf ( stream, "\tdecl\t%s\n", this->operands[0] );
                break;
            case CLTD:
                fprintf ( stream, "\tcltd\n" );
                break;
            case CBW:
                fprintf ( stream, "\tcbw\n" );
                break;
            case CWDE:
                fprintf ( stream, "\tcwde\n" );
                break;
            case CMPZERO:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tcmpl\t$0,%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tcmpl\t$0,%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case CMP:
                if ( this->offsets[0] == 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tcmpl\t%s,%s\n",
                            this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] != 0 && this->offsets[1] == 0 )
                    fprintf ( stream, "\tcmpl\t%d(%s),%s\n",
                            this->offsets[0], this->operands[0], this->operands[1]
                            );
                else if ( this->offsets[0] == 0 && this->offsets[1] != 0 )
                    fprintf ( stream, "\tcmpl\t%s,%d(%s)\n",
                            this->operands[0], this->offsets[1], this->operands[1]
                            );
                break;
            case SETL:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsetl\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsetl\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case SETG:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsetg\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsetg\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case SETLE:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsetle\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsetle\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case SETGE:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsetge\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsetge\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case SETE:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsete\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsete\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;
            case SETNE:
                if ( this->offsets[0] == 0 )
                    fprintf ( stream, "\tsetne\t%s\n", this->operands[0] );
                else
                    fprintf ( stream, "\tsetne\t%d(%s)\n",
                            this->offsets[0], this->operands[0]
                            );
                break;

            case CALL: case SYSCALL:
                fprintf ( stream, "\tcall\t" );
                if ( this->opcode == CALL )
                    fputc ( '_', stream );
                fprintf ( stream, "%s\n", this->operands[0] );
                break;
            case LABEL: 
                fprintf ( stream, "_%s:\n", this->operands[0] );
                break;

            case JUMP:
                fprintf ( stream, "\tjmp\t%s\n", this->operands[0] );
                break;
            case JUMPZERO:
                fprintf ( stream, "\tjz\t%s\n", this->operands[0] );
                break;
            case JUMPEQ:
                fprintf ( stream, "\tje\t%s\n", this->operands[0] );
                break;
            case JUMPNONZ:
                fprintf ( stream, "\tjnz\t%s\n", this->operands[0] );
                break;

            case LEAVE: fputs ( "\tleave\n", stream ); break;
            case RET:   fputs ( "\tret\n", stream );   break;

            case STRING:
                        fprintf ( stream, "%s\n", this->operands[0] );
                        break;

            case NIL:
                        break;

            default:
                        fprintf ( stderr, "Error in instruction stream\n" );
                        break;
        }
        this = this->next;
    }
}


    static void
instructions_finalize ( void )
{
    instruction_t *this = start, *next;
    while ( this != NULL )
    {
        next = this->next;
        if ( this->operands[0] != eax && this->operands[0] != ebx &&
                this->operands[0] != ecx && this->operands[0] != edx &&
                this->operands[0] != ebp && this->operands[0] != esp &&
                this->operands[0] != esi && this->operands[0] != edi &&
                this->operands[0] != al && this->operands[0] != bl 
           )
            free ( this->operands[0] );
        free ( this );
        this = next;
    }
}
