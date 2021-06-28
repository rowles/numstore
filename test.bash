echo "write_chunked"
seq 500 | ./build/numstore write_chunked -f out2.vec

echo "read_chunked"
./build/numstore read_chunked -f out2.vec
#./build/numstore info -f out.vec
#./build/numstore read -f out.vec > /dev/null

