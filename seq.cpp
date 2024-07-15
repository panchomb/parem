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

#define num_thr 16

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

bool rem_parser(int nth) {
    int i;
    state j;
    state k;
    int start_position;
    string pi_input;
    omp_set_num_threads(nth);
    vector<state>::iterator ip;
    size_t len_str = input_str.size();
    size_t chunk_size = len_str/nth;
    // Initialize final result
    vector<vector<vector<state> > > I(nth);
    #pragma omp parallel private(i, j, k, pi_input, start_position, ip)
    {
        //#pragma omp for schedule(static) nowait
        int start_position, end_position;
        i = omp_get_thread_num();

        

        start_position = i*chunk_size;
        end_position = start_position + (len_str/nth);


        //pi_input = input_str.substr(start_position, (len_str/nth));

        vector<state> S(num_states,-1);
        for(state j = 0; j < num_states; j++){
            state* row = transition_table[j];
            if (row[char_to_int(input_str[start_position])] != -1) S[j] = j;
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
            for (int k = start_position; k < end_position; k++){
                //state* row = transition_table[r];
                Rr.push_back(r);
                r = transition_table[r][char_to_int(input_str[k])];
            }
            Rr.push_back(r);
            I[i].push_back(Rr);
        }

    }  /* end of parallel section */

    // lecture I
    // int thread_id = 0;
    // for(const auto thread: I){
    //     cout << "Thread: " << thread_id++ << "\n";
    //     for(const auto initial_states: thread){
    //         for (const auto state: initial_states) cout << state << " ";
    //         cout << "\n";
    //     }
    // }


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
    
    for (int i = 1; i < nth; i++)
    {
        int start_position, end_position;
        start_position = i*(len_str/nth);
        end_position = start_position + (len_str/nth);
        bool flag = 0;

        if (I[i].empty())
            return 0;

        for (auto Rr : I[i]) {
            // verify if rr size is partition_size and if it continues the matching
            if (cur == Rr.front() && Rr.size() == end_position - start_position + 1){
                flag = 1; // continue matching
                cur = Rr.back();
                break;
            }
        }

        if (!flag) // if there is no matching left
            return false;
    }
    if (find(final_states.begin(), final_states.end(), cur) != final_states.end())
        return true;
    else
        return false;
}

state transition_seq(state q, char c) {
    state* row = transition_table[q];
    return row[char_to_int(c)];
}

state rem_parser_seq(state q) {
    for (int i = 0; i < input_str.size(); i++) {
        if (q == -1) {
            return q;
        }

        char c = input_str[i];
        q = transition_seq(q, c);
    }

    return q;
}


bool match_re(int n_threads) {
    return rem_parser(n_threads);
}

bool match_re_seq() {
    state final_state = rem_parser_seq(0);
    for(const auto state: final_states) if(final_state == state) return true;
    return false;
}


int main(int argc, char *argv[]) {
    double itime, ftime, exec_time;
    int n_threads = 1;

    define_automata();
    // dfapaper.txt is from the paper
    // 
    cin >> input_str;
    string og_str = input_str;

    while (n_threads <= 16) {
        for (int i = 10; i <= 24; i++) {
            input_str = og_str;
            int len = pow(2, i);
            while (input_str.size() < len) {
                input_str += input_str.substr(0, len - input_str.size());
            }
            for (int j = 0; j < 100; j++) {
                itime = omp_get_wtime();
                if (num_thr == 0)
                    bool match = match_re_seq();
                else
                    bool match = match_re(n_threads);
                ftime = omp_get_wtime();

                exec_time = ftime - itime;
                printf("%d %d %f\n", n_threads, i, exec_time);
            }
        }
        n_threads *= 2;
    }

    // if (match_re(4)) {
    //     cout << "YES, input match" << endl;
    // } else {
    //     cout << "NO, input does not match" << endl;
    // }

    return 0;
}