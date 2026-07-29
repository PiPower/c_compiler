int* get_arr()
{
    return 0;
}

int sum_array(int *arr, int n)
{
    int s = 0;
    int i;

    for (i = 0; i < n; i++)
    {
        s += arr[i];
        //arr[2] = 2;
    }

    return s;
}
 