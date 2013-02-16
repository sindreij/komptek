%{
#include "parser.h"
#ifdef DUMP_TOKENS
    #define RETURN(t) do {                                      \
        fprintf ( stderr, "TOKEN ( %d,\t'%s' )\n", t, yytext );    \
        return t;                                               \
    } while ( 0 )
#else
    #define RETURN(t) return t
#endif
%}

%option array
%option noyywrap
%option yylineno

WHITESPACE [\ \t\n]
DIGIT [0-9]
CHAR [^\n"\""]
LETTER [a-zA-Z_]

%%
"FUNC"                          { RETURN(FUNC); }
"PRINT"                         { RETURN(PRINT); }
"RETURN"                        { RETURN(RETURN); }
"CONTINUE"                      { RETURN(CONTINUE); }
"IF"                            { RETURN(IF); }
"THEN"                          { RETURN(THEN); }
"ELSE"                          { RETURN(ELSE); }
"FI"                            { RETURN(FI); }
"WHILE"                         { RETURN(WHILE); }
"DO"                            { RETURN(DO); }
"DONE"                          { RETURN(DONE); }
"VAR"                           { RETURN(VAR); }
"FOR"                           { RETURN(FOR); }
"TO"                            { RETURN(TO); }

":="                            { RETURN(ASSIGN); }

"<="                            { RETURN(LEQUAL); }
">="                            { RETURN(GEQUAL); }
"=="                            { RETURN(EQUAL); }
"!="                            { RETURN(NEQUAL); }

{DIGIT}+                        { yylval = atoi(yytext); RETURN(NUMBER); }
"\""({CHAR}|"\\\"")*"\""        { RETURN(STRING); }
{LETTER}({LETTER}|{DIGIT})*     { RETURN(IDENTIFIER); }

"//"[^\n]*\n                    {}
{WHITESPACE}                    {}

.                               { RETURN(yytext[0]); }
%%
