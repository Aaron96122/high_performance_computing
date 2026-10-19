#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <complex.h>

#define MAX_ATTR_VALUES 12
#define MAX_ITERATIONS 50
#define PIXEL_TEXT_LEN 12
#define PIXEL_TEXT_SIZE (PIXEL_TEXT_LEN + 1)

static char attr_pixel_strings[MAX_ATTR_VALUES][PIXEL_TEXT_SIZE];
static char conv_pixel_strings[MAX_ITERATIONS + 1][PIXEL_TEXT_SIZE];
const unsigned int platte[MAX_ATTR_VALUES][3] =
{
    {255,   0,   0},  // 0: red
    {  0, 255,   0},  // 1: green
    {  0,   0, 255},  // 2: blue
    {255, 255,   0},  // 3: yellow
    {255,   0, 255},  // 4: magenta
    {  0, 255, 255},  // 5: cyan
    {255, 128,   0},  // 6: orange
    {128,   0, 255},  // 7: purple
    {  0, 128, 128},  // 8: teal
    {128, 128,   0},  // 9: olive
    {255, 255, 255},  // 10: white
    {  0,   0,   0}   // 11: black
};
typedef uint8_t TYPE_ATTR;
typedef uint8_t TYPE_CONV;

typedef struct
{
    double r;
    double i;
} Point;
static int check_attractors(Point x, int degree, const Point *roots);
static Point newton_step(Point x, int degree);

typedef struct 
{
    int val;
    char pad[56];
} int_padded;

