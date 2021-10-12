#include<unordered_map>
#include<vector>
#include<queue>

using std::queue;
using std::unordered_map;
using std::pair;
using std::vector;

using signal_t = int32_t;
using gate = pair< void (*)(pair<signal_t, vector<signal_t> >, unordered_map<signal_t, bool>), pair<signal_t, vector<signal_t> > >;

namespace  {
	void read_input(vector<gate> const *gates) {
		//TODO by Jakub Nowacki
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
