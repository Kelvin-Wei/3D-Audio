#include <bits/stdc++.h>

using namespace std;

class Maze{

#define _BLOCK_ 0
#define _VISIT_ -1
#define _VALID_ 1
#define _MAX_MAZE_SIZE_ 10

    struct _Point{
        int valid;
        int accessNeighbor;
        char direct;
    };

    int col, row;
    int allNode;
    char direction_c[4];
    pair<int,int> position, entry, terminate;
    struct _Point maze[_MAX_MAZE_SIZE_][_MAX_MAZE_SIZE_];
    int direction[4][2];
    bool reverseFlag;

    void initialize(){
        direction[0][0] = -1, direction[0][1] = 0, direction_c[0] = '^';
        direction[1][0] = 1, direction[1][1] = 0, direction_c[1] = 'v';
        direction[2][0] = 0, direction[2][1] = -1, direction_c[2] = '<';
        direction[3][0] = 0, direction[3][1] = 1, direction_c[3] = '>';
    }

    void parseRawMaze(string str){
        int base;
        for(int i = 0;i < row;i++){
            base = i * col;
            for(int j = 0;j < col;j++){
                if(str[base+j] == '1')
                    maze[i][j] = {_VALID_, 4, '.'};
                else if(str[base+j] == 's' || str[base+j] == 'S'){
                    entry = make_pair(i,j);
                    maze[i][j] = {_VALID_, 4, '.'};
                }
                else
                    maze[i][j] = {_BLOCK_, 4, ' '};
            }
        }
    }

    void updateNeighbor(){
        int x = position.first, y = position.second;
        if(x != 0)  maze[x-1][y].accessNeighbor -= isValid(x-1, y)?maze[x][y].valid:0;
        if(x != row - 1)    maze[x+1][y].accessNeighbor -= isValid(x+1, y)?maze[x][y].valid:0;
        if(y != 0)  maze[x][y-1].accessNeighbor -= isValid(x, y-1)?maze[x][y].valid:0;
        if(y != col - 1)    maze[x][y+1].accessNeighbor -= isValid(x, y+1)?maze[x][y].valid:0;
    }

    void setValid(){
        int x = position.first, y = position.second;
        updateNeighbor();
        maze[x][y].valid = _VALID_;
        maze[x][y].direct = '.';
        allNode++;
    }

    void setVisit(){
        int x = position.first, y = position.second;
        updateNeighbor();
        maze[x][y].valid = _VISIT_;
        allNode--;
    }

    bool isValid(int x, int y) const {
        return maze[x][y].valid == _VALID_;
    }

    bool isEntry(int x, int y) const {
        return (x == entry.first) && (y == entry.second);
    }

    bool isTerminate(int x, int y) const {
        return (x == terminate.first) && (y == terminate.second);
    }

    void setupMaze(){
        string mazeString, mazeLine;
        cout << "Input:" << endl;
        while(mazeString.size() < row * col){
            cin >> mazeLine;
            mazeString.append(mazeLine);
        }
        parseRawMaze(mazeString);
        allNode = 0;
        terminate = make_pair(-1,-1);
        for(int i = 0;i < row;i++){
            for(int j = 0;j < col;j++){
                if(!maze[i][j].valid)   continue;
                maze[i][j].accessNeighbor -= (i == 0 || !maze[i-1][j].valid) + (i == row - 1 || !maze[i+1][j].valid);
                maze[i][j].accessNeighbor -= (j == 0 || !maze[i][j-1].valid) + (j == col - 1 || !maze[i][j+1].valid);
                if(maze[i][j].accessNeighbor == 1 && !isEntry(i,j)){
                    terminate = make_pair(i,j);
                    maze[i][j].direct = 'O';
                }
                allNode++;
            }
        }
    }

    void startFrom(){
        reverseFlag = false;
        if(terminate.first != -1){
            swap(entry, terminate);
            reverseFlag = true;
        }
        position = entry;
    }

    bool checkOutRange(int dx, int dy){
        pair<int,int> pos = nextStep(dx, dy);
        if(pos.first > -1 && pos.first < row && pos.second > -1 && pos.second < col && isValid(pos.first, pos.second))
                return true;
        return false;
    }

    bool moveForward(int dx, int dy){
        if(checkOutRange(dx, dy)){
            position.first += dx;
            position.second += dy;
            return true;
        }
        return false;
    }

    void moveBackward(int dx, int dy){
        position.first -= dx;
        position.second -= dy;
    }

    pair<int,int> nextStep(int dx, int dy){
        pair<int,int> pos = position;
        pos.first += dx;
        pos.second += dy;
        return pos;
    }

