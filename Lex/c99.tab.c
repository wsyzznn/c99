/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30704

/* Bison version string.  */
#define YYBISON_VERSION "3.7.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "c99.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* 声明外部接口 */
extern int yylex();
extern int yylineno;
extern char* yytext;

/* 错误处理函数 */
void yyerror(const char *s) {
    fprintf(stderr, "Syntax Error at line %d: %s near '%s'\n", yylineno, s, yytext);
}

/* 全局根节点，供语义分析和中间代码组使用 */
ASTNode *g_ast_root = NULL;

#line 91 "c99.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "c99.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_STRING_LITERAL = 4,             /* STRING_LITERAL  */
  YYSYMBOL_I_CONSTANT = 5,                 /* I_CONSTANT  */
  YYSYMBOL_F_CONSTANT = 6,                 /* F_CONSTANT  */
  YYSYMBOL_CHAR = 7,                       /* CHAR  */
  YYSYMBOL_INT = 8,                        /* INT  */
  YYSYMBOL_FLOAT = 9,                      /* FLOAT  */
  YYSYMBOL_DOUBLE = 10,                    /* DOUBLE  */
  YYSYMBOL_VOID = 11,                      /* VOID  */
  YYSYMBOL_IF = 12,                        /* IF  */
  YYSYMBOL_ELSE = 13,                      /* ELSE  */
  YYSYMBOL_WHILE = 14,                     /* WHILE  */
  YYSYMBOL_DO = 15,                        /* DO  */
  YYSYMBOL_FOR = 16,                       /* FOR  */
  YYSYMBOL_RETURN = 17,                    /* RETURN  */
  YYSYMBOL_BREAK = 18,                     /* BREAK  */
  YYSYMBOL_CONTINUE = 19,                  /* CONTINUE  */
  YYSYMBOL_LE_OP = 20,                     /* LE_OP  */
  YYSYMBOL_GE_OP = 21,                     /* GE_OP  */
  YYSYMBOL_EQ_OP = 22,                     /* EQ_OP  */
  YYSYMBOL_NE_OP = 23,                     /* NE_OP  */
  YYSYMBOL_AND_OP = 24,                    /* AND_OP  */
  YYSYMBOL_OR_OP = 25,                     /* OR_OP  */
  YYSYMBOL_LOWER_THAN_ELSE = 26,           /* LOWER_THAN_ELSE  */
  YYSYMBOL_27_ = 27,                       /* '='  */
  YYSYMBOL_28_ = 28,                       /* '<'  */
  YYSYMBOL_29_ = 29,                       /* '>'  */
  YYSYMBOL_30_ = 30,                       /* '+'  */
  YYSYMBOL_31_ = 31,                       /* '-'  */
  YYSYMBOL_32_ = 32,                       /* '*'  */
  YYSYMBOL_33_ = 33,                       /* '/'  */
  YYSYMBOL_34_ = 34,                       /* '%'  */
  YYSYMBOL_35_ = 35,                       /* '('  */
  YYSYMBOL_36_ = 36,                       /* ')'  */
  YYSYMBOL_37_ = 37,                       /* ','  */
  YYSYMBOL_38_ = 38,                       /* ';'  */
  YYSYMBOL_39_ = 39,                       /* '{'  */
  YYSYMBOL_40_ = 40,                       /* '}'  */
  YYSYMBOL_YYACCEPT = 41,                  /* $accept  */
  YYSYMBOL_primary_expression = 42,        /* primary_expression  */
  YYSYMBOL_postfix_expression = 43,        /* postfix_expression  */
  YYSYMBOL_argument_expression_list = 44,  /* argument_expression_list  */
  YYSYMBOL_unary_expression = 45,          /* unary_expression  */
  YYSYMBOL_cast_expression = 46,           /* cast_expression  */
  YYSYMBOL_multiplicative_expression = 47, /* multiplicative_expression  */
  YYSYMBOL_additive_expression = 48,       /* additive_expression  */
  YYSYMBOL_relational_expression = 49,     /* relational_expression  */
  YYSYMBOL_equality_expression = 50,       /* equality_expression  */
  YYSYMBOL_logical_and_expression = 51,    /* logical_and_expression  */
  YYSYMBOL_logical_or_expression = 52,     /* logical_or_expression  */
  YYSYMBOL_assignment_expression = 53,     /* assignment_expression  */
  YYSYMBOL_expression = 54,                /* expression  */
  YYSYMBOL_declaration = 55,               /* declaration  */
  YYSYMBOL_declaration_specifiers = 56,    /* declaration_specifiers  */
  YYSYMBOL_type_specifier = 57,            /* type_specifier  */
  YYSYMBOL_init_declarator_list = 58,      /* init_declarator_list  */
  YYSYMBOL_declarator = 59,                /* declarator  */
  YYSYMBOL_direct_declarator = 60,         /* direct_declarator  */
  YYSYMBOL_parameter_list = 61,            /* parameter_list  */
  YYSYMBOL_statement = 62,                 /* statement  */
  YYSYMBOL_compound_statement = 63,        /* compound_statement  */
  YYSYMBOL_block_item_list = 64,           /* block_item_list  */
  YYSYMBOL_block_item = 65,                /* block_item  */
  YYSYMBOL_expression_statement = 66,      /* expression_statement  */
  YYSYMBOL_selection_statement = 67,       /* selection_statement  */
  YYSYMBOL_iteration_statement = 68,       /* iteration_statement  */
  YYSYMBOL_jump_statement = 69,            /* jump_statement  */
  YYSYMBOL_translation_unit = 70,          /* translation_unit  */
  YYSYMBOL_external_declaration = 71,      /* external_declaration  */
  YYSYMBOL_function_definition = 72        /* function_definition  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  14
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   118

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  41
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  103

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    34,     2,     2,
      35,    36,    32,    30,    37,    31,     2,    33,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    38,
      28,    27,    29,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    39,     2,    40,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    72,    72,    73,    74,    75,    79,    80,    81,    85,
      88,    95,    96,   100,   104,   105,   106,   110,   111,   112,
     116,   117,   118,   122,   123,   127,   128,   132,   133,   137,
     138,   142,   148,   160,   169,   170,   171,   175,   185,   189,
     190,   202,   203,   212,   213,   214,   215,   216,   220,   221,
     225,   228,   235,   236,   240,   241,   245,   246,   250,   254,
     258,   264,   274,   275,   279
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER",
  "STRING_LITERAL", "I_CONSTANT", "F_CONSTANT", "CHAR", "INT", "FLOAT",
  "DOUBLE", "VOID", "IF", "ELSE", "WHILE", "DO", "FOR", "RETURN", "BREAK",
  "CONTINUE", "LE_OP", "GE_OP", "EQ_OP", "NE_OP", "AND_OP", "OR_OP",
  "LOWER_THAN_ELSE", "'='", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'('", "')'", "','", "';'", "'{'", "'}'", "$accept",
  "primary_expression", "postfix_expression", "argument_expression_list",
  "unary_expression", "cast_expression", "multiplicative_expression",
  "additive_expression", "relational_expression", "equality_expression",
  "logical_and_expression", "logical_or_expression",
  "assignment_expression", "expression", "declaration",
  "declaration_specifiers", "type_specifier", "init_declarator_list",
  "declarator", "direct_declarator", "parameter_list", "statement",
  "compound_statement", "block_item_list", "block_item",
  "expression_statement", "selection_statement", "iteration_statement",
  "jump_statement", "translation_unit", "external_declaration",
  "function_definition", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,    61,    60,    62,
      43,    45,    42,    47,    37,    40,    41,    44,    59,   123,
     125
};
#endif

#define YYPACT_NINF (-84)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       8,   -84,   -84,   -84,   -84,    -2,   -84,     3,   -84,   -84,
     -84,   -33,   -31,   -20,   -84,   -84,    17,   -84,    24,   -84,
     -84,   -84,   -14,   -11,     1,    -8,     1,     1,   -84,   -84,
     -84,    27,     6,   -84,    18,    23,   -18,    37,    41,    51,
     -84,    47,   -84,    -2,   -84,   -84,    55,   -84,   -84,   -84,
     -84,   -84,   -84,    61,     1,     1,    63,   -84,   -84,   -84,
      62,    75,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,   -84,   -84,   -84,   -84,   -84,    66,    69,   -84,
     -84,   -84,    38,   -84,   -84,   -84,   -84,    18,    18,    23,
      23,   -18,    37,    41,    65,    65,   -84,     1,    86,   -84,
     -84,    65,   -84
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    35,    36,    34,    63,     0,    33,     0,    61,    62,
      39,     0,    37,    38,     1,    32,     0,    64,    41,     2,
       3,     4,     0,     0,     0,     0,     0,     0,    54,    48,
       6,    11,    13,    14,    17,    20,    23,    25,    27,    29,
      31,     0,    52,     0,    53,    43,     0,    50,    44,    45,
      46,    47,    42,     0,     0,     0,     0,    60,    13,    12,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    37,    49,    51,    40,     0,     0,    59,
       5,     7,     0,     9,    30,    15,    16,    18,    19,    22,
      21,    24,    26,    28,     0,     0,     8,     0,    56,    58,
      10,     0,    57
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -84,   -84,   -84,   -84,   -26,   -17,    26,    21,    39,    42,
      36,   -84,   -48,    60,   109,   113,   -84,   -84,    73,   -84,
     -84,   -83,   105,   -84,    72,   -84,   -84,   -84,   -84,   -84,
     -84,   -84
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    30,    31,    82,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,     6,    11,    12,    13,
      53,    44,    45,    46,    47,    48,    49,    50,    51,     7,
       8,     9
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      58,    10,    67,    14,    19,    15,    20,    21,    16,    59,
      68,    98,    99,    83,    84,    18,     1,     2,   102,     3,
      19,    54,    20,    21,    55,     1,     2,    52,     3,    22,
      57,    23,    26,    62,    24,    25,    27,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    85,    86,    26,   100,
      63,    64,    27,    65,    66,    28,    16,    29,    19,    69,
      20,    21,    61,     1,     2,    70,     3,    22,    19,    23,
      20,    21,    24,    25,    96,    97,    71,    22,    19,    23,
      20,    21,    24,    25,    56,    72,    26,    60,    89,    90,
      27,    87,    88,    28,    16,    74,    26,    76,    80,   101,
      27,    79,    94,    28,    16,    95,    26,    93,    91,     4,
      27,    81,    92,     5,    77,    78,    73,    17,    75
};

static const yytype_int8 yycheck[] =
{
      26,     3,    20,     0,     3,    38,     5,     6,    39,    26,
      28,    94,    95,    61,    62,    35,     8,     9,   101,    11,
       3,    35,     5,     6,    35,     8,     9,     3,    11,    12,
      38,    14,    31,    27,    17,    18,    35,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    63,    64,    31,    97,
      32,    33,    35,    30,    31,    38,    39,    40,     3,    22,
       5,     6,    35,     8,     9,    24,    11,    12,     3,    14,
       5,     6,    17,    18,    36,    37,    25,    12,     3,    14,
       5,     6,    17,    18,    24,    38,    31,    27,    67,    68,
      35,    65,    66,    38,    39,    40,    31,    36,    36,    13,
      35,    38,    36,    38,    39,    36,    31,    71,    69,     0,
      35,    36,    70,     0,    54,    55,    43,    12,    46
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     8,     9,    11,    55,    56,    57,    70,    71,    72,
       3,    58,    59,    60,     0,    38,    39,    63,    35,     3,
       5,     6,    12,    14,    17,    18,    31,    35,    38,    40,
      42,    43,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    62,    63,    64,    65,    66,    67,
      68,    69,     3,    61,    35,    35,    54,    38,    45,    46,
      54,    35,    27,    32,    33,    30,    31,    20,    28,    22,
      24,    25,    38,    59,    40,    65,    36,    54,    54,    38,
      36,    36,    44,    53,    53,    46,    46,    47,    47,    48,
      48,    49,    50,    51,    36,    36,    36,    37,    62,    62,
      53,    13,    62
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    41,    42,    42,    42,    42,    43,    43,    43,    44,
      44,    45,    45,    46,    47,    47,    47,    48,    48,    48,
      49,    49,    49,    50,    50,    51,    51,    52,    52,    53,
      53,    54,    55,    56,    57,    57,    57,    58,    59,    60,
      60,    61,    61,    62,    62,    62,    62,    62,    63,    63,
      64,    64,    65,    65,    66,    66,    67,    67,    68,    69,
      69,    70,    71,    71,    72
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     1,     3,     1,     3,     4,     1,
       3,     1,     2,     1,     1,     3,     3,     1,     3,     3,
       1,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       4,     0,     1,     1,     1,     1,     1,     1,     2,     3,
       1,     2,     1,     1,     1,     2,     5,     7,     5,     3,
       2,     1,     1,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
# ifndef YY_LOCATION_PRINT
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yykind < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yykind], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* primary_expression: IDENTIFIER  */
#line 72 "c99.y"
                 { (yyval.ast) = ast_var((yyvsp[0].str_val)); }
