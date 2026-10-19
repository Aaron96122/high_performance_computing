__kernel
void
mat_mul(
    __global const float *a,
    __global const float *b,
    __global float *c,
    int width_a,
    int width_b
    )
{
  int ix = get_global_id(0);
  int iy = get_global_id(1);
  
  float value = 0;
  for ( int kx = 0; kx < width_a; ++kx ) {
    float a_elt = a[iy * width_a + kx];
    float b_elt = b[kx * width_b + ix];
    value += a_elt * b_elt;
  }
  
  c[iy * width_b + ix] = value;
}
