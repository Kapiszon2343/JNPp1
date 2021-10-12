#include<unordered_map>
#include<vector>
#include<queue>
#include<vector>
#include<string>
#include<regex>
#include<regex.h>
#include<istream>
#include<iostream>

using std::queue;
using std::unordered_map;
using std::pair;
using std::vector;
using std::string;
using std::regex;
using std::regex_match;
using std::istringstream;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;


using signal_t = int32_t;
using gate = pair< void (*)(pair<signal_t, vector<signal_t> >, unordered_map<signal_t, bool>), pair<signal_t, vector<signal_t> > >;

namespace  {
	void AND_f(pair<signal_t, vector<signal_t> > out_in, 
		unordered_map<signal_t, bool> states) {
		bool output_state = true;
		for(auto in_id : out_in.second) {
			output_state &= states[in_id];
		}
		states[out_in.first] = output_state;
	}
	
	void read_input(vector<gate> const *gates) {

	    regex not_g("^\\s*NOT(\\s+\\d{1,9}){2}\\s*$");
	    regex xor_g("^\\s*XOR(\\s+\\d{1,9}){3}\\s*$");
        regex rest_g("^\\s*(AND|NAND|OR|NOR)(\\s+\\d{1,9}){3,}\\s*$");

	    bool reading_input = true;
	    int no_line = 1;

	    while(reading_input) {
            string input;
            getline(cin, input);

            if(input.size() == 0) {
                cout << "Nic tu nie ma" << endl;
                //gdy nie ma nic na wejściu
            }
            else {
                istringstream iss(input);
                int data;
                if(regex_match(input, not_g)) {
                    iss >> data;
                    //obsluz not
                }
                else if(regex_match(input, xor_g)) {
                    //obsłuż xor
                }
                else if(regex_match(input, rest_g)) {
                    //reszta
                }
                else{
                    cerr << "Error in line " << no_line << ": " << input << endl;
                }
            }
            no_line++;
        }
	}
	
	bool check_for_sequential_logic(vector<gate> const *gates) {
		vector<int> waiting_for_inputs;
		unordered_map<signal_t, vector<size_t> > gates_per_input;
		unordered_map<signal_t, int> in_out; // bit 1 lit -> is an input; bit 2 lit -> is an output
		
		for(size_t i = 0; i < gates->size(); ++i) {
			waiting_for_inputs.push_back((*gates)[i].second.second.size());
			in_out[(*gates)[i].second.first] |= 2;
			for(signal_t input : (*gates)[i].second.second) {
				gates_per_input[input].push_back(i);
				in_out[input] |= 1;
			}
		}
		
		queue<signal_t> ready_inputs;
		
		for ( auto it = in_out.begin(); it != in_out.end(); ++it  ) {
			if(it->second == 1) {
				ready_inputs.push(it->first);
			}
		}
		
		while(!ready_inputs.empty()) {
			signal_t input = ready_inputs.front();
			ready_inputs.pop();
			
			for(size_t i : gates_per_input[input]) {
				--waiting_for_inputs[i];
				if(waiting_for_inputs[i] == 0) {
					ready_inputs.push((*gates)[i].second.first);
				}
			}
		}
		
		bool sequential_logic = false;
		
		for(int nr_of_uncheked_inputs : waiting_for_inputs) {
			if(nr_of_uncheked_inputs > 0) {
				sequential_logic = true;
				break;
			}
		}
		
		return sequential_logic;
	}
	
	void output(vector<gate> const *gates) {
		//TODO
	}
}

int main() {
	vector<gate> gates;
	
	read_input(&gates);
	
	check_for_sequential_logic(&gates);
	
	output(&gates);
}
