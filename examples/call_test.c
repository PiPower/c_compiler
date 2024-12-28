long int yolo(long int x, long int j, long int p);

long int test(long int a, long int b, long int c, long int d, long int e, long int f, long int g, long int h, long int i);
long int testReg(long int a,long int b, long int c,  long int d,  long int e, long int f);
long int printLongInt(long int a);

long int j, z;
long int fib(long int n);
long int main()
{
    test(1, 2, 3, 4, 5, 6, 7, 8, 9);
    j = z = 23;
    j += 1;
    j = j - z;
    z *= 2;
    testReg(1231, 3234, 4223,  4546, 324, 43564);
    z ^= z;
    yolo(z + j * 2, 3, j - 4);
    z = yolo(2,3,4);
    z -= 2345;
    z <<= 2;
}

long int yolo(long int x, long int j, long int p)
{
    z = 23 * 3+ 345;
    testReg(x,j,p,0,0,222222);
    test(x + j, x + p, j + p, x - 23 * p, p*j, p * 0, x+p+j, 0,0);
    printLongInt(fib(10));
    return 3;
}

long int fib(long int n)
{
    if(n == 0) 
    {
        return 0;
    }
    if(n == 1)
    {
        return 1;
    }
    return fib(n-1) + fib(n-2);
}