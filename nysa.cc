#include<unordered_map>
#include<vector>
#include<queue>
#include<set>
#include<string>
#include<regex>
#include<istream>
#include<iostream>
#include<cassert>

using std::queue;
using std::unordered_map;
using std::pair;
using std::vector;
using std::string;
using std::set;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::istringstream;
using std::cin;
using std::cout;
using std::endl;
using std::cerr;


using signal_t = int32_t;
using states_map = unordered_map<signal_t, bool>;
using gate = pair<
	void (*)(pair<signal_t, vector<signal_t> >, states_map*),
	pair<signal_t, vector<signal_t> > 
	>;
typedef void (*GATE_f)(pair<signal_t, vector<signal_t> > out_in, states_map* states);

namespace  {
	void AND_f(const pair<signal_t, vector<signal_t> > out_in,
		states_map *states) {
	    assert(out_in.second.size() >= 2);
		bool output_state = true;
		for(auto in_id : out_in.second) {
			output_state &= (*states)[in_id];
		}
		(*states)[out_in.first] = output_state;
	}

    void NAND_f(const pair<signal_t, vector<signal_t> > out_in,
               states_map *states) {
        assert(out_in.second.size() >= 2);
        bool output_state = true;
        for(auto in_id : out_in.second) {
            output_state &= (*states)[in_id];
        }
        (*states)[out_in.first] = !output_state;
    }

    void OR_f(const pair<signal_t, vector<signal_t> > out_in,
                states_map *states) {
        assert(out_in.second.size() >= 2);
        bool output_state = false;
        for(auto in_id : out_in.second) {
            output_state |= (*states)[in_id];
        }
        (*states)[out_in.first] = output_state;
    }

    void XOR_f(const pair<signal_t, vector<signal_t> > out_in,
                states_map *states) {
        assert(out_in.second.size() == 2);
        bool output_state;
        if((*states)[out_in.second.at(0)] != (*states)[out_in.second.at(1)])
            output_state = true;
        else
            output_state = false;

        (*states)[out_in.first] = output_state;
    }

    void NOT_f(const pair<signal_t, vector<signal_t> > out_in,
                states_map *states) {
        assert(out_in.second.size() == 1);
        bool output_state = true;
        for(auto in_id : out_in.second) {
            output_state = !((*states)[in_id]);
        }
        (*states)[out_in.first] = output_state;
    }

    void NOR_f(const pair<signal_t, vector<signal_t> > out_in,
                states_map *states) {
        assert(out_in.second.size() >= 2);
        bool output_state = false;
        for(auto in_id : out_in.second) {
            output_state |= (*states)[in_id];
        }
        (*states)[out_in.first] = !output_state;
    }

    GATE_f get_correct_gate_pointer(string &name){
	    if(name == "AND")
            return AND_f;
        else if(name == "NAND")
            return NAND_f;
        else if(name == "OR")
            return OR_f;
        else if(name == "XOR")
            return XOR_f;
        else if(name == "NOT")
            return NOT_f;
        else if(name == "NOR")
            return NOR_f;
        else
            cerr << "B????d wykonania w f. get_correct_gate_pointer, otrzymana nazwa to " << name << endl;

        return nullptr;
	}

	void add_gate(vector<signal_t> in, signal_t out, string gate_name, vector<gate> *gates){
	    pair<signal_t, vector<signal_t> > signals (out, in);
        pair<void (*)(pair<signal_t, vector<signal_t> >, states_map*), pair<signal_t, vector<signal_t> > >
                gate_a (get_correct_gate_pointer(gate_name), signals);
        gates->push_back(gate_a);
	}

