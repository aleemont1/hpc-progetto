#!/bin/sh

# Questo script exegue il programma omp-matmul sfruttando OpenMP con
# un numero di core variabile da 1 al numero di core disponibili sulla
# macchina (estremi inclusi); ogni esecuzione considera sempre la
# stessa dimensione dell'input, quindi i tempi misurati possono essere
# usati per calcolare speedup e strong scaling efficiency. Ogni
# esecuzione viene ripetuta 5 volte; vengono stampati a video i tempi
# di esecuzione di tutte le esecuzioni.

# NB: La dimensione del problema (PROB_SIZE, cioè il numero di righe o
# colonne della matrice) può essere modificato per ottenere dei tempi
# di esecuzione adeguati alla propria macchina.  Idealmente, sarebbe
# utile che i tempi di esecuzione non fossero troppo brevi, dato che
# tempi brevi tendono ad essere molto influenzati dall'overhead di
# OpenMP.

# Ultimo aggiornamento 2023-10-04
# Moreno Marzolla (moreno.marzolla@unibo.it)

PROG=./src/omp-circles

if [ ! -f "$PROG" ]; then
    echo
    echo "Non trovo il programma $PROG."
    echo
    exit 1
fi

echo "p\tt1\tt2\tt3\tt4\tt5"

PROB_SIZE=10000 # default problem size
CORES=`cat /proc/cpuinfo | grep processor | wc -l` # number of cores

for p in `seq $CORES`; do
    echo -n "$p\t"
    for rep in `seq 5`; do
        EXEC_TIME="$( OMP_NUM_THREADS=$p "$PROG" $PROB_SIZE 100 | grep "Elapsed time" | sed 's/Elapsed time: //' )"
        echo -n "${EXEC_TIME}\t"
    done
    echo ""
done
