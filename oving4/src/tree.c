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
    //free(discard->entry);
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

void collect_strings(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (node->type.index == TEXT) {
        char* text = node->data;
        int32_t* index = malloc(sizeof(int32_t));
        *index = strings_add(text);
        node->data = index;
    }

    for (uint32_t i=0; i < node->n_children; i++) {
        collect_strings(node->children[i]);
    }
}

void collect_functions(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (node->type.index == FUNCTION) {
        symbol_t* symbol = (symbol_t*) malloc(sizeof(symbol_t));
        symbol->stack_offset = 0;
        symbol->label = STRDUP(node->children[0]->data);
        symbol_insert(symbol->label, symbol);
    } else {
        for (uint32_t i=0; i < node->n_children; i++) {
            collect_functions(node->children[i]);
        }
    }
}

void collect_names(node_t *node) {
    if (node == NULL) {
        return;
    }
    /*if (node->type.index == FUNCTION) {
        scope_add();
        node_t* variable_list = node->children[1];
        for (uint32_t i=0; i<variable_list->n_children; i++) {
            symbol_t* symbol = (symbol_t*) malloc(sizeof(symbol_t));
            //TODO
            symbol->stack_offset = 0;
            symbol->label = NULL;

        }
    }*/
}

void bind_names ( node_t *root ){
    collect_strings(root);
    //root-scope
    scope_add();
    collect_functions(root);

    collect_names(root);
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
