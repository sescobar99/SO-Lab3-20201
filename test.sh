#!/bin/bash
#run "chmod u+x test.sh"
echo "gcc -Wall saxpy-v0.c -o saxpy-v0.out -lpthread"
gcc -Wall saxpy-v0.c -o saxpy-v0.out -lpthread

echo "gcc -Wall saxpy.c -o saxpy.out -lpthread"
gcc -Wall saxpy.c -o saxpy.out -lpthread


GREEN='\033[0;32m'
NC='\033[0m'
printf "${GREEN}Please be patient, this may take a while...${NC}\n"

# 10 iteraciones para -i 1 -n 1  version paralela y secuencial
printf "${GREEN}Doing 10 iterations for -i 1 -n 1 (parallel and sequential)${NC}\n"
for i in {1..10}; do
    echo "Repeticion $i" >> "SAXPY_T_i1_n1.txt"
    ./saxpy-v0.out -i 1 -n 1 >> "SAXPY_T_i1_n1.txt"
    echo "" >> "SAXPY_T_i1_n1.txt"

    echo "Repeticion $i" >> "SAXPY_NT_i1.txt"
    ./saxpy.out -i 1 -n 1 >> "SAXPY_NT_i1.txt"
    echo "" >> "SAXPY_NT_i1.txt"
done


# 10 iteraciones para -i 100 -n 1 version paralela y secuencial
printf "${GREEN}Doing 10 iterations for -i 100 -n 1 (parallel and sequential)${NC}\n"
for i in {1..10}; do
    echo "Repeticion $i" >> "SAXPY_T_i100_n1.txt"
    ./saxpy-v0.out -i 100 -n 1 >> "SAXPY_T_i100_n1.txt"
    echo "" >> "SAXPY_T_i100_n1.txt"

    echo "Repeticion $i" >> "SAXPY_NT_i100.txt"
    ./saxpy.out -i 100 -n 1 >> "SAXPY_NT_i100.txt"
    echo "" >> "SAXPY_NT_i100.txt"
done


# 10 iteraciones para -i 100 -n [2,4,8]
printf "${GREEN}Doing 10 iterations for -i 100 -n [2,4,8]${NC}\n"
for i in {1..10}; do
    echo "Repeticion $i" >> "SAXPY_T_i100_n2.txt"
    ./saxpy-v0.out -i 100 -n 2 >> "SAXPY_T_i100_n2.txt"
    echo "" >> "SAXPY_T_i100_n2.txt"

    echo "Repeticion $i" >> "SAXPY_T_i100_n4.txt"
    ./saxpy-v0.out -i 100 -n 4 >> "SAXPY_T_i100_n4.txt"
    echo "" >> "SAXPY_T_i100_n4.txt"

    echo "Repeticion $i" >> "SAXPY_T_i100_n8.txt"
    ./saxpy-v0.out -i 100 -n 8 >> "SAXPY_T_i100_n8.txt"
    echo "" >> "SAXPY_T_i100_n8.txt"
done

printf "${GREEN}...Maybe more than just a while...${NC}\n"

# 5 iteraciones para -i 1000 -n 4 version paralela y secuencial
printf "${GREEN}Doing 5 iterations for -i 1000 -n 4 (parallel and sequential = -n 1)${NC}\n"
for i in {1..5}; do
    echo "Repeticion $i" >> "SAXPY_T_i1000_n4.txt"
    ./saxpy-v0.out -i 1000 -n 4 >> "SAXPY_T_i1000_n4.txt"
    echo "" >> "SAXPY_T_i1000_n4.txt"

    echo "Repeticion $i" >> "SAXPY_NT_i1000.txt"
    ./saxpy.out -i 1000 -n 4 >> "SAXPY_NT_i1000.txt"
    echo "" >> "SAXPY_NT_i1000.txt"
done

# Desarrolle un informe donde evalue el desempeño su versión paralela,
#  tomando el tiempo de ejecución del programa utilizando varios hilos, y desarrollando varias repeticiones (mínimo 10 por caso), grafique los resultados y analice las gráficas.



printf "${GREEN}Done${NC}\n"