#line 1229 "c99.tab.c"
    break;

  case 3: /* primary_expression: I_CONSTANT  */
#line 73 "c99.y"
                 { (yyval.ast) = ast_int((yyvsp[0].intval)); }
#line 1235 "c99.tab.c"
    break;

  case 4: /* primary_expression: F_CONSTANT  */
#line 74 "c99.y"
                 { (yyval.ast) = ast_double((yyvsp[0].fval)); }
#line 1241 "c99.tab.c"
    break;

  case 5: /* primary_expression: '(' expression ')'  */
#line 75 "c99.y"
                         { (yyval.ast) = (yyvsp[-1].ast); }
#line 1247 "c99.tab.c"
    break;

  case 6: /* postfix_expression: primary_expression  */
#line 79 "c99.y"
                         { (yyval.ast) = (yyvsp[0].ast); }
#line 1253 "c99.tab.c"
    break;

  case 7: /* postfix_expression: postfix_expression '(' ')'  */
#line 80 "c99.y"
                                 { (yyval.ast) = ast_call((yyvsp[-2].ast)->varname, NULL, 0); }
#line 1259 "c99.tab.c"
    break;

  case 8: /* postfix_expression: postfix_expression '(' argument_expression_list ')'  */
#line 81 "c99.y"
                                                          { (yyval.ast) = ast_call((yyvsp[-3].ast)->varname, (yyvsp[-1].list).items, (yyvsp[-1].list).count); }
