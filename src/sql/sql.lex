%{
#include <cstring>
#include <cstddef>

#include "SQLStatement.h"
#include "SQLParserException.h"

#include "SQLParserInternals.hpp"

int yyerror(const char *);
int yylex(void);

%}

%option ansi-prototypes noyywrap yy_scan_string

%%

select       return SELECT;
from         return FROM;
insert       return INSERT;
into         return INTO;
values       return VALUES;
create       return CREATE;
table        return TABLE;
"("          return *yytext;
")"          return *yytext;
","          return *yytext;
"*"          return *yytext;
";"          return *yytext;
"int"        return INT;
"float"      return FLOAT;
"varchar"    return VARCHAR;
"exit"       return EXIT;
"quit"       return EXIT;
[a-z0-9]+    { yylval.string_v = strdup(yytext); return IDENTIFIER; }
[ \t]        {}
\r           {}
.            { yyerror("Unknown Character"); return UNKNOWN_TOKEN; }

%%