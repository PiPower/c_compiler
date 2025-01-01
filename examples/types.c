
const long long volatile int xyf1;
const long long volatile int xyf = 2;

struct YOLO;
//struct YOLO xd; <- ERROR

struct PointE2
{};

struct PointE1
{
    ;
    float;
};

struct Point4D;

struct Point4D 
{
    ;
    float; 
    float **x, y, z, g;
};
struct Point4D  po, kl;
//unnamed 
struct
{
    double x, y, z, g;
} beta, gamma;

struct Point3D
{
    float x, y, z;
} x, y, z;

typedef struct Point2D
{
   float x, y, z;
} Point2D;

typedef const struct Point2D XDDDD;

struct composite
{
    XDDDD x;
    struct Point3D y;
};

struct Point3D fn1()
{

}

int main()
{
    //x.x = 2;
;;;;;;;;;;;;;;;;;
    // po.x = 234; <- error
    po.x = (float**)234; // correct 
    Point2D y;
    // Point3D dy; <- error
    struct Point3D p3;
    //p3.x = fn1().x;
    struct Point3D* p4, **p5;
    (&p3)->x = 223;
    p4 = &p3;
    p5 = &p4;
    p4->y = 23;
}