typedef struct 
{
    const Point **v;
    const Point *roots;
    TYPE_ATTR **attractors;
    TYPE_CONV **convergences;
    int ib;
    int istep;
    int degree;
    int sz;
    int tx;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_t;

typedef struct 
{
    // const Point **v;
    TYPE_ATTR **attractors;
    TYPE_CONV **convergences;
    int sz;
    int nthrds;
    int exp;
    pthread_mutex_t *mtx;
    pthread_cond_t *cnd;
    int_padded *status;
} thrd_info_check_t;

void * main_thrd(void *args)
{
    const thrd_info_t *thrd_info = (thrd_info_t*)args;
    const Point **v = thrd_info->v;
    TYPE_ATTR **attractors = thrd_info->attractors;
    TYPE_CONV **convergences = thrd_info->convergences;
    const int ib = thrd_info->ib;
    const int istep = thrd_info->istep;
    const int sz = thrd_info->sz;
    const int tx = thrd_info->tx;
    const Point *roots = thrd_info->roots;
    const int degree = thrd_info->degree;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    for (int ix=ib; ix<sz; ix+=istep)
    {
        const Point *vix = v[ix];
        TYPE_ATTR *attractors_line = malloc(sz*sizeof(TYPE_ATTR));
        TYPE_CONV *convergences_line = malloc(sz*sizeof(TYPE_CONV));
        if (attractors_line == NULL || convergences_line == NULL)
        {
            fprintf(stderr, "failed to allocate memory for attractors_line and/or convergences_line");
            exit(1);
        }
        for (int jx=0; jx<sz; jx++)
        {
            Point x = vix[jx];
            TYPE_CONV conv = 0;
            TYPE_ATTR attr = (TYPE_ATTR)(degree + 2);
            // check whether x has reached a stopping condition
            for (;;)
            {
                // check whether x has reached a stopping condition.
                int result = check_attractors(x, degree, roots);
                if (result >=0)
                {
                    attr = (TYPE_ATTR)result;
                    break;
                }
                // check whether the iteration has reached 50 times
                if (conv >= MAX_ITERATIONS)
                {
                    break;
                }
                // otherwise, we perform newton update
                x = newton_step(x, degree);
                conv++;
            }
            attractors_line[jx] = attr;
            convergences_line[jx] = conv;
        }
        pthread_mutex_lock(mtx);
        attractors[ix] = attractors_line;
        convergences[ix] = convergences_line;
        status[tx].val = ix + istep;
        pthread_mutex_unlock(mtx);
        pthread_cond_signal(cnd);
    }
    return NULL;
}

void *main_thrd_check(void *args)
{
    const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
    // const Point **v = thrd_info->v;
    TYPE_ATTR **attractors = thrd_info->attractors;
    TYPE_CONV **convergences = thrd_info->convergences;
    const int sz = thrd_info->sz;
    const int nthrds = thrd_info->nthrds;
    const int exp = thrd_info->exp;
    pthread_mutex_t *mtx = thrd_info->mtx;
    pthread_cond_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    // here we prepare the filenames.
    char attr_filenames[128];
    char conv_filenames[128];
    snprintf(
        attr_filenames,
        sizeof(attr_filenames),
        "newton_attractions_x%d.ppm",
        exp
    );
    snprintf(
        conv_filenames,
        sizeof(conv_filenames),
        "newton_convergence_x%d.ppm",
        exp
    );
    // here we open the output files
    FILE *attr_file = fopen(attr_filenames, "w");
    FILE *conv_file = fopen(conv_filenames, "w");
    if (attr_file == NULL || conv_file == NULL)
    {
        perror("Cannot open the output file\n");
        if (attr_file != NULL)
            fclose(attr_file);
        if (conv_file != NULL)
            fclose(conv_file);
        return NULL;
    }
    // here we write the PPM headers
    fprintf(attr_file, "P3\n%d %d\n255\n", sz, sz);
    fprintf(conv_file, "P3\n%d %d\n255\n", sz, sz);

    for (int ix=0, ibnd; ix<sz;)
    {
        for (pthread_mutex_lock(mtx); ;)
        {
            ibnd = sz;
            for (int tx=0; tx<nthrds; tx++)
            {
                if (ibnd > status[tx].val)
                    ibnd = status[tx].val;
            }
            if (ibnd <= ix)
                pthread_cond_wait(cnd, mtx);
            else
            {
                pthread_mutex_unlock(mtx);
                break;
            }
        }
        for (; ix<ibnd; ix++)
        {
            TYPE_ATTR *attractors_line = attractors[ix];
            TYPE_CONV *convergences_line = convergences[ix];
            // here we write every pixel in the current row
            for (int cx=0; cx<sz; cx++)
            {
                TYPE_ATTR a = attractors_line[cx];
                TYPE_CONV c = convergences_line[cx];
                // here we check each pixel within the range
                assert (a < MAX_ATTR_VALUES && c <= MAX_ITERATIONS);
                // here we write each pixel into file correspondingly
                fwrite(
                    attr_pixel_strings[a],
                    1,
                    PIXEL_TEXT_LEN,
                    attr_file
                );
                fwrite(
                    conv_pixel_strings[c],
                    1,
                    PIXEL_TEXT_LEN,
                    conv_file
                );
            }
            // end each row with line breaker at the end
            fputc('\n', attr_file);
            fputc('\n', conv_file);
            free(attractors_line);
            free(convergences_line);
        }
    }
    fclose(attr_file);
    fclose(conv_file);
    return NULL;
}

int main(int argc, char *argv[])
{
    // here we check the input format and parse the input command line.
    if (argc > 4)
    {
        printf("input format shall be like ./newton -t5 -l1000 7");
    }
    int num_thread = 8;
    int num_lines = 100;
    int exponent = 5;
    for (int i=1; i<argc; i++)
    {
        if (argv[i][1] == 't')
        {
            num_thread = atoi(argv[i] + 2);
        }
        else if (argv[i][1] == 'l')
        {
            num_lines = atoi(argv[i] + 2);
        }
        else
        {
            exponent = atoi(argv[i]);
        }
    }
    if (exponent < 1 || exponent >= 10)
    {
        fprintf(stderr, "degree must be between 1 and 9 inclusive\n");
        exit(1);
    }
    
    // here we precompute the roots given exponent
    Point roots[9];
    const double pi = acos(-1.0);
    for (int k=0; k < exponent; k++)
    {
        double angle = 2.0 * pi * k /exponent;
        roots[k].r = cos(angle);
        roots[k].i = sin(angle);
    }

    // here we prepare the strings using snprintf()
    // here for the attractors:
    for (int a=0; a < MAX_ATTR_VALUES; a++)
    {
        snprintf(
            attr_pixel_strings[a],
            sizeof(attr_pixel_strings[a]),
            "%3u %3u %3u ",
            platte[a][0],
            platte[a][1],
            platte[a][2]
        );
    }
    // here for the convergences
    for (unsigned int c=0; c<=MAX_ITERATIONS; c++)
    {
        unsigned int gray = 255u * c / MAX_ITERATIONS;
        snprintf(
            conv_pixel_strings[c],
            sizeof(conv_pixel_strings[c]),
            "%3u %3u %3u ",
            gray, 
            gray,
            gray
        );
    }

    // here we construct the starting points across the num_lines * num_lines grid 
    TYPE_ATTR **attractors = (TYPE_ATTR**) malloc(num_lines*sizeof(TYPE_ATTR*));
    TYPE_CONV **convergences = (TYPE_CONV**) malloc(num_lines*sizeof(TYPE_CONV*));
    Point **v = (Point**) malloc(num_lines*sizeof(Point*));
    Point *ventries = (Point*) malloc(num_lines*num_lines*sizeof(Point));
    if (v == NULL || ventries == NULL || attractors == NULL || convergences == NULL)
    {
        fprintf(stderr, "failed to allocate input points\n");
        free(v);
        free(ventries);
        free(attractors);
        free(convergences);
        exit(1);
    }
    for (int ix=0; ix<num_lines; ix++)
    {
        v[ix] = ventries + (size_t)ix * num_lines;
        for (int cx=0; cx<num_lines; cx++)
        {
            double real;
            double imag;
            if (num_lines == 1)
            {
                real = 0.0;
                imag = 0.0;
            }
            else
            {
                real = -2.0 + 4.0 * cx / (num_lines - 1);
                imag = 2.0 - 4.0 * ix / (num_lines - 1);
            }
            v[ix][cx].r = real;
            v[ix][cx].i = imag;
        }
    }   
    
    // here we apply threads
    const int nthrds = num_thread;
    pthread_t thrds[nthrds];
    pthread_t thrd_check;
    thrd_info_t thrds_info[nthrds];
    thrd_info_check_t thrd_info_check;
    pthread_mutex_t mtx;
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_t cnd;
    pthread_cond_init(&cnd, NULL);
    int_padded status[nthrds];

    for (int tx=0; tx<nthrds; tx++)
    {
        thrds_info[tx].v = (const Point**) v;
        thrds_info[tx].attractors = attractors;
        thrds_info[tx].convergences = convergences;
        thrds_info[tx].ib = tx;
        thrds_info[tx].roots = roots;
        thrds_info[tx].degree = exponent;
        thrds_info[tx].istep = nthrds;
        thrds_info[tx].sz = num_lines;
        thrds_info[tx].tx = tx;
        thrds_info[tx].mtx = &mtx;
        thrds_info[tx].cnd = &cnd;
        thrds_info[tx].status = status;
        status[tx].val = (tx < num_lines) ? tx : num_lines;

        int r = pthread_create(thrds+tx, NULL, main_thrd, (void*)(thrds_info+tx));
        if (r != 0)
        {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
        // pthread_detach(thrds[tx]);
    }

    {
        // thrd_info_check.v = (const Point**) v;
        thrd_info_check.attractors = attractors;
        thrd_info_check.convergences = convergences;
        thrd_info_check.sz = num_lines;
        thrd_info_check.nthrds = nthrds;
        thrd_info_check.exp = exponent;
        thrd_info_check.mtx = &mtx;
        thrd_info_check.cnd = &cnd;
        thrd_info_check.status = status;

        int r = pthread_create(&thrd_check, NULL, main_thrd_check, (void*)(&thrd_info_check));
        if (r != 0)
        {
            fprintf(stderr, "failed to create thread\n");
            exit(1);
        }
    }

    pthread_join(thrd_check, NULL);
    for (int tx=0; tx<nthrds; tx++)
    {
        pthread_join(thrds[tx], NULL);
    }
    free(ventries);
    free(v);
    free(attractors);
    free(convergences);
}

static int check_attractors(Point x, int degree, const Point *roots)
{
    // 1. check whether the real or the imaginary part is too large
    if (fabs(x.r) > 1.0e5 || fabs(x.i) > 1.0e5)
        return degree;
    double norm2 = x.r * x.r + x.i * x.i;
    // 2. check whether x is too close to the origin
    if (norm2 < 1.0e-6)
        return degree + 1;
    // 3. check whether x is closer to one of the roots.
    for (int k=0; k<degree; k++)
    {
        double dot = x.r * roots[k].r + x.i * roots[k].i;
        double distance2 = norm2 + 1.0 - 2.0 * dot;
        if (distance2 < 1.0e-6)
        {
            return k;
        }
    }
    return -1;
}

static Point newton_step(Point x, int degree)
{
    // conver the point to a C complex number.
    const double complex z = x.r + x.i * I;
    
    double complex next;
    switch (degree)
    {
        case 1:
            next = 1.0;
            break;
        case 2:
            next = (z + 1.0 / z) / 2.0;
            break;
        case 3:
            next = (2.0 * z + 1.0 / (z * z)) / 3.0;
            break;
        case 4:
            next = (3.0 * z + 1.0 / (z * z * z)) / 4.0;
            break;
        case 5:
            next = (4.0 * z + 1.0 / (z * z * z * z)) / 5.0;
            break;
        case 6:
            next = (5.0 * z + 1.0 / (z * z * z * z * z)) / 6.0;
            break;
        case 7:
            next = (6.0 * z + 1.0 / (z * z * z * z * z * z)) / 7.0;
            break;
        case 8:
            next = (7.0 * z + 1.0 / (z * z * z * z * z * z * z)) / 8.0;
            break;
        case 9:
            next = (8.0 * z + 1.0 / (z * z * z * z * z * z * z * z)) / 9.0;
            break;
        default:
            fprintf(stderr, "unexpected degree: %d\n", degree);
            exit(EXIT_FAILURE);
    }
    return (Point) {.r = creal(next), .i=cimag(next)};
}