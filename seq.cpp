#include <iostream>
#include <string>
#include <vector>

#include <omp.h>
#include <algorithm>

using namespace std;

typedef long state;

int num_states;
int alphabet_size;
int num_final_states;
state **transition_table;
vector<state> final_states;
string input_str;

#define NUM_THR 4

void define_automata() {
    state final_state;
    freopen("dfapaper.txt","r",stdin);
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

void read_table(){

    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            cout << transition_table[i][j] << " ";
        }
        cout << endl;
    }

}

state rem_parser(state q) {
    int i = 0;
    state j;
    state k;
    int start_position;
    string pi_input;
    omp_set_num_threads(NUM_THR);
    vector<state>::iterator ip;
    size_t len_str = input_str.size();
    // Initialize final result
    vector<vector<state>> I( NUM_THR , vector<state> (len_str/NUM_THR));
    #pragma omp parallel default(shared) private(i, j, k, pi_input, start_position, ip)
    {
        //#pragma omp for schedule(dynamic) nowait
        //#pragma omp for schedule(static,chunk) private(i) nowait
        #pragma omp for schedule(static) nowait
        for(int i = 0; i < NUM_THR; i++){
            int start_position;
            start_position = i*(len_str/NUM_THR);
            
            pi_input = input_str.substr(start_position, (len_str/NUM_THR));
            cout << pi_input << "  ";
            // cout << start_position << "  "<<omp_get_thread_num() << "\n";
            vector<state> S(num_states,-1);
            for(state j = 0; j < num_states; j++){ // 0 1 2 3 4
                state* row = transition_table[j];
                // cout << pi_input.at(0) << " ";
                // cout << row[char_to_int(pi_input.at(0))] << " ";
                if (row[char_to_int(pi_input.at(0))] != -1) S[j] = j;
            }

            vector<state> L(num_states,-1);
            for(state k = 0; k < num_states; k++){ // 0 1 2 3 4
                state* row2 = transition_table[k];
                // cout << pi_input.at(len_str/NUM_THR - 1) << " ";
                // cout << row[char_to_int(pi_input.at(len_str/NUM_THR - 1))] << " ";
                if (row2[char_to_int(pi_input.at(len_str/NUM_THR -1))] != -1) L[k] = row2[char_to_int(pi_input.at(len_str/NUM_THR -1))];
            }
            if(omp_get_thread_num() == 0)
            for(const auto dat: S) cout << dat << " ";
            // cout << "\n";
            if(omp_get_thread_num() == 0)
            for(const auto dat: L) cout << dat << " ";
            // cout << "\n";

            vector<state> R;


            set_intersection(S.begin(), S.end(),L.begin(), L.end(),inserter(R, R.begin()));

            cout << "\n";
            if(omp_get_thread_num() == 2)
                for(const auto dat: R) cout << dat << " ";

    
            
            for (int r : R){
                vector<state> Rr(pi_input.size(),0);
                for (const auto charPI: pi_input){
                    // state* row = transition_table[r];
                    Rr[i] = r;
                }
                I[i] = (Rr);
            }


        }       
    }  /* end of parallel section */

    cout << "\n";
    for(int i = 0; i < NUM_THR; i++){
        for(int j = 0; j < len_str/NUM_THR; j++){
            cout << I[i][j] << " ";
        }
        cout << endl;
    }

    for (int i = 0; i < input_str.size(); i++) {
        if (q == -1) {
            printf("INVALID STATE\n");
            return q;
        }

        char c = input_str[i];
        // printf("CURRENT STATE: %ld, next char: %c\n", q, c);
        q = transition(q, c);
    }
    printf("FINAL STATE: %ld\n", q);

    return q;
}


bool match_re() {
    state final_state = rem_parser(0);
    for(const auto state: final_states) if(final_state == state) return true;
    return false;
    // if (find(final_states.begin(), final_states.end(), final_state) != final_states.end()) {
    //     return true;
    // } else {
    //     return false;
    // }
}

// #define N 10
int main(int argc, char *argv[]) {
    define_automata();
    // read_table();
    cin >> input_str;

    if (match_re()) {
        cout << "YES" << endl;
    } else {
        cout << "NO" << endl;
    }

    return 0;
}