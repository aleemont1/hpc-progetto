/****************************************************************************
 *
 * circles.c - Circles intersection
 *
 * Copyright (C) 2023 by Moreno Marzolla <moreno.marzolla(at)unibo.it>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/***
% Circles intersection (MPI version)
% Alessandro Monticelli - 0001028456
% Last updated on 04-07-2024

This is a parallel implementation of the circle intersection program
described in the specifications, using the MPI parallelization paradigm.


NOTE> A Makefile is provided, read that for compilation instructions.
If you want to compile manually, follow the instructions below.

To compile:

        mpicc -std=c99 -Wall -Wpedantic mpi-circles.c -o mpi-circles -lm

To execute:

        mpirun mpi-circles [ncircles] [iterations]

where `ncircles` is the number of circles, and `iterations` is the
number of iterations to execute.

If you want to produce a movie (this is not required, and should be
avoided when measuring the performance of the parallel versions of
this program) compile with:

        mpicc -std=c99 -Wall -Wpedantic -DMOVIE mpi-circles.c -o mpi-circles.movie -lm

and execute with:

        mpirun mpi-circles.movie 200 500

A lot of `mpi-circles-xxxxx.gp` files will be produced; these files must
be processed using `gnuplot` to create individual frames:

        for f in mpi-circles-*.gp; do gnuplot "$f"; done

and then assembled to produce the movie `circles.avi`:

        ffmpeg -y -i "mpi-circles-%05d.png" -vcodec mpeg4 mpi-circles.avi

***/

#include "hpc.h"
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>

typedef struct
{
    float x, y;   /* coordinates of center */
    float r;      /* radius */
    float dx, dy; /* displacements due to interactions with other circles */
} circle_t;

/* These constants can be replaced with #define's if necessary */
const float XMIN = 0.0;
const float XMAX = 1000.0;
const float YMIN = 0.0;
const float YMAX = 1000.0;
const float RMIN = 10.0;
const float RMAX = 100.0;
const float EPSILON = 1e-5;
const float K = 1.5;

int ncircles;
circle_t *circles = NULL;

/**
 * Return a random float in [a, b]
 */
float randab(float a, float b)
{
    return a + (((float)rand()) / RAND_MAX) * (b - a);
}

/**
 * Create and populate the array `circles[]` with randomly placed
 * circls.
 *
 * Do NOT parallelize this function.
 */
void init_circles(int n)
{
    assert(circles == NULL);
    ncircles = n;
    circles = (circle_t *)malloc(n * sizeof(*circles));
    assert(circles != NULL);
    for (int i = 0; i < n; i++)
    {
        circles[i].x = randab(XMIN, XMAX);
        circles[i].y = randab(YMIN, YMAX);
        circles[i].r = randab(RMIN, RMAX);
        circles[i].dx = circles[i].dy = 0.0;
    }
}

/**
 * Set all displacements to zero.
 */
void reset_displacements(void)
{
    for (int i = 0; i < ncircles; i++)
    {
        circles[i].dx = circles[i].dy = 0.0;
    }
}

/**
 * Compute the force acting on each circle; returns the number of
 * overlapping pairs of circles (each overlapping pair must be counted
 * only once).
 */
int compute_forces(int start, int end)
{
    int n_intersections = 0;
    for (int i = start; i < end; i++)
    {
        for (int j = 0; j < ncircles; j++)
        {
            if (i == j)
                continue;
            const float deltax = circles[j].x - circles[i].x;
            const float deltay = circles[j].y - circles[i].y;
            const float dist = hypotf(deltax, deltay);
            const float Rsum = circles[i].r + circles[j].r;
            if (dist < Rsum - EPSILON)
            {
                n_intersections++;
                const float overlap = Rsum - dist;
                assert(overlap > 0.0);
                float overlap_x, overlap_y;
                if (dist < EPSILON)
                {
                    // If the distance is very small, distribute the overlap equally in an arbitrary direction
                    overlap_x = overlap / sqrtf(2.0);
                    overlap_y = overlap / sqrtf(2.0);
                }
                else
                {
                    overlap_x = overlap / dist * deltax;
                    overlap_y = overlap / dist * deltay;
                }
                circles[i].dx -= overlap_x / K;
                circles[i].dy -= overlap_y / K;
                circles[j].dx += overlap_x / K;
                circles[j].dy += overlap_y / K;
            }
        }
    }
    return n_intersections;
}

