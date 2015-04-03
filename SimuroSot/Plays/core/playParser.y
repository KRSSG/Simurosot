%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "playParser.h"

typedef union {
  int   result;
	float number;
  int   role;
  int   coord_sys;
  char  identifier[100];
} yylType;

extern char *yytext;
extern int lineno;

void yyerror(const char *msg);
int yylex(void);

%}


%union {
  int   result;
	float number;
  int   role;
  int   coord_sys;
  char  identifier[100];
};

  /* For printing more specific error messages in cases of syntax errors found */
%error-verbose

  /* Specifying the token names used in the grammar */
%token PLAY
%token APPLICABLE
%token DONE
%token RESULT
%token TIMEOUT
%token FIXEDROLES
%token OROLE
%token ROLE
%token ROLE_ID
%token NONE
%token COORDINATE_SYSTEM
%token ARG_POINT
%token ARG_LINE
%token ARG_REGION_RECT
%token ARG_REGION_CIRC
%token ARG_ANGLE
%token ARG_ROLE_ID
%token NUM
%token IDENTIFIER
%token END

  /* List of Play outcomes */
%token SUCCEEDED
%token COMPLETED
%token ABORTED
%token FAILED

  /* List of coordinate systems */
%token BALL_VELOCITY_REL
%token BALL_VELOCITY_ABS
%token HOME_TEAM_ABS
%token AWAY_TEAM_ABS

  /* Errenous symbol encountered in the .PLAY file */
%token STRAY_SYMBOL

%start prog	/* prog is the start symbol of the grammar */

%% /* Grammar rules follow */

prog:                     PLAY IDENTIFIER statements END
                          ;

statements:               header roles
                          ;

header:                   comp_header opt_header
                          ;
			  
comp_header:              app_header done_header
                          ;
                          
app_header:               app_stm
                        | app_stm app_header;
                        
app_stm:                  APPLICABLE term_list 
                          ;
                          
done_header:              done_stm
                        | done_stm done_header
                          ;
                          
done_stm:                 DONE RESULT term_list
                          ;

term_list:                term term_list
                        | term
                          ;                       
                          
term:                     IDENTIFIER
                        | '!' IDENTIFIER
                          ;                        

opt_header:               timeout_stm orole_stm fixedrole_stm 
                          ;
                          
timeout_stm:              /* epsilon : for usage of system's default timeout settings */
                        | TIMEOUT NUM
                          ;
 
orole_stm:                /* epsilon : for no opponent role specification */
                        | oroles
                          ;
					   
oroles:                   orole oroles
                        | orole
                          ;
                          
orole:                    OROLE ROLE_ID IDENTIFIER
                          ;

fixedrole_stm:            /* epsilon : for dynamic role assignment */
                        | FIXEDROLES role_id_list
                          ;

role_id_list:             ROLE_ID role_id_list
                        | ROLE_ID
                          ;
                          
                          
roles:                    role roles 
                        | role
                          ;
                          
role:                     ROLE ROLE_ID tactic_list NONE
                          ;
                          
tactic_list:              tactic tactic_list 
                        | tactic
                          ;
                          
tactic:                   IDENTIFIER coord_sys '(' param_list ')'
                          ;
                          
coord_sys:                /* epsilon : for default coordinate system */
                        | COORDINATE_SYSTEM
                          ;

param_list:               param ',' param_list
                        | param
                          ;
                          
param:                    /* epsilon : for parameter-less tactics */
                        | region
                        | line
                        | point
                        | angle
                        | role_id
                          ;

region:                   region_rect
                        | region_circ
                          ;
                          
region_rect:              ARG_REGION_RECT '(' '(' NUM ',' NUM ')' ',' NUM ',' NUM ',' NUM ')'
                          ;
                          
region_circ:              ARG_REGION_CIRC '(' '(' NUM ',' NUM ')' ',' NUM ')'
                          ;
                          
line:                     ARG_LINE '(' NUM ',' NUM ')' ',' '(' NUM ',' NUM ')'
                          ;
                          
point:                    ARG_POINT '(' NUM ',' NUM ')'
                          ;
                          
angle:                    ARG_ANGLE NUM
                          ;

role_id:                  ARG_ROLE_ID ROLE_ID

%%

void yyerror(char const *msg) 
{
	fprintf(stderr, "Line %d : %s at %s\n", lineno, msg, yytext);
}

int main()
{
  switch (yyparse()) 
  {
  case 0:	
    printf("\nParsing successful\n"); 
    break;
    
  case 1:	
    printf("\nParsing failed because of invalid input\n"); 
    break;
    	
  case 2: 
    printf("\nParsing failed because of memory exhaustion\n");
  }
  return 0;
}