#line 1265 "c99.tab.c"
    break;

  case 9: /* argument_expression_list: assignment_expression  */
#line 85 "c99.y"
                            { 
        (yyval.list).items = (ASTNode**)malloc(sizeof(ASTNode*)); (yyval.list).items[0] = (yyvsp[0].ast); (yyval.list).count = 1; 
    }
#line 1273 "c99.tab.c"
    break;

  case 10: /* argument_expression_list: argument_expression_list ',' assignment_expression  */
#line 88 "c99.y"
                                                         {
        (yyvsp[-2].list).items = (ASTNode**)realloc((yyvsp[-2].list).items, sizeof(ASTNode*) * ((yyvsp[-2].list).count + 1));
        (yyvsp[-2].list).items[(yyvsp[-2].list).count++] = (yyvsp[0].ast); (yyval.list) = (yyvsp[-2].list);
    }
#line 1282 "c99.tab.c"
    break;

  case 11: /* unary_expression: postfix_expression  */
#line 95 "c99.y"
                         { (yyval.ast) = (yyvsp[0].ast); }
#line 1288 "c99.tab.c"
    break;

  case 12: /* unary_expression: '-' cast_expression  */
#line 96 "c99.y"
                          { (yyval.ast) = ast_binop('-', ast_int(0), (yyvsp[0].ast)); /* 简易处理负数 */ }
