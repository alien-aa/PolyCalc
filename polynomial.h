#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#define VAR_COUNT 26

typedef struct Monomial {
    int coeff;
    int powers[VAR_COUNT];
    struct Monomial *next;
} Monomial;

typedef struct Polynomial {
    Monomial *head;
} Polynomial;

typedef struct {
    char var;
    int value;
} VarAssign;

typedef struct {
    VarAssign *vars;
    int count;
} VarAssignList;

typedef struct {
    int values[VAR_COUNT];
} VarValues;


typedef struct PolyEntry {
    char *name;
    Polynomial *poly;
    struct PolyEntry *next;
} PolyEntry;


Monomial* create_monom(int coeff, int *powers);
Polynomial* create_polynomial(Monomial *m);
Polynomial* polynomial_from_const(int c);
Polynomial* polynomial_from_var(char var);
void free_polynomial(Polynomial *p);

Polynomial* polynomial_add(Polynomial *p1, Polynomial *p2);
Polynomial* polynomial_mul(Polynomial *p1, Polynomial *p2);
Polynomial* polynomial_pow(Polynomial *p, int n);
Polynomial* polynomial_div(Polynomial *p1, Polynomial *p2); 

void print_polynomial(Polynomial *p);
int evaluate_polynomial(Polynomial *p, VarValues *vars);

int is_zero_polynomial(Polynomial *p); 
int is_constant_polynomial(Polynomial *p); 
int check_vars_in_polynomial(Polynomial *p, VarValues *vars);
int get_constant_value(Polynomial *p); 

void poly_table_init(void);
void poly_table_set(const char *name, Polynomial *p);
Polynomial* poly_table_get(const char *name);
void poly_table_free(void);
void free_var_assign_list(VarAssignList *list);

#endif // POLYNOMIAL_H
