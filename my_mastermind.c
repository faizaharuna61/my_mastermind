#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


#define DEFAULT_ATTEMPTS 10
#define CODE_LENGTH 4
#define COLORS "012345678"


void print_prompt();
void generate_my_random_code(char *code);
void play_my_game(char *secret_code, int attempts);
int read_my_input(char *buffer);
void check_my_guess(char *secret_code, char *guess, int *well_placed, int *misplaced);




int main(int argc, char *argv[]) {
    char secret_code[CODE_LENGTH + 1] = {0};
    int attempts = DEFAULT_ATTEMPTS;
    int c_flag = 0; 

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0 && (i + 1) < argc) {
            strncpy(secret_code, argv[i + 1], CODE_LENGTH);
            c_flag = 1;
            i++;
        } else if (strcmp(argv[i], "-t") == 0 && (i + 1) < argc) {
            attempts = atoi(argv[i + 1]);
            i++;
        }
    }

    if (!c_flag) {
        generate_my_random_code(secret_code);
    }

    play_my_game(secret_code, attempts);
    return 0;
}

void print_prompt() {
    printf("Will you find the secret code?\n");
    printf("Please enter a valid guess\n");
}

void generate_my_random_code(char *code) {
    srand(time(NULL));
    for (int i = 0; i < CODE_LENGTH; i++) {
        char random_char;
        int is_unique;
        do {
            is_unique = 1;
            random_char = COLORS[rand() % strlen(COLORS)];
            for (int j = 0; j < i; j++) {
                if (code[j] == random_char) {
                    is_unique = 0;
                    break;
                }
            }
        } while (!is_unique);
        code[i] = random_char;
    }
    code[CODE_LENGTH] = '\0';
}

void play_my_game(char *secret_code, int attempts) {
    char guess[CODE_LENGTH + 1];
    int round = 0;

    print_prompt();

    while (round < attempts) {
        printf("---\nRound %d\n", round);

        int read_result = read_my_input(guess);
        if (read_result == 1) {
            printf("Wrong input!\n");
            continue;
        } else if (read_result == -1) {
            printf("\nEOF detected. Exiting game.\n");
            return;
        }

        int well_placed, misplaced;
        check_my_guess(secret_code, guess, &well_placed, &misplaced);

        if (well_placed == CODE_LENGTH) {
            printf("Congratz! You did it!\n");
            return;
        }

        printf("Well placed pieces: %d\n", well_placed);
        printf("Misplaced pieces: %d\n", misplaced);
        round++;
    }

    printf("Game over! The secret code was %s\n", secret_code);
}

int read_my_input(char *buffer) {
    char c;
    int length = 0;
    int read_result;

    while (length < CODE_LENGTH) {
        read_result = read(0, &c, 1);
        if (read_result == 0) {
            // EOF detected
            return -1;
        }
        if (c == '\n') {
            break;
        }
        buffer[length++] = c;
    }

    buffer[length] = '\0';

    // Check if the input length is correct and all characters are valid
    if (length != CODE_LENGTH) {
        return 1;
    }

    for (int i = 0; i < CODE_LENGTH; i++) {
        if (strchr(COLORS, buffer[i]) == NULL) {
            return 1;
        }
    }

    return 0;
}

void check_my_guess(char *secret_code, char *guess, int *well_placed, int *misplaced) {
    *well_placed = 0;
    *misplaced = 0;
    int secret_counts[9] = {0};
    int guess_counts[9] = {0};

    for (int i = 0; i < CODE_LENGTH; i++) {
        if (guess[i] == secret_code[i]) {
            (*well_placed)++;
        } else {
            secret_counts[secret_code[i] - '0']++;
            guess_counts[guess[i] - '0']++;
        }
    }

    for (int i = 0; i < 9; i++) {
        *misplaced += (secret_counts[i] < guess_counts[i]) ? secret_counts[i] : guess_counts[i];
    }
}