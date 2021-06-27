

file_header
---
total_num_elements uint_64t
num_chunks         uint_64t

chunk_header
---
offset_pos         uint_64t
total_bytes        uint_64t
num_elements       uint_64t

chunk_data
---
data         unsigned char*



file layout
---
header
chunk
chunk
...
chunk