    pair<int,int> prevStep(int dx, int dy){
        pair<int,int> pos = position;
        pos.first -= dx;
        pos.second -= dy;
        return pos;
    }

    void setDirect(int f, int i){
        int x = position.first, y = position.second;
        if(reverseFlag){
            if(f < 0)
                maze[x][y].direct = direction_c[i];
            else
                maze[x][y].direct = direction_c[f];
        }
        else
            maze[x][y].direct = direction_c[i];
    }

    bool coverAll() const {
        return (position == terminate && allNode == 0) || allNode == 0;
    }

public:
    explicit Maze(int r, int c):col(c), row(r){
        initialize();
        setupMaze();
        startFrom();
    }

    bool solution_trial(int fromTo){
        bool result;
        int step = -1;
        setVisit();
        if(coverAll()){
            if(!reverseFlag)
                maze[position.first][position.second].direct = 'T';
            else
                setDirect(fromTo, -1);
            return true;
        }
        if(position == terminate && allNode != 0){
            setValid();
            moveBackward(direction[fromTo][0], direction[fromTo][1]);
            return false;
        }
        for(int i = 0;i < 4;i++){
            if(!checkOutRange(direction[i][0], direction[i][1]))    continue;
            if(isValid(position.first+direction[i][0], position.second+direction[i][1]) && maze[position.first+direction[i][0]][position.second+direction[i][1]].accessNeighbor <= 1){
                step = i;
                break;
            }
        }
        if(step > -1){
            if(moveForward(direction[step][0], direction[step][1]) && solution(step)){
                moveBackward(direction[step][0], direction[step][1]);
                setDirect(fromTo, step);
                return true;
            }
        }
        for(int i = 0;i < 4;i++){
            if(i == step)   continue;
            if(!moveForward(direction[i][0], direction[i][1]))  continue;
            result = solution(i);
            if(result){
                moveBackward(direction[i][0], direction[i][1]);
                setDirect(fromTo, i);
                return result;
            }
        }
        setValid();
        moveBackward(direction[fromTo][0], direction[fromTo][1]);
        return false;
    }

    bool solution(int fromTo){
        bool result;
        setVisit();
        if(coverAll()){
            if(!reverseFlag)
                maze[position.first][position.second].direct = 'T';
            else
                setDirect(fromTo, -1);
            return true;
        }
        if(position == terminate && allNode != 0){
            setValid();
            moveBackward(direction[fromTo][0], direction[fromTo][1]);
            return false;
        }
        for(int i = 0;i < 4;i++){
            if(!moveForward(direction[i][0], direction[i][1]))  continue;
            result = solution(i);
            if(result){
                moveBackward(direction[i][0], direction[i][1]);
                setDirect(fromTo, i);
                return result;
            }
        }
        setValid();
        moveBackward(direction[fromTo][0], direction[fromTo][1]);
        return false;
    }

    void showMaze(char option='d') const {
        cout << "~~~~~ Maze ~~~~~" << endl;
        for(int i = 0;i < row;i++){
            for(int j = 0;j < col;j++){
                if(option == 'r'){
                    if(maze[i][j].valid){
                        if(isTerminate(i,j))
                            putchar('T');
                        else if(isEntry(i,j))
                            putchar('S');
                        else
                            putchar('+');
                    }
                    else
                        putchar(' ');
                }
                else if(option == 'd'){
                    if(isEntry(i, j)){
                        if(reverseFlag)
                            putchar('T');
                        else
                            putchar(maze[i][j].direct);
                    }
                    else{
                        if(reverseFlag){
                            if(maze[i][j].direct == '^')    putchar('v');
                            else if(maze[i][j].direct == 'v')   putchar('^');
                            else if(maze[i][j].direct == '<')   putchar('>');
                            else if(maze[i][j].direct == '>')   putchar('<');
                            else    putchar(maze[i][j].direct);
                        }
                        else    putchar(maze[i][j].direct);
                    }
                }
                else if(option == 'n')
                    printf("%d", maze[i][j].accessNeighbor);
            }
            putchar('\n');
        }
        cout << "~~~~~~~~~~~~~~~~" << endl;
    }
};

int main(int argc, char **argv){
    if(argc < 3){
        cout << "insufficient parameters" << endl;
        exit(0);
    }
    bool result;
    int startTime, endTime;
    Maze maze(atoi(argv[1]), atoi(argv[2]));
    maze.showMaze('r');
    startTime = clock();
    result = maze.solution(-1);
    endTime = clock();
    if(!result)
        cout << "No Solution!?" << endl;
    else
        maze.showMaze('d');
    cout << "Run time: " << (float)(endTime - startTime) / CLOCKS_PER_SEC * 1000 << "ms" << endl;
    return 0;
}
