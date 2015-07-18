Parallel multiplication
========================
An implementation of parallel Cannon's matrix multiplication algorithm over n*n grid with MPI and Intel Math Kernel Library.

#Usage
-A matrixA_path -B matrixB_path -m NUM -k NUM -n NUM

  -a, --method=METHOD        Algorithm used.
  -A, --inputA=FILE          Path to input FILE containing matrix A data.
  -B, --inputB=FILE          Path to input FILE containing matrix B data.
  -d, -C, --debug[=DIR]      Path to debug directory.
  -k NUM                     Number of rows of B.
  -l, --list                 Show list of available algorithms.
  -m NUM                     Number of rows of A.
  -n NUM                     Number of columns of B.
  -o, --output[=FILE]        Path to output FILE containing matrix C=A*B data.
  -q, --quiet                Do not show any computations.
  -t, --time                 Show elapsed time.
  -v, --verbose              Show all computations.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version


