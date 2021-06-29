echo "write_chunked"
time (seq 5000000 | ./build/numstore write_chunked -f out2.vec)

#shuf -i 0-100000 -n 500000 | ./build/numstore write_chunked -f out2.vec
echo "read_chunked"
time (./build/numstore read_chunked -f out2.vec > /dev/null)
#./build/numstore info -f out.vec
#./build/numstore read -f out.vec > /dev/null

