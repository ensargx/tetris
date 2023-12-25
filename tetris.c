#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <pthread.h>

#define PADDING_TOP 2
#define PADDING_LEFT 4

int msleep(unsigned int tms) {
#ifdef _WIN32
    Sleep(tms);
return 0;
#else
  return usleep(tms * 1000);
#endif
}

void clear_screen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void move_cursor(int x, int y)
{
#ifdef _WIN32
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    printf("\033[%d;%dH", y, x);
#endif
}

char get_char()
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

Tetromino g_tetrominos[7] = 
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

int g_height, g_width;

typedef struct {
    Tetromino *pBlock;
    Tetromino *pNextBlock;
    int *isRunning;
    int* isBlockFalling;
    int height;
    int width;
    char* board;
    int* speed;
} timerThreadArgs;

int collision_check(Tetromino *block, int height, int width, char board[height][width], int delta_x, int delta_y);

void new_block(Tetromino *block, Tetromino *next_block, int height, int width, char board[height][width])
{
    *block = *next_block;
    *next_block = g_tetrominos[rand() % 7];
    block->x = g_width / 2 - 2;
    block->y = 0;
    block->rotation = 0;

    for (int i = 1; i < 3; i++)
    {
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 2 + i);
        for (int j = 0; j < 4; j++)
        {
            if (next_block->shape[i][j] == '*')
            {
                printf("*");
            }
            else
            {
                printf(" ");
            }
        }
    }

    // RE-render shadow
    Tetromino shadow = *block;
    while (!collision_check(&shadow, g_height, g_width, board, 0, 1))
    {
        shadow.y++;
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (shadow.shape[i][j] == '*' && board[shadow.y + i][shadow.x + j] != '*' && shadow.y != block->y)
            {
                move_cursor(PADDING_LEFT + shadow.x + j + 1, PADDING_TOP + shadow.y + i + 1);
                printf("◌");
            }
        }
    }
    fflush(stdout);
    move_cursor(0, 0); 
}

void* timerThread(void* args)
{
    msleep(1000);
    Tetromino* block = ((timerThreadArgs*)args)->pBlock;
    int* isRunning = ((timerThreadArgs*)args)->isRunning;
    int* isBlockFalling = ((timerThreadArgs*)args)->isBlockFalling;
    int height = ((timerThreadArgs*)args)->height;
    int width = ((timerThreadArgs*)args)->width;
    char* board = ((timerThreadArgs*)args)->board;
    int speed = *((timerThreadArgs*)args)->speed;

    // pointer to pointer of board, we will not modify board
    while (1)
    {
        msleep(speed);
        if (!block || !*isRunning || !*isBlockFalling)
            continue;
        // check if can move down        
        block->y++;

        if (collision_check(block, height, width, board, 0, 0))
        {
            block->y--;
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j< 4; j++)
                {
                    if (block->shape[i][j] == '*')
                    {
                        int idx = (block->y + i) * width + block->x + j;
                        board[idx] = '*';
                    }
                }
            }
            new_block(block, ((timerThreadArgs*)args)->pNextBlock, height, width, board);
        }

        // re-render block
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (block->shape[i][j] == '*')
                {
                    move_cursor(PADDING_LEFT + block->x + j + 1, PADDING_TOP + block->y + i);
                    printf(" ");
                }
            }
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (block->shape[i][j] == '*')
                {
                    move_cursor(PADDING_LEFT + block->x + j + 1, PADDING_TOP + block->y + i + 1);
                    printf("*");
                }
            }
        }
        move_cursor(0, 0);
        fflush(stdout);
    }
}

Tetromino get_random_block()
{
    Tetromino block = g_tetrominos[rand() % 7];
    block.x = g_width / 2 - 2;
    block.y = -1;
    return block;
}

int collision_check(Tetromino *block, int height, int width, char board[height][width], int delta_x, int delta_y)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j< 4; j++)
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

void init_board(int height, int width)
{
    clear_screen();
    
    move_cursor(PADDING_LEFT, PADDING_TOP);
    printf("/");
    for (int i = 0; i < width + 2; i++)
    {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < height; i++)
    {
        move_cursor(PADDING_LEFT, PADDING_TOP + i + 1);
        printf("|");
        for (int j = 0; j < width; j++)
        {
            printf(" ");
        }
        printf("|\n");
    }
    move_cursor(PADDING_LEFT, PADDING_TOP + height + 1);
    printf("\\");
    for (int i = 0; i < width + 2; i++)
    {
        printf("-");
    }
    printf("\n");
    move_cursor(0, 0);
}