#line 1294 "c99.tab.c"
    break;

  case 13: /* cast_expression: unary_expression  */
#line 100 "c99.y"
                       { (yyval.ast) = (yyvsp[0].ast); }
#line 1300 "c99.tab.c"
    break;

  case 14: /* multiplicative_expression: cast_expression  */
#line 104 "c99.y"
                      { (yyval.ast) = (yyvsp[0].ast); }
#line 1306 "c99.tab.c"
    break;

  case 15: /* multiplicative_expression: multiplicative_expression '*' cast_expression  */
#line 105 "c99.y"
                                                    { (yyval.ast) = ast_binop('*', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1312 "c99.tab.c"
    break;

  case 16: /* multiplicative_expression: multiplicative_expression '/' cast_expression  */
#line 106 "c99.y"
                                                    { (yyval.ast) = ast_binop('/', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1318 "c99.tab.c"
    break;

  case 17: /* additive_expression: multiplicative_expression  */
#line 110 "c99.y"
                                { (yyval.ast) = (yyvsp[0].ast); }
#line 1324 "c99.tab.c"
    break;

  case 18: /* additive_expression: additive_expression '+' multiplicative_expression  */
#line 111 "c99.y"
                                                        { (yyval.ast) = ast_binop('+', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1330 "c99.tab.c"
    break;

  case 19: /* additive_expression: additive_expression '-' multiplicative_expression  */
#line 112 "c99.y"
                                                        { (yyval.ast) = ast_binop('-', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1336 "c99.tab.c"
    break;

  case 20: /* relational_expression: additive_expression  */
#line 116 "c99.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 1342 "c99.tab.c"
    break;

  case 21: /* relational_expression: relational_expression '<' additive_expression  */
#line 117 "c99.y"
                                                    { (yyval.ast) = ast_binop('<', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1348 "c99.tab.c"
    break;

  case 22: /* relational_expression: relational_expression LE_OP additive_expression  */
#line 118 "c99.y"
                                                      { (yyval.ast) = ast_binop('l', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1354 "c99.tab.c"
    break;

  case 23: /* equality_expression: relational_expression  */
#line 122 "c99.y"
                            { (yyval.ast) = (yyvsp[0].ast); }
#line 1360 "c99.tab.c"
    break;

  case 24: /* equality_expression: equality_expression EQ_OP relational_expression  */
#line 123 "c99.y"
                                                      { (yyval.ast) = ast_binop('e', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1366 "c99.tab.c"
    break;

  case 25: /* logical_and_expression: equality_expression  */
#line 127 "c99.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 1372 "c99.tab.c"
    break;

  case 26: /* logical_and_expression: logical_and_expression AND_OP equality_expression  */
#line 128 "c99.y"
                                                        { (yyval.ast) = ast_binop('&', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1378 "c99.tab.c"
    break;

  case 27: /* logical_or_expression: logical_and_expression  */
#line 132 "c99.y"
                             { (yyval.ast) = (yyvsp[0].ast); }
#line 1384 "c99.tab.c"
    break;

  case 28: /* logical_or_expression: logical_or_expression OR_OP logical_and_expression  */
#line 133 "c99.y"
                                                         { (yyval.ast) = ast_binop('|', (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1390 "c99.tab.c"
    break;

  case 29: /* assignment_expression: logical_or_expression  */
#line 137 "c99.y"
                            { (yyval.ast) = (yyvsp[0].ast); }
#line 1396 "c99.tab.c"
    break;

  case 30: /* assignment_expression: unary_expression '=' assignment_expression  */
#line 138 "c99.y"
                                                 { (yyval.ast) = ast_assign((yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1402 "c99.tab.c"
    break;

  case 31: /* expression: assignment_expression  */
#line 142 "c99.y"
                            { (yyval.ast) = (yyvsp[0].ast); }
#line 1408 "c99.tab.c"
    break;

  case 32: /* declaration: declaration_specifiers init_declarator_list ';'  */
#line 148 "c99.y"
                                                      {
        /* 对应你的 AST_DECLARATION: specs, scount, inits, icount */
        (yyval.ast) = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.ast)->type = AST_DECLARATION;
        (yyval.ast)->declaration.specs = (yyvsp[-2].ast)->ds.specs;
        (yyval.ast)->declaration.scount = (yyvsp[-2].ast)->ds.scount;
        (yyval.ast)->declaration.inits = (yyvsp[-1].list).items;
        (yyval.ast)->declaration.icount = (yyvsp[-1].list).count;
    }
#line 1422 "c99.tab.c"
    break;

  case 33: /* declaration_specifiers: type_specifier  */
#line 160 "c99.y"
                     { 
        ASTNode **a = (ASTNode**)malloc(sizeof(ASTNode*)); a[0] = (yyvsp[0].ast);
        (yyval.ast) = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.ast)->type = AST_DECL_SPEC;
        (yyval.ast)->ds.specs = a; (yyval.ast)->ds.scount = 1;
    }
#line 1433 "c99.tab.c"
    break;

  case 34: /* type_specifier: VOID  */
#line 169 "c99.y"
           { (yyval.ast) = ast_var("void"); }
#line 1439 "c99.tab.c"
    break;

  case 35: /* type_specifier: INT  */
#line 170 "c99.y"
           { (yyval.ast) = ast_var("int"); }
#line 1445 "c99.tab.c"
    break;

  case 36: /* type_specifier: FLOAT  */
#line 171 "c99.y"
            { (yyval.ast) = ast_var("float"); }
#line 1451 "c99.tab.c"
    break;

  case 37: /* init_declarator_list: declarator  */
#line 175 "c99.y"
                 { 
        (yyval.list).items = (ASTNode**)malloc(sizeof(ASTNode*)); 
        (yyval.list).items[0] = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.list).items[0]->type = AST_INIT_DECL;
        (yyval.list).items[0]->id.declr = (yyvsp[0].ast); (yyval.list).items[0]->id.init = NULL;
        (yyval.list).count = 1; 
    }
#line 1463 "c99.tab.c"
    break;

  case 38: /* declarator: direct_declarator  */
#line 185 "c99.y"
                        { (yyval.ast) = (yyvsp[0].ast); }
#line 1469 "c99.tab.c"
    break;

  case 39: /* direct_declarator: IDENTIFIER  */
#line 189 "c99.y"
                 { (yyval.ast) = ast_var((yyvsp[0].str_val)); }
#line 1475 "c99.tab.c"
    break;

  case 40: /* direct_declarator: direct_declarator '(' parameter_list ')'  */
#line 190 "c99.y"
                                               { 
        (yyval.ast) = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.ast)->type = AST_FUNC_TYPE;
        (yyval.ast)->ft.name = (yyvsp[-3].ast)->varname;
        (yyval.ast)->ft.params = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.ast)->ft.params->type = AST_PARAM_LIST;
        (yyval.ast)->ft.params->pl.params = (yyvsp[-1].list).items;
        (yyval.ast)->ft.params->pl.pcount = (yyvsp[-1].list).count;
    }
#line 1489 "c99.tab.c"
    break;

  case 41: /* parameter_list: %empty  */
#line 202 "c99.y"
                  { (yyval.list).items = NULL; (yyval.list).count = 0; }
#line 1495 "c99.tab.c"
    break;

  case 42: /* parameter_list: IDENTIFIER  */
#line 203 "c99.y"
                 { /* 简易处理参数 */
        (yyval.list).items = (ASTNode**)malloc(sizeof(ASTNode*)); 
        (yyval.list).items[0] = ast_var((yyvsp[0].str_val)); (yyval.list).count = 1;
    }
#line 1504 "c99.tab.c"
    break;

  case 43: /* statement: compound_statement  */
#line 212 "c99.y"
                         { (yyval.ast) = (yyvsp[0].ast); }
#line 1510 "c99.tab.c"
    break;

  case 44: /* statement: expression_statement  */
#line 213 "c99.y"
                           { (yyval.ast) = (yyvsp[0].ast); }
#line 1516 "c99.tab.c"
    break;

  case 45: /* statement: selection_statement  */
#line 214 "c99.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 1522 "c99.tab.c"
    break;

  case 46: /* statement: iteration_statement  */
#line 215 "c99.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 1528 "c99.tab.c"
    break;

  case 47: /* statement: jump_statement  */
#line 216 "c99.y"
                     { (yyval.ast) = (yyvsp[0].ast); }
#line 1534 "c99.tab.c"
    break;

  case 48: /* compound_statement: '{' '}'  */
#line 220 "c99.y"
              { (yyval.ast) = ast_compound(NULL, 0); }
#line 1540 "c99.tab.c"
    break;

  case 49: /* compound_statement: '{' block_item_list '}'  */
#line 221 "c99.y"
                              { (yyval.ast) = ast_compound((yyvsp[-1].list).items, (yyvsp[-1].list).count); }
#line 1546 "c99.tab.c"
    break;

  case 50: /* block_item_list: block_item  */
#line 225 "c99.y"
                 { 
        (yyval.list).items = (ASTNode**)malloc(sizeof(ASTNode*)); (yyval.list).items[0] = (yyvsp[0].ast); (yyval.list).count = 1; 
    }
#line 1554 "c99.tab.c"
    break;

  case 51: /* block_item_list: block_item_list block_item  */
#line 228 "c99.y"
                                 {
        (yyvsp[-1].list).items = (ASTNode**)realloc((yyvsp[-1].list).items, sizeof(ASTNode*) * ((yyvsp[-1].list).count + 1));
        (yyvsp[-1].list).items[(yyvsp[-1].list).count++] = (yyvsp[0].ast); (yyval.list) = (yyvsp[-1].list);
    }
#line 1563 "c99.tab.c"
    break;

  case 52: /* block_item: declaration  */
#line 235 "c99.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 1569 "c99.tab.c"
    break;

  case 53: /* block_item: statement  */
#line 236 "c99.y"
                { (yyval.ast) = (yyvsp[0].ast); }
#line 1575 "c99.tab.c"
    break;

  case 54: /* expression_statement: ';'  */
#line 240 "c99.y"
          { (yyval.ast) = NULL; }
#line 1581 "c99.tab.c"
    break;

  case 55: /* expression_statement: expression ';'  */
#line 241 "c99.y"
                     { (yyval.ast) = (yyvsp[-1].ast); }
#line 1587 "c99.tab.c"
    break;

  case 56: /* selection_statement: IF '(' expression ')' statement  */
#line 245 "c99.y"
                                                            { (yyval.ast) = ast_if((yyvsp[-2].ast), (yyvsp[0].ast), NULL); }
#line 1593 "c99.tab.c"
    break;

  case 57: /* selection_statement: IF '(' expression ')' statement ELSE statement  */
#line 246 "c99.y"
                                                     { (yyval.ast) = ast_if((yyvsp[-4].ast), (yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1599 "c99.tab.c"
    break;

  case 58: /* iteration_statement: WHILE '(' expression ')' statement  */
#line 250 "c99.y"
                                         { (yyval.ast) = ast_while((yyvsp[-2].ast), (yyvsp[0].ast)); }
#line 1605 "c99.tab.c"
    break;

  case 59: /* jump_statement: RETURN expression ';'  */
#line 254 "c99.y"
                            { 
        (yyval.ast) = (ASTNode*)malloc(sizeof(ASTNode));
        (yyval.ast)->type = AST_RETURN; (yyval.ast)->expr = (yyvsp[-1].ast);
    }
#line 1614 "c99.tab.c"
    break;

  case 60: /* jump_statement: BREAK ';'  */
#line 258 "c99.y"
                { (yyval.ast) = (ASTNode*)malloc(sizeof(ASTNode)); (yyval.ast)->type = AST_BREAK; }
#line 1620 "c99.tab.c"
    break;

  case 61: /* translation_unit: external_declaration  */
#line 264 "c99.y"
                           { 
        g_ast_root = (ASTNode*)malloc(sizeof(ASTNode)); // 必须先给它 malloc！
        g_ast_root->type = AST_TRANSLATION_UNIT;
        ASTNode **a = (ASTNode**)malloc(sizeof(ASTNode*)); 
        a[0] = (yyvsp[0].ast);
        g_ast_root->seq.list = a; 
        g_ast_root->seq.count = 1;
    }
#line 1633 "c99.tab.c"
    break;

  case 62: /* external_declaration: function_definition  */
#line 274 "c99.y"
                          { (yyval.ast) = (yyvsp[0].ast); }
#line 1639 "c99.tab.c"
    break;

  case 63: /* external_declaration: declaration  */
#line 275 "c99.y"
                  { (yyval.ast) = (yyvsp[0].ast); }
#line 1645 "c99.tab.c"
    break;

  case 64: /* function_definition: declaration_specifiers declarator compound_statement  */
#line 279 "c99.y"
                                                           {
        (yyval.ast) = (yyvsp[-1].ast); // 从 direct_declarator 传来的 AST_FUNC_TYPE
        (yyval.ast)->ft.ret_type = (yyvsp[-2].ast)->ds.specs[0];
        (yyval.ast)->ft.body = (yyvsp[0].ast);
    }
#line 1655 "c99.tab.c"
    break;


#line 1659 "c99.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturn;
#endif


/*-------------------------------------------------------.
| yyreturn -- parsing is finished, clean up and return.  |
`-------------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 286 "c99.y"
