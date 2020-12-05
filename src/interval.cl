typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

__kernel void interval_select(__global Interval *A, __global Interval *R, __global Interval *S) {
  int i = get_global_id(0);
  const double min = A[i].min, max = A[i].max;
  const double mid = (min + max) / 2;
  const double target = 2.0/3;
  if (min <= target && target < mid) {
    R[i].min = min;
    R[i].max = mid;
  } else {
    R[i].min = NAN;
    R[i].max = NAN;
  }
  if (mid <= target && target < max) {
    S[i].min = mid;
    S[i].max = max;
  } else {
    S[i].min = NAN;
    S[i].max = NAN;
  }
}
