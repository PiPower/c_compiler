// struct_access_test.c
// C99

#include <stdio.h>

struct Point {
    int x;
    int y;
};

struct PointNested {
    int x;
    int y;
    struct Point  p2;
};

struct Line {
    struct Point start;
    struct Point end;
};

struct Container {
    int id;
    struct Point point;
    int values[4];
};

typedef struct {
    int a;
    int b;
} Pair;

typedef struct Node {
    int value;
    struct Node *next;
} Node;

struct Outer {
    struct {
        int inner;
    } anon;
};
/*
int sum_point(struct Point p)
{
    return p.x + p.y;
}

void modify_point(struct Point *p)
{
    p->x += 10;
    p->y += 20;
}*/

int main(void)
{
    // ------------------------------------------------------------
    // Basic member access
    // ------------------------------------------------------------

    struct Point p;

    p.x = 1;
    p.y = 2;

    struct  PointNested pn;
    pn.p2.x = p.x;

    printf("%d %d\n", p.x, p.y);
}
/*
    // ------------------------------------------------------------
    // Pointer access
    // ------------------------------------------------------------

    struct Point *pp = &p;

    pp->x = 3;
    pp->y = 4;

    printf("%d %d\n", pp->x, pp->y);

    // ------------------------------------------------------------
    // Parenthesized expressions
    // ------------------------------------------------------------

    (*pp).x = 5;
    (*pp).y = 6;

    printf("%d %d\n", (*pp).x, (*pp).y);

    // ------------------------------------------------------------
    // Nested structs
    // ------------------------------------------------------------

    struct Line line;

    line.start.x = 10;
    line.start.y = 20;

    line.end.x = 30;
    line.end.y = 40;

    printf("%d %d %d %d\n",
           line.start.x,
           line.start.y,
           line.end.x,
           line.end.y);

    // ------------------------------------------------------------
    // Nested pointer access
    // ------------------------------------------------------------

    struct Line *lp = &line;

    lp->start.x = 100;
    lp->end.y = 200;

    printf("%d %d\n",
           lp->start.x,
           lp->end.y);

    // ------------------------------------------------------------
    // Struct containing arrays
    // ------------------------------------------------------------

    struct Container c;

    c.id = 99;

    c.point.x = 7;
    c.point.y = 8;

    c.values[0] = 1;
    c.values[1] = 2;
    c.values[2] = 3;
    c.values[3] = 4;

    printf("%d %d %d\n",
           c.id,
           c.point.x,
           c.values[2]);

    // ------------------------------------------------------------
    // Array of structs
    // ------------------------------------------------------------

    struct Point pts[3];

    pts[0].x = 11;
    pts[0].y = 12;

    pts[1].x = 21;
    pts[1].y = 22;

    pts[2].x = 31;
    pts[2].y = 32;

    printf("%d %d\n",
           pts[1].x,
           pts[2].y);

    // ------------------------------------------------------------
    // Pointer to array element
    // ------------------------------------------------------------

    struct Point *q = &pts[2];

    printf("%d %d\n",
           q->x,
           q->y);

    // ------------------------------------------------------------
    // Struct assignment
    // ------------------------------------------------------------

    struct Point p2;

    p2 = p;

    printf("%d %d\n",
           p2.x,
           p2.y);

    // ------------------------------------------------------------
    // Function returning struct
    // ------------------------------------------------------------

    struct Point p3 = make_point(50, 60);

    printf("%d %d\n",
           p3.x,
           p3.y);

    // ------------------------------------------------------------
    // Struct passed by value
    // ------------------------------------------------------------

    printf("%d\n",
           sum_point(p3));

    // ------------------------------------------------------------
    // Struct passed by pointer
    // ------------------------------------------------------------

    modify_point(&p3);

    printf("%d %d\n",
           p3.x,
           p3.y);

    // ------------------------------------------------------------
    // Typedef struct
    // ------------------------------------------------------------

    Pair pair;

    pair.a = 7;
    pair.b = 8;

    printf("%d %d\n",
           pair.a,
           pair.b);

    Pair pair2 = make_pair(100, 200);

    printf("%d %d\n",
           pair2.a,
           pair2.b);

    // ------------------------------------------------------------
    // Pointer chain
    // ------------------------------------------------------------

    Node n1;
    Node n2;

    n1.value = 1;
    n1.next = &n2;

    n2.value = 2;
    n2.next = NULL;

    printf("%d\n",
           n1.next->value);

    // ------------------------------------------------------------
    // Const struct
    // ------------------------------------------------------------

    const struct Point cp = { 9, 10 };

    printf("%d %d\n",
           cp.x,
           cp.y);

    // ------------------------------------------------------------
    // Anonymous inner struct
    // ------------------------------------------------------------

    struct Outer o;

    o.anon.inner = 123;

    printf("%d\n",
           o.anon.inner);

    // ------------------------------------------------------------
    // Temporary return value member access
    // ------------------------------------------------------------

    printf("%d\n",
           make_point(70, 80).x);

    printf("%d\n",
           make_pair(9, 11).b);

    // ------------------------------------------------------------
    // Compound literals (C99)
    // ------------------------------------------------------------

    printf("%d\n",
           ((struct Point){1, 2}).x);

    printf("%d\n",
           ((Pair){3, 4}).b);

    struct Point *temp = &(struct Point){5, 6};

    printf("%d %d\n",
           temp->x,
           temp->y);

    // ------------------------------------------------------------
    // Complex chained accesses
    // ------------------------------------------------------------

    struct Container *cptr = &c;

    printf("%d\n",
           cptr->point.x);

    printf("%d\n",
           (*cptr).point.y);

    printf("%d\n",
           cptr->values[1]);

    printf("%d\n",
           (&line)->end.x);

    printf("%d\n",
           (&pts[1])->y);

    return 0;
}
*/