typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

int f(double x) {
  return (int) (x*3);
}

bool test(int degree, __global const Interval* x) {
  return f(x->min) != f(x->max); // FIXME
}

void subdivide(int degree, int phase, __global const Interval* from, __global Interval* to1, __global Interval* to2) {
  for (int d=0; d<degree; ++d) {
    __global const Interval* fromi = from+d;
    __global Interval*       to1i  = to1+d;
    __global Interval*       to2i  = to2+d;
    if (d==phase) {
      const double mid = (fromi->min + fromi->max) / 2;
      to1i->min = fromi->min;
      to1i->max = mid;
      to2i->min = mid;
      to2i->max = fromi->max;
    } else {
      to1i->min = fromi->min;
      to1i->max = fromi->max;
      to2i->min = fromi->min;
      to2i->max = fromi->max;
    }
  }
}

void clear(int degree, __global Interval* x) {
  for (int d=0; d<degree; ++d) {
    (x+d)->min = NAN;
    (x+d)->max = NAN;
  }
}

__kernel void interval_select(int degree, int phase, __global Interval *A, __global Interval *R, __global Interval *S) {
  int i     = get_global_id(0);
  int start = i*degree;
  if (test(degree, A+start)) {
    subdivide(degree, phase, A+start, R+start, S+start);
  } else {
    clear(degree, R+start);
    clear(degree, S+start);
  }
}