void rotate_block(Tetromino *block, int height, int width, char board[height][width])
{
    int rotation = block->rotation;
    rotation++;
    rotation %= 4;
    block->rotation = rotation;

    // first row becomes last column
    // second row becomes second last column
    // third row becomes third last column
    // fourth row becomes first column
    char temp[4][4];
    for (int i = 0; i < 4; i++)
    {
        temp[i][0] = block->shape[3][i];
        temp[i][1] = block->shape[2][i];
        temp[i][2] = block->shape[1][i];
        temp[i][3] = block->shape[0][i];
    }

    // copy temp to block->shape
    for (int i = 0; i < 4; i++)
    {
        block->shape[i][0] = temp[i][0];
        block->shape[i][1] = temp[i][1];
        block->shape[i][2] = temp[i][2];
        block->shape[i][3] = temp[i][3];
    }

    if (!collision_check(block, height, width, board, 0, 0))
    {
        return;
    }

    // try to move left
    if (!collision_check(block, height, width, board, -1, 0))
    {
        block->x--;
        return;
    }

    // try to move right
    if (!collision_check(block, height, width, board, 1, 0))
    {
        block->x++;
        return;
    }

    // try to move up
    if (!collision_check(block, height, width, board, 0, -1))
    {
        block->y--;
        return;
    }
       
}

