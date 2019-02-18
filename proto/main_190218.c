#include <stdio.h>
#include <stdlib.h>

struct BOARD {
    int piece[25];
    int selecter_pos;
    int last_selecter_pos;
    int *last_B_state_arr;
};

int *B_state_arr;

#define B_NUM 0
#define B_SEL 1 // selecter
#define B_NON 2 // none
#define B_OVER 9 // game over(:= B_NON * 4 + B_SEL)

struct BOARD *B;
#define YOUR_TURN 0
#define OPP_TURN 1

char turn;
int you_point;
int opp_point;

void init_board() {
    int pieces[25] = {9, -9, 8, -8, 7, -7, 6, -6, 5, -5, 4, -4,
                      3, -3, 3, -3, 2, -2, 2, -2, 1, -1, 1, -1, 0};

    // 1: 選ばれた, 0: 選ばれていない
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

        B_state_arr[p_pos] = B_NUM;
        if (pieces[abs_index] == 0) {
            B->selecter_pos = p_pos;
            B_state_arr[p_pos] = B_SEL;
        }
        B->piece[p_pos] = pieces[abs_index];
    }
}

void print_cell(int index) {
    char s = B_state_arr[index];
    if (s == B_SEL) {
        printf("  X|");
    } else if (s == B_NON) {
        printf("   |");
    } else {
        printf("%3d|", B->piece[index]);
    }
}

void print_board() {
    printf("+-------------------+\n");
    for (int i = 0; i < 25; i++) {
        if (i % 5 == 0) {
            printf("|");
        }

        print_cell(i);

        if ((i + 1) % 5 == 0) {
            printf(" %d", (i / 5) + 1);
        }
        if (0 < i && i < 24 && (i + 1) % 5 == 0) {
            printf("\n|---+---+---+---+---|\n");
        }
    }
    printf("\n+-------------------+\n");
    printf("  a   b   c   d   e\n\n");
}

// debug用
void print_B_state_arr() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("  %d", B_state_arr[i*5 + j]);
        }
        printf("\n");
    }
}

int is_gameover() {
    int s_pos = B->selecter_pos;
    int s_row = s_pos / 5;
    int s_col = s_pos % 5;

    int state_sum = 0;
    if (turn == YOUR_TURN) {
        for (int i = 0; i < 5; i++) {
            state_sum += B_state_arr[s_row * 5 + i];
        }
    } else {
        for (int i = 0; i < 5; i++) {
            state_sum += B_state_arr[s_col + i * 5];
        }
    }

    if (state_sum == B_OVER) {
        return 1;
    }
    return 0;
}

int move2pos(char move[2]) {
    int row = move[1] - '1';
    int col = move[0] - 'a';
    return row * 5 + col;
}

void pos2move(int pos, char move[2]) {
    int row = pos / 5;
    int col = pos % 5;
    move[0] = col + 'a';
    move[1] = row + '1';
}

int is_legal_move_form(char move[2]) {
    int row = move[1] - '1';
    int col = move[0] - 'a';
    if (0 <= row && row <= 4 && 0 <= col && col <= 4) {
        return 1;
    }
    return 0;
}

int is_legal_move(char move[2]) {
    if (!is_legal_move_form(move)) {
        printf("input needs in a1,..., e5\n");
        return 0;
    }

    int s_pos = B->selecter_pos;
    int s_row = s_pos / 5;
    int s_col = s_pos % 5;

    int m_pos = move2pos(move);
    int m_row = m_pos / 5;
    int m_col = m_pos % 5;

    if (turn == YOUR_TURN) {
        // allow only horizontal move
        if (s_pos != m_pos && s_row == m_row && B_state_arr[m_pos] != B_NON) {
            return 1;
        }
        return 0;
    } else {
        // allow only vertical move
        if (s_pos != m_pos && s_col == m_col && B_state_arr[m_pos] != B_NON) {
            return 1;
        }
        return 0;
    }
}

// return the valid moves array(vma).
// vma's 1st element is length of the vma except itself.
// ex. [3, 5, 6, 9] means that valid moves is 5, 6, 9(a2, b2, e2).
int *get_legal_moves() {
    int s_pos = B->selecter_pos;
    int s_row = s_pos / 5;
    int s_col = s_pos % 5;

    int *ret_arr = malloc(sizeof(int) * 5);
    int legal_moves_count = 0;

    // 縦一列を探す
    if (turn == OPP_TURN) {
        for (int i = 0; i < 5; i++) {
            int pos = i * 5 + s_col;
            if (i == s_row || B_state_arr[pos] == B_NON) {
                continue;
            }
            legal_moves_count++;
            ret_arr[legal_moves_count] = pos;
        }
    }

    // 横一列を探す
    if (turn == YOUR_TURN) {
        for (int i = 0; i < 5; i++) {
            int pos = i + s_row * 5;
            if (i == s_col || B_state_arr[pos] == B_NON) {
                continue;
            }
            legal_moves_count++;
            ret_arr[legal_moves_count] = pos;
        }
    }

    ret_arr[0] = legal_moves_count;
    return ret_arr;
}

void print_result() {
    printf("%d - %d\n", you_point, opp_point);
    if (you_point > opp_point) {
        printf("you win\n");
    } else if (you_point < opp_point) {
        printf("you lose\n");
    } else {
        printf("draw\n");
    }
}

