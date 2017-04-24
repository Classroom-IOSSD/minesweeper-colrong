/**
@file minesweeper.c
@version 0.1
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "conio.h"
#define MAX 10

// background color
#define KNRM  "\x1B[0m"
#define BRED  "\x1B[41m"
#define BGRN  "\x1B[42m"
#define BYEL  "\x1B[43m"
#define BBLU  "\x1B[44m"
#define BMAG  "\x1B[45m"
#define BCYN  "\x1B[46m"
#define BWHT  "\x1B[47m"
// text color
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FLAG_NEW 101
#define FLAG_FLAG 102
#define FLAG_CHECK 103
#define FLAG_NONE 104
#define FLAG_QUIT 109

#define MODE_INPUT 0
#define MODE_FLAG 1
#define MODE_CHECK 2

#define BLOCK_BLANK 0
#define BLOCK_MINE 99

#define FALSE 0
#define TRUE 1

// global variables
// game table
unsigned char table_array[MAX][MAX];
// location of cursor
int posX = 0, posY = 0;
// flag: input mode = 0, flag mode = 1, check mode = 2
int game_mode = MODE_INPUT;

/*This is a recursive function which uncovers blank cells while they are adjacent*/
int uncover_blank_cell(int row, int col) {
	int value, rows[8], columns[8], i;

	if (table_array[row][col] != 0)
		return 0; // error

	table_array[row][col] += 10; // uncover current cell

								 // Get position of adjacent cells of current cell
	rows[0] = row - 1;	columns[0] = col + 1;
	rows[1] = row;		columns[1] = col + 1;
	rows[2] = row + 1;	columns[2] = col + 1;
	rows[3] = row - 1;	columns[3] = col;
	rows[4] = row + 1;	columns[4] = col;
	rows[5] = row - 1;	columns[5] = col - 1;
	rows[6] = row;		columns[6] = col - 1;
	rows[7] = row + 1;	columns[7] = col - 1;

	for (i = 0; i < 8; i++) {
		value = table_array[rows[i]][columns[i]];

		if ((rows[i] >= 0 && rows[i] < MAX) && (columns[i] >= 0 && columns[i] < MAX)) {		// to prevent negative index and out of bounds
			if (value > 0 && value <= 8)
				table_array[rows[i]][columns[i]] += 10;										// it is a cell with 1-8 number so we need to uncover
			else if (value == BLOCK_BLANK)
				uncover_blank_cell(rows[i], columns[i]);
		}

	}

	return 1; // success!
}

void print_table() {
	// clear screen
	system("clear");

	int i, j, value;
	for (i = 0; i < MAX; i++) {
		for (j = 0; j < MAX; j++) {
			if (posX == j && posY == i) {
				if (game_mode == MODE_FLAG) {
					printf("|%sF%s", BMAG, KNRM);
					continue;
				}
				else if (game_mode == MODE_CHECK) {
					printf("|%sC%s", BMAG, KNRM);
					continue;
				}

			}
			value = table_array[i][j];

			if ((value >= 0 && value <= 8) || value == BLOCK_BLANK || value == BLOCK_MINE)
				printf("|X");
			else if (value == 10) // clean area
				printf("|%s%d%s", KCYN, value - 10, KNRM);
			else if (value == 11) // the number of near mine is 1
				printf("|%s%d%s", KYEL, value - 10, KNRM);
			else if (value > 11 && value <= 18) // the number of near mine is greater than 1
				printf("|%s%d%s", KRED, value - 10, KNRM);
			else if ((value >= 20 && value <= 28) || value == 100)
				printf("|%sF%s", KGRN, KNRM);
			else
				printf("ERROR"); // test purposes

		}
		printf("|\n");
	}

	printf("cell values: 'X' unknown, '%s0%s' no mines close, '1-8' number of near mines, '%sF%s' flag in cell\n", KCYN, KNRM, KGRN, KNRM);
	if (game_mode == MODE_INPUT) {
		printf("f (put/remove Flag in cell), c (Check cell), n (New game), q (Exit game): ");
	}
	else if (game_mode == MODE_FLAG) {
		printf("Enter (select to put/remove Flag in cell), q (Exit selection): ");
	}
	else if (game_mode == MODE_CHECK) {
		printf("Enter (select to check cell), q (Exit selection): ");
	}


}

void TableInit(int numOfMines) {
	int i, j, r, c, rows[8], columns[8];
	for (i = 0; i < numOfMines; i++) {
		/* initialize random seed: */

		r = rand() % 10;					// it generates a integer in the range 0 to 9
		c = rand() % 10;

		// put mines
		if (table_array[r][c] != BLOCK_MINE) {
			table_array[r][c] = BLOCK_MINE;

			// Get position of adjacent cells of current cell
			rows[0] = r - 1;	columns[0] = c + 1;
			rows[1] = r;		columns[1] = c + 1;
			rows[2] = r + 1;	columns[2] = c + 1;
			rows[3] = r - 1;	columns[3] = c;
			rows[4] = r + 1;	columns[4] = c;
			rows[5] = r - 1;	columns[5] = c - 1;
			rows[6] = r;		columns[6] = c - 1;
			rows[7] = r + 1;	columns[7] = c - 1;

			for (j = 0; j < 8; j++) {
				if ((rows[j] >= 0 && rows[j] < MAX) && (columns[j] >= 0 && columns[j] < MAX)) {		// to prevent negative index and out of bounds
					if (table_array[rows[j]][columns[j]] != BLOCK_MINE)																// to prevent remove mines
						table_array[rows[j]][columns[j]] += 1;									// sums 1 to each adjacent cell
				}
			}

		}
		else {								// to make sure that there are the properly number of mines in table
											//i--;
			numOfMines++;
		}
	}
}

