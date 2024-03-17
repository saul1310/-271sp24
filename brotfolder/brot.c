#include <stdlib.h>
#include <stdio.h>
#include <complex.h>

// Create a 3-dimensional unsigned char array of size * size * 3 to hold pixel data
unsigned char ***create_base(int size)
{
    unsigned char ***base;
    int i, j;

    // Allocate memory for the first dimension
    base = (unsigned char ***)malloc(size * sizeof(unsigned char **));
    if (base == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for the second dimension
    for (i = 0; i < size; i++) {
        base[i] = (unsigned char **)malloc(size * sizeof(unsigned char *));
        if (base[i] == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
        
        // Allocate memory for the third dimension
        for (j = 0; j < size; j++) {
            base[i][j] = (unsigned char *)malloc(3 * sizeof(unsigned char));
            if (base[i][j] == NULL) {
                fprintf(stderr, "Memory allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    return base;
}

// Calculate z_(n+1) = z_n^2 + c and return the result
double complex m_seq(double complex z_n, double complex c)
{
    return z_n * z_n + c;
}

// Convert a complex value to pixel coordinates on the image based on the size
void c2b(double complex c, int size, int *x, int *y)
{
    // Scale the real and imaginary parts of c to fit within the image size
    double real = creal(c);
    double imag = cimag(c);

    // Map the scaled values to pixel coordinates
    *x = (int)((real + 2.0) / 4.0 * size);
    *y = (int)((imag + 2.0) / 4.0 * size);

    // Clamp the coordinates to be within the image bounds
    *x = (*x < 0) ? 0 : (*x >= size) ? size - 1 : *x;
    *y = (*y < 0) ? 0 : (*y >= size) ? size - 1 : *y;
}

// Convert pixel coordinates to a complex number in the complex plane
double complex b2c(int size, int x, int y)
{
    // Scale the pixel coordinates to fit within the range [-2, 2] in the complex plane
    double real = ((double)x / size) * 4.0 - 2.0;
    double imag = ((double)y / size) * 4.0 - 2.0;

    return real + imag * I;
}

// Determine whether a given complex number escapes to infinity after a certain number of iterations
int escapes(double complex c, int iters)
{
    double complex z = 0.0;
    int i;
    for (i = 0; i < iters; i++) {
        z = m_seq(z, c);
        if (cabs(z) > 2.0)
            return 1; // Escapes to infinity
    }
    return 0; // Does not escape
}

// Update the pixel value of a single point in the Buddhabrot image based on the number of iterations it takes for a complex number to escape
void one_val(unsigned char ***base, int size, int iters, int color, double complex c)
{
    int color_val;
    if (escapes(c, iters)) {
        int x, y;
        c2b(c, size, &x, &y);
        // Increase the pixel value corresponding to the escape point
        color_val = base[x][y][color];
        color_val = color_val + 100; // Increased increment value
        base[x][y][color] = color_val;
    }
}

// Generate the colors for the Buddhabrot image
void get_colors(unsigned char ***base, int size, int iters)
{
    // Iterate over all pixels in the image
    int x, y;
    for (x = 0; x < size; x++) {
        for (y = 0; y < size; y++) {
            // Calculate the color intensity based on the number of escapes
            int intensity = base[x][y][0] + base[x][y][1] + base[x][y][2];
            // Map the intensity to a value between 0 and 255 for each color channel
            unsigned char r = (unsigned char)(255 * base[x][y][0] / intensity);
            unsigned char g = (unsigned char)(255 * base[x][y][1] / intensity);
            unsigned char b = (unsigned char)(255 * base[x][y][2] / intensity);
            // Set the color values
            base[x][y][0] = r;
            base[x][y][1] = g;
            base[x][y][2] = b;
        }
    }
}

// Equalize the pixel values in the image to improve its visual quality (optional)
void equalize(unsigned char ***base, int size)
{
    // This function can be implemented to equalize the pixel values in the image
    // using histogram equalization or other techniques.
    // However, implementing it is beyond the scope of this example.
    // You may refer to image processing libraries for implementations of histogram equalization.
}

// Given an edge size and starting iteration count, make a Buddhabrot.
void make_brot(int size, int iters)
{
    unsigned char ***base = create_base(size);

    // Iterate over all pixels in the image
    int x, y;
    for (x = 0; x < size; x++) {
        for (y = 0; y < size; y++) {
            double complex c = b2c(size, x, y);
            one_val(base, size, iters, 0, c);
        }
    }

    get_colors(base, size, iters);

    // Save the image to a file
    FILE *fp = fopen("brot.ppm", "wb");
    fprintf(fp, "P6\n%d %d\n255\n", size, size);
    for (x = 0; x < size; x++) {
        for (y = 0; y < size; y++) {
            fwrite(base[x][y], 1, 3, fp);
        }
    }
    fclose(fp);

    // Free the memory allocated for the image
    for (x = 0; x < size; x++) {
        for (y = 0; y < size; y++) {
            free(base[x][y]);
        }
        free(base[x]);
    }
    free(base);
}

int main()
{
    make_brot(8000, 100); // Increased resolution and iteration count
    return 0;
}
