time ( seq 10000 | ./build/numstore write -f out.vec )
./build/numstore info -f out.vec
./build/numstore read -f out.vec > /dev/null

