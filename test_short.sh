#!/bin/bash
#run "chmod u+x test.sh"
echo "gcc -Wall saxpy-v0.c -o saxpy-v0.out -lpthread"
gcc -Wall saxpy-v0.c -o saxpy-v0.out -lpthread

echo "gcc -Wall saxpy.c -o saxpy.out -lpthread"
gcc -Wall saxpy.c -o saxpy.out -lpthread


GREEN='\033[0;32m'
NC='\033[0m'

printf "${GREEN}Doing 10 iterations for -i 1 -n [1_NT,1,2,4,8] -p 10${NC}\n"
for i in {1..10}; do
    echo "Repeticion $i" >> "SAXPY_NT_i1_p10.txt"
    ./saxpy.out -i 1 -n 1 -p 10 >> "SAXPY_NT_i1_p10.txt"
    echo "" >> "SAXPY_NT_i1_p10.txt"

    echo "Repeticion $i" >> "SAXPY_T_i1_n1_p10.txt"
    ./saxpy-v0.out -i 1 -n 1 -p 10 >> "SAXPY_T_i1_n1_p10.txt"
    echo "" >> "SAXPY_T_i1_n1_p10.txt"

    echo "Repeticion $i" >> "SAXPY_T_i1_n2_p10.txt"
    ./saxpy-v0.out -i 1 -n 2 -p 10 >> "SAXPY_T_i1_n2_p10.txt"
    echo "" >> "SAXPY_T_i1_n2_p10.txt"

    echo "Repeticion $i" >> "SAXPY_T_i1_n4_p10.txt"
    ./saxpy-v0.out -i 1 -n 4 -p 10 >> "SAXPY_T_i1_n4_p10.txt"
    echo "" >> "SAXPY_T_i1_n4_p10.txt"

    echo "Repeticion $i" >> "SAXPY_T_i1_n8_p10.txt"
    ./saxpy-v0.out -i 1 -n 8 -p 10 >> "SAXPY_T_i1_n8_p10.txt"
    echo "" >> "SAXPY_T_i1_n8_p10.txt"
done


printf "${GREEN}Done${NC}\n"
