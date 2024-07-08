#!/bin/sh

# Questo script esegue il programma omp-circles sfruttando OpenMP con
# un numero di core da 1 al numero di core disponibili sulla macchina
# (estremi inclusi). Il test con p processori viene effettuato su un
# input che ha dimensione N0 * (p^(1/2)), dove N0 e' la dimensione
# dell'input con p=1 thread OpenMP.
#
# Per come è stato implementato il programma parallelo, questo
# significa che all'aumentare del numero p di thread OpenMP, la
# dimensione del problema viene fatta crescere in modo che la quantità
# di lavoro per thread resti costante.
#
#-----------------------------------------------------------------------
# ATTENZIONE: il calcolo sopra vale solo per il programma omp-circles;
# in generale non è detto che la dimensione dell'input debba crescere
# come la radice cubica del numero di core. Il calcolo andrà
# modificato in base all'effettivo costo asintotico dell'algoritmo da
# misurare, come spiegato a lezione.
#-----------------------------------------------------------------------

# Ultimo aggiornamento 2023-10-04
# Moreno Marzolla (moreno.marzolla@unibo.it)

PROG=./src/mpi-circles

if [ ! -f "$PROG" ]; then
    echo
    echo "Non trovo il programma $PROG."
    echo
    exit 1
fi

echo "n\tp\tt1\tt2\tt3\tt4\tt5"

N0=1000 # base problem size
CORES=`cat /proc/cpuinfo | grep processor | wc -l` # number of cores

for p in `seq $CORES`; do
    
    #La complessita' computazionale dell'algoritmo e' O(N^2) 
    PROB_SIZE=`echo "scale=3; sqrt($p) * $N0" | bc -q`
    echo -n "$PROB_SIZE\t"
    echo -n "$p\t"
    for rep in `seq 5`; do
        EXEC_TIME="$( mpirun -n $p "$PROG" $PROB_SIZE 100 2>&1| sed '/No protocol specified/d' | grep "Elapsed time" | sed 's/Elapsed time: //' )"
        echo -n "${EXEC_TIME}\t"
    done
    echo ""
done
