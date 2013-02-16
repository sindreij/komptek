#include "rpn.h"
#include <stdio.h>
#include <stdlib.h>

RpnCalc newRpnCalc(){
    //initilize new RpnCalc
    RpnCalc rpn;
    rpn.size = 0;
    rpn.top = 0;
    //We set the stack to NULL now, we are lazy.
    //The stack will be allocated when we first
    //push to it
    rpn.stack = NULL;
    return rpn;
}

void push(RpnCalc* rpn, double n){
    //Push the double n on the stack

    //First we check if the stack need to grow
    if (rpn->top == rpn->size) { 
        //We need to get more memory
        //We add two elements on the stack. It is many more algorithms we can use here,
        //but this is simple and works.
        int newSize = rpn->size + 2;
        //I have now learned that we can use realloc here, and that
        //will make the next 8 lines to one line, but this also work

        //Allocate new memory
        double* newStack = (double*)malloc(sizeof(double)*newSize);
        //Copy the old data to the new stack
        for (int i = 0; i<rpn->size; i++) {
            newStack[i] = rpn->stack[i];
        }
        //Free the old stack (or don't if this is the first time)
        if (rpn->stack != NULL) {
           free(rpn->stack);
           rpn->stack = NULL;
        }
        //Set the variables in the rpn
        rpn->stack = newStack;
        rpn->size = newSize;
    }
    //Add the variable to the top of the stack
    rpn->stack[rpn->top] = n;
    rpn->top++;
}

void performOp(RpnCalc* rpn, char op){
    //Get the two operators (all operations is on two operators)
    double op1 = rpn->stack[rpn->top-1];
    double op2 = rpn->stack[rpn->top-2];
    //And shrink the stack
    rpn->top -= 2;
    //Find the result by what the operator is
    double result;
    switch(op) {
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op2 - op1;
            break;
        case '*':
            result = op1 * op2;
            break;
        case '/':
            result = op2 / op1;
            break;
        default:
            result = 0;
            printf("Kjenner ikke igjen %c", op);
            break;
    }
    //We should maybe have used push() to do this, but this works. Since
    //we have taken two variables from the stack, we know that there is
    //enough space
    //Push the memory to the stack.
    rpn->stack[rpn->top] = result;
    rpn->top++;
}

double peek(RpnCalc* rpn){
    //Return the topmost element on the stack
    return rpn->stack[rpn->top-1];
}
