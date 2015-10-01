# Drawing-Teapot-from-triangles
Sorting vertices of the triangle by bubble sort and interpolate Z to draw each pixel  with least Z

The .asc files contains vertices of the triangles with NormalX, NormalY, NormalZ, U and V values.
Create a new renderer, initialize with default values.
Sort the vertices by Y using bubble sort technique.
Interpolate Z at each pixel and solve for Ax + By + Cz + D = 0.
Compare Zpix with Zfb(framebuffer), lower value wins and write the same to framebuffer.
Output can be seen in the ppm file
