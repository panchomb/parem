#include <iostream>
#include <string>
#include <vector>

#include <set>
#include <math.h>
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


void read_table(){

    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < alphabet_size; j++) {
            cout << transition_table[i][j] << " ";
        }
        cout << endl;
    }

}

vector<vector<state>> eliminateDuplicate(const vector<vector<state>>& matriz) {
    set<vector<state>> filasUnicas;
    vector<vector<state>> resultado;

    for (const auto& fila : matriz) {
        vector<state> filaSinPrimerValor(fila.begin() + 1, fila.end());
        if (filasUnicas.insert(filaSinPrimerValor).second) {
            resultado.push_back(fila);
        }
    }

    return resultado;
}

bool rem_parser() {
    int i = 0;
    state j;
    state k;
    int start_position;
    string pi_input;
    omp_set_num_threads(NUM_THR);
    vector<state>::iterator ip;
    size_t len_str = input_str.size();
    // Initialize final result
    vector<vector<vector<state>>> I(NUM_THR);
    #pragma omp parallel default(shared) private(i, j, k, pi_input, start_position, ip)
    {

        #pragma omp for schedule(static) nowait
        for(int i = 0; i < NUM_THR; i++){
            int start_position, end_position;
            start_position = i*(len_str/NUM_THR);
            end_position = start_position + (len_str/NUM_THR);
            pi_input = input_str.substr(start_position, (len_str/NUM_THR));
            vector<state> S(num_states,-1);
            for(state j = 0; j < num_states; j++){
                state* row = transition_table[j];
                if (row[char_to_int(pi_input.at(0))] != -1) S[j] = j;
            }

            vector<state> L(num_states,0);
            for(state k = 0; k < num_states; k++){
                state* row2 = transition_table[k];
                if (i && (row2[char_to_int(input_str[start_position-1])] != -1)) 
                    L[k] = row2[char_to_int(input_str[start_position-1])];
            }
            vector<state> R;

            sort(S.begin(), S.end());
            sort(L.begin(), L.end());
            set_intersection(S.begin(), S.end(),L.begin(), L.end(),inserter(R, R.begin()));
            
            for (int r : R){
                vector<state> Rr;
                for (const auto charPI: pi_input){
                    state* row = transition_table[r];
                    Rr.push_back(r);
                    r = transition_table[r][char_to_int(charPI)];
                }
                Rr.push_back(r);
                I[i].push_back(Rr);
            }


        }       
    }  /* end of parallel section */


    // reduction I
    for (size_t i = 0; i < NUM_THR; ++i) {
        I[i] = eliminateDuplicate(I[i]);
    }

    // lecture I
    int thread_id = 0;
    for(const auto thread: I){
        cout << "Thread: " << thread_id++ << "\n";
        for(const auto initial_states: thread){
            for(const auto state: initial_states) cout << state << " ";
            cout << "\n";
        }
    }


    bool f_state = 0;
    // if theres no final state
    for (auto Rr : I.back())
    {
        if (find(final_states.begin(), final_states.end(), Rr.back()) != final_states.end())
        {
            f_state = 1;
            break;
        }
    }
    if (!f_state)
        return 0;

    int cur = I.front().front().back();
    
    for (int i = 1; i < NUM_THR; i++)
    {
        int start_position, end_position;
        start_position = i*(len_str/NUM_THR);
        end_position = start_position + (len_str/NUM_THR);
        bool flag = 0;

        if (I[i].empty())
            return 0;

        for (auto Rr : I[i]) 
            if (cur == Rr.front() && Rr.size() == end_position - start_position + 1){
                flag = 1;
                cur = Rr.back();
                break;
            }

        if (!flag)
            return false;
    }

    return true;
}


bool match_re() {
    return rem_parser();
}


int main(int argc, char *argv[]) {
    define_automata();
    // dfapaper.txt is from the paper
    // 
    cin >> input_str;

    if (match_re()) {
        cout << "YES, input match" << endl;
    } else {
        cout << "NO, input does not match" << endl;
    }

    return 0;
}