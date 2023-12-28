#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define PADDING_TOP 2
#define PADDING_LEFT 4

void clean_stdin()
{
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void moveCursor(int x, int y)
{
#ifdef _WIN32
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y, x);
#endif
}

char getChar()
{
#ifdef _WIN32
    return _getch();
#else
    system("stty raw");
    system("stty -echo");
    char ch = getchar();
    system("stty cooked");
    system("stty echo");
    return ch;
#endif
}

typedef struct
{
    char shape[4][4];
    int x;
    int y;
    int rotation;
} Tetromino;

Tetromino getRandomBlock(Tetromino tetrominos[7], int width)
{
    Tetromino block = tetrominos[rand() % 7];
    block.x = width / 2 - 2;
    block.y = -1;
    return block;
}

int collisionCheck(Tetromino *block, int height, int width, char board[height][width], int delta_x, int delta_y)
{
    int i;
    int j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j< 4; j++)
        {
            if (block->shape[i][j] == '*')
            {
                if (block->x + j + delta_x < 0 || block->x + j + delta_x >= width)
                {
                    return 1;
                }
                if (block->y + i + delta_y >= height)
                {
                    return 1;
                }
                if (board[block->y + i + delta_y][block->x + j + delta_x] == '*')
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

void renderBoard(int height, int width, char board[height][width], int level, int score, int maxScore, Tetromino block, Tetromino nextBlock)
{
        clearScreen();
        /* render board */
        moveCursor(PADDING_LEFT, PADDING_TOP);
        printf("┌");
        int i;
        int j;
        for (i = 0; i < width; i++)
        {
            printf("─");
        }
        printf("┐\n");
        for (i = 0; i < height; i++)
        {
            moveCursor(PADDING_LEFT, PADDING_TOP + i + 1);
            printf("│");
            for (j = 0; j < width; j++)
            {
                printf("%c", board[i][j]);
            }
            printf("│\n");
        }
        moveCursor(PADDING_LEFT, PADDING_TOP + height + 1);
        printf("└");
        for (i = 0; i < width; i++)
        {
            printf("─");
        }
        printf("┘\n");
        /* render shadow of the block */
        Tetromino shadow = block;
        while (!collisionCheck(&shadow, height, width, board, 0, 1))
        {
            shadow.y++;
        }
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (shadow.shape[i][j] == '*' && board[shadow.y + i][shadow.x + j] != '*' && shadow.y != block.y)
                {
                    moveCursor(PADDING_LEFT + shadow.x + j + 1, PADDING_TOP + shadow.y + i + 1);
                    printf("◌");
                }
            }
        }
        /* render block */
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                if (block.shape[i][j] == '*')
                {
                    moveCursor(PADDING_LEFT + block.x + j + 1, PADDING_TOP + block.y + i + 1);
                    printf("*");
                }
            }
        }
        /* render next block */
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 1);
        printf("Next block:");
        moveCursor(PADDING_LEFT + width + 3, PADDING_TOP + 2);
        printf("┌");
        for (i = 0; i < 4; i++)
        {
            printf("─");
        }
        printf("┐\n");
        for (i = 1; i < 3; i++)
        {
            moveCursor(PADDING_LEFT + width + 3, PADDING_TOP + 2 + i);
            printf("│");
            for (j = 0; j < 4; j++)
            {
                if (nextBlock.shape[i][j] == '*')
                {
                    printf("*");
                }
                else
                {
                    printf(" ");
                }
            }
            printf("│\n");
        }
        moveCursor(PADDING_LEFT + width + 3, PADDING_TOP + 5);
        printf("└");
        for (i = 0; i < 4; i++)
        {
            printf("─");
        }
        printf("┘\n");
        /* render level */
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 7);
        printf("Level: %d", level);
        /* render score */
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 8);
        printf("Max score: %d", maxScore);
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 9);
        printf("Score: %d", score);
        /* render controls */
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 11);
        printf("Controls:");
        moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 12);
        printf("←a  →d  ↑w  ↓space Xq");
        moveCursor(0, 0);
}

void rotateBlock(Tetromino *block, int height, int width, char board[height][width])
{
    /* rotates the tetromino */
    int rotation = block->rotation;
    rotation++;
    rotation %= 4;
    block->rotation = rotation;

    char temp[4][4];
    int i;
    for (i = 0; i < 4; i++)
    {
        temp[i][0] = block->shape[3][i];
        temp[i][1] = block->shape[2][i];
        temp[i][2] = block->shape[1][i];
        temp[i][3] = block->shape[0][i];
    }

    for (i = 0; i < 4; i++)
    {
        block->shape[i][0] = temp[i][0];
        block->shape[i][1] = temp[i][1];
        block->shape[i][2] = temp[i][2];
        block->shape[i][3] = temp[i][3];
    }

    if (!collisionCheck(block, height, width, board, 0, 0))
    {
        return;
    }

    if (!collisionCheck(block, height, width, board, -1, 0))
    {
        block->x--;
        return;
    }

    if (!collisionCheck(block, height, width, board, 1, 0))
    {
        block->x++;
        return;
    }

    if (!collisionCheck(block, height, width, board, 0, -1))
    {
        block->y--;
        return;
    }

    if (!collisionCheck(block, height, width, board, -2, 0))
    {
        block->x -= 2;
        return;
    }

    if (!collisionCheck(block, height, width, board, 2, 0))
    {
        block->x += 2;
        return;
    }
       
}