int CheckGameOver(int numOfMines) {
	short isValid = FALSE;
	char playerInput = '\0';
	int flag = FLAG_NEW;
	game_mode = MODE_INPUT;
	print_table();
	printf("\nGAME OVER\n");

	if (numOfMines == 0)
		printf("you won!!!!\n");

	else
		printf("BOOM! you LOOSE!\n");

	do {
		printf("Are you sure to exit? (y or n)? ");
		playerInput = getch();
		putchar('\n');

		if (playerInput == 'y' || playerInput == 'Y') {
			flag = FLAG_QUIT;
			isValid = TRUE;
		}
		else if (playerInput == 'n' || playerInput == 'N') {
			isValid = TRUE;
		}
		else
			printf("Please answer y or n\n");

		if (isValid){
			return flag;
		}

	} while (1);
}

int main(int argc, char *argv[]) {

	char playerInput;
	int numOfMines; // the number of the remaining mines
	int i, j, value;
	int loopFlag = FLAG_NONE;//While loop's condition NEW/CHECK/FLAG/QUIT

	while(loopFlag != FLAG_QUIT){
		// the number of mines
		numOfMines = 10;
		if (argc == 2) {
			numOfMines = atoi(argv[1]);
		}
		srand(time(NULL));						// random seed
												// setting cursor
		posX = 0;
		posY = 0;
		// set all cells to 0
		for (i = 0; i < 10; i++)
			for (j = 0; j < 10; j++)
				table_array[i][j] = 0;

		TableInit(numOfMines);
		loopFlag = FLAG_NONE;
		while (numOfMines != 0) {				// when nMines becomes 0 you will win the game

			char direction;

			if(loopFlag != FLAG_FLAG && loopFlag != FLAG_CHECK){
				print_table();
				playerInput = getch();
				loopFlag = FLAG_NONE;
			}else if(loopFlag == FLAG_FLAG)
				playerInput = 'f';
			else
				playerInput = 'c';
			// cursor direction

			switch (playerInput) {
			case 'f':
			case 'F': 
				game_mode = MODE_FLAG;
				do {
					print_table();
					direction = getch();
					// arrow direction
					if (direction == '8') {
						// up
						posY = (MAX + --posY) % MAX;
					}
					else if (direction == '2') {
						// down
						posY = ++posY % MAX;
					}
					else if (direction == '4') {
						posX = (MAX + --posX) % MAX;
					}
					else if (direction == '6') {
						posX = ++posX % MAX;
					}
					else if (direction == 'c' || direction == 'C') {
						loopFlag = FLAG_CHECK;
						break;
					}
					else if (direction == '\n') {
						value = table_array[posY][posX];

						if (value == BLOCK_MINE) {					// mine case
							table_array[posY][posX] += 1;
							numOfMines -= 1;				// mine found
						}
						else if (value >= 0 && value <= 8) {	// number of mines case (the next cell is a mine)
							table_array[posY][posX] += 20;
						}
						else if (value >= 20 && value <= 28) {
							table_array[posY][posX] -= 20;
						}

						if (numOfMines == 0){
							loopFlag = FLAG_QUIT;
							break;
						}
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = MODE_INPUT;
				if(loopFlag != FLAG_QUIT && loopFlag != FLAG_CHECK)
					loopFlag = FLAG_NONE;
				break;

				// check cell
			case 'c':
			case 'C':
				game_mode = MODE_CHECK;
				do {
					print_table();
					direction = getch();

					// arrow direction
					if (direction == '8') {
						// up
						posY = (MAX + --posY) % MAX;
					}
					else if (direction == '2') {
						// down
						posY = ++posY % MAX;
					}
					else if (direction == '4') {
						posX = (MAX + --posX) % MAX;
					}
					else if (direction == '6') {
						posX = ++posX % MAX;
					}
					else if (direction == 'f' || direction == 'F') {
						//goto flag_mode;
						loopFlag = FLAG_FLAG;
						break;
					}

					else if (direction == '\n') {
						value = table_array[posY][posX];
						if (value == BLOCK_BLANK)						// blank case
							uncover_blank_cell(posY, posX);
						else if (value == BLOCK_MINE) {				// mine case
							loopFlag = FLAG_QUIT;
							break;
						}
						else if (value > 0 && value <= 8)	// number case (the next cell is a mine)
							table_array[posY][posX] += 10;

						//	break;
					}
				} while (direction != 'q' && direction != 'Q');
				game_mode = MODE_INPUT;

				if(loopFlag != FLAG_QUIT && loopFlag != FLAG_FLAG)
					loopFlag = FLAG_NONE;
				break;
				// jump to a new game
			case 'n':
			case 'N':
				loopFlag = FLAG_NEW;
				break;
				// exit
			case 'q':
			case 'Q':
				loopFlag = FLAG_QUIT;
				break;
				//goto end_of_game;

			default:
				break;
			}

			if (loopFlag == FLAG_QUIT){
				loopFlag = CheckGameOver(numOfMines);
				break;
			}else if(loopFlag == FLAG_NEW)
				break;
		}
	}
	printf("See you next time!\n");

	fflush(stdin);

	return 0;
}
