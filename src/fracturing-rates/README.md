Fracturing Rate Analysis

Different types of solutions result in different behavior of the number of remaining potential intervals as subdivision and culling occurs:

* if solutions are k points in an n-space, then number of intervals should trend towards ck for some constant c

* if solutions are 1D manifolds in an n-space, the number of intervals should roughly double after n subdivisions

* if solutions are 2D manifolds in an n-space, … [TBD]

By watching the change in the number of remaining intervals, we may be able to infer characteristics of the solution space, and therefore opt for other implementations instead of allowing the number of intervals to continue to grow exponentially.
