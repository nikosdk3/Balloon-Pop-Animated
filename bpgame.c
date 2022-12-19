#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bpgame.h"

struct bpgame
{
	char** gameBoard;
	struct bpgame* prev;
	int rows;
	int cols;
	int score;
};

BPGame* bp_create(int nrows, int ncols)
{
	if(nrows < 0 || ncols < 0 || nrows >= MAX_ROWS || ncols >= MAX_COLS)
	{
		fprintf(stderr, "Invalid row or column number!\n");
		return NULL;
	}
	
	BPGame* b = (BPGame*)malloc(sizeof(BPGame));
	b->gameBoard = (char**)malloc(sizeof(char*) * nrows);
	b->prev = NULL;
	b->rows = nrows;
	b->cols = ncols;
	b->score = 0;
	
	srand(time(NULL));
	
	for(int x = 0; x < nrows; x++)
	{
		b->gameBoard[x] = (char*)malloc(sizeof(char) * ncols);
		for (int y = 0; y < ncols; y++)
		{
			char randomBalloon = rand() % 4;
			if(randomBalloon == 0)      randomBalloon = Red;
			else if(randomBalloon == 1) randomBalloon = Blue;
			else if(randomBalloon == 2) randomBalloon = Green;
			else if(randomBalloon == 3) randomBalloon = Yellow;
			b->gameBoard[x][y] = randomBalloon;
		}
	}
	
	return b;
}

BPGame* bp_create_from_mtx(char mtx[][MAX_COLS], int nrows, int ncols)
{
	if(nrows < 0 || ncols < 0 || nrows >= MAX_ROWS || ncols >= MAX_COLS)
	{
		fprintf(stderr, "Invalid row or column number!\n");
		return NULL;
	}
	
	BPGame* b = (BPGame*)malloc(sizeof(BPGame));
	b->gameBoard = (char**)malloc(sizeof(char*) * nrows);
	b->prev = NULL;
	b->rows = nrows;
	b->cols = ncols;
	b->score = 0;
	
	for(int x = 0; x < nrows; x++)
	{
		b->gameBoard[x] = (char*)malloc(sizeof(char) * ncols);
		for (int y = 0; y < ncols; y++)
		{
			if (!strchr("^=o+.", mtx[x][y]))
			{
				fprintf(stderr, "Invalid character being added to matrix!\n");
				for(int z = 0; z < x; z++)
					free(b->gameBoard[z]);
				free(b->gameBoard);
				free(b);
				
				return NULL;
			}
			b->gameBoard[x][y] = mtx[x][y];
		}
	}

	return b;
}

void bp_destroy(BPGame* b)
{
	if(!b)
		return;

	bp_destroy(b->prev);
	
	for(int x = 0; x < b->rows; x++)
		free(b->gameBoard[x]);

	free(b->gameBoard);
	free(b);
}

void bp_display(BPGame* b)
{
	if(b->rows < 10)
		printf("  +");
	else
		printf("   +");
	
	for(int x = 0; x < 2 * b->cols + 1; x++)
		printf("-");
	printf("+\n");
	
	for(int x = 0; x < b->rows; x++)
	{
		if(x < 10 && b->rows >= 10)
			printf("%d  | ", x);
		else
			printf("%d | ", x);
		
		for(int y = 0; y < b->cols; y++)
			printf("%c ", b->gameBoard[x][y]);
		printf("|\n");
	}
	
	if(b->rows < 10)
		printf("  +");
	else
		printf("   +");
	
	for(int x = 0; x < 2 * b->cols + 1; x++)
		printf("-");
	printf("+\n");
	
	
	if(b->rows < 10)
		printf("    ");
	else
		printf("     ");
	
	for(int x = 0; x < b->cols; x++)
		printf("%d ", x / 10);
	printf("\n");
	
	if(b->rows < 10)
		printf("    ");
	else
		printf("     ");
	
	for(int x = 0; x < b->cols; x++)
		printf("%d ", x % 10);
	printf("\n");
}

void bp_display_STD(BPGame* b)
{
    for (int i = 0; i < b->rows; i++)
	{
        for (int j = 0; j < b->cols; j++)
            if (b->gameBoard[i][j] == ' ')
                printf(".");
            else
                printf("%c", b->gameBoard[i][j]);
        printf("\n");
    }
}

int popBalloons(BPGame* b, int r, int c, char color)
{
	if (r < 0 || c < 0 || r >= b->rows || c >= b->cols || b->gameBoard[r][c] != color)
		return 0;

	b->gameBoard[r][c] = None;

	return 1 + popBalloons(b, r + 1, c, color) + popBalloons(b, r - 1, c, color)
		     + popBalloons(b, r, c + 1, color) + popBalloons(b, r, c - 1, color);
}

int bp_pop(BPGame* b, int r, int c)
{
	if (r < 0 || c < 0 || r >= b->rows || c >= b->cols || b->gameBoard[r][c] == None)
		return 0;

	char mtx[MAX_ROWS][MAX_COLS];
	for(int x = 0; x < b->rows; x++)
		for(int y = 0; y < b->cols; y++)
			mtx[x][y] = b->gameBoard[x][y];

	char color = b->gameBoard[r][c];
	int popped = popBalloons(b, r, c, color);
	
	if (popped == 1)
	{
		b->gameBoard[r][c] = color;
		return 0;
	}
	
	BPGame* temp = bp_create_from_mtx(mtx, b->rows, b->cols);
	temp->prev = b->prev;
	temp->score = b->score;
	b->prev = temp;
	
	b->score += popped * (popped - 1);
	
	return popped;
}

int bp_is_compact(BPGame* b)
{
	for(int x = 0; x < b->rows - 1; x++)
		for(int y = 0; y < b->cols; y++)
			if(b->gameBoard[x][y] == None && b->gameBoard[x + 1][y] != None)
				return 0;

	return 1;
}

void bp_float_one_step(BPGame* b)
{
	for(int x = 0; x < b->rows - 1; x++)
		for(int y = 0; y < b->cols; y++)
			if(b->gameBoard[x][y] == None && b->gameBoard[x + 1][y] != None)
			{
				b->gameBoard[x][y] = b->gameBoard[x + 1][y];
				b->gameBoard[x + 1][y] = None;
			}
}

int bp_score(BPGame* b)
{
	return b->score;
}

int bp_get_balloon(BPGame* b, int r, int c)
{
	if (r < 0 || c < 0 || r >= b->rows || c >= b->cols)
		return -1;
	
	return b->gameBoard[r][c];
}

int bp_can_pop(BPGame* b)
{
	for(int x = 0; x < b->rows - 1; x++)
		for(int y = 0; y < b->cols - 1; y++)
			if (b->gameBoard[x][y] != None)
				if (b->gameBoard[x][y] == b->gameBoard[x + 1][y] || b->gameBoard[x][y] == b->gameBoard[x][y + 1])
						return 1;
	return 0;
}

int bp_undo(BPGame* b)
{
	if (!b->prev)
		return 0;

	BPGame* temp = b->prev;
	
	for(int x = 0; x < b->rows; x++)
		for(int y = 0; y < b->cols; y++)
			b->gameBoard[x][y] = temp->gameBoard[x][y];
		
	b->score = temp->score;
	b->prev = b->prev->prev;
	temp->prev = NULL;
	bp_destroy(temp);
	
	return 1;
}