int get_COM_imm_opt_move() {
    int s_pos = B->selecter_pos;
    int s_row = s_pos / 5;
    int s_col = s_pos % 5;

    // 縦一列のmaxを探す
    int tmp = -10000;
    int ret;
    for (int i = 0; i < 5; i++) {
        int pos = i * 5 + s_col;
        if (i == s_row || B_state_arr[pos] == B_NON) {
            continue;
        }
        if (B->piece[pos] > tmp) {
            tmp = B->piece[pos];
            ret = pos;
        }
    }
    return ret;
}

int get_YOUR_imm_opt_move() {
    int s_pos = B->selecter_pos;
    int s_row = s_pos / 5;
    int s_col = s_pos % 5;

    // 横一列のmaxを探す
    int tmp = -10000;
    int ret;
    for (int i = 0; i < 5; i++) {
        int pos = i + s_row * 5;
        if (i == s_col || B_state_arr[pos] == B_NON) {
            continue;
        }
        if (B->piece[pos] > tmp) {
            tmp = B->piece[pos];
            ret = pos;
        }
    }
    return ret;
}

// put a next state
void push_pos(int pos) {
    B->last_B_state_arr = B_state_arr;
    B_state_arr[pos] = B_SEL;
    B_state_arr[B->selecter_pos] = B_NON;
    B->last_selecter_pos = B->selecter_pos;
    B->selecter_pos = pos;

    // YOUR_TURN => OPP_TURN, OPP_TURN => YOUR_TURN
    turn ^= 1;
}

// incorrect?, cannot call pop() -> pop()
void pop() {
    B_state_arr = B->last_B_state_arr;
    B->selecter_pos = B->last_selecter_pos;
    turn ^= 1;
}

// incorrect!
int min_max(int depth) {
    if (depth == 0) {
        return B->piece[B->selecter_pos];
    }

    // malloc 必要？
    int *B_state_arr_org = B_state_arr;
    int selecter_pos_org = B->selecter_pos;
    char turn_org = turn;

    int ret_pos;

    int *legal_moves = get_legal_moves();
    for (int i = 0; i < legal_moves[0]; i++) {
        int val;
        int val_MAX = -10000;

        int pos = legal_moves[i+1];
        push_pos(pos);
        val = min_max(depth - 1);

        B_state_arr = B_state_arr_org;
        B->selecter_pos = selecter_pos_org;
        turn = turn_org;

        if (val > val_MAX) {
            val_MAX = val;
            ret_pos = pos;
        }
    }
    return ret_pos;
}

int min_max_simple() {
    // malloc 必要？
    int *B_state_arr_org = B_state_arr;
    int selecter_pos_org = B->selecter_pos;
    char turn_org = turn;

    int ret_pos;

    int *legal_moves = get_legal_moves();
    int val_MAX = -10000;
    int tmp_val = 0;
    for (int i = 0; i < legal_moves[0]; i++) {
        int pos = legal_moves[i+1];
        tmp_val += B->piece[pos];
        push_pos(pos);

        int *legal_moves_opp = get_legal_moves();
        for (int j = 0; j < legal_moves_opp[0]; j++) {
            int pos_opp = legal_moves_opp[j+1];
            tmp_val -= B->piece[pos];

            int *B_state_arr_org_opp = B_state_arr;
            int selecter_pos_org_opp = B->selecter_pos;
            char turn_org_opp = turn;

            push_pos(pos);

            if (tmp_val > val_MAX) {
                tmp_val = val_MAX;
                ret_pos = pos;
            }

            tmp_val = 0;

            B_state_arr = B_state_arr_org_opp;
            B->selecter_pos = selecter_pos_org_opp;
            turn = turn_org_opp;
        }

        B_state_arr = B_state_arr_org;
        B->selecter_pos = selecter_pos_org;
        turn = turn_org;
    }
    return ret_pos;
}

void next_state() {
    if (turn == YOUR_TURN) {
        char move[2];
        printf("move: ");
        scanf("%s", move);
        while (!is_legal_move(move)) {
            printf("invalid move, choose again: ");
            scanf("%s", move);
        }

        int m_pos = move2pos(move);
        you_point += B->piece[m_pos];
        push_pos(m_pos);
    } else {
        int com_move_pos = get_COM_imm_opt_move();
        //int com_move_pos = min_max(1);
        //int com_move_pos = min_max_simple();
        char com_move[2];
        pos2move(com_move_pos, com_move);
        printf("COM move: %s\n", com_move);

        opp_point += B->piece[com_move_pos];
        push_pos(com_move_pos);
    }
}

int main() {
    B = malloc(sizeof(struct BOARD));
    B_state_arr = malloc(sizeof(int) * 25);
    init_board();

    printf("Input mode(1: vs COM): ");
    int mode;
    scanf("%d", &mode);
    if (mode != 1) {
        printf("BYE\n");
        return 0;
    }

    turn = YOUR_TURN;

    // 先手は横、後手は縦
    you_point = 0;
    opp_point = 0;
    while (1) {
        print_board();

        if (is_gameover()){
            printf("game over\n");
            print_result();
            break;
        }

        next_state();
    }

    return 0;
}
