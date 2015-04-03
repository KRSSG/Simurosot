%top {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "playParser.h"
#define YY_DECL int yylex(void)

int lineno = 1;

}

%option noyywrap
%x CMNT

%%

"/*"         	                        {BEGIN CMNT;}
	/* Rejecting everything except a newline character that comes within double inverted quotes inside a comment */
<CMNT>"\"".*"\""
	/* Rejecting everything that comes within a comment */
<CMNT>.

<CMNT>\n

	/* End of comment found */
<CMNT>"*/"   	                        {BEGIN INITIAL;}

  /* Rejecting single line comments */
"//".*\n

  /* Discarding the white-space characters */
\n                                    {lineno++;}
\t                                    
\r                                    
" "                                   

"PLAY"                                {return PLAY;}

"APPLICABLE"                          {return APPLICABLE;}

"DONE"                                {return DONE;}

"SUCCEEDED"                           {yylval.result = SUCCEEDED; return RESULT;}
"COMPLETED"                           {yylval.result = COMPLETED; return RESULT;}
"ABORTED"                             {yylval.result = ABORTED;   return RESULT;}
"FAILED"                              {yylval.result = FAILED;    return RESULT;}

"!"                                   {return '!';}

"TIMEOUT"                             {return TIMEOUT;}

"FIXEDROLES"                          {return FIXEDROLES;}

"OROLE"                               {return OROLE;}

"ROLE"                                {return ROLE;}

  /* List of role IDs */
"R0"                                  {yylval.role = 0; return ROLE_ID;}
"R1"                                  {yylval.role = 1; return ROLE_ID;}
"R2"                                  {yylval.role = 2; return ROLE_ID;}
"R3"                                  {yylval.role = 3; return ROLE_ID;}
"R4"                                  {yylval.role = 4; return ROLE_ID;}

"NONE"                                {return NONE;}
"("                                   {return '(';}
")"                                   {return ')';}
","                                   {return ',';}

  /* Coordinate system used in the parameters for tactics */
"BVR"                                 {yylval.coord_sys = BALL_VELOCITY_REL; return COORDINATE_SYSTEM;}
"BVA"                                 {yylval.coord_sys = BALL_VELOCITY_ABS; return COORDINATE_SYSTEM;}
"HTA"                                 {yylval.coord_sys = HOME_TEAM_ABS;     return COORDINATE_SYSTEM;}
"ATA"                                 {yylval.coord_sys = AWAY_TEAM_ABS;     return COORDINATE_SYSTEM;}
  
  /* Types of Tactics Parameters */
"P"                                   {return ARG_POINT;}
"L"                                   {return ARG_LINE;}
"RR"                                  {return ARG_REGION_RECT;}
"RC"                                  {return ARG_REGION_CIRC;}
"A"                                   {return ARG_ANGLE;}
"R"                                   {return ARG_ROLE_ID;}

"END"                                 {return END;}

([+-]?[0-9]+"."?[0-9]*)               {yylval.number = atof(yytext);      return NUM;}
([a-zA-Z][a-zA-Z0-9_]*)               {strcpy(yylval.identifier, yytext); return IDENTIFIER;}

.                                     {return STRAY_SYMBOL;}

