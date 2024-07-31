# Viterbi Algorithm for Hidden Markov Model with C++
- Hi, welcome to my README page. This project is a self designed viterbi algorithm for hidden markov model for POS (Part of speech) tagging in Natural Language Processing
- This program uses the C++14 compiler and most of the functions are created using the standard template library in C++.
- For testing, the program achieved an accuracy level of 96% for a file with 56,000 words.
- There are several methods that are implemented for the design of the OOV (Our of vocabulary) words tagging, including K-smoothing, hard coded grammars, and etc.

### To Run the Program

#### Files Info
- A training file (WSJ_02-21.pos) consisting of about 950K words. Each line consists of a token, a single blank, and the part-of-speech of that token using the Penn Treebank tag set. Sentence boundaries are marked by an empty line (about 40K sentences).
- A Development file (WSJ_24.pos) in the same format as the training file, consisting of about 32.9 K words (about 1350 sentences).
- A Development test file (WSJ_24.words) -- the same as WSJ_24.pos, but without the POS tags
- A Test file (WSJ_23.words) in the same format as the Development test file. (56.7K words, about 2415 sentences.)
- A scoring program (score.py) for you to see how well you are doing on the development set.

#### Using Terminal/Command Line
0. Pre Steps
- Make sure you have successfully downloaded the compiler for c++ on your device. 
- Make sure you have downloaded std=c++14 to use c++14 features.
- Make sure you have stored WSJ_02-21.pos", "WSJ_24.pos", "WSJ_23.words", and the "main.cpp" in the same directory 

1. Change the path in "main.cpp".
- Change the path on line 51 to the path to WSJ_24.pos on your device
- Change the path on line 104 to the path to WSJ_02-21.pos on your device
- Change the path on line 209 to the path to WSJ_23.words on your device
- Change the path on line 214 to the path to your directory

2. Go to the directory or folder that stores the "WSJ_02-21.pos", "WSJ_24_sys.pos", "WSJ_23.words", and the "main.cpp".
- Use cd path/to/your/folder command to forward to the folder.

3. Compile the file in your terminal with c++14
- Type below in terminal: 
- g++ -std=c++14 -o my_program main.cpp

4. Run the my_program to generate output for the submission.pos
- ./my_program

#### To use an IDE, please generate an environment using main.cpp

### For OOV words, I handled them using below methods:
- K-smoothing
- Adding rules specific to the words (several methods implemented which will be elaborated)

For the first word of every sentence:
1. We iterate every state with the word. If the word and the current state is not known, we then check our implemented rules
2. If the word ends with "ly" then we adjust the current_state to "RB" an adverb.
3. If the word ends with "ing" then we adjust the current_state to "VBG"
4. Else, we adjust the word to the state being put in the parameter.
5. If the current iterated state == adjusted_state, then we assign a probability by ourselves. If it's a "RB" or "VBG", we assign emission[word][state] = 0.6. 
6. If the word is in one of the special groups like days of week, conjunctions, name of months, and etc., we assign the right POS to "CC" or "NNP".
7. At the end, we add K-smoothing constant to the emission probability of emission[word][adjusted_state] with K=1e-4
8. By have K-smoothing, we are guaranteed to have a result.

For the other words of every sentence that is not the first word:
1. We iterate every state with the word. If the word and the current state is not known, we then check our implemented rules.
2. If the current word contains digits and no other alphabet letter, we assign emission probability of word to CD to 1
3. If the word is all caps then we adjust the current_state to "NNP" and assign a emission probability of 0.7
4. If the word is in one of the special groups like days of week, conjunctions, name of months, and etc. we assign the right POS.
5. We check the word's suffix and adjust probabilities based on the suffixes. 
6. We assign higher probability to "NNS" for unknown words ending in "s"
7. We assign higher probability to "JJ" for unknown words containing "-" 
8. If the word starts with a capital letter then we assign the emission probability of emission[word]["NN"] and emission[word]["NNP"] to a higher number.
9. Then, we add K-smoothing constant to the emission probability of emission[word][current_state] with K=1e-4
10. By have K-smoothing, we are guaranteed to have a result.
