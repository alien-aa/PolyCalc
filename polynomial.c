#include "polynomial.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

extern int yylineno; 

static PolyEntry *poly_table = NULL;


Monomial* create_monom(int coeff, int *powers) {
    Monomial *m = malloc(sizeof(Monomial));
    m->coeff = coeff;
    if (powers)
        memcpy(m->powers, powers, sizeof(int) * VAR_COUNT);
    else
        memset(m->powers, 0, sizeof(int) * VAR_COUNT);
    m->next = NULL;
    return m;
}

Polynomial* create_polynomial(Monomial *m) {
    Polynomial *p = malloc(sizeof(Polynomial));
    p->head = m;
    return p;
}

Polynomial* polynomial_from_const(int c) {
    Monomial *m = create_monom(c, NULL);
    return create_polynomial(m);
}

Polynomial* polynomial_from_var(char var) {
    int powers[VAR_COUNT] = {0};
    if (var >= 'A' && var <= 'Z') var = var - 'A' + 'a';
    if (var >= 'a' && var <= 'z') {
        powers[var - 'a'] = 1;
    }
    Monomial *m = create_monom(1, powers);
    return create_polynomial(m);
}

void free_polynomial(Polynomial *p) {
    Monomial *m = p->head;
    while (m) {
        Monomial *next = m->next;
        free(m);
        m = next;
    }
    free(p);
}

int monom_equal(Monomial *a, Monomial *b) { // auxiliary, not in the header
    for (int i = 0; i < VAR_COUNT; i++) {
        if (a->powers[i] != b->powers[i])
            return 0;
    }
    return 1;
}

