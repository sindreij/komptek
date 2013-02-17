#include "tree.h"


#ifdef DUMP_TREES
void
node_print ( FILE *output, node_t *root, uint32_t nesting )
{
    if ( root != NULL )
    {
        fprintf ( output, "%*c%s", nesting, ' ', root->type.text );
        if ( root->type.index == INTEGER )
            fprintf ( output, "(%d)", *((int32_t *)root->data) );
        if ( root->type.index == VARIABLE || root->type.index == EXPRESSION )
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
    free(discard->entry);
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


