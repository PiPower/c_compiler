signed long long int x = 4, y = 454;
double z22 = 21321.4343;
float p22 = 32.23234;

int main()
{
    double i;
    for(i = 0; i < 32; i = i + 1)
    {
        x = x + i;
    }

    if( x > y)
    {
        float z = 2;
        x = 19;
        double i;
        for(i =0; i < 32; i+= 1)
        {
            x = x + i;
        }
        i = 0;
        while(i < 32)
        {
            x = x + i;
            i = i + 1;
        }

    }
    else if ( x == y)
    {
        x = 2232;
        float i = 0;
        do
        {
            x = x + i;
            i = i + 1;
        }while(i < 32);
    }
    else
    {
        y = 39129;
    }
    return x * 2 + y;
}