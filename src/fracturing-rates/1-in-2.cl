typedef struct __attribute__ ((packed)) _st_Interval {
  double min,max;
} Interval;

// function: f(x,y) = ((x-0.3)^2 + (y-0.7)^2) == 0.1^2 or y==x^3

bool testCircle(int degree, __global const Interval* x) {
  __global const Interval* y = x+1;
  
  // center {x,y}{0,1} on the circle's center
  double cx  = 0.3, cy = 0.7;
  double x0  = nextafter(x->min-cx, -1E9), x1 = nextafter(x->max-cx, 1E9);
  double y0  = nextafter(y->min-cy, -1E9), y1 = nextafter(y->max-cy, 1E9);

  // compute x^2 and y^2 inclusion functions
  double fx0, fx1;
  if (0<=x0) {
    fx0 = x0*x0;
    fx1 = x1*x1;
  } else if (x1<=0) {
    fx0 = x1*x1;
    fx1 = x0*x0;
  } else {
    fx0 = 0;
    fx1 = max(x0*x0, x1*x1);
  }
  double fy0, fy1;
  if (0<=y0) {
    fy0 = y0*y0;
    fy1 = y1*y1;
  } else if (y1<=0) {
    fy0 = y1*y1;
    fy1 = y0*y0;
  } else {
    fy0 = 0;
    fy1 = max(y0*y0, y1*y1);
  }
  double f0 = fx0 + fy0, f1 = fx1 + fy1, r2 = 0.1*0.1;
  if (f1<r2) return false;
  if (r2<f0) return false;
  return true;
}

bool testPolynomial(int degree, __global const Interval* x) {
  __global const Interval* y = x+1;
  
  double x3min = nextafter(x->min*x->min*x->min, -1E9), ymin = nextafter(y->min, -1E9);
  double x3max = nextafter(x->max*x->max*x->max,  1E9), ymax = nextafter(y->max, -1E9);
  if (ymax  < x3min) return false;
  if (x3max < ymin)  return false;
  return true;
}

bool test(int degree, __global const Interval* x) {
  return testCircle(degree, x) || testPolynomial(degree, x);
}

// subdivides a collection of intervals in a "degree"-dimensional space into two preallocated arrays,
// dividing in the dimension "phase"
void subdivide(int degree, int phase, __global const Interval* from, __global Interval* to1, __global Interval* to2) {
  printf("subdividing [%0.6f,%0.6f] × [%0.6f,%0.6f] (dimensions %0.6f × %0.6f)\n", from[0].min, from[0].max, from[1].min, from[1].max, from[0].max-from[0].min, from[1].max-from[1].min);
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
