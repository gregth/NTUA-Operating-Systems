/*
 * mandel.c
 *
 * A program to draw the Mandelbrot Set on a 256-color xterm.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "mandel-lib.h"

#define MANDEL_MAX_ITERATION 100000

// Declare a pointer for semaphore in global context
sem_t* semaphore;
int N_THREADS;

/***************************
 * Compile-time parameters *
 ***************************/

/*
 * Output at the terminal is is x_chars wide by y_chars long
*/
int y_chars = 50;
int x_chars = 90;

/*
 * The part of the complex plane to be drawn:
 * upper left corner is (xmin, ymax), lower right corner is (xmax, ymin)
*/
double xmin = -1.8, xmax = 1.0;
double ymin = -1.0, ymax = 1.0;

/*
 * Every character in the final output is
 * xstep x ystep units wide on the complex plane.
 */
double xstep;
double ystep;

/*
 * This function computes a line of output
 * as an array of x_char color values.
 */
void compute_mandel_line(int line, int color_val[])
{
	/*
	 * x and y traverse the complex plane.
	 */
	double x, y;

	int n;
	int val;

	/* Find out the y value corresponding to this line */
	y = ymax - ystep * line;

	/* and iterate for all points on this line */
	for (x = xmin, n = 0; n < x_chars; x+= xstep, n++) {

		/* Compute the point's color value */
		val = mandel_iterations_at_point(x, y, MANDEL_MAX_ITERATION);
		if (val > 255)
			val = 255;

		/* And store it in the color_val[] array */
		val = xterm_color(val);
		color_val[n] = val;
	}
}

/*
 * This function outputs an array of x_char color values
 * to a 256-color xterm.
 */
void output_mandel_line(int fd, int color_val[])
{
	int i;

	char point ='@';
	char newline='\n';

	for (i = 0; i < x_chars; i++) {
		/* Set the current color, then output the point */
		set_xterm_color(fd, color_val[i]);
		if (write(fd, &point, 1) != 1) {
			perror("compute_and_output_mandel_line: write point");
			exit(1);
		}
	}

	/* Now that the line is done, output a newline character */
	if (write(fd, &newline, 1) != 1) {
		perror("compute_and_output_mandel_line: write newline");
		exit(1);
	}
}

void *safe_malloc(size_t size)
{
	void *p;

	if ((p = malloc(size)) == NULL) {
		fprintf(stderr, "Out of memory, failed to allocate %zd bytes\n",
			size);
		exit(1);
	}

	return p;
}

void compute_and_output_mandel_line(int fd, int line)
{
	/*
	 * A temporary array, used to hold color values for the line being drawn
	 */
	int color_val[x_chars];

	compute_mandel_line(line, color_val);
    // Wait for the corresponding sempahore, then output line
    sem_wait(&semaphore[line % N_THREADS]);
	output_mandel_line(fd, color_val);
    // Increase sempahore of next line
    sem_post(&semaphore[(line + 1) % N_THREADS]);
}



void *thread_function(void* line_arg) {
    int* line_ptr = (int*)line_arg;
    // Compute and  lines line, line  + N_THREADS, line + 2*NTHREADS, etc
    int line;
    for (line = *line_ptr; line < y_chars; line += N_THREADS) {
        compute_and_output_mandel_line(1, line);
    }
    pthread_exit(0);
}

int main(int argc, char* argv[])
{
    // Check if sufficient arguments have been provided
    if (argc != 2) {
        printf("Usage: ./mandel <N_THREADS>\n");
        exit(1);
    }

    N_THREADS = atoi(argv[1]);
    printf("Running for N_THREADS = %d\n", N_THREADS);

    /* Allocate memory space for semaphores, initialize
     * them to 0, excpet for the semaphore[0] wich shoulde
     * be initialized to value 1
     */
    semaphore = safe_malloc(N_THREADS * sizeof(sem_t));
    sem_init(&semaphore[0], 0, 1);
    int i = 0;
    for (i = 1; i < N_THREADS; i++) {
        sem_init(&semaphore[i], 0, 0);
    }

	int line;

	xstep = (xmax - xmin) / x_chars;
	ystep = (ymax - ymin) / y_chars;

    // Allocate space for threads type
    pthread_t *threads = safe_malloc(N_THREADS * sizeof(pthread_t));
    int* args = safe_malloc(N_THREADS * sizeof(int));


	/*
	 * draw the Mandelbrot Set, one line at a time.
	 * Output is sent to file descriptor '1', i.e., standard output.
	 */

    // Create N_THREADS
	for (line = 0; line < N_THREADS; line++) {
        args[line] = line;
        pthread_create(&threads[line], NULL, thread_function, &args[line]);
	}

	for (line = 0; line < N_THREADS; line++) {
        pthread_join(threads[line], NULL);
    }

	reset_xterm_color(1);
	return 0;
}
