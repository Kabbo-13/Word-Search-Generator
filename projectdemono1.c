#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 10   // Grid size 10x10
#define WORDS 5   // Number of words

char grid[SIZE][SIZE];
char wordList[WORDS][15] = {"CAT", "DOG", "BOOK", "STAR", "MOON"};

// Function declarations
void fillGrid();
void placeWords();
void printGrid();
int searchWord(char word[]);
void toUpper(char str[]);

int main() {
    srand(time(NULL));

    fillGrid();       // Fill grid with random letters
    placeWords();     // Place the words
    printGrid();      // Show the puzzle

    printf("\nHidden words are:\n");
    for (int i = 0; i < WORDS; i++) {

    }

    printf("\n\nLet's play! (Type EXIT to quit)\n");

    char input[20];
    while (1) {
        printf("\nEnter a word to search: ");
        scanf("%s", input);

        toUpper(input);  // Convert user input to uppercase

        if (strcmp(input, "EXIT") == 0) {
            printf("\nThank you for playing the Word Search Game!\n");
            break;
        }

        // Check if word is found
        if (searchWord(input)) {
            printf(" Great! '%s' is found in the grid.\n", input);
        } else {
            printf(" Sorry, '%s' is not found.\n", input);
        }
    }

    return 0;
}

// Function to convert string to uppercase
void toUpper(char str[]) {
    for (int i = 0; str[i]; i++) {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] = str[i] - 32;
    }
}

// Fill grid with random letters
void fillGrid() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            grid[i][j] = 'A' + rand() % 26;  // Random capital letter
        }
    }
}

// Randomly place words horizontally
void placeWords() {
    for (int w = 0; w < WORDS; w++) {
        int row = rand() % SIZE;
        int col = rand() % (SIZE - strlen(wordList[w]));

        // Place word horizontally
        for (int i = 0; i < strlen(wordList[w]); i++) {
            grid[row][col + i] = wordList[w][i];
        }
    }
}

// Print the grid
void printGrid() {
    printf("\nWORD SEARCH PUZZLE:\n\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            printf("%c ", grid[i][j]);
        }
        printf("\n");
    }
}

// Search word horizontally or vertically
int searchWord(char word[]) {
    int len = strlen(word);

    // Search horizontally
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j <= SIZE - len; j++) {
            int match = 1;
            for (int k = 0; k < len; k++) {
                if (grid[i][j + k] != word[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) return 1;
        }
    }

    // Search vertically
    for (int i = 0; i <= SIZE - len; i++) {
        for (int j = 0; j < SIZE; j++) {
            int match = 1;
            for (int k = 0; k < len; k++) {
                if (grid[i + k][j] != word[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) return 1;
        }
    }

    return 0;  // Not found
}
