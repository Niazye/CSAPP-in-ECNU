/*
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void solve_32(int M, int N, int A[N][M], int B[M][N]);
void solve_64(int M, int N, int A[N][M], int B[M][N]);
void solve_61(int M, int N, int A[N][M], int B[M][N]);
/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    /* First, check the interval between A and B */

    /* The A and B matrix is defined in tracegen.c */
    /* And then it calls the transpose function which in trans.c (here) */
    /* Since A is defined as int[256][256] */
    /* The address difference between B and A is 256*256*4 == 262144 = 0x40000 bytes */
    /* Also */
    /* Run test-trans with the sample function 'trans' below here and check the file 'trace.f0' */
    /* The difference between the addresses of first L instruction and first S instruction is 0x40000, too */

    /* That means, A and B share the same cache set and cache line for corresponding elements */
    /* The cache set and line of A[i][j] and B[i][j] are the same for all (i, j) */
    if (M == 32)
    {
        solve_32(M, N, A, B);
    }
    else if (M == 64)
    {
        solve_64(M, N, A, B);
    }
    else
    {
        solve_61(M, N, A, B);
    }
}
/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

void solve_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int tmp1, tmp2, tmp3, tmp4;
    int tmp5, tmp6, tmp7, tmp8;
    for (k = 0; k < 4; k++)
    {
        for (j = 0; j < 4; j++)
        {
            for (i = 0; i < 8; i++)
            {
                tmp1 = A[i + k * 8][j * 8 + 0];
                tmp2 = A[i + k * 8][j * 8 + 1];
                tmp3 = A[i + k * 8][j * 8 + 2];
                tmp4 = A[i + k * 8][j * 8 + 3];
                tmp5 = A[i + k * 8][j * 8 + 4];
                tmp6 = A[i + k * 8][j * 8 + 5];
                tmp7 = A[i + k * 8][j * 8 + 6];
                tmp8 = A[i + k * 8][j * 8 + 7];
                B[j * 8 + 0][i + k * 8] = tmp1;
                B[j * 8 + 1][i + k * 8] = tmp2;
                B[j * 8 + 2][i + k * 8] = tmp3;
                B[j * 8 + 3][i + k * 8] = tmp4;
                B[j * 8 + 4][i + k * 8] = tmp5;
                B[j * 8 + 5][i + k * 8] = tmp6;
                B[j * 8 + 6][i + k * 8] = tmp7;
                B[j * 8 + 7][i + k * 8] = tmp8;
            }
        }
    }
}
void solve_64(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k;
    int tmp1, tmp2, tmp3, tmp4;
    int tmp5, tmp6, tmp7, tmp8;
    /* If be the same with solve_32: 8x8 chunks which have a size of 8x8 ints*/
    /* We will get 4000+ misses*/
    /* Since the array line is 64 ints which is 256 bytes */
    /* Just 4 array lines can be loaded in cache */
    /* If the chunks size is 8x8, the i-th line and i+4-th line will be conflicted */
    /* Maybe we can use 16x16 chunks which have a size of 4x4 ints */
    /* The misses decreased, about 1700, but not enough to be optimal */

    /* The 1700 misses ver.*/
    /*
    for (k = 0; k < 16; k++)
    {
        for (j = 0; j < 16; j++)
        {
            for (i = 0; i < 4; i++)
            {
                tmp1 = A[i + k * 4][j * 4 + 0];
                tmp2 = A[i + k * 4][j * 4 + 1];
                tmp3 = A[i + k * 4][j * 4 + 2];
                tmp4 = A[i + k * 4][j * 4 + 3];
                B[j * 4 + 0][i + k * 4] = tmp1;
                B[j * 4 + 1][i + k * 4] = tmp2;
                B[j * 4 + 2][i + k * 4] = tmp3;
                B[j * 4 + 3][i + k * 4] = tmp4;
            }
        }
    }
    */

    /* Compare the 4x4 chunk and 8x8 chunk */
    /* Every cache line have a block size of 32 bytes */
    /* But chunk size is 4x4 ints, or 16x16 bytes */
    /* The cache line is not fully used */
    /* For a single cache line, we just access its first 16 bytes, Then it will be evicted */
    /* The last 16 bytes are not used */
    /* It will be loaded again in the next access */
    /* That will cause a lot of misses */

    /* We can load 8 ints (A 8x8 chunk line) at once */
    /* Put them in no-conflict cache lines (no more than 4 consecutive line in B) */
    /* When a cache line will be evicted, load and use the last 4 ints at the same time */
    /* Then the line will not be accessed another time */
    /* So it will not cause more misses */

    for (i = 0; i < 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            /* For each 8x8 chunk, transpose it according to the following steps */
            for (k = 0; k < 4; k++)
            {
                /* transpose the left-up subchunk */
                tmp1 = A[i * 8 + k][j * 8 + 0];
                tmp2 = A[i * 8 + k][j * 8 + 1];
                tmp3 = A[i * 8 + k][j * 8 + 2];
                tmp4 = A[i * 8 + k][j * 8 + 3];
                B[j * 8 + 0][i * 8 + k] = tmp1;
                B[j * 8 + 1][i * 8 + k] = tmp2;
                B[j * 8 + 2][i * 8 + k] = tmp3;
                B[j * 8 + 3][i * 8 + k] = tmp4;
            }
            for (k = 0; k < 4; k++)
            {
                /* copy right-up subchunk int A to right-up subchunk in B with tranpose */
                /* So that we will not access the same cache line twice */
                tmp1 = A[i * 8 + k][j * 8 + 4];
                tmp2 = A[i * 8 + k][j * 8 + 5];
                tmp3 = A[i * 8 + k][j * 8 + 6];
                tmp4 = A[i * 8 + k][j * 8 + 7];
                B[j * 8 + 0][i * 8 + k + 4] = tmp1;
                B[j * 8 + 1][i * 8 + k + 4] = tmp2;
                B[j * 8 + 2][i * 8 + k + 4] = tmp3;
                B[j * 8 + 3][i * 8 + k + 4] = tmp4;
            }
            for (k = 0; k < 4; k++)
            {
                /* Get the left-down subchunk in A and the right-up subchunk in B */
                /* And move them to correct position in B */
                tmp1 = A[i * 8 + 4][j * 8 + k];
                tmp2 = A[i * 8 + 5][j * 8 + k];
                tmp3 = A[i * 8 + 6][j * 8 + k];
                tmp4 = A[i * 8 + 7][j * 8 + k];
                tmp5 = B[j * 8 + k][i * 8 + 4];
                tmp6 = B[j * 8 + k][i * 8 + 5];
                tmp7 = B[j * 8 + k][i * 8 + 6];
                tmp8 = B[j * 8 + k][i * 8 + 7];
                B[j * 8 + k][i * 8 + 4] = tmp1;
                B[j * 8 + k][i * 8 + 5] = tmp2;
                B[j * 8 + k][i * 8 + 6] = tmp3;
                B[j * 8 + k][i * 8 + 7] = tmp4;
                B[j * 8 + k + 4][i * 8 + 0] = tmp5;
                B[j * 8 + k + 4][i * 8 + 1] = tmp6;
                B[j * 8 + k + 4][i * 8 + 2] = tmp7;
                B[j * 8 + k + 4][i * 8 + 3] = tmp8;
            }
            for (k = 0; k < 4; k++)
            {
                /* Transpose the right-down subchunk */
                tmp1 = A[i * 8 + 4][j * 8 + k + 4];
                tmp2 = A[i * 8 + 5][j * 8 + k + 4];
                tmp3 = A[i * 8 + 6][j * 8 + k + 4];
                tmp4 = A[i * 8 + 7][j * 8 + k + 4];
                B[j * 8 + k + 4][i * 8 + 4] = tmp1;
                B[j * 8 + k + 4][i * 8 + 5] = tmp2;
                B[j * 8 + k + 4][i * 8 + 6] = tmp3;
                B[j * 8 + k + 4][i * 8 + 7] = tmp4;
            }
        }
    }
}
void solve_61(int M, int N, int A[N][M], int B[M][N])
{
    /* Just try different chunk size */
    /* 4x4 chunks have 2426 misses */
    /* 8x8 chunks have 2216 misses */
    /* 16x16 chunks have 2003 misses */
    /* 17x17 chunks have 1971 misses */
    int i, j, k;
    int m;
    for (k = 0; k < 5; k++)
    {
        for (j = 0; j < 5; j++)
        {
            for (i = 0; i < 17; i++)
            {
                for (m = 0; m < 17; m++)
                {
                    if (k * 17 + i < M && j * 17 + m < N)
                        B[k * 17 + i][j * 17 + m] = A[j * 17 + m][k * 17 + i];
                }
            }
        }
    }
}
/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