int main()
{
    int is_running = 1;

    int score = 0;
    int max_score = 0;

    printf("Enter height and width: ");
    scanf("%d %d", &g_height, &g_width);

    int level = 1;
    int speed = 1000; // ms per block, the lower the faster

    // setbuf(stdout, NULL);


    // set rand seed
    srand(time(NULL));

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    int is_full_row[g_height];
    for (int i = 0; i < g_height; i++)
        is_full_row[i] = 0;
    
    char board[g_height][g_width];
    for (int i = 0; i < g_height; i++)
        for (int j = 0; j < g_width; j++)
            board[i][j] = ' ';

    Tetromino block;
    block = get_random_block();
    Tetromino next_block = get_random_block();
    int is_block_falling = 1;

    pthread_t thread_id;
    timerThreadArgs args;
    args.isRunning = &is_running;
    args.pBlock = &block;
    args.pNextBlock = &next_block;
    args.isBlockFalling = &is_block_falling;
    args.height = g_height;
    args.width = g_width;
    args.board = (char*)board;
    args.speed = &speed;

    pthread_create(&thread_id, NULL, timerThread, &args);

    // init_board(g_height, g_width);

    int quit = 0;

    while(!quit)
    {

    while (is_running)
    {

        char ch;
        ch = get_char();
        switch (ch)
        {
        case 'a':
            // check if can move left
            if (!collision_check(&block, g_height, g_width, board, -1, 0))
                block.x--;
            break;
        case 'd':
            // check if can move right
            if (!collision_check(&block, g_height, g_width, board, 1, 0))
                block.x++;
            break;
        case 'w':
            rotate_block(&block, g_height, g_width, board);
            break;
        case 's':
            // check if can move down
            if (!collision_check(&block, g_height, g_width, board, 0, 1))
                block.y++;
            break;
        case 'q':
            is_running = 0;
            break;
        case ' ':
            while (!collision_check(&block, g_height, g_width, board, 0, 1))
            {
                block.y++;
            }
            break;
        default:
            break;
        }

        // if can move down then move down
        // else add block to board and get new block
        if (is_block_falling)
        {
            if (!collision_check(&block, g_height, g_width, board, 0, 1))
            {
                // block.y++;
            }
            else if (collision_check(&block, g_height, g_width, board, 0, 1))
            {
                // add block to board
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j< 4; j++)
                    {
                        if (block.shape[i][j] == '*')
                        {
                            board[block.y + i][block.x + j] = '*';
                        }
                    }
                }
                is_block_falling = 0;
            }
        }
        else
        {
            block = next_block;
            next_block = get_random_block();
            // check if game over, if collision with block on top
            for (int i = 0; i < 4; i++)
            {
                if (board[0][block.x + i] == '*')
                {
                    is_running = 0;
                    break;
                }
            }
            is_block_falling = 1;
        }

        // check any full row
        int full_num = 0;
        for (int i = 0; i < g_height; i++)
        {
            int is_full = 1;
            for (int j = 0; j < g_width; j++)
            {
                if (board[i][j] != '*')
                {
                    is_full = 0;
                    break;
                }
            }
            if (is_full)
            {
                is_full_row[i] = 1;
                full_num++;
                // move all rows above down
                for (int k = i; k > 0; k--)
                {
                    for (int j = 0; j < g_width; j++)
                    {
                        board[k][j] = board[k - 1][j];
                    }
                }
            }
        }
        switch (full_num)
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
            speed -= 100;
        }


        if (!is_block_falling)
        {
            block = next_block;
            next_block = get_random_block();
            // check if game over, if collision with block on top
            for (int i = 0; i < 4; i++)
            {
                if (board[0][block.x + i] == '*')
                {
                    is_running = 0;
                    break;
                }
            }
            is_block_falling = 1;
        }


        // RENDER
        clear_screen();

        // render board
        move_cursor(PADDING_LEFT, PADDING_TOP);
        printf("┌");
        for (int i = 0; i < g_width; i++)
        {
            printf("─");
        }
        printf("┐\n");
        for (int i = 0; i < g_height; i++)
        {
            move_cursor(PADDING_LEFT, PADDING_TOP + i + 1);
            printf("│");
            for (int j = 0; j < g_width; j++)
            {
                printf("%c", board[i][j]);
            }
            printf("│\n");
        }
        move_cursor(PADDING_LEFT, PADDING_TOP + g_height + 1);
        printf("└");
        for (int i = 0; i < g_width; i++)
        {
            printf("─");
        }
        printf("┘\n");

        // render block
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (block.shape[i][j] == '*')
                {
                    move_cursor(PADDING_LEFT + block.x + j + 1, PADDING_TOP + block.y + i + 1);
                    printf("*");
                }
            }
        }

        // render shadow of the block
        Tetromino shadow = block;
        while (!collision_check(&shadow, g_height, g_width, board, 0, 1))
        {
            shadow.y++;
        }
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (shadow.shape[i][j] == '*' && board[shadow.y + i][shadow.x + j] != '*' && shadow.y != block.y)
                {
                    move_cursor(PADDING_LEFT + shadow.x + j + 1, PADDING_TOP + shadow.y + i + 1);
                    printf("◌");
                }
            }
        }

        // render next block
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 1);
        printf("Next block:");
        move_cursor(PADDING_LEFT + g_width + 3, PADDING_TOP + 2);
        printf("┌");
        for (int i = 0; i < 4; i++)
        {
            printf("─");
        }
        printf("┐\n");

        for (int i = 1; i < 3; i++)
        {
            move_cursor(PADDING_LEFT + g_width + 3, PADDING_TOP + 2 + i);
            printf("│");
            for (int j = 0; j < 4; j++)
            {
                if (next_block.shape[i][j] == '*')
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

        move_cursor(PADDING_LEFT + g_width + 3, PADDING_TOP + 5);
        printf("└");
        for (int i = 0; i < 4; i++)
        {
            printf("─");
        }
        printf("┘\n");

        // render score
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 8);
        printf("Max score: %d", max_score);
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 9);
        printf("Score: %d", score);

        // render controls
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 11);
        printf("Controls:");
        move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 12);
        printf("←a  →d  ↑w  ↓s  space  q");

        move_cursor(0, 0);

    }

    // game over
    if (score > max_score)
    {
        max_score = score;
    }

    move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 11);
    printf("Game over!");
    move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 12);
    printf("                        ");
    move_cursor(PADDING_LEFT + g_width + 4, PADDING_TOP + 12);
    printf("Play again? (y/n): ");
    char ch = getchar();
    if (ch == 'y')
    {
        is_running = 1;
        score = 0;
        for (int i = 0; i < g_height; i++)
            for (int j = 0; j < g_width; j++)
                board[i][j] = ' ';
        
        block = get_random_block();
        next_block = get_random_block();
    }
    else
    {
        move_cursor(0, PADDING_TOP + g_height + 3);
        quit = 1;
    }

    }

    return 0;
}
