/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_C99_TAB_HPP_INCLUDED
# define YY_YY_C99_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IDENTIFIER = 258,              /* IDENTIFIER  */
    STRING_LITERAL = 259,          /* STRING_LITERAL  */
    I_CONSTANT = 260,              /* I_CONSTANT  */
    F_CONSTANT = 261,              /* F_CONSTANT  */
    CHAR = 262,                    /* CHAR  */
    INT = 263,                     /* INT  */
    FLOAT = 264,                   /* FLOAT  */
    DOUBLE = 265,                  /* DOUBLE  */
    VOID = 266,                    /* VOID  */
    IF = 267,                      /* IF  */
    ELSE = 268,                    /* ELSE  */
    WHILE = 269,                   /* WHILE  */
    DO = 270,                      /* DO  */
    FOR = 271,                     /* FOR  */
    RETURN = 272,                  /* RETURN  */
    BREAK = 273,                   /* BREAK  */
    CONTINUE = 274,                /* CONTINUE  */
    LE_OP = 275,                   /* LE_OP  */
    GE_OP = 276,                   /* GE_OP  */
    EQ_OP = 277,                   /* EQ_OP  */
    NE_OP = 278,                   /* NE_OP  */
    AND_OP = 279,                  /* AND_OP  */
    OR_OP = 280,                   /* OR_OP  */
    LOWER_THAN_ELSE = 281          /* LOWER_THAN_ELSE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 22 "c99.y"
<<<<<<< HEAD

    int intval;
    double fval;
    char *str_val;
    ASTNode *ast;  /* 统一使用 ASTNode* */
    struct {       /* 专门处理列表的临时结构 */
        ASTNode **items;
        int count;
    } list;
=======

    int intval;
    double fval;
    char *str_val;
    ASTNode *ast;  /* 统一使用 ASTNode* */
    struct {       /* 专门处理列表的临时结构 */
        ASTNode **items;
        int count;
    } list;
>>>>>>> origin/main

#line 101 "c99.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_C99_TAB_HPP_INCLUDED  */
