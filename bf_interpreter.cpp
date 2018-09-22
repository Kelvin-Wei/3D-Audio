#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

vector<int> mem;

int main(int argc, char** argv){
    if(argc == 0){
        cout << "Please choose src file" << endl;
        return 0;
    }
    string srcFile(argv[1]);
    ifstream inputStream;
    inputStream.open(srcFile);
    if(!inputStream.is_open()){
        cout << "Fail to open the src file" << endl;
        return 0;
    }
    string code = "", eachPart;
    while(!inputStream.eof()){
        inputStream >> eachPart;
        code += eachPart;
        eachPart.clear();
    }
    inputStream.close();
    int ptr = 0, tail;
    stack<int> loopStart;
    mem.push_back(0);
    for(int i = 0;i < code.length();i++){
        switch(code[i]){
        case '>':
            ptr++;
            if(ptr >= mem.size())
                mem.push_back(0);
            break;
        case '<':
            ptr--;
            if(ptr < 0){
                mem.insert(mem.begin(), 0);
                ptr = 0;
            }
            break;
        case '+':
            mem[ptr]++;
            break;
        case '-':
            mem[ptr]--;
            break;
        case '.':
            putchar((char)mem[ptr]);
            break;
        case ',':
            scanf("%c",(char*)&mem[ptr]);
            break;
        case '[':
            loopStart.push(i);
            tail = i;
            while(code[tail] != ']')
                tail++;
            if(mem[ptr] == 0){
                i = tail;
                loopStart.pop();
            }
            break;
        case ']':
            if(mem[ptr] != 0)
                i = loopStart.top();
            else
                loopStart.pop();
            break;
        default:
            break;
        }
    }
    return 0;
}
