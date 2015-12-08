%{
#include <cstring>
#include <cstddef>

#include "../common/Common.h"

#include "SQLStatement.h"
#include "SQLParserException.h"

#include "SQLParserInternals.hpp"

int yyerror(const char *);
int yylex(void);

%}

%option ansi-prototypes noyywrap yy_scan_string

%%

select            return SELECT;
from              return FROM;
insert            return INSERT;
into              return INTO;
values            return VALUES;
create            return CREATE;
table             return TABLE;
"("               return *yytext;
")"               return *yytext;
","               return *yytext;
"*"               return *yytext;
";"               return *yytext;
"int"             return INT;
"double"          return DOUBLE;
"varchar"         return VARCHAR;
"exit"            return EXIT;
"quit"            return EXIT;
[0-9]+            { yylval.string_v = strdup(yytext); return INT_NUM;    }
[0-9]+.[0-9]+     { yylval.string_v = strdup(yytext); return DOUBLE_NUM; }
\"[\sa-z0-9]+\"   { yylval.string_v = strdup(yytext); return WORD;       }
[a-z]+[a-z0-9_]*  { yylval.string_v = strdup(yytext); return IDENTIFIER; }
[ \t]             {}
\r                {}
.                 { yyerror("Unknown Character"); return UNKNOWN_TOKEN; }

%%