	bool read_input(vector<gate> *gates) {

	    regex not_g("^\\s*NOT(\\s+[1-9]\\d{0,8}){2}\\s*$");
	    regex xor_g("^\\s*XOR(\\s+[1-9]\\d{0,8}){3}\\s*$");
	    regex rest_g("^\\s*(AND|NAND|OR|NOR)(\\s+[1-9]\\d{0,8}){3,}\\s*$");
	    regex name_g("(AND|NAND|OR|XOR|NOT|NOR)");

	    bool first = true, no_error = true;
	    int no_line = 1;
        string line;
        set<signal_t> output_set;

	    while(getline(cin, line)) {
            vector<signal_t> in;
            signal_t out;
            int data;
            bool zero_signal = false;

            if( regex_match(line, not_g) ||
                regex_match(line, xor_g) ||
                regex_match(line, rest_g)) {

                auto gate_name_iter = std::sregex_iterator(line.begin(), line.end(), name_g);
                std::smatch match = *gate_name_iter; //match z nazw?? bramki
                string name_gate = match.str();

                string num_line = regex_replace(line, name_g, "");
                istringstream iss(num_line);
                while(!zero_signal && iss >> data) {
                    if(data == 0){
                        zero_signal = true;
                    }
                    if(first) {
                        if(output_set.empty() || output_set.find(data) == output_set.end()) {
                            output_set.insert(data);
                        }
                        else {
                            cerr << "Error in line " << no_line << ": signal "
                            << data << " is assigned to multiple outputs." << endl;
                            no_error = false;
                        }
                        out = data;
                        first = false;
                    }
                    else {
                        in.push_back(data);
                    }
                }
                if(zero_signal){
                    cerr << "Error in line " << no_line << ": " << line << endl;
                    no_error = false;
                }
                else{
                    add_gate(in, out, name_gate, gates);
                }

            }
            else {
                cerr << "Error in line " << no_line << ": " << line << endl;
                no_error = false;
            }

            no_line++;
            first = true;
        }

        if (cin.bad()) {
            cerr << "I/O error" << endl;
            exit(1);
        }
        else if (!cin.eof()) {
            cerr << "format error" << endl;
            exit(1);
        }
        else {
            return no_error;
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
		
		for (auto it = in_out.begin(); it != in_out.end(); ++it  ) {
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
		
		bool no_error = true;
		
		for(int nr_of_uncheked_inputs : waiting_for_inputs) {
			if(nr_of_uncheked_inputs > 0) {
				no_error = false;
				break;
			}
		}
		
		return no_error;
	}
	
	void calculate_single_output(
		vector<gate> const *gates,
		unordered_map<signal_t, vector<size_t> > *gates_per_input,
		vector<signal_t> const *inputs,
		vector<signal_t> const *outputs, 
		states_map *states) {
		
		queue<size_t> ready_inputs;
		vector<int> waiting_for_inputs;
		for(size_t i = 0; i < gates->size(); ++i) {
			waiting_for_inputs.push_back((*gates)[i].second.second.size());
		}
		for(signal_t input : *inputs) {
			ready_inputs.push(input);
		}
		
		
		while(!ready_inputs.empty()) {
			signal_t input = ready_inputs.front();
			ready_inputs.pop();
			
			for(size_t i : (*gates_per_input)[input]) {
				--waiting_for_inputs[i];
				if(waiting_for_inputs[i] == 0) {
					(*gates)[i].first((*gates)[i].second, states);
					ready_inputs.push((*gates)[i].second.first);
				}
			}
		}

		for(signal_t output : *outputs) {
			cout << (*states)[output];
		}
		cout << endl;
	}
	
	void output_step(
		vector<gate> const *gates,
		unordered_map<signal_t, vector<size_t> > *gates_per_input,
		vector<signal_t> const *inputs, 
		vector<signal_t> const *outputs, 
		states_map *states, 
		size_t done_steps) {
			if(done_steps == inputs->size()) {
				calculate_single_output(
					gates,
					gates_per_input, 
					inputs, 
					outputs, 
					states);
			}
			else {
				(*states)[(*inputs)[done_steps]] = 0;
				output_step(
					gates,
					gates_per_input, 
					inputs, 
					outputs, 
					states, 
					done_steps+1);
				(*states)[(*inputs)[done_steps]] = 1;
				output_step(
					gates,
					gates_per_input, 
					inputs, 
					outputs, 
					states, 
					done_steps+1);
			}
	}
	
	void output(vector<gate> const *gates) {
		vector<int> waiting_for_inputs;
		unordered_map<signal_t, vector<size_t> > gates_per_input;
		unordered_map<signal_t, int> in_out; // bit 1 lit -> is an input; bit 2 lit -> is an output
		vector<signal_t> only_inputs, only_outputs;
		
		for(size_t i = 0; i < gates->size(); ++i) {
			waiting_for_inputs.push_back((*gates)[i].second.second.size());
			in_out[(*gates)[i].second.first] |= 2;
			for(signal_t input : (*gates)[i].second.second) {
				gates_per_input[input].push_back(i);
				in_out[input] |= 1;
			}
		}
		
		for ( auto it = in_out.begin(); it != in_out.end(); ++it  ) {
			if(it->second == 1) {
				only_inputs.push_back(it->first);
			}
			only_outputs.push_back(it->first);
		}
		
		sort(only_inputs.begin(), only_inputs.end());
		sort(only_outputs.begin(), only_outputs.end());
		
		states_map states;
		output_step(
			gates,
			&gates_per_input, 
			&only_inputs, 
			&only_outputs, 
			&states, 
			0);
	}
}

int main() {
	vector<gate> gates;
	
	if(!read_input(&gates)) {
		return 0;
	}
	
	if(!check_for_sequential_logic(&gates)) {
		cerr << "Error: sequential logic analysis has not yet been implemented." << endl;
		return 0;
	}
	
	output(&gates);
}
