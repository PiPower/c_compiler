long main() 
{
    unsigned long x = 2;
    int z = 343;
    long p =  x * z * x + 234;

    p = x&(z |z) ^ x;

    float d = 232.234;
    float j = d + z * z;
 
    double xfd = 3.322432;
    double pr;
    pr = xfd * (d - j);
    return 0;
}
