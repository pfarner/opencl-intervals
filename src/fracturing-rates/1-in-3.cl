typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

// function: f(x,y,z) = z=xy² and x+y+z==3

bool test(int degree, __global const Interval* x) {
  __global const Interval* y = x+1;
  __global const Interval* z = x+2;
  double x0  = nextafter(x->min, -1E9), x1 = nextafter(x->max, 1E9);
  double y0  = nextafter(y->min, -1E9), y1 = nextafter(y->max, 1E9);
  double z0  = nextafter(z->min, -1E9), z1 = nextafter(z->max, 1E9);

  double xy2min = x0*y0*y0, xy2max = x1*y1*y1;
  if (z1 < xy2min) return false;
  if (xy2max < z0) return false;
  
  double sum0 = x0+y0+z0, sum1 = x1+y1+z1;
  if (3 < sum0) return false;
  if (sum1 < 3) return false;
  
  return true;
}

// subdivides a collection of intervals in a "degree"-dimensional space into two preallocated arrays,
// dividing in the dimension "phase"
void subdivide(int degree, int phase, __global const Interval* from, __global Interval* to1, __global Interval* to2) {
  printf("subdividing [%0.6f,%0.6f,%0.6f] × [%0.6f,%0.6f,%0.6f] (dimensions %0.6f × %0.6f × %0.6f)\n", from[0].min, from[0].max, from[1].min, from[1].max, from[2].min, from[2].max, from[0].max-from[0].min, from[1].max-from[1].min, from[2].max-from[2].min);
  for (int d=0; d<degree; ++d) {
    __global const Interval* fromi = from+d;
    __global Interval*       to1i  = to1+d;
    __global Interval*       to2i  = to2+d;
    if (d==phase) {
      const double mid = (fromi->min + fromi->max) / 2;
      if (mid <= fromi->min || fromi->max <= mid) {
        // if subdivision results in zero-width intervals, don't subdivide
        to1i->min = fromi->min;
        to1i->max = fromi->max;
        to2i->min = NAN;
        to2i->max = NAN;
        printf("… mid was on an edge (dim %d), so keeping [%0.6f,%0.6f]\n", d, to1i->min, to1i->max);
      } else {
        to1i->min = fromi->min;
        to1i->max = mid;
        to2i->min = mid;
        to2i->max = fromi->max;
        printf("… divided dim %d into [%0.6f,%0.6f] and [%0.6f,%0.6f]\n", d, to1i->min, to1i->max, to2i->min, to2i->max);
      }
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
