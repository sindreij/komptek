#include "rpn.h"
#include <stdio.h>
#include <stdlib.h>

RpnCalc newRpnCalc(){
    // TODO initialize new RpnCalc
    RpnCalc rpn;
    rpn.size = 0;
    rpn.top = 0;
    rpn.stack = NULL;
    return rpn;
}

void push(RpnCalc* rpn, double n){
    if (rpn->top == rpn->size) {
        //We need to get more memory
        int newSize = rpn->size + 2;
        double* newStack = (double*)malloc(sizeof(double)*newSize);
        for (int i = 0; i<rpn->size; i++) {
            newStack[i] = rpn->stack[i];
        }
        if (rpn->stack != NULL) {
           free(rpn->stack);
           rpn->stack = NULL;
        }
        rpn->stack = newStack;
        rpn->size = newSize;
    }
    rpn->stack[rpn->top] = n;
    rpn->top++;
}

void performOp(RpnCalc* rpn, char op){
    double op1 = rpn->stack[rpn->top-1];
    double op2 = rpn->stack[rpn->top-2];
    rpn->top -= 2;
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
            printf("Kjenner ikke igjen %c", op);
            break;
    }
    rpn->stack[rpn->top] = result;
    rpn->top++;
}

double peek(RpnCalc* rpn){
    return rpn->stack[rpn->top-1];
}
