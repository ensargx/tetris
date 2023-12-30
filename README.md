# Tetris
BLM1011 Bilgisayar Bilimlerine Giriş Proje Ödevi - Ensar GÖK
> Ödevin Github versiyonuna **[buradan](https://github.com/ensargx/tetris)** ulaşabilirsiniz.
> 
> Ödevin Youtube videosuna **[buradan](https://www.youtube.com/watch?v=RQIwropb2ng)** ulaşabilirsiniz. 

## Menü

```
 _____    _        _        ____
|_   _|__| |_ _ __(_)___   / ___| __ _ _ __ ___   ___
  | |/ _ \ __| '__| / __| | |  _ / _` | '_ ` _ \ / _ \
  | |  __/ |_| |  | \__ \ | |_| | (_| | | | | | |  __/
  |_|\___|\__|_|  |_|___/  \____|\__,_|_| |_| |_|\___|
 1- Scores:                                 byEnsarGok
    - 100 points for 1 line
    - 250 points for 2 lines
    - 500 points for 3 lines
    - 1000 points for 4 lines
 2- Controls:
    - ←a  Move left
    - →d  Move right
    - ↑w  Rotate block
    - ↓space  Drop
    - Xq  Quit
 3- Score 1000 points to level up
 4- Press q to quit               (eg. 15x10)
 Enter height and width to start:
```
> Puanlandırma ve oyun kontrolleri menüde belirtilmiştir.
>
> Oyun başlamadan önce kullanıcıdan oyun tahtasının boyutu istenmektedir.

## Tetromino

```c
typedef struct
{
    char shape[4][4];
    int x;
    int y;
    int rotation;
} Tetromino;

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
```
> Tüm Tetrominolar'ın 4x4'lük şekli vardır.
>
> Oyundaki 7 farklı Tetromino **' '** ve **'*'** karakteriyle oluşturulmuştur.

## Kullanıcıdan Giriş Alma
```c
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
    isGameRunning  =  0;
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
```
> *getChar* fonksiyonu ile kullanıcıdan giriş alınmıştır. Alınan girişe göre mevcut blok **eğer çakışma olmuyorsa** istenilen  pozisyona getirilmektedir.

## Çakışma Durumu
### Parametreler
- *Tetromino \*block*: Çakışma durumu kontrol edilecek blok.
- *int height,  int width*: Oyun tahtasının boyutları.
- *char board[height][width]*: Oyun tahtası.
- *int delta_x, int delta_y*: Çakışmanın kontrol edileceği koordinat farkı. Örneğin *delta_y* değeri *-1*, *delta_y* değeri *0* verilirse, istenilen parçanın 1 blok aşağıda olduğu varsayılarak kontrol yapılacaktır, bu sayede bloğun aşağı inip inemeyeceği kontrol edilebilir. Aynı şekilde *delta_x* değeri değiştirilerek bloğun sağa veya sola kayıp kayamayacağı kontrol edilebilir.

### İmplementasyon
```c
int collisionCheck(Tetromino *block, int height, int width, char board[height][width], int delta_x, int delta_y)
{
    int i;
    int j;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
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
```
Fonksiyon verilen bloğun tüm şeklini kontrol edip *'\*'* karakterinin olduğu yerlerin dolu olup olmadığını kontrol eder.

## Bloğu Döndürme
### Parametreler
- *Tetromino \*block*: Döndürülecek blok.
- *int height,  int width*: Oyun tahtasının boyutları.
- *char board[height][width]*: Oyun tahtası.
### İmplementasyon
```c
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
```
> Döndürmek için bloğun satılarını ve sütunlarını değiştiriyoruz.
> 
> Eğer çakışma durumu varsa engelleyecek şekilde bloğa gerekli işlemi yapıyor.

## Bloğun Yerde Olması
Bir blok eğer altı doluysa bulunduğu konumda tahtaya eklenir.

```c
if (collisionCheck(&block, height, width, board, 0, 1))
{
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            if (block.shape[i][j] == '*')
            {
                board[block.y + i][block.x + j] = '*';
            }
        }
    }
    block = nextBlock;
    if (collisionCheck(&block, height, width, board, 0, 0))
    {
        isGameRunning = 0;
    }
    nextBlock = getRandomBlock(tetrominos, width);
}
```

## Satırın Dolması
Eğer oyunda herhangi bir satır tamamen dolduysa o satır silinir ve üstündeki satırlar aşağıya iner.
- **1 Satır** -> 100 puan.
- **2 Satır** -> 250 puan.
- **3 Satır** -> 500 puan.
- **4 Satır** -> 1000 puan.

```c
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
case  1:
    score += 100;
    break;
case  2:
    score += 250;
    break;
case  3:
    score += 500;
    break;
case  4:
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
```
> Her 1000 puanda seviye atlanır.
> 
> Ödevde istenilen '**Oyun tahtasının en üst hattında herhangi bir parçası bulunuyorsa program sona ermelidir.**' burada implement edilmiştir.
---
## Oyun Görüntüleri

![Görüntü 1](https://raw.githubusercontent.com/ensargx/tetris/main/assets/img1.PNG)

![Görüntü 2](https://raw.githubusercontent.com/ensargx/tetris/main/assets/img2.PNG)

![Görüntü 3](https://raw.githubusercontent.com/ensargx/tetris/main/assets/img3.PNG)

---
Ensar GÖK - 22011032
