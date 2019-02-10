#include <stdio.h>
#include <stdlib.h>

struct BOARD {
    int piece[25];
    int selecter_pos;
};

struct BOARD *B;

void init_board() {
    int pieces[25] = {9, -9, 8, -8, 7, -7, 6, -6, 5, -5, 4, -4,
                      3, -3, 3, -3, 2, -2, 2, -2, 1, -1, 1, -1, 0};

    // 1: 選ばれた, 0:選ばれていない
    char choice_array[25] = {0};

    // Fisher-Yates shuffle
    for (int p_pos = 0; p_pos < 25; p_pos++) {
        int choice_count = 0;

        for (int i = 0; i < 25; i++) {
            if (choice_array[i] == 1) {
                choice_count += 1;
            }
        }

        int rand_n = arc4random() % (25 - choice_count);

        int abs_count = 0;
        int abs_index;
        for (int j = 0; j < 25; j++) {
            if (choice_array[j] == 0) {
                if (abs_count == rand_n) {
                    abs_index = j;
                    break;
                }
                abs_count++;
            }
        }

        choice_array[abs_index] = 1;
        if (pieces[abs_index] == 0) {
            B->selecter_pos = p_pos;
        }
        B->piece[p_pos] = pieces[abs_index];
    }
}

void print_board() {
    printf("+-------------------+\n");
    for (int i = 0; i < 25; i++) {
        if (i % 5 == 0) {
            printf("|");
        }
        int n = B->piece[i];
        if (n == 0) {
            printf("  X|");
        } else {
            printf("%3d|", B->piece[i]);
        }

        if (0 < i && i < 24 && (i + 1) % 5 == 0) {
            printf("\n");
            printf("|---+---+---+---+---|\n");
        }
    }
    printf("\n+-------------------+\n");
}

int main() {
    B = malloc(sizeof(struct BOARD));
    init_board();

    print_board();

    printf("selecter position: %d\n", B->selecter_pos);

    return 0;
}
