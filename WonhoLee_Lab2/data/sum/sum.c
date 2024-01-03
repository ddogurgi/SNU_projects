int sum_and_minus(int n, int m)
{
  int i;
  int res=0;
  for(i=0;i<n;i++)
  {
    res+=i;
  }
  return res-m;
}

int main(void)
{
  int num=11;
  int m=11;

  int result = sum_and_minus(num,m);
  return 0;
}
