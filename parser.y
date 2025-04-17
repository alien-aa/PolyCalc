%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "polynomial.h"

extern int yylineno;
extern char last_char;

int yylex(void);
void yyerror(const char *s);
int check_vars_in_polynomial(Polynomial *p, VarValues *vars);
%}

%union {
    char *str;
    int num;
    Polynomial *poly;
    VarAssign var_assign;
    VarAssignList var_assign_list;
}

%token <str> POLY_VAR VARIABLE
%token PRINT EVAL
%token <num> NUMBER
%token ADD SUB MUL DIV POW LPAREN RPAREN SEMI ASSIGN COMMA

%type <poly> expr term factor
%type <var_assign> var_assign
%type <var_assign_list> var_assign_list

%left ADD SUB
%left MUL DIV
%right POW
%right UMINUS

%%

program:
    | program statement
    ;

statement:
      PRINT LPAREN POLY_VAR RPAREN SEMI {
          Polynomial *p = poly_table_get($3);
          if (!p) {
              fprintf(stderr, "[ERR] Line %d: Error: polynomial %s is not defined\n", yylineno, $3);
          } else {
              printf("[OUT] ");
              print_polynomial(p);
          }
          free($3);
      }
    | POLY_VAR ASSIGN expr SEMI {
          if ($3 != NULL) {
              poly_table_set($1, $3);
            } 
            free($1);
      }
    | EVAL LPAREN POLY_VAR RPAREN SEMI {
        Polynomial *p = poly_table_get($3);
        if (!p) {
            fprintf(stderr, "[ERR] Line %d: Error: polynomial %s is not defined\n", yylineno, $3);
        } else {
            if (is_constant_polynomial(p)) {
                printf("[OUT] %d\n", p->head->coeff);
            } else {
                fprintf(stderr, "[ERR] Line %d: Cannot evaluate polynomial without variable values\n", yylineno);
            }
        }
        free($3);
      }
    | EVAL LPAREN POLY_VAR COMMA var_assign_list RPAREN SEMI {
    Polynomial *p = poly_table_get($3);
    if (!p) {
        fprintf(stderr, "[ERR] Line %d: Error: polynomial %s is not defined\n", yylineno, $3);
    } else {
        VarValues vars = {0};
        for (int i = 0; i < $5.count; i++) {
            char v = $5.vars[i].var;
            int val = $5.vars[i].value;
            if (v >= 'a' && v <= 'z') {
                vars.values[v - 'a'] = val;
            }
        }
        if (check_vars_in_polynomial(p, &vars)) {
            int res = evaluate_polynomial(p, &vars);
            printf("[OUT] %d\n", res);
        } else {
            fprintf(stderr, "[ERR] Line %d: Some variables are not present in the evaluation list\n", yylineno);
        }
    }
    free($3);
    free_var_assign_list(&$5);
    }
    ;

var_assign_list:
      var_assign {
          $$ = (VarAssignList){malloc(sizeof(VarAssign)), 1};
          $$ .vars[0] = $1;
      }
    | var_assign_list COMMA var_assign {
          $$ = $1;
          $$ .vars = realloc($$ .vars, sizeof(VarAssign) * ($$ .count + 1));
          $$ .vars[$$ .count] = $3;
          $$ .count++;
      }
    ;

var_assign:
      VARIABLE ASSIGN NUMBER {
          $$ = (VarAssign){$1[0], $3};
          free($1);
      }
    ;

expr:
      expr ADD term {
          Polynomial *res = polynomial_add($1, $3);
          free_polynomial($1);
          free_polynomial($3);
          $$ = res;
      }
    | expr SUB term {
          Polynomial *neg = polynomial_mul(polynomial_from_const(-1), $3);
          Polynomial *res = polynomial_add($1, neg);
          free_polynomial($1);
          free_polynomial($3);
          free_polynomial(neg);
          $$ = res;
      }
    | expr DIV term {
        Polynomial *res = polynomial_div($1, $3);
        if (!res) {
            free_polynomial($1);
            free_polynomial($3);
            $$ = NULL; 
        } else {
            free_polynomial($1);
            free_polynomial($3);
            $$ = res;
        }
      }
    | term { $$ = $1; }
    ;

term:
      term MUL factor {
          Polynomial *res = polynomial_mul($1, $3);
          free_polynomial($1);
          free_polynomial($3);
          $$ = res;
      }
    | factor { $$ = $1; }
    ;

factor:
      SUB factor %prec UMINUS {
          Polynomial *neg = polynomial_mul(polynomial_from_const(-1), $2);
          free_polynomial($2);
          $$ = neg;
      }
    | VARIABLE {
          Polynomial *p = polynomial_from_var($1[0]);
          free($1);
          $$ = p;
      }
    | POLY_VAR {
          Polynomial *p = poly_table_get($1);
          if (!p) {
              fprintf(stderr, "[ERR] Line %d: Error: polynomial %s is not defined\n", yylineno, $1);
              p = polynomial_from_const(0);
          }
          Polynomial *copy = polynomial_add(p, polynomial_from_const(0));
          free($1);
          $$ = copy;
      }
    | NUMBER {
          Polynomial *p = polynomial_from_const($1);
          $$ = p;
      }
    | LPAREN expr RPAREN { $$ = $2; }
    | factor POW NUMBER {
          Polynomial *res = polynomial_pow($1, $3);
          free_polynomial($1);
          $$ = res;
      }
    ;

%%

void yyerror(const char *s) {
    if (last_char != '\0') {
        fprintf(stderr, "[ERR] Line %d: Syntax error near character '%c': %s\n", yylineno, last_char, s);
        last_char = '\0';
    } else {
        fprintf(stderr, "[ERR] Line %d: Syntax error: %s\n", yylineno, s);
    }
}

int main() {
    poly_table_init();
    yyparse();
    poly_table_free();
    return 0;
}
