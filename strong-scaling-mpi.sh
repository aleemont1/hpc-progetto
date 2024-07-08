#!/bin/sh

# Questo script exegue il programma omp-circles sfruttando OpenMP con
# un numero di core variabile da 1 al numero di core disponibili sulla
# macchina (estremi inclusi); ogni esecuzione considera sempre la
# stessa dimensione dell'input, quindi i tempi misurati possono essere
# usati per calcolare speedup e strong scaling efficiency. Ogni
# esecuzione viene ripetuta 5 volte; vengono stampati a video i tempi
# di esecuzione di tutte le esecuzioni.

# NB: La dimensione del problema (PROB_SIZE, cioè il numero di cerchi)
# può essere modificato per ottenere dei tempi di esecuzione adeguati alla propria macchina.
# Idealmente, sarebbe utile che i tempi di esecuzione non fossero troppo brevi, dato che
# tempi brevi tendono ad essere molto influenzati dall'overhead di OpenMP.
#
# NOTE: This script assumes that your program prints a string in this format at some point:
# "Elapsed time: VALUE\n"
# You can change this behavior by changing the calculation of the EXEC_TIME variable.
#
# Ultimo aggiornamento 2023-27-01
# Alessandro Monticelli (alessandr.monticell4@studio.unibo.it)
# This is a modified version of the script provided by professor Marzolla for the HPC course
# a.a. 2023/24 @ "Alma Mater Studiorum - University of Bologna"
# This software comes with NO WARRANTY and is delivered AS IS.
#
# Credits: prof. Moreno Marzolla (moreno.marzolla@unibo.it)
PROG=./mpi-circles

if [ ! -f "$PROG" ]; then
    echo
    echo "Non trovo il programma $PROG."
    echo
    exit 1
fi

echo "p\tt1\tt2\tt3\tt4\tt5"

PROB_SIZE=5000 # default problem size
CORES=`cat /proc/cpuinfo | grep processor | wc -l` # number of cores

for p in `seq $CORES`; do
    echo -n "$p\t"
    for rep in `seq 5`; do
        EXEC_TIME="$( mpirun -n $p "$PROG" $PROB_SIZE 100 2>&1 | sed '/No protocol specified/d' | grep "Elapsed time" | sed 's/Elapsed time: //' )"
        echo -n "${EXEC_TIME}\t"
    done
    echo ""
done
