Compiling instructions through Makefile.

- **`make`**\
  build both the OpenMP and MPI versions of the program

- **`make clean`**\
   remove all output files and executables

- **`make omp`**\
   build the OpenMP version of the program

- **`make mpi`**\
   build the MPI version of the program

- **`make omp-circles.movie`**\
   build the OpenMP version of the program with the MOVIE flag
   enabled. It produces an executable named 'omp-circles.movie' which,
   when executed, will produce a number of `omp-circles-xxxxx.gp` files. The files can be processed
   through the `gnuplot` application to create single frames:
   ```
   for f in omp-circles-*.gp; do gnuplot "$f"; done
   ```
   And then through `ffmpeg` to assemble them:
   ```
   ffmpeg -y -i "omp-circles-%05d.png" -vcodec mpeg4 omp-circles.avi
   ```
   This will produce a `omp-circles.avi` video.

- **`make mpi-circles.movie`**\
   build the OpenMP version of the program with the MOVIE flag
   enabled. It produces an executable named `mpi-circles.movie` which,
   when executed, will produce a number of `mpi-circles-xxxxx.gp` files. The files can be processed
   through the `gnuplot` application to create single frames:
   ```
   for f in mpi-circles-*.gp; do gnuplot "$f"; done
   ```
   And then through `ffmpeg` to assemble them:
   ```
   ffmpeg -y -i "mpi-circles-%05d.png" -vcodec mpeg4 mpi-circles.avi
   ```
   This will produce a `mpi-circles.avi` video.

- **`make omp-movie`**\
   compile the executable omp-circles.movie that writes a gnuplot
   file at each iteration; the executable is run and the output
   is processed to produce an animation omp-circles.avi.\
   (This action runs the `omp-circles.movie` target and processes the output through `gnuplot` and `ffmpeg`, automagically).\
   *Note: the execution is run on 8 threads by default. Set the OMP_NUM_THREADS variable to change this option (e.g. `make omp-movie OMP_NUM_THREADS=6`)*

- **`make mpi-movie`**\
   compile the executable mpi-circles.movie that writes a gnuplot
   file at each iteration; the executable is run and the output
   is processed to produce an animation mpi-circles.avi.\
   (This action runs `mpi-circles.movie` target and processes the output through `gnuplot` and `ffmpeg`, automagically).
