#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define MAX_WORDS 2000
#define WORD_LEN  30

#define RESET "\033[0m"
#define GREEN "\033[1;32m"
#define RED   "\033[1;31m"
#define BLUE  "\033[1;34m"  // Added for menu styling
#define YELLOW "\033[1;33m" // Added for menu styling

/* ---------------- STRUCT ---------------- */
typedef struct
{
    char *text;
    int length;
    int found;
    int row[WORD_LEN];
    int col[WORD_LEN];
} Word;

/* ---------------- GLOBALS ---------------- */
char **grid;
int **mask;          // 0=normal, 1=green(found), 2=red(unfound)
int GRID_SIZE;
int HIDDEN_WORDS;

/* ---------------- UTIL ---------------- */
void toUpperCase(char s[])
{
    for (int i = 0; s[i]; i++)
        s[i] = toupper(s[i]);
}

/* ---------------- CAPTCHA ---------------- */
void generateCaptcha(char c[], int len)
{
    char pool[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    for(int i=0; i<len; i++) c[i]=pool[rand()%strlen(pool)];
    c[len]='\0';
}

int captchaVerification()
{
    char c[10], in[10];
    generateCaptcha(c,6);
    printf("\nCAPTCHA: %s\nEnter CAPTCHA: ", c);
    scanf("%s", in);
    return strcmp(c,in)==0;
}


/* ---------------- MENU ---------------- */
void menuScreen()
{
    printf(BLUE "\n");
    printf("+==============================================================+\n");
    printf("|                                                              |\n");
    printf("|                  " YELLOW "WELCOME TO WORD SEARCH GAME" BLUE "                 |\n");
    printf("|                                                              |\n");
    printf("|  Find hidden words in the grid! Guess correctly to win.      |\n");
    printf("|  Enter 'EXIT' during gameplay to quit.                       |\n");
    printf("|                                                              |\n");
    printf("+==============================================================+\n");
    printf(RESET "\n");

    int valid = 0;
    do
    {
        printf(YELLOW "Enter grid size (min 10, max 50): " RESET);
        if (scanf("%d", &GRID_SIZE) != 1)
        {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (GRID_SIZE < 10 || GRID_SIZE > 50)
        {
            printf(RED "Grid size must be between 10 and 50.\n" RESET);
        }
        else
        {
            valid = 1;
        }
    }
    while (!valid);

    valid = 0;
    do
    {
        printf(YELLOW "Enter number of hidden words (min 5, max 100): " RESET);
        if (scanf("%d", &HIDDEN_WORDS) != 1)
        {
            printf(RED "Invalid input. Please enter a number.\n" RESET);
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        if (HIDDEN_WORDS < 5 || HIDDEN_WORDS > 100)
        {
            printf(RED "Hidden words must be between 5 and 100.\n" RESET);
        }
        else
        {
            valid = 1;
        }
    }
    while (!valid);

    printf(GREEN "\nSettings confirmed! Grid: %dx%d, Hidden Words: %d\n" RESET, GRID_SIZE, GRID_SIZE, HIDDEN_WORDS);
    printf("Press Enter to start...");
    getchar(); // Wait for Enter
    while (getchar() != '\n'); // Clear any extra input
}

/* ---------------- GRID MEMORY ---------------- */
void allocateGrid()
{
    grid = malloc(GRID_SIZE*sizeof(char*));
    mask = malloc(GRID_SIZE*sizeof(int*));
    for(int i=0; i<GRID_SIZE; i++)
    {
        grid[i]=malloc(GRID_SIZE*sizeof(char));
        mask[i]=malloc(GRID_SIZE*sizeof(int));
    }
}

void initGrid()
{
    for(int i=0; i<GRID_SIZE; i++)
        for(int j=0; j<GRID_SIZE; j++)
        {
            grid[i][j]='\0';
            mask[i][j]=0;
        }
}

/* ---------------- LOAD FILE ---------------- */
int loadFile(const char *file, Word **arr)
{
    FILE *fp=fopen(file,"r");
    if(!fp)
    {
        printf("Cannot open %s\n",file);
        exit(0);
    }
    *arr=malloc(MAX_WORDS*sizeof(Word));
    char buf[WORD_LEN];
    int c=0;
    while(fscanf(fp,"%s",buf)!=EOF)
    {
        toUpperCase(buf);
        if(strlen(buf)<4) continue;
        (*arr)[c].text=strdup(buf);
        (*arr)[c].length=strlen(buf);
        (*arr)[c].found=0;
        c++;
    }
    fclose(fp);
    return c;
}

/* ---------------- PLACE WORD ---------------- */
void placeWord(Word *w)
{
    int len=w->length, placed=0;
    while(!placed)
    {
        int dir=rand()%4;
        int r,c,i;
        placed=1;

        if(dir==0)  // H
        {
            r=rand()%GRID_SIZE;
            c=rand()%(GRID_SIZE-len+1);
            for(i=0; i<len; i++)
                if(grid[r][c+i] && grid[r][c+i]!=w->text[i]) placed=0;
            if(placed)
                for(i=0; i<len; i++)
                {
                    grid[r][c+i]=w->text[i];
                    w->row[i]=r;
                    w->col[i]=c+i;
                }
        }
        else if(dir==1)  // V
        {
            r=rand()%(GRID_SIZE-len+1);
            c=rand()%GRID_SIZE;
            for(i=0; i<len; i++)
                if(grid[r+i][c] && grid[r+i][c]!=w->text[i]) placed=0;
            if(placed)
                for(i=0; i<len; i++)
                {
                    grid[r+i][c]=w->text[i];
                    w->row[i]=r+i;
                    w->col[i]=c;
                }
        }
        else if(dir==2)  // SE
        {
            r=rand()%(GRID_SIZE-len+1);
            c=rand()%(GRID_SIZE-len+1);
            for(i=0; i<len; i++)
                if(grid[r+i][c+i] && grid[r+i][c+i]!=w->text[i]) placed=0;
            if(placed)
                for(i=0; i<len; i++)
                {
                    grid[r+i][c+i]=w->text[i];
                    w->row[i]=r+i;
                    w->col[i]=c+i;
                }
        }
        else  // NE
        {
            r=rand()%(GRID_SIZE-len+1)+(len-1);
            c=rand()%(GRID_SIZE-len+1);
            for(i=0; i<len; i++)
                if(grid[r-i][c+i] && grid[r-i][c+i]!=w->text[i]) placed=0;
            if(placed)
                for(i=0; i<len; i++)
                {
                    grid[r-i][c+i]=w->text[i];
                    w->row[i]=r-i;
                    w->col[i]=c+i;
                }
        }
    }
}

/* ---------------- FINALIZE GRID ---------------- */
void fillRandom()
{
    for(int i=0; i<GRID_SIZE; i++)
        for(int j=0; j<GRID_SIZE; j++)
            if(!grid[i][j])
                grid[i][j]='A'+rand()%26;
}

/* ---------------- PRINT GRID ---------------- */
void printGrid()
{
    // Top border
    printf(BLUE "+");
    for(int j=0; j<GRID_SIZE; j++) printf("--");
    printf("+\n" RESET);

    for(int i=0; i<GRID_SIZE; i++)
    {
        printf(BLUE "|" RESET); // Left border
        for(int j=0; j<GRID_SIZE; j++)
        {
            if(mask[i][j]==1) printf(GREEN "%c " RESET,grid[i][j]);
            else if(mask[i][j]==2) printf(RED "%c " RESET,grid[i][j]);
            else printf("%c ",grid[i][j]);
        }
        printf(BLUE "|\n" RESET); // Right border
    }

    // Bottom border
    printf(BLUE "+");
    for(int j=0; j<GRID_SIZE; j++) printf("--");
    printf("+\n" RESET);
}


/* ---------------- MAIN ---------------- */
int main()
{
    srand(time(NULL));

    if(!captchaVerification())
    {
        printf("Access denied\n");
        return 0;
    }

    menuScreen();
    allocateGrid();
    initGrid();

    Word *answers,*dictionary;
    int ac=loadFile("project.txt",&answers);
    int dc=loadFile("dictionary.txt",&dictionary);

    Word *hidden=malloc(HIDDEN_WORDS*sizeof(Word));
    for(int i=0; i<HIDDEN_WORDS; i++)
    {
        hidden[i]=answers[rand()%ac];
        placeWord(&hidden[i]);
    }
    fillRandom();

    int wrong=0,found=0;
    char guess[WORD_LEN];

    while(1)
    {
        printf("\nWORD SEARCH GRID\n");
        printGrid();
        printf("\nGuess (EXIT to quit): ");
        scanf("%s",guess);
        toUpperCase(guess);

        if(strcmp(guess,"EXIT")==0) break;

        int ok=-1;
        for(int i=0; i<HIDDEN_WORDS; i++)
            if(!hidden[i].found && strcmp(hidden[i].text,guess)==0)
                ok=i;

        if(ok!=-1)
        {
            hidden[ok].found=1;
            for(int k=0; k<hidden[ok].length; k++)
                mask[hidden[ok].row[k]][hidden[ok].col[k]]=1;
            found++;
            wrong=0;
            printf("Correct! %d/%d\n",found,HIDDEN_WORDS);
        }
        else
        {
            int valid=0;
            for(int i=0; i<dc; i++)
                if(strcmp(dictionary[i].text,guess)==0) valid=1;
            if(valid)
            {
                printf("Valid word but not an answer.\n");
                wrong=0;
            }
            else
            {
                wrong++;
                printf("Wrong (%d/3)\n",wrong);
                if(wrong==3) break;
            }
        }
        if(found==HIDDEN_WORDS)
            break;
    }

    // Endgame logic
    if (found == HIDDEN_WORDS)
    {
        printf(GREEN "\nCONGRATULATIONS! You found all hidden words!\n" RESET);
    }
    else
    {
        printf(RED "\nBAD LUCK! You didn't find all words.\n" RESET);
        printf("Hidden words:\n");
        for(int i=0; i<HIDDEN_WORDS; i++)
        {
            if(!hidden[i].found)
            {
                printf("%s\n",hidden[i].text);
                for(int k=0; k<hidden[i].length; k++)
                    mask[hidden[i].row[k]][hidden[i].col[k]]=2;
            }
        }
    }
    printGrid();
    return 0;
}
