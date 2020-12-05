typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

int f(double x) {
  return (int) (x*3);
}

__kernel void interval_select(int degree, __global Interval *A, __global Interval *R, __global Interval *S) {
  int i = get_global_id(0);
  const double min = A[i].min, max = A[i].max;
  if (f(min) != f(max)) {
    const double mid = (min + max) / 2;
    R[i].min = min;
    R[i].max = mid;
    S[i].min = mid;
    S[i].max = max;
  } else {
    R[i].min = NAN;
    R[i].max = NAN;
    S[i].min = NAN;
    S[i].max = NAN;
  }
}
