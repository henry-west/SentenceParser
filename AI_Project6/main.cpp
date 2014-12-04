//
//  main.cpp
//  AI_Project6
//
//  Created by Henry West and Ian Walker
//  Copyright (c) 2014 Henry Westside. All rights reserved.
//

/*
 * description: a natural language processor
 * reads  in a file with sentences, prints out if it is a complete sentence
 * limited grammar for now
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <stack>

using namespace std;
//node of parse tree
struct Node {
    string partOfSentence;
    /*
     * sentence : nounPhrase verbPhrase | verbPhrase
     * nounPhrase : artice noun | adjective noun
     * verbPhrase : verb adjective | verb nounPhrase | verb sentence
     * noun
     * adjective
     * article
     * verb
     */
    vector<string> words;
};
//stores word and part of speech
struct Word {
    string word, partofSpeech;
    Word(string a, string b) {
        word = a;
        partofSpeech = b;
    }
};

typedef stack<Node> Sentence;
typedef vector<Sentence> SentenceList;
//cout overload for node (debug assist)
std::ostream& operator<<(std::ostream& out, Node& w){
    string temp;
    for (vector<string>::iterator it = w.words.begin(); it != w.words.end(); ++it) {
        temp += *it;
        temp += " ";
    }
    return out << temp;
}
//returns part of sentence if two nodes are combined
string canCombine(Node a, Node b) {

    if (a.partOfSentence == "verb" && (b.partOfSentence == "adjective" || b.partOfSentence == "nounPhrase" || b.partOfSentence == "sentence")) {
        return "verbPhrase";
    }
    else if ((a.partOfSentence == "article" && b.partOfSentence == "noun") || (a.partOfSentence == "adjective" && b.partOfSentence == "noun") || (a.partOfSentence == "article" && b.partOfSentence == "nounPhrase")) {
        return "nounPhrase";
    }
    else if ((a.partOfSentence == "nounPhrase" && b.partOfSentence == "verbPhrase")) {
        return "sentence";
    }
    else {
        return "fragment"; //not a part of a sentence
    }
}
//recursive function to parse sentence
//takes Sentence as argument  and returns sentence
Sentence process(Sentence sentence) {

    Node top = sentence.top();
    sentence.pop();
    //checks if can pop next node off stack
    if (sentence.size() > 0) {
        Node next = sentence.top();
        sentence.pop();
        Node newTop;

        //if not a fragment
        if (canCombine(next, top) != "fragment") {

            //combine top two nodes from stack and re-push
            newTop.words.reserve(top.words.size() + next.words.size()); //set aside space
            newTop.words.insert(newTop.words.end(), next.words.begin(), next.words.end());
            newTop.words.insert(newTop.words.end(), top.words.begin(), top.words.end());

            newTop.partOfSentence = canCombine(next, top);

            sentence.push(newTop);
            process(sentence);
            return sentence;

        }
        else {
            //push first from top and continue reprocess without pushing top until sentence is returned
            //this allows us to properly form the sentence
            sentence.push(next);
            sentence = process(sentence);
            sentence.push(top);
            return process(sentence);
        }
    }
    else {
        if (top.partOfSentence == "sentence") {
            cout << "Sentence works" << endl; //success
            sentence.push(top);
        }
        return sentence;
    }

    

}

int main(int argc, const char * argv[]) {
    //input file
    ifstream inFile;
    inFile.open("in.txt");
    //read word list
    ifstream dict;
    dict.open("dictionary.txt");

    Sentence sentence;
    SentenceList sentenceList; //contains a stack for each sentence in input file
    vector<Word> dictionary;

    string tempWord, tempPart;
    //create dictionary
    while (dict >> tempWord >> tempPart) {
        dictionary.push_back(Word(tempWord, tempPart));
    }

    string currIn;
    //write sentences to list
    while (inFile >> currIn) {
        if (currIn != ".") {
            Node temp;
            temp.partOfSentence = " ";
            for (vector<Word>::iterator iter = dictionary.begin(); iter != dictionary.end(); ++iter) {
                Word tempWord = *iter;
                if (tempWord.word == currIn) {
                    temp.partOfSentence = tempWord.partofSpeech;
                }
            }
            cout << currIn << " ";
            temp.words = {currIn};
            sentence.push(temp);
        }
        else {
            cout << endl;
            cout << "Size of sentence: " << sentence.size() << endl;
            sentenceList.push_back(sentence);
            while (!sentence.empty()) {
                sentence.pop();
            }
        }
    }

    Sentence temp;
    //parse sentences
    for (vector<Sentence>::iterator iter = sentenceList.begin(); iter != sentenceList.end(); ++iter) {
        temp = *iter;
        temp = process(temp);
        if (temp.size() == 0) {
            cout << "Not a sentence" << endl;
        }

    }
    //only you can prevent memory leaks
    inFile.close();
    dict.close();

    return 0;
}
