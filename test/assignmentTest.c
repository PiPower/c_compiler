int j =0 ;
int z = 45;

int main()
{
    j = z = 23;
    j += 1;
    j = j - z;
    z *= 2;
    z ^= z;
    z -= 2345;
    z <<= 2;
}