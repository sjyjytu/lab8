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
#define step 8
/*
name:袁健勇
ID:ics517021910930
*/

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans32(int M, int N, int A[N][M], int B[M][N]);
void trans6167(int M, int N, int A[N][M], int B[M][N]);
void trans(int M, int N, int A[N][M], int B[M][N]);
void trans64(int M, int N, int A[N][M], int B[M][N]);

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
    if (M == 32 && N == 32) {
        trans32(M, N, A, B);
    }
    else if (M == 64 && N == 64)
    {
        trans64(M, N, A, B);
    }
    else if ((M == 61 && N == 67) || (M == 67 && N == 61))
    {
        trans6167(M, N, A, B);
    }
    else
    {
        trans(M, N, A, B);
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

void trans32(int M, int N, int A[N][M], int B[M][N])
{
    int k, i, j , l;
    int tmp[8];
    for (k = 0; k < 4;k++)
    {
        for(l = 0; l < 4; l++)
        {
            for (i = k * 8; i < k * 8 + 8; i++)
            {
                for(j = l * 8; j < l * 8 + 8; j++)
                {
                    tmp[j - l * 8] = A[i][j];
                }
                for(j = l * 8; j < l * 8 + 8; j++)
                {
                    B[j][i] = tmp[j - l * 8];
                }
            }
            
        }
    }
    
}

void trans6167(int M, int N, int A[N][M], int B[M][N])
{
    int k, i, j , l;
    int tmp[8];
    for (k = 0; k < N / step + 1;k++)
    {
        for(l = 0; l < M / step + 1; l++)
        {
            for (i = k * step; i < k * step + step && i < N; i++)
            {
                for(j = l * step; j < l * step + step && j < M; j++)
                {
                    tmp[j - l * step] = A[i][j];
                }
                for(j = l * step; j < l * step + step && j < M; j++)
                {
                    B[j][i] = tmp[j - l * step];
                }
            }
            
        }
    }
}

void help_trans64(int M, int N, int A[N][M], int B[M][N],int k_start,int k_end,int l_start,int l_end,int store_pos)
{
    int k, i, j , l;
    int tmp[8];
    //先只处理1-56行，1-56列的
    for (k = k_start; k < k_end;k++)
    {
        for(l = l_start; l < l_end; l++)
        {
            //1-4行
            for (i = k * 8; i < k * 8 + 4; i++)
            {
                //1-4列
                for(j = l * 8; j < l * 8 + 4; j++)
                {
                    tmp[j - l * 8] = A[i][j];
                }
                for(j = l * 8; j < l * 8 + 4; j++)
                {
                    B[j][i] = tmp[j - l * 8];
                }
                //5-8列，暂时用不到，存起来
                //统一存在B[60-63][60-63]
                for(j = l * 8 + 4; j < l * 8 + 8; j++)
                {
                    B[i - k * 8 + store_pos][j - l * 8 - 4 + store_pos] = A[i][j];
                }
                
            }
            //5-8行，解决B的第二象限第四象限
            for (i = k * 8 + 4; i < k * 8 + 8; i++)
            {
                //1-4列
                for(j = l * 8; j < l * 8 + 4; j++)
                {
                    tmp[j - l * 8] = A[i][j];
                }
                for(j = l * 8; j < l * 8 + 4; j++)
                {
                    B[j][i] = tmp[j - l * 8];
                }
            }
            //5-8行，解决B的第四象限
            for (i = k * 8 + 4; i < k * 8 + 8; i++)
            {
                //5-8列
                for(j = l * 8 + 4; j < l * 8 + 8; j++)
                {
                    tmp[j - l * 8 - 4] = A[i][j];
                }
                for(j = l * 8 + 4; j < l * 8 + 8; j++)
                {
                    B[j][i] = tmp[j - l * 8 - 4];
                }
            }
            //再回头处理存起来的东西，解决B的第三象限
            for (i = k * 8; i < k * 8 + 4; i++)
            {
                for(j = l * 8 + 4; j < l * 8 + 8; j++)
                {
                    B[j][i] = B[i - k * 8 + store_pos][j - l * 8 - 4 + store_pos];
                }
            }
        }
    }
}

void trans64(int M, int N, int A[N][M], int B[M][N])
{
    //先只处理1-56行，1-56列的（除去1-8行，1-8列）
    help_trans64(M, N, A, B, 1, 7, 1, 7, 60);
    help_trans64(M, N, A, B, 0, 1, 1, 7, 60);
    help_trans64(M, N, A, B, 1, 7, 0, 1, 60);

    //现在处理边框的
    //先把[56-63][0-7]的搞好，待会要借1号cache来存数据
    help_trans64(M, N, A, B, 7, 8, 0, 1, 60);
    
    //现在借助[0-7][0-7]来处理边框
    help_trans64(M, N, A, B, 7, 8, 1, 8, 0);
    help_trans64(M, N, A, B, 0, 8, 7, 8, 0);

    //好了，最后把[0-7][0-7]弄好，用个low方法好了
    int k, i, j , l;
    int tmp[8];
    for (k = 0; k < 2;k++)
    {
        for(l = 0; l < 2; l++)
        {
            for (i = k * 4; i < k * 4 + 4; i++)
            {
                for(j = l * 4; j < l * 4 + 4; j++)
                {
                    tmp[j - l * 4] = A[i][j];
                }
                for(j = l * 4; j < l * 4 + 4; j++)
                {
                    B[j][i] = tmp[j - l * 4];
                }
            }
            
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

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

