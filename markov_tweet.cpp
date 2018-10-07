#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

class MarkovChain{
public:
    map<string,map<string,int> > wordMapping;
    map<string,int> wordCounting;
    vector<string> allWords;

    MarkovChain(){};
    void train(vector<string>);
    string predict(string word);
    void check();
};

void MarkovChain::train(vector<string> corpus){
    map<string,int>::iterator countingIter;
    map<string,map<string,int> >::iterator mappingIter;
    allWords.push_back(corpus[0]);
    for(int i = 1;i < corpus.size();i++){
        allWords.push_back(corpus[i]);
        countingIter = wordCounting.find(corpus[i-1]);
        if(countingIter != wordCounting.end())
            countingIter->second++;
        else
            wordCounting[corpus[i-1]] = 1;
        mappingIter = wordMapping.find(corpus[i-1]);
        if(mappingIter != wordMapping.end()){
            map<string,int>::iterator freqIter = mappingIter->second.find(corpus[i]);
            if(freqIter != mappingIter->second.end())
                freqIter->second++;
            else
                mappingIter->second[corpus[i]] = 1;
        }
        else{
            map<string,int> tempMap;
            tempMap[corpus[i]] = 1;
            wordMapping[corpus[i-1]] = tempMap;
        }
    }
}

string MarkovChain::predict(string prevWord){
    string nextWord;
    int randomIndex;
    map<string,int>::iterator countingIter = wordCounting.find(prevWord);
    map<string,map<string,int> >::iterator mappingIter = wordMapping.find(prevWord);
    if(mappingIter != wordMapping.end()){
        randomIndex = rand() % countingIter->second;
        map<string,int>::iterator ptr = mappingIter->second.begin();
        int addup = 0;
        while(ptr != mappingIter->second.end()){
            if(randomIndex <= addup+ptr->second && randomIndex > addup){
                nextWord = ptr->first;
                break;
            }
            else{
                addup += ptr->second;
                ptr++;
            }
        }
    }
    else{
        randomIndex = rand() % allWords.size();
        nextWord = allWords[randomIndex];
    }
    return nextWord;
}

void MarkovChain::check(){
    map<string,int>::iterator counterIter = wordCounting.begin();
    while(counterIter != wordCounting.end()){
        cout << counterIter->first << "\t" << counterIter->second << endl;
        map<string,int>::iterator mappingIter = wordMapping[counterIter->first].begin();
        while(mappingIter != wordMapping[counterIter->first].end()){
            cout << "\t" << mappingIter->first << "\t" << mappingIter->second << endl;
            mappingIter++;
        }
        counterIter++;
    }
}

vector<string> getInput(string fileName){
    ifstream inputStream;
    inputStream.open(fileName);
    vector<string> inputLines;
    string eachLine;
    inputStream >> eachLine;
    while(!inputStream.eof()){
        inputLines.push_back(eachLine);
        inputStream >> eachLine;
    }
    return inputLines;
}

vector<string> parse(string str){
    int nextSpace, currentSpace = 0;
    vector<string> wordList;
    while(currentSpace < str.size()){
        nextSpace = str.find(' ', currentSpace);
        if(nextSpace < 0)
            nextSpace = str.size();
        wordList.push_back(str.substr(currentSpace, nextSpace));
        currentSpace = nextSpace;
    }
    return wordList;
}

int main(){
    MarkovChain markov;
    srand(time(NULL));
    vector<string> corpus;
    string fileName;
    cout << "Please input file path: ";
    cout.flush();
    cin >> fileName;
    corpus = getInput(fileName);
    for(int i = 0;i < corpus.size();i++)
        markov.train(corpus);
    vector<string> output;
    string start = "START";
    int length = 0;
    while(length < 280){
        start = markov.predict(start);
        cout << start << " ";
        length += start.size()+1;
    }
    cout << endl;

    return 0;
}
