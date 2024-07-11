#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef long state;

int num_states;
int alphabet_size;
int num_final_states;
state **transition_table;
vector<state> final_states;
string input_str;

void define_automata() {
    state final_state;
    
    cin >> num_states >> alphabet_size >> num_final_states;

    transition_table = new state*[num_states];
    for (int i = 0; i < num_states; i++) {
        transition_table[i] = new state[alphabet_size];
        for (int j = 0; j < alphabet_size; j++) {
            cin >> transition_table[i][j];
        }
    }

    for (int i = 0; i < num_final_states; i++) {
        cin >> final_state;
        final_states.push_back(final_state);
    }
}

int char_to_int(char c) {
    return (int) c - 'a';
}

state transition(state q, char c) {
    state* row = transition_table[q];
    return row[char_to_int(c)];
}

state rem_parser(state q) {
    for (int i = 0; i < input_str.size(); i++) {
        if (q == -1) {
            printf("INVALID STATE\n");
            return q;
        }

        char c = input_str[i];
        printf("CURRENT STATE: %ld, next char: %c\n", q, c);
        q = transition(q, c);
    }
    printf("FINAL STATE: %ld\n", q);

    return q;
}


bool match_re() {
    state final_state = rem_parser(0);

    if (find(final_states.begin(), final_states.end(), final_state) != final_states.end()) {
        return true;
    } else {
        return false;
    }
}

int main() {
    define_automata();

    cin >> input_str;

    if (match_re()) {
        cout << "YES" << endl;
    } else {
        cout << "NO" << endl;
    }

    return 0;
}