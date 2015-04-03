/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PLAY = 258,
     APPLICABLE = 259,
     DONE = 260,
     RESULT = 261,
     TIMEOUT = 262,
     FIXEDROLES = 263,
     OROLE = 264,
     ROLE = 265,
     ROLE_ID = 266,
     NONE = 267,
     COORDINATE_SYSTEM = 268,
     ARG_POINT = 269,
     ARG_LINE = 270,
     ARG_REGION_RECT = 271,
     ARG_REGION_CIRC = 272,
     ARG_ANGLE = 273,
     ARG_ROLE_ID = 274,
     NUM = 275,
     IDENTIFIER = 276,
     END = 277,
     SUCCEEDED = 278,
     COMPLETED = 279,
     ABORTED = 280,
     FAILED = 281,
     BALL_VELOCITY_REL = 282,
     BALL_VELOCITY_ABS = 283,
     HOME_TEAM_ABS = 284,
     AWAY_TEAM_ABS = 285,
     STRAY_SYMBOL = 286
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 2068 of yacc.c  */
#line 24 "playParser.y"

  int   result;
	float number;
  int   role;
  int   coord_sys;
  char  identifier[100];



/* Line 2068 of yacc.c  */
#line 91 "playParser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


