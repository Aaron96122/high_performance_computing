#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <stdint.h>
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
    mtx_t *mtx;
    cnd_t *cnd;
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
    mtx_t *mtx;
    cnd_t *cnd;
    int_padded *status;
} thrd_info_check_t;
static int check_attractors(Point x, int degree, const Point *roots);
static Point newton_step(Point x, int degree);

int main_thrd(void *args)
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
    mtx_t *mtx = thrd_info->mtx;
    cnd_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    for (int ix=ib; ix<sz; ix+=istep)
    {
        const Point *vix = v[ix];
        TYPE_ATTR *attractors_line = malloc(sz*sizeof(TYPE_ATTR));
        TYPE_CONV *convergences_line = malloc(sz*sizeof(TYPE_CONV));
        if (attractors_line == NULL || convergences_line == NULL)
        {
            fprintf(stderr, "failed to allocate memory for attractors_line and convergences_line.\n");
            exit(1);
        }
        
        for (int jx=0; jx<sz; jx++)
        {
            Point x = vix[jx];
            TYPE_CONV conv = 0;
            TYPE_ATTR attr = (TYPE_ATTR)(degree + 2);
            for (;;)
            {
                // check whether x has reached a stopping criteria
                int result = check_attractors(x, degree, roots);
                if (result >=0)
                {
                    attr = (TYPE_ATTR)result;
                    break;
                }
                // check whether the ieration has reached 50 times
                if (conv >= MAX_ITERATIONS)
                {
                    break;
                }
                // otherwise, we perform the newton ieration and update
                x = newton_step(x, degree);
                conv++;
            }
            attractors_line[jx] = attr;
            convergences_line[jx] = conv;
        }
        mtx_lock(mtx);
        attractors[ix] = attractors_line;
        convergences[ix] = convergences_line;
        status[tx].val = ix + istep;
        mtx_unlock(mtx);
        cnd_signal(cnd);
    }
    return 0;
}

int main_thrd_check(void *args)
{
    const thrd_info_check_t *thrd_info = (thrd_info_check_t*) args;
    TYPE_ATTR **attractors = thrd_info->attractors;
    TYPE_CONV **convergences = thrd_info->convergences;
    const int sz = thrd_info->sz;
    const int nthrds = thrd_info->nthrds;
    const int exp = thrd_info->exp;
    mtx_t *mtx = thrd_info->mtx;
    cnd_t *cnd = thrd_info->cnd;
    int_padded *status = thrd_info->status;

    // here we prepare the filenames
    char attr_filenames[32];
    char conv_filenames[32];
    snprintf{
        attr_filenames,
        sizeof(attr_filenames),
        "newton_attractors_x%d.ppm",
        exp
    };
    snprintf{
        conv_filenames,
        sizeof(conv_filenames),
        "newton_convergence_x%d.ppm",
        exp
    };
    FILE *attr_file = fopen(attr_filenames, "w");
    FILE *conv_file = fopen(conv_filenames, "w");
    if (attr_file == NULL || conv_file == NULL)
    {
        fprintf(stderr, "cannot open the output file\n");
        fclose(attr_file);
        fclose(conv_file);
        return -1;
    }
    // here we write the PPM headers
    fprintf(attr_file, "P3\n%d %d\n255\n", sz, sz);
    fprintf(conv_file, "P3\n%d %d\n255\n", sz, sz);

    // here we write the RGB into the file
    size_t row_bytes = (size_t) sz * PIXEL_TEXT_LEN;
    char *attr_row = malloc(row_bytes);
    char *conv_row = malloc(row_bytes);
    if (attr_row == NULL || conv_row == NULL)
    {
        fprintf(stderr, "failed to allocate memory to store pixel values into each row");
        free(attr_row);
        free(conv_row);
        fclose(attr_file);
        fclose(conv_file);
        return -1;
    }
    for (int ix=0, ibnd; ix<sz;)
    {
        for (mtx_lock(mtx); ;)
        {
            ibnd = sz;
            for (int tx=0; tx<nthrds; tx++)
            {
                if (ibnd > status[tx].val)
                    ibnd = status[tx].val;
            }
            if (ibnd <= ix)
                cnd_wait(cnd, mtx);
            else
            {
                mtx_unlock(mtx);
                break;
            }
        }
        for (; ix<ibnd; ix++)
        {
            TYPE_ATTR *attractors_line = attractors[ix];
            TYPE_CONV *convergences_line = convergences[ix];
            for (int cx=0; cx<sz; cx++)
            {
                TYPE_ATTR a = attractors_line[cx];
                TYPE_CONV c = convergences_line[cx];
                if (a >= MAX_ATTR_VALUES || c > MAX_ITERATIONS)
                    return -1;
                memcpy(attr_row + (size_t)cx * PIXEL_TEXT_LEN,
                        attr_pixel_strings[a], PIXEL_TEXT_LEN);
                memcpy(conv_row + (size_t)cx * PIXEL_TEXT_LEN,
                        conv_pixel_strings[c], PIXEL_TEXT_LEN);
            }
            fwrite(attr_row, 1, row_bytes, attr_file);
            fwrite(conv_row, 1, row_bytes, conv_file);
            fputc('\n', attr_file);
            fputc('\n', conv_file);
            free(attractors_line);
            free(convergences_line);
        }
    }
    fclose(attr_file);
    fcloase(conv_file);
    return 0;
}
