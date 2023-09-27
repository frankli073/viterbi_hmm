#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <memory> 
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <cctype>

// Use ListNode to go through all training corpus
struct ListNode {
    std::string token;
    std::string pos;
    std::shared_ptr<ListNode> next;
	
	ListNode() {
		token = "";
		pos = "";
		next = nullptr;
		
	}
	
	ListNode(std::string t, std::string p) {
		token = t; 
		pos = p;
		next = nullptr;
	}
};
// Prototype for the viterbi algorithm
std::string get_pos_start(const std::string& word, const std::string &state);
std::string viterbi_algo(std::vector<std::string> &words, 
					std::vector<std::string> &states_set, 
//					std::unordered_set<std::string> &set_of_words,
					std::unordered_map<std::string, int> &count_of_words,
					std::unordered_map<std::string, std::unordered_map<std::string, double> > &emission, 
					std::unordered_map<std::string, std::unordered_map<std::string, double> > &transition);
bool is_line_empty(const std::string& line) {
    for (char c : line) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

int main() {
	// Change the path on line 51 to the path to WSJ_24.pos on your device
    std::ifstream in_file("/Users/liqingyang/nyu_courses/natural_lang_processing/homeworks/hw3/WSJ_24.pos");
    
	// Use a singly linked list to track all the words and traverse.
    std::shared_ptr<ListNode> dummy = std::make_shared<ListNode>();
	dummy->token = "START";
	dummy->pos = "-";
    std::shared_ptr<ListNode> current = dummy;
	std::unordered_map<std::string, int> count_of_words {};
	
	// Keep a set to track all the words we have used
	std::unordered_set<std::string> set_of_words {};

    std::string line;

    if (in_file.is_open()) {
        while (std::getline(in_file, line)) {
            size_t tabPos = line.find('\t');
            if(tabPos != std::string::npos) {
                std::string token = line.substr(0, tabPos);
                std::string pos = line.substr(tabPos + 1);
				
				std::shared_ptr<ListNode> ptr = std::make_shared<ListNode>(token, pos);
                current->next = ptr;
                current = current->next;

				set_of_words.emplace(token);	
				count_of_words[token]++;
            }
			else {
				std::string token = "END";
				std::string pos = "^";
				
				std::shared_ptr<ListNode> ptr = std::make_shared<ListNode>(token, pos);
				current->next = ptr;
				current = current->next;
				
				std::string token_ = "START";
				std::string pos_ = "-";
				
				std::shared_ptr<ListNode> ptr_ = std::make_shared<ListNode>(token_, pos_);
				current->next = ptr_;
				current = current->next;
			}
        }
        
    } 
	else {
        std::cout << "Unable to open file for WSJ_24.pos" << std::endl;
		return 1;
    }
	in_file.close();

	// Change the below path on line 99 to the path to WSJ_02-21.pos on your device
	std::ifstream in_file_1("/Users/liqingyang/nyu_courses/natural_lang_processing/homeworks/hw3/WSJ_02-21.pos");
	if (in_file_1.is_open()) {
        while (std::getline(in_file_1, line)) {
            size_t tabPos = line.find('\t');
            if(tabPos != std::string::npos) {
                std::string token = line.substr(0, tabPos);
                std::string pos = line.substr(tabPos + 1);
				
				std::shared_ptr<ListNode> ptr = std::make_shared<ListNode>(token, pos);
                current->next = ptr;
                current = current->next;
				
				set_of_words.emplace(token);				
				count_of_words[token]++;
            }

			else {
				std::string token = "END";
				std::string pos = "^";
				
				std::shared_ptr<ListNode> ptr = std::make_shared<ListNode>(token, pos);
				current->next = ptr;
				current = current->next;
				
				std::string token_ = "START";
				std::string pos_ = "-";
				
				std::shared_ptr<ListNode> ptr_ = std::make_shared<ListNode>(token_, pos_);
				current->next = ptr_;
				current = current->next;
			}
        }
    }
	else {
        std::cout << "Unable to open file for WSJ_02-21.pos" << std::endl;
		return 1;
    }
	in_file_1.close();
	

	//Record the number of times each tag has occured for each word
	std::unordered_map<std::string, std::unordered_map<std::string, int> > frequency_table {};
	
	//Record the likelihood of a given occured before a certain tag
	std::unordered_map<std::string, std::unordered_map<std::string, int> > likelihood_table {};

	current = dummy->next;
    while (current) {
		if (current->token == "''" || current->token == "," || current->token == "``" || current->token == ".") {
			frequency_table[current->token][current->token]++;
			frequency_table[current->token]["TOTAL"]++;
			
			if (current->next == nullptr) {
				break;
			}
			
			if ((current->next->pos != "-") && (current->next->pos != "^")){
				likelihood_table[current->pos][current->next->pos]++;
				likelihood_table[current->pos]["TOTAL"]++;
			}
			current = current->next;
		}
		
		frequency_table[current->token][current->pos]++;
		frequency_table[current->token]["TOTAL"]++;
		if (current->next == nullptr) {
				break;
			}
		if ((current->next->pos != "-") && (current->next->pos != "^")){
			likelihood_table[current->pos][current->next->pos]++;
			likelihood_table[current->pos]["TOTAL"]++;
		}
		current = current->next;
    }

	std::unordered_map<std::string, std::unordered_map<std::string, double>> emission {};
	std::unordered_map<std::string, std::unordered_map<std::string, double>> transition {};
	std::unordered_set<std::string> states_set {};

	
	//For probability
	for (const auto &word: frequency_table) {
		for (const auto &pos: word.second) {
			emission[word.first][pos.first] = static_cast<double> (pos.second)/(frequency_table[word.first]["TOTAL"]);
		}
	}
	
	for (const auto &prev: likelihood_table) {
		states_set.emplace(prev.first);
		for (const auto &cur: prev.second) {
			transition[prev.first][cur.first] = static_cast<double> (cur.second)/(likelihood_table[prev.first]["TOTAL"]);
		}
	}
	
	std::vector<std::string> months {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	for (const std::string &month : months) {
		emission[month]["NNP"] = 1;
	}
	
	std::vector<std::string> states(states_set.begin(), states_set.end());
	
	
	
	// Change the below path on line 209 to the path to WSJ_23.words on your device
	std::ifstream testing("/Users/liqingyang/nyu_courses/natural_lang_processing/homeworks/hw3/WSJ_23.words");
    std::string line_testing;
	std::string prev_testing;
	
	// Change the below path on line 214 to the path to your directory
	std::ofstream outfile("/Users/liqingyang/nyu_courses/natural_lang_processing/homeworks/hw3/submission.pos"); 

    if (!outfile) {  // Always check if the file has been opened successfully
        std::cerr << "Error opening the file!" << std::endl;
        return 1;
    }
	
    if (testing.is_open()) {
		std::vector<std::string> words {};
		while (std::getline(testing, line_testing)) {
			
//			if (line_testing == ".") {
//				prev_testing = ".";
//				continue;
//			}
//            else 
			if (line_testing != "") {
				words.emplace_back(line_testing);
			}
			else {
				if (words.size() != 0) {
					std::string temp = viterbi_algo(words, states, count_of_words, emission, transition);
					std::cout << "shit" << std::endl;
					std::cout << temp << "\n";
					outfile << temp << "\n";
					words.clear();
				}
				else {
					words.emplace_back(line_testing);
					
				}
				
				
			}
//					}
//					else {
//						std::string temp = viterbi_algo(words, states, set_of_words, emission, transition);
//						std::cout << temp << ".\t.\n";
//						outfile << temp << ".\t.\n";
//						words.clear();
//						words.emplace_back(line_testing);
//					}
//				}
//				else {
//					words.emplace_back(line_testing);
//				}
//				prev_testing = line_testing;
//			else if ((line_testing.empty() || is_line_empty(line_testing))) {
//				if (prev_testing == ".") {
//					if (words.size() == 0) {
//						outfile << ".\t.\n\n";
//					}
//					else {
//			else{
//						std::string temp = viterbi_algo(words, states, set_of_words, emission, transition);
//						std::cout << temp << ".\t.\n\n";
//						outfile << temp << ".\t.\n\n";
//						words.clear();
//					}
//				}
//				else {
//					if (words.size() == 0) {
//						outfile << "\n";
//					}
//					else {
//						std::string temp = viterbi_algo(words, states, set_of_words, emission, transition);
//						std::cout << temp << "\n";
//						outfile << temp << "\n";
//						
//						words.clear();
//						prev_testing = "";
//					}
		}
	}
				
			
	else {
        std::cout << "Unable to open file" << std::endl;
		return 1;
    }

	std::cout << "Finished" << std::endl;
	testing.close();
	outfile.close();
    return 0;
}

std::string get_pos_start(const std::string& word, const std::string &state) {
    const std::unordered_map<std::string, std::string> suffixPosMap = {
        {"ly", "RB"}, // Adverb
        {"ing", "VBG"}, // Verb, gerund/present participle
        // ... Add more suffix to POS mappings as needed
    };
	const std::vector<std::string> conjunctions {"For", "And", "Nor", "But", "Or", "Yet", "So"};
	const std::vector<std::string> days_of_week {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
	
	if (word == "Yes" || word == "No") {
		return "RB";
	}
	
	if (std::find(conjunctions.begin(), conjunctions.end(), word) != conjunctions.end()) {
		return "CC";
	}
	
	if (std::find(days_of_week.begin(), days_of_week.end(), word) != days_of_week.end()) {
		return "NNP";
	}
    
    for (const auto& pair : suffixPosMap) {
        if (word.size() > pair.first.size() && 
            word.compare(word.size() - pair.first.size(), pair.first.size(), pair.first) == 0) {
            return pair.second;
        }
    }
    return state; // Return noun as default POS for unknown capitalized words
}

bool is_all_upper(const std::string& str) {
    for (char c : str) {
        if (std::isalpha(c) && std::islower(c)) {
            return false;
        }
    }
    return true;
}

std::string viterbi_algo(std::vector<std::string> &words, 
					std::vector<std::string> &states_set, 
					std::unordered_map<std::string, int> &count_of_words,
					std::unordered_map<std::string, std::unordered_map<std::string, double> > &emission, 
					std::unordered_map<std::string, std::unordered_map<std::string, double> > &transition) {

	std::vector<std::unordered_map<std::string, double>> v_probs(words.size());
	std::vector<std::unordered_map<std::string, std::string>> backpointers(words.size());

	for (const std::string &state : states_set) {
		double emit_prob = 1e-4; // Default small probability
		if (count_of_words[words[0]] > 2) {
			emit_prob = emission[words[0]][state];
			const std::vector<std::string> conjunctions {"For", "And", "Nor", "But", "Or", "Yet", "So"};
			const std::vector<std::string> days_of_week {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
			
			if (words[0] == "Yes" || words[0] == "No") {
				if (state == "RB") {
					emit_prob = 1;
				}
			}
			
			if (std::find(conjunctions.begin(), conjunctions.end(), words[0]) != conjunctions.end()) {
				if (state == "CC") {
					emit_prob = 1;
				}
				else {
					emit_prob = 0;
				}
			}
			
			if (std::find(days_of_week.begin(), days_of_week.end(), words[0]) != days_of_week.end()) {
				if (state == "NNP") {
					emit_prob = 1;
				}
				else {
					emit_prob = 0;
				}
			}
		} 
		else {
        // If the word is unknown and starts with a capital letter, adjust its POS probability
			std::string adjustedPos = get_pos_start(words[0], state);
			if (state == adjustedPos) {
				if (state == "CC") {
					emit_prob = 1;
				}
				else if (state == "RB") {
					emit_prob = 0.6;
				}
				else {
					emit_prob = 0.5; // Adjusted high probability for the determined POS
				}
			}
		}
		double trans_prob = transition["-"][state] + 1e-4;
		v_probs[0][state] = trans_prob * emit_prob;
		if ((words[0] == "Yes" || words[0] == "No") && (state == "RB")) {
			v_probs[0][state] = 1;
		}	
		backpointers[0][state] = "START";
	}

	
	for (int i {1}; i < words.size(); i++) {
//		const std::vector<std::string> punc {",", "''", "``"};
//		bool special_punc = std::find(punc.begin(), punc.end(), words[i]) != punc.end();
		const std::vector<std::string> days_of_week {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
		bool week_day = (std::find(days_of_week.begin(), days_of_week.end(), words[i]) != days_of_week.end());
		bool is_word_unknown = (count_of_words[words[i]] < 3);		
		bool starts_with_capital = (std::isupper(words[i][0]) && words[i].length() > 1 && (std::all_of(words[i].begin() + 1, words[i].end(), [](char c) { return std::islower(c);})));
		bool all_cap = (std::all_of(words[i].begin(), words[i].end(), [](char c) { return std::isupper(c);}));
		bool digits =((words[i].length() > 1) && (std::any_of(words[i].begin(), words[i].end(), [](char c) { return std::isdigit(c) && (!std::isalpha(c)); })));
		
		for (const std::string &state:states_set) {
			double max = 0;
			std::string best_prev_state {};
			for (const std::string &prev_state:states_set) {
				double emit_prob = 0;
				if (week_day) {
					if (state == "NNP")	{
						emit_prob = 1;
//						max = 1;
//						best_prev_state = prev_state;
//						break;
					}
					else {
						emit_prob = 0;
					}
				}
				
				else if (digits && (!week_day)) {
					if (state == "CD") {
						emit_prob = 1;
					}
					else {
						emit_prob = 0;
					}
				}
				
				else if (!is_word_unknown && (!week_day) && (!digits)) {
					emit_prob = emission[words[i]][state];
				}

				else {
					if (starts_with_capital) {
						if ((state == "NN")) {
							emit_prob = 0.4; // for example, assign a high probability for nouns
						}
						
						if ((state == "NNP")) {
							emit_prob = 0.7; // for example, assign a high probability for nouns
						}
					}
					
					else if (all_cap) {
						if (state == "NNP") {
							emit_prob = 1;
						}
						else {
							emit_prob = 0;
						}
					}
					else {
						int last_one = words[i].length() - 1;
						
						if (words[i].find('-') != std::string::npos) {
							if (state == "JJ") {
								emit_prob = 0.7;
							}
						}
						
						if (digits) {
							if (state == "CD") {
								emit_prob = 1;
							}
							else {
								emit_prob = 0;
							}
						}
						
						if ((words[i][last_one] == 's') && (state == "NNS") && (words[i].find('-') == std::string::npos)) {
							emit_prob = 0.3;
						}
						
						else if ((words[i][last_one] == 's') && (state == "VBD") && (words[i].find('-') == std::string::npos)) {
							emit_prob = 0.2;
						}
												
						if (words[i].length() >= 3 && (!digits)  && (words[i].find('-') == std::string::npos)) {
							const std::unordered_map<std::string, std::string> suffixPosMap = {
								{"ly", "RB"}, // Adverb
								{"ing", "VBG"},
								{"ing", "ADJ"},
								{"ed", "VBD"}
							};
							for (const auto& pair : suffixPosMap) {
								if ((words[i].compare(words[i].size() - pair.first.size(), pair.first.size(), pair.first) == 0) && (state == pair.second)){
									emit_prob = 0.6;
								}
							}
						}
						else {
							emit_prob = 0;
						}
					}
				}
				emit_prob = emit_prob + 1e-4;
				
				double trans_prob = transition[prev_state][state];
				double curr_prob = v_probs[i-1][prev_state] * trans_prob * emit_prob;
				
				if (curr_prob > max) {
					max = curr_prob;
					best_prev_state = prev_state;
				}
			}
			v_probs[i][state] = max;
			backpointers[i][state] = best_prev_state;
		}
	}
	
	std::string best_final_state {};
	double best = 0;
	for (std::string &state:states_set) {
		int last_index = (words.size() - 1);
		if (v_probs[last_index][state] > best) {
			best = v_probs[last_index][state];
			best_final_state = state;
		}
	}
	
	std::vector<std::string> best_path(words.size());
	best_path[words.size() - 1] = best_final_state;

	for (int t = words.size() - 2; t >= 0; t--) {
		best_path[t] = backpointers[t+1][best_path[t+1]];
	}
	
	std::string returning {};
	for (int i {0}; i < best_path.size(); i++) {
		returning += words[i];
		returning += "\t";
		returning += best_path[i];
		returning += "\n";
	}
	
	return returning;
	}