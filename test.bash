seq 100 | ./build/numstore write_chunked -f out2.vec

./build/numstore read_chunked -f out2.vec
#./build/numstore info -f out.vec
#./build/numstore read -f out.vec > /dev/null