/**
 * Move the circles to a new position according to the forces acting
 * on each one.
 */
void move_circles(void)
{
    for (int i = 0; i < ncircles; i++)
    {
        circles[i].x += circles[i].dx;
        circles[i].y += circles[i].dy;
    }
}

#ifdef MOVIE
/**
 * Dumps the circles into a text file that can be processed using
 * gnuplot. This function may be used for debugging purposes, or to
 * produce a movie of how the algorithm works.
 *
 * You may want to completely remove this function from the final
 * version.
 */
void dump_circles(int iterno)
{
    char fname[64];
    snprintf(fname, sizeof(fname), "mpi-circles-%05d.gp", iterno);
    FILE *out = fopen(fname, "w");
    const float WIDTH = XMAX - XMIN;
    const float HEIGHT = YMAX - YMIN;
    fprintf(out, "set term png notransparent large\n");
    fprintf(out, "set output \"mpi-circles-%05d.png\"\n", iterno);
    fprintf(out, "set xrange [%f:%f]\n", XMIN - WIDTH * .2, XMAX + WIDTH * .2);
    fprintf(out, "set yrange [%f:%f]\n", YMIN - HEIGHT * .2, YMAX + HEIGHT * .2);
    fprintf(out, "set size square\n");
    fprintf(out, "plot '-' with circles notitle\n");
    for (int i = 0; i < ncircles; i++)
    {
        fprintf(out, "%f %f %f\n", circles[i].x, circles[i].y, circles[i].r);
    }
    fprintf(out, "e\n");
    fclose(out);
}
#endif

int main(int argc, char *argv[])
{
    int n = 10000;
    int iterations = 20;

    if (argc > 3)
    {
        fprintf(stderr, "Usage: %s [ncircles [iterations]]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (argc > 1)
    {
        n = atoi(argv[1]);
    }

    if (argc > 2)
    {
        iterations = atoi(argv[2]);
    }

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        init_circles(n);
    }

    MPI_Bcast(&ncircles, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank != 0)
    {
        circles = (circle_t *)malloc(ncircles * sizeof(*circles));
    }
    MPI_Bcast(circles, ncircles * sizeof(circle_t), MPI_BYTE, 0, MPI_COMM_WORLD);

    const double tstart_prog = hpc_gettime();
#ifdef MOVIE
    dump_circles(0);
#endif
    for (int it = 0; it < iterations; it++)
    {
        const double tstart_iter = hpc_gettime();
        reset_displacements();

        int start = (rank * ncircles) / size;
        int end = ((rank + 1) * ncircles) / size;

        int local_overlaps = compute_forces(start, end);

        int total_overlaps;
        MPI_Reduce(&local_overlaps, &total_overlaps, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        // Gathering the displacements from all processes
        MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL, circles, ncircles / size * sizeof(circle_t), MPI_BYTE, MPI_COMM_WORLD);

        move_circles();

        // Broadcasting updated positions to all processes
        MPI_Bcast(circles, ncircles * sizeof(circle_t), MPI_BYTE, 0, MPI_COMM_WORLD);

        const double elapsed_iter = hpc_gettime() - tstart_iter;
        if (rank == 0)
        {
            printf("Iteration %d of %d, %d overlaps (%f s)\n", it + 1, iterations, total_overlaps, elapsed_iter);
#ifdef MOVIE
            dump_circles(it + 1);
#endif
        }
    }

    const double elapsed_prog = hpc_gettime() - tstart_prog;
    if (rank == 0)
    {
        printf("Elapsed time: %f\n", elapsed_prog);
    }

    free(circles);
    MPI_Finalize();

    return EXIT_SUCCESS;
}
