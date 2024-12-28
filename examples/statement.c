signed long long int x = 4, y = 454;
int main()
{
    if( x > y)
    {
        float z = 2;
        x = 19;
        double i;
        for(i =0; i < 32; i+= 1)
        {
            x += i;
        }
        i = 0;
        while(i < 32)
        {
            x += i;
            i+= 1;
        }

    }
    else if ( x == y)
    {
        x = 2232;
        float i = 0;
        do
        {
            x += i;
            i+= 1;
        }while(i < 32);
    }
    else
    {
        y = 39129;
    }
    return x * 2 + y;
}