int main()
{
    int isGameRunning = 1;
    int score = 0;
    int maxScore = 0;
    int level = 1;
    int height;
    int width;

    /* set random seed */
    srand(time(NULL));

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    char banner[] = {
        " _____    _        _        ____\n"
        "|_   _|__| |_ _ __(_)___   / ___| __ _ _ __ ___   ___\n"
        "  | |/ _ \\ __| '__| / __| | |  _ / _` | '_ ` _ \\ / _ \\\n"
        "  | |  __/ |_| |  | \\__ \\ | |_| | (_| | | | | | |  __/\n"
        "  |_|\\___|\\__|_|  |_|___/  \\____|\\__,_|_| |_| |_|\\___|\n"
        " 1- Scores:                                 byEnsarGok\n"
        "    - 100 points for 1 line\n"
        "    - 250 points for 2 lines\n"
        "    - 500 points for 3 lines\n"
        "    - 1000 points for 4 lines\n"
        " 2- Controls:\n"
        "    - ←a  Move left\n"
        "    - →d  Move right\n"
        "    - ↑w  Rotate block\n"
        "    - ↓space  Drop\n"
        "    - Xq  Quit\n"
        " 3- Score 1000 points to level up\n"
        " 4- Press q to quit               (eg. 15x10)\n"
        " Enter height and width to start: "
    };

    printf("%s", banner);
    scanf("%dx%d", &height, &width);

    Tetromino tetrominos[7] = 
    {
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {'*', '*', '*', '*'},
                {' ', ' ', ' ', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {'*', ' ', ' ', ' '},
                {'*', '*', '*', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {' ', ' ', '*', ' '},
                {'*', '*', '*', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {' ', '*', '*', ' '},
                {' ', '*', '*', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {'*', '*', '*', ' '},
                {' ', '*', ' ', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {' ', '*', '*', ' '},
                {'*', '*', ' ', ' '},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        },
        {
            .shape = {
                {' ', ' ', ' ', ' '},
                {' ', '*', '*', ' '},
                {' ', ' ', '*', '*'},
                {' ', ' ', ' ', ' '}
            },
            .x = 0,
            .y = 0,
            .rotation = 0
        }
    };

    int isRowFull[height];
    int i;
    for (i = 0; i < height; i++)
        isRowFull[i] = 0;
    
    char board[height][width];
    int j;
    for (i = 0; i < height; i++)
        for (j = 0; j < width; j++)
            board[i][j] = ' ';

    Tetromino block = getRandomBlock(tetrominos, width);
    Tetromino nextBlock = getRandomBlock(tetrominos, width);

    renderBoard(height, width, board, level, score, maxScore, block, nextBlock);

    int quit = 0;
    while(!quit)
    {

    while (isGameRunning)
    {

        char ch;
        ch = getChar();
        switch (ch)
        {
        case 'a':
            if (!collisionCheck(&block, height, width, board, -1, 0))
                block.x--;
            break;
        case 'd':
            if (!collisionCheck(&block, height, width, board, 1, 0))
                block.x++;
            break;
        case 'w':
            rotateBlock(&block, height, width, board);
            break;
        case 'q':
            isGameRunning = 0;
            break;
        case ' ':
            while (!collisionCheck(&block, height, width, board, 0, 1))
            {
                block.y++;
            }
            break;
        default:
            break;
        }

        /* if block is on the ground */
        if (collisionCheck(&block, height, width, board, 0, 1))
        {
            /* add block to board */
            for (i = 0; i < 4; i++)
            {
                for (j = 0; j< 4; j++)
                {
                    if (block.shape[i][j] == '*')
                    {
                        board[block.y + i][block.x + j] = '*';
                    }
                }
            }
            block = nextBlock;
            /* if next block collapses, game over */
            if (collisionCheck(&block, height, width, board, 0, 0))
            {
                isGameRunning = 0;
            }
            nextBlock = getRandomBlock(tetrominos, width);
        }

        /* check if a row is full */
        int fullRowCount = 0;
        int i;
        int j;
        int k;
        for (i = 0; i < height; i++)
        {
            int is_full = 1;
            for (j = 0; j < width; j++)
            {
                if (board[i][j] != '*')
                {
                    is_full = 0;
                    /* break; */
                }
            }
            if (is_full)
            {
                isRowFull[i] = 1;
                fullRowCount++;
                /* move all rows above down */
                for (k = i; k > 0; k--)
                {
                    for (j = 0; j < width; j++)
                    {
                        board[k][j] = board[k - 1][j];
                    }
                }
            }
        }
        switch (fullRowCount)
        {
        case 1:
            score += 100;
            break;
        case 2:
            score += 250;

            break;
        case 3:
            score += 500;
            break;
        case 4:
            score += 1000;
            break;
        default:
            break;
        }
        if (score >= level * 1000)
        {
            level++;
        }

        /* check if top row is full */
        for (j = 0; j < width; j++)
        {
            if (board[0][j] == '*')
            {
                isGameRunning = 0;
            }
        }

        /* RENDER */
        renderBoard(height, width, board, level, score, maxScore, block, nextBlock);
    }

    /* game over */
    if (score > maxScore)
        maxScore = score;

    moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 11);
    printf("Game over!");
    moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 12);
    printf("                        ");
    moveCursor(PADDING_LEFT + width + 4, PADDING_TOP + 12);
    printf("Play again? (y/n): ");
    
    char ch;
    fflush(stdin);
    scanf("%c", &ch);
    if (ch == 'y')
    {
        isGameRunning = 1;
        score = 0;
        level = 1;
        for (i = 0; i < height; i++)
            for (j = 0; j < width; j++)
                board[i][j] = ' ';
        for (i = 0; i < height; i++)
            isRowFull[i] = 0;
        block = getRandomBlock(tetrominos, width);
        nextBlock = getRandomBlock(tetrominos, width);
    }
    else
    {
        quit = 1;
    }

    }

    /* press any key to exit */
    moveCursor(0, PADDING_TOP + height + 2);
    printf("Press enter to continue...");
    clean_stdin();
    getchar();

    return 0;
}
