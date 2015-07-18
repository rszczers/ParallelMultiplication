Parallel multiplication
========================
An implementation of parallel Cannon's matrix multiplication algorithm over n*n grid with MPI and Intel Math Kernel Library.

##Usage
<pre>
Usage: test.o [OPTION...] -A matrixA_path -B matrixB_path -m NUM -k NUM -n NUM
Parallel matrix multiplication with Intel Math Kernel Library

  -a, --method=METHOD        Algorithm used.
  -A, --inputA=FILE          Path to input FILE containing matrix A data.
  -B, --inputB=FILE          Path to input FILE containing matrix B data.
  -d, --debug[=DIR]          Path to debug directory.
  -k NUM                     Number of rows of B.
  -l, --list                 Show list of available algorithms.
  -m NUM                     Number of rows of A.
  -n NUM                     Number of columns of B.
  -o, -C, --output[=FILE]    Path to output FILE containing matrix C=A*B data.
  -q, --quiet                Do not show any computations.
  -t, --time                 Show elapsed time.
  -v, --verbose              Show all computations.
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <rafal.szczerski@gmail.com>.
</pre>
