int foo(long,long,long,long,long,long,long,long,long,long,long);

long weird_op(long m1, long m2, long m3, long m4, long m5, long m6, long m7)
{
  foo(m1,m2,m3,m4,m5,m6,m7,0,0,0,0);
  return m1*m2*m3*m4/(m5*m6*m7);
}

long fred(long v1, long v2, long v3, long v4, long v5, long v6, long v7, long v8, long v9, long v10, long v11)
{
  foo(v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11);
  return v1+v2+v3+v4+v5+v6+v7+v8+v9+v10+v11 + weird_op(3*(v2<v5),2,4*v7,1,1,2,2);
}

int main()
{
  long a; long b; long c; long d; long e; long f; 
  long g; long h; long i; long j; long k; 
  a=1; b=2; c=3; d=4; e=5; f=6; 
  g=7; h=8; i=9; j=10; k=11;
  long out;
  out = j - c + g + fred(a * 2, b, c + 1, d, e, f, g, h, i, j, k);
  foo(a,b,c,d,e,f,g,h,i,j,k);
  return out;
}