void add_monom_to_polynomial(Polynomial *p, Monomial *m) { // auxiliary, not in the header
    if (m->coeff == 0) {
        free(m);
        return;
    }
    Monomial *cur = p->head;
    Monomial *prev = NULL;
    while (cur) {
        if (monom_equal(cur, m)) {
            cur->coeff += m->coeff;
            free(m);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
    if (prev)
        prev->next = m;
    else
        p->head = m;
    m->next = NULL;
}


Polynomial* polynomial_add(Polynomial *p1, Polynomial *p2) {
    Polynomial *res = create_polynomial(NULL);
    for (Monomial *m = p1->head; m != NULL; m = m->next) {
        int powers[VAR_COUNT];
        memcpy(powers, m->powers, sizeof(int) * VAR_COUNT);
        Monomial *copy = create_monom(m->coeff, powers);
        add_monom_to_polynomial(res, copy);
    }
    for (Monomial *m = p2->head; m != NULL; m = m->next) {
        int powers[VAR_COUNT];
        memcpy(powers, m->powers, sizeof(int) * VAR_COUNT);
        Monomial *copy = create_monom(m->coeff, powers);
        add_monom_to_polynomial(res, copy);
    }
    return res;
}

Polynomial* polynomial_mul(Polynomial *p1, Polynomial *p2) {
    Polynomial *res = create_polynomial(NULL);
    for (Monomial *m1 = p1->head; m1 != NULL; m1 = m1->next) {
        for (Monomial *m2 = p2->head; m2 != NULL; m2 = m2->next) {
            int powers[VAR_COUNT];
            for (int i = 0; i < VAR_COUNT; i++)
                powers[i] = m1->powers[i] + m2->powers[i];
            Monomial *prod = create_monom(m1->coeff * m2->coeff, powers);
            add_monom_to_polynomial(res, prod);
        }
    }
    return res;
}

Polynomial* polynomial_pow(Polynomial *p, int n) {
    if (n == 0) {
        return polynomial_from_const(1);
    }
    Polynomial *res = polynomial_from_const(1);
    for (int i = 0; i < n; i++) {
        Polynomial *tmp = polynomial_mul(res, p);
        free_polynomial(res);
        res = tmp;
    }
    return res;
}

Polynomial* polynomial_div(Polynomial *p1, Polynomial *p2) {
    if (!is_constant_polynomial(p2)) {
        fprintf(stderr, "[ERR] Line %d: Division only supported by constant polynomials\n", yylineno);
        return NULL;
    }
    int divisor = get_constant_value(p2);
    if (divisor == 0) {
        fprintf(stderr, "[ERR] Line %d: Division by zero\n", yylineno);
        return NULL; 
    }
    Polynomial *result = create_polynomial(NULL);
    for (Monomial *m = p1->head; m != NULL; m = m->next) {
        Monomial *new_m = malloc(sizeof(Monomial));
        new_m->coeff = m->coeff / divisor; 
        memcpy(new_m->powers, m->powers, sizeof(int) * VAR_COUNT);
        new_m->next = NULL;
        add_monom_to_polynomial(result, new_m);
    }
    return result;
}


void print_polynomial(Polynomial *p) {
    Monomial *m = p->head;
    int first = 1;
    while (m) {
        if (m->coeff == 0) {
            m = m->next;
            continue;
        }
        if (!first) {
            if (m->coeff > 0)
                printf(" + ");
            else
                printf(" - ");
        } else {
            if (m->coeff < 0)
                printf("-");
        }
        int abs_coeff = m->coeff < 0 ? -m->coeff : m->coeff;
        int printed_coeff = 0;
        if (abs_coeff != 1) {
            printf("%d", abs_coeff);
            printed_coeff = 1;
        }
        for (int i = 0; i < VAR_COUNT; i++) {
            if (m->powers[i] > 0) {
                printf("%c", 'a' + i);
                if (m->powers[i] > 1)
                    printf("^%d", m->powers[i]);
                printed_coeff = 1;
            }
        }
        if (!printed_coeff)
            printf("%d", abs_coeff);
        first = 0;
        m = m->next;
    }
    if (first)
        printf("0");
    printf("\n");
}

int evaluate_polynomial(Polynomial *p, VarValues *vars) {
    int sum = 0;
    for (Monomial *m = p->head; m != NULL; m = m->next) {
        int term = m->coeff;
        for (int i = 0; i < VAR_COUNT; i++) {
            if (m->powers[i] > 0) {
                if (vars->values[i] == 0) return INT_MIN;
                int base = vars->values[i];
                int exp = m->powers[i];
                int power = 1;
                for (int j = 0; j < exp; j++) power *= base;
                term *= power;
            }
        }
        sum += term;
    }
    return sum;
}


int is_zero_polynomial(Polynomial *p) {
    if (!p || !p->head) return 1;
    Monomial *m = p->head;
    while (m) {
        if (m->coeff != 0) return 0;
        m = m->next;
    }
    return 1;
}

int is_constant_polynomial(Polynomial *p) {
    if (!p || !p->head) return 1; 
    Monomial *m = p->head;
    if (m->next != NULL) return 0; 
    for (int i = 0; i < VAR_COUNT; i++) {
        if (m->powers[i] != 0) return 0; 
    }
    return 1;
}

int check_vars_in_polynomial(Polynomial *p, VarValues *vars) {
    int has_vars = 0;
    for (int i = 0; i < VAR_COUNT; i++) {
        int var_present = 0;
        for (Monomial *m = p->head; m != NULL; m = m->next) {
            if (m->powers[i] > 0) {
                has_vars = 1;
                var_present = 1;
                break;
            }
        }
        if (var_present && vars->values[i] == 0) {
            return 0;
        }
    }
    return has_vars;
}


int get_constant_value(Polynomial *p) {
    if (!p || !p->head) return 0;
    return p->head->coeff;
}


void poly_table_init(void) {
    poly_table_free();
    poly_table = NULL;
}

void poly_table_set(const char *name, Polynomial *p) {
    PolyEntry *cur = poly_table;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            free_polynomial(cur->poly);
            cur->poly = p;
            return;
        }
        cur = cur->next;
    }
    PolyEntry *new_entry = malloc(sizeof(PolyEntry));
    new_entry->name = strdup(name);
    new_entry->poly = p;
    new_entry->next = poly_table;
    poly_table = new_entry;
}

Polynomial* poly_table_get(const char *name) {
    PolyEntry *cur = poly_table;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            return cur->poly;
        }
        cur = cur->next;
    }
    return NULL;
}

void poly_table_free(void) {
    PolyEntry *cur = poly_table;
    while (cur) {
        PolyEntry *next = cur->next;
        free(cur->name);
        free_polynomial(cur->poly);
        free(cur);
        cur = next;
    }
    poly_table = NULL;
}

void free_var_assign_list(VarAssignList *list) {
    if (list->vars) {
        free(list->vars);
        list->vars = NULL;
    }
    list->count = 0;
}
