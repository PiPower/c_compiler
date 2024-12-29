struct Point4D
{
    float x, y, z, g;
};
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


int main()
{
    x.x = 2;
    Point2D y;
    // Point3D dy; <- error
    struct Point3D p3;
}