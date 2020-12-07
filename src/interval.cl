typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

int f(double x) {
  return (int) (x*3);
}

bool test(__global const Interval* interval) {
  return f(interval->min) != f(interval->max); // FIXME
}

void subdivide(int degree, __global Interval* from, __global Interval* to1, __global Interval* to2) {
  const double mid = (from->min + from->max) / 2;
  to1->min = from->min;
  to1->max = mid;
  to2->min = mid;
  to2->max = from->max;
}

void clear(__global Interval* interval) {
  interval->min = NAN;
  interval->max = NAN;
}

__kernel void interval_select(int degree, __global Interval *A, __global Interval *R, __global Interval *S) {
  int i = get_global_id(0);
  if (test(A+i)) {
    subdivide(degree, A+i, R+i, S+i);
  } else {
    clear(R+i);
    clear(S+i);
  }
}
