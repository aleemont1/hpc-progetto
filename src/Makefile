# Default makefile for the circles intersections program.
#
# Available targets:
#
# - make
#   builds the standard executable
#
# - make clean
#   remove all output files and executables
#
# - make movie
#   compile the executable circles.movie that writes a gnuplot
#   file at each iteration; the executable is run and the output
#   is processed to produce an animation circles.avi

# Last modified 2023-12-06 by Moreno Marzolla

MPICC:=mpicc
EXE:=circles
OMP-EXE:=omp-circles
MPI-EXE:=mpi-circles
CFLAGS=-std=c99 -Wall -Wpedantic
OMP-CFLAGS:=$(CFLAGS) -fopenmp
LDLIBS+=-lm

ALL: $(EXE)

$(EXE).movie: CFLAGS+=-DMOVIE
$(EXE).movie: $(EXE).c
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

all: ALL omp mpi

mpi: $(MPI-EXE)
	$(MPICC) $(CFLAGS) $(MPI-EXE).c -o $(MPI-EXE) $(LDLIBS)

omp: $(OMP-EXE)
	$(CC) $(OMP-CFLAGS) $(OMP-EXE).c -o $(OMP-EXE) $(LDLIBS)

movie: $(EXE).movie
	./$(EXE).movie 200 500
	for f in *.gp; do echo "Processing $$f"; gnuplot "$$f"; done
	ffmpeg -y -i "circles-%05d.png" -vcodec mpeg4 circles.avi

omp-movie: $(OMP-EXE).movie
	./$(OMP-EXE).movie 200 500
	for f in *.gp; do echo "Processing $$f"; gnuplot "$$f"; done
	ffmpeg -y -i "circles-%05d.png" -vcodec mpeg4 circles.avi

mpi-movie: $(MPI-EXE).movie
	./$(MPI-EXE).movie 200 500
	for f in *.gp; do echo "Processing $$f"; gnuplot "$$f"; done
	ffmpeg -y -i "circles-%05d.png" -vcodec mpeg4 circles.avi

clean:
	\rm -f $(OMP-EXE) $(MPI-EXE) $(EXE) $(EXE).movie *.o *~ *.gp *.png *.avi

