# makefile for the circles intersections program.
#
# Available targets:
#
# - make
#   builds both the OpenMP and MPI versions of the program
#
# - make omp
#   builds the OpenMP version of the program
#
# - make mpi
#   builds the MPI version of the program
#
# - make clean
#   remove all output files and executables
#
# - make omp-movie
#   compile the executable omp-circles.movie that writes a gnuplot
#   file at each iteration; the executable is run and the output
#   is processed to produce an animation omp-circles.avi
#
# - make mpi-movie
#   compile the executable mpi-circles.movie that writes a gnuplot
#   file at each iteration; the executable is run and the output
#   is processed to produce an animation mpi-circles.avi


# Last modified 2023-01-10 by Alessandro Monticelli

CC:=gcc
MPICC:=mpicc
OMP-EXE:=omp-circles
MPI-EXE:=mpi-circles
CFLAGS=-std=c99 -Wall -Wpedantic -Wextra -O3
OMP-CFLAGS:=$(CFLAGS) -fopenmp
LDLIBS+=-lm
OMP_NUM_THREADS:=12

ALL: omp mpi

$(OMP-EXE).movie: OMP-CFLAGS+=-DMOVIE
$(OMP-EXE).movie: $(OMP-EXE).c
	$(CC) $(OMP-CFLAGS) $< -o $@ $(LDLIBS)

$(MPI-EXE).movie: CFLAGS+=-DMOVIE
$(MPI-EXE).movie: $(MPI-EXE).c
	$(MPICC) $(CFLAGS) $< -o $@ $(LDLIBS)

mpi:
	$(MPICC) $(CFLAGS) $(MPI-EXE).c -o $(MPI-EXE) $(LDLIBS)

omp:
	$(CC) $(OMP-CFLAGS) $(OMP-EXE).c -o $(OMP-EXE) $(LDLIBS)

omp-movie: $(OMP-EXE).movie
	OMP_NUM_THREADS=$(OMP_NUM_THREADS) ./$(OMP-EXE).movie 2000 500
	for f in omp*.gp; do echo "Processing $$f"; gnuplot "$$f"; done
	ffmpeg -y -i "omp-circles-%05d.png" -vcodec mpeg4 omp-circles.avi

mpi-movie: $(MPI-EXE).movie
	./$(MPI-EXE).movie 1000 500
	for f in mpi*.gp; do echo "Processing $$f"; gnuplot "$$f"; done
	ffmpeg -y -i "mpi-circles-%05d.png" -vcodec mpeg4 mpi-circles.avi

clean:
	\rm -f $(OMP-EXE) $(MPI-EXE) $(EXE) $(OMP-EXE).movie $(MPI-EXE).movie $(EXE).movie *.o *~ *.gp *.png *.avi
