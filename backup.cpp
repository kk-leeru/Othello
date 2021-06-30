//#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#define max(a,b) (a>=b) ? a : b
#define min(a,b) (a<=b) ? a : b

//struct Point {
//    int x, y;
//};
struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	Point(int x, int y) : x(x), y(y){}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;
//added
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const std::array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    std::array<std::array<int, SIZE>, SIZE> board;
    std::vector<Point> next_valid_spots;
    std::array<int, 3> disc_count;
    int cur_player;
    bool done;
    int winner;
    bool last;
public:
    //changed from private
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
public:
    OthelloBoard() {
        reset();
    }
    void reset() {
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        board[3][4] = board[4][3] = BLACK;
        board[3][3] = board[4][4] = WHITE;
        cur_player = BLACK;
        disc_count[EMPTY] = 8*8-4;
        disc_count[BLACK] = 2;
        disc_count[WHITE] = 2;
        next_valid_spots = get_valid_spots();
        done = false;
        winner = -1;
    }
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    //edited here from bool to void
    void put_disc(Point p) {
        if(!is_spot_valid(p)) {
            winner = get_next_player(cur_player);
            done = true;
            last = false;
            //return false;
        }
        set_disc(p, cur_player);
        disc_count[cur_player]++;
        disc_count[EMPTY]--;
        flip_discs(p);
        // Give control to the other player.
        cur_player = get_next_player(cur_player);
        next_valid_spots = get_valid_spots();
        // Check Win
        if (next_valid_spots.size() == 0) {
            cur_player = get_next_player(cur_player);
            next_valid_spots = get_valid_spots();
            if (next_valid_spots.size() == 0) {
                // Game ends
                done = true;
                last = true;
                int white_discs = disc_count[WHITE];
                int black_discs = disc_count[BLACK];
                if (white_discs == black_discs) winner = EMPTY;
                else if (black_discs > white_discs) winner = BLACK;
                else winner = WHITE;
            }
        }
        //return true;
    }
};
//cpy ended
int state_value(OthelloBoard);  //stable *4  (+3), danger *-1 (-1), safe square *2 +1, adjacent *3 +2
std::vector<Point> stable = {Point(0,0), Point(0,7), Point(7,0), Point(7,7)}; //*4 +3
std::vector<Point> strong_adj = {Point(0,2),Point(0,5), Point(2,7),Point(5,7), Point(7,2),Point(7,5), Point(2,0),Point(5,0)};
std::vector<int>cr_stable = {0,0,0,0};
std::vector<Point> danger = {Point(1,0),Point(1,1),Point(0,1), Point(0,6),Point(1,6),Point(1,7), Point(6,0),Point(6,1),Point(7,1), Point(7,6),Point(6,6),Point(6,7)}; //*(-1)
int minimax(OthelloBoard node, int depth, bool maximizingPlayer, int alpha, int beta);


void read_board(std::ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> board[i][j];
        }
    }
}

void read_valid_spots(std::ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    int x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        next_valid_spots.push_back({x, y});
    }
}

void write_valid_spot(std::ofstream& fout) {
//    int n_valid_spots = next_valid_spots.size();
//    srand(time(NULL));
    // Choose random spot. (Not random uniform here)
    //int index = (rand() % n_valid_spots);
    //Point p = next_valid_spots[index];
//    int steps = 3;
//    while (steps--){
//        for (auto c: next_valid_spots){
//
//        }
//    }
    int max_ns = -10000;
    int beta = 10000;
//    Point p = next_valid_spots[0];
    Point p;
    //int idx = 0;
    for (auto c: next_valid_spots){

        int value = 0;
        OthelloBoard game_cpy;
        game_cpy.cur_player = player;
        game_cpy.board = board;
        game_cpy.put_disc(c);
//        value = state_value(game_cpy);
//        if (max_ns <= value){
//            max_ns = value;
//            p = c;
//        }
        value = minimax(game_cpy, 5, false, max_ns, beta);
        if (max_ns <= value){
            max_ns = value;
            p = c;
            fout << p.x << " " << p.y << std::endl;
        }
        //idx++;
    }

    //Point p = *next_valid_spots.rbegin();
    // Remember to flush the output to ensure the last action is written to file.
    //fout << p.x << " " << p.y << std::endl;
    fout.flush();
}
int minimax(OthelloBoard state, int depth, bool maximizingPlayer, int alpha, int beta)
{
    if (depth == 0){ //deepest
        return state_value(state);
    }
    if (maximizingPlayer == true){
        int value = -10000;
        for (auto c: state.get_valid_spots()){
            OthelloBoard game_cpy = state;
            game_cpy.put_disc(c);
            value = max(value, minimax(game_cpy, depth-1, false, alpha, beta));
            alpha = max(alpha, value);
            if (alpha >= beta){
                //return alpha;
                break;
            }
        }
        return alpha;
    }
    else if (maximizingPlayer == false){
        int value = 10000;
        for (auto c: state.get_valid_spots()){
            OthelloBoard game_cpy = state;
            game_cpy.put_disc(c);
            value = min(value, minimax(game_cpy, depth-1, true, alpha, beta));
            beta = min(beta, value);
            if (beta <= alpha){
                //return beta;
                break;
            }
        }
        return beta;
    }
    return state_value(state);
}
int state_value(OthelloBoard game_cpy)
{
    //check the number for the possible ns in respect to current board
    //flip//check number for possible state
//    std::array<std::array<int, SIZE>, SIZE> board_cpy;
    //OthelloBoard game_cpy;
//    game_cpy.cur_player = player;
//    game_cpy.board = board;
    //game_cpy.cur_player = game.cur_player;
    //game_cpy.board = game.board;
    game_cpy.disc_count[game_cpy.BLACK] = 0; //black
    game_cpy.disc_count[game_cpy.WHITE] = 0; //white
    game_cpy.disc_count[game_cpy.EMPTY] = 64; //empty
    //game_cpy.put_disc(p);
    //*since after flip current player is also flipped, flip first*
    //game_cpy.cur_player = game_cpy.get_next_player(game_cpy.cur_player);
    //edit

    int score = 0;
    for (int i=0; i<SIZE; i++){
        for (int j=0; j<SIZE; j++){
            if (game_cpy.board[i][j] == game_cpy.BLACK){
                game_cpy.disc_count[game_cpy.BLACK]++;
                game_cpy.disc_count[game_cpy.EMPTY]--;
            }
            else if (game_cpy.board[i][j] == game_cpy.WHITE){
                game_cpy.disc_count[game_cpy.WHITE]++;
                game_cpy.disc_count[game_cpy.EMPTY]--;

            }

            //safe square
            if (board[i][j] == game_cpy.cur_player && i>=2 && i<= 5 && j>= 2 && j<= 5){
                if (i>=3 && i<=4 && j>=3 && j<=4){
                    //score = score + 2;// new// center
                    score = score-1;
                }
                else {
                    score = score+1;
                }
            }
            //recent
//            else if (board[i][j] == game_cpy.get_next_player(game_cpy.cur_player) && i>=2 && i<= 5 && j>= 2 && j<= 5){
//                if (i>=3 && i<=4 && j>=3 && j<=4){
//                    //score = score + 2;// new// center
//                    score = score+1;
//                }
//                else {
//                    score = score-1;
//                }
//            }
            //end
            //ajacent
            else if (board[i][j] == game_cpy.cur_player && (((i==0||i==7) && (j>=2 && j<=5))|| ((j==0 || j==7)&&(i>=2 && i<=5)))){
                score = score+2; //2 to 1 test
            }
            //adjacent danger
//            else if (board[i][j] == game_cpy.cur_player && (i == 0||i==7) && (j>=2&&j<=5)){
//                 if((board[i][j-1]== game_cpy.EMPTY||board[i][j-1]==game_cpy.cur_player) &&(board[i][j+1]== game_cpy.EMPTY||board[i][j+1]==game_cpy.cur_player)){
//                    score = score + 2; //
//                 }
//                 else {
//                    if (board[i][0]!= game_cpy.cur_player || board[i][7]!=game_cpy.cur_player){
//                        score = score -2;
//                    }
//                 }
//            }
//            else if (board[i][j] == game_cpy.cur_player && (j == 0||j==7) && (i>=2&&i<=5)){
//                 if((board[i-1][j]== game_cpy.EMPTY||board[i-1][j]==game_cpy.cur_player) &&(board[i+1][j]== game_cpy.EMPTY||board[i+1][j]==game_cpy.cur_player)){
//                    score = score + 2;
//                 }
//                 else {
//                    if (board[0][j]!= game_cpy.cur_player || board[7][j]!= game_cpy.cur_player){
//                        score = score - 2;
//                    }
//                 }
//            }
            //recent
//            else if (board[i][j] == game_cpy.get_next_player(game_cpy.cur_player) && (i == 0||i==7) && (j>=2&&j<=5)){
//                 if((board[i][j-1]== game_cpy.EMPTY||board[i][j-1]==game_cpy.get_next_player(game_cpy.cur_player)) &&(board[i][j+1]== game_cpy.EMPTY||board[i][j+1]==game_cpy.get_next_player(game_cpy.cur_player))){
//                    score = score - 1; //
//                 }
//                 else {
//                    if (board[i][0]!= game_cpy.get_next_player(game_cpy.cur_player) || board[i][7]!=game_cpy.get_next_player(game_cpy.cur_player)){
//                        score = score +1;
//                    }
//                 }
//            }
//            else if (board[i][j] == game_cpy.get_next_player(game_cpy.cur_player) && (j == 0||j==7) && (i>=2&&i<=5)){
//                 if((board[i-1][j]== game_cpy.EMPTY||board[i-1][j]==game_cpy.get_next_player(game_cpy.cur_player)) &&(board[i+1][j]== game_cpy.EMPTY||board[i+1][j]==game_cpy.get_next_player(game_cpy.cur_player))){
//                    score = score + 1;
//                 }
//                 else {
//                    if (board[0][j]!= game_cpy.get_next_player(game_cpy.cur_player) || board[7][j]!= game_cpy.get_next_player(game_cpy.cur_player)){
//                        score = score - 1;
//                    }
//                 }
//            }
            //end
//            else if (board[i][j] == game_cpy.cur_player && (((i==1||i==6) && (j>=1 && j<=6))|| ((j==1 || j==6)&&(i>=1 && i<=6)))){
//                score = score-1; //new  //bad outside
//            }
            //bad outside
            else if (board[i][j] == game_cpy.cur_player && (i==1||i==6)&& (j>1 && j<6)){
                if (i==1){
                    if (cr_stable[0] || cr_stable[1]){
                        score = score-1;
                    }
                    else score = score-2;
                }
                else if (i==6){
                    if (cr_stable[2]||cr_stable[3]){
                        score = score-1;
                    }
                    else score = score - 2;
                }
            }
            else if (board[i][j] == game_cpy.cur_player && (j==1||j==6)&& (i>1 && i<6)){
                if (j==1){
                    if (cr_stable[0]||cr_stable[2]){
                        score = score-1;
                    }
                    else score = score-2;
                }
                else if (j==6){
                    if (cr_stable[1]||cr_stable[3]){
                        score = score-1;
                    }
                    else score = score-2;
                }
            }
            //recent
//            else if (board[i][j] == game_cpy.get_next_player(game_cpy.cur_player) && (i==1||i==6)&& (j>1 && j<6)){
//                if (i==1){
//                    if (game_cpy.board[0][0] == game_cpy.get_next_player(game_cpy.cur_player) || game_cpy.board[0][7] == game_cpy.get_next_player(game_cpy.cur_player)){
//                        score = score+1;
//                    }
//                    else score = score+2;
//                }
//                else if (i==6){
//                    if (game_cpy.board[7][0]==game_cpy.get_next_player(game_cpy.cur_player)||game_cpy.board[7][7]==game_cpy.get_next_player(game_cpy.cur_player)){
//                        score = score+1;
//                    }
//                    else score = score + 2;
//                }
//            }
//            else if (board[i][j] == game_cpy.get_next_player(game_cpy.cur_player) && (j==1||j==6)&& (i>1 && i<6)){
//                if (j==1){
//                    if (cr_stable[0]||cr_stable[2]){
//                        score = score+1;
//                    }
//                    else score = score+2;
//                }
//                else if (j==6){
//                    if (cr_stable[1]||cr_stable[3]){
//                        score = score+1;
//                    }
//                    else score = score+2;
//                }
//            }
            //end
        }
    }

    if (game_cpy.winner == game_cpy.cur_player){
        //means win
        score += 100;
    }
    if (game_cpy.EMPTY == 0 && game_cpy.last == true){
        //means win
        score += 5;
    }
    score += game_cpy.get_valid_spots().size();
    score += game_cpy.disc_count[game_cpy.cur_player]; //what if ?
    //score += (game_cpy.disc_count[player]-game_cpy.disc_count[game_cpy.get_next_player(player)])/100;
    //strong adj
    for (int z=0; z < 8; z++){
        int r = strong_adj[z].x;
        int c = strong_adj[z].y;
        if (game_cpy.board[r][c] == game_cpy.cur_player){
            score = score + 2;
        }
        else if (game_cpy.board[r][c] == game_cpy.get_next_player(game_cpy.cur_player)){
            score= score -2;
        }
    }
    for (int i=0; i<4; i++){
        //stable corner
        int s_i = stable[i].x;
        int s_j = stable[i].y;
        if (game_cpy.board[s_i][s_j] == game_cpy.cur_player){
            score = score+13;
            cr_stable[i] = 1;
            for (int k=0; k<3; k++){
                int d_i = danger[i*3+k].x;
                int d_j = danger[i*3+k].y;
                //danger
                if (game_cpy.board[d_i][d_j] == game_cpy.cur_player){
                    score = score+3; // -1 to -2
                }
            }
        }
        else if (game_cpy.board[s_i][s_j] == game_cpy.EMPTY){
            cr_stable[i] = 0;
            for (int k=0; k<3; k++){
                int d_i = danger[i*3+k].x;
                int d_j = danger[i*3+k].y;
                //danger
                std::vector<Point> dir = {Point(1,1), Point(1,-1), Point(-1,1), Point(-1,-1)};
                if (game_cpy.board[d_i][d_j]== game_cpy.cur_player){
                    if (k == 1){
                        score = score - 5;
                    }
                    else {
                        score = score - 4;
                    }
                }
//                if (game_cpy.board[d_i][d_j] == game_cpy.cur_player){
//                    //score = score-10; // -1 to -2
//                    int dir_i = dir[k].x;
//                    int dir_j = dir[k].y;
//                    if (game_cpy.board[d_i+dir_i][d_j+dir_j] != game_cpy.cur_player){
//                        score = score - 10;
//                    }
//                    else {
//                        score = score-5;
//                    }
//                }
            }
        }
        else {
            cr_stable[i] = 0;
            score = score-13;
            for (int k=0; k<3; k++){
                int d_i = danger[i*3+k].x;
                int d_j = danger[i*3+k].y;
                //danger
                if (game_cpy.board[d_i][d_j]== game_cpy.cur_player){
                    if (k == 1){
                        score = score - 5;
                    }
                    else {
                        score = score - 4;
                    }
                }
            }
            for (int k=0; k<8; k++){
                //int col = 0;
                //int row = 0;
                if (i == 0){
                    if (game_cpy.board[0][k] == game_cpy.cur_player){
                        score = score-1;
                    }
                    if (game_cpy.board[k][0] == game_cpy.cur_player){
                        score = score-1;
                    }
                }
                else if (i==1){
                    if (game_cpy.board[0][k] == game_cpy.cur_player){
                        score = score-1;
                    }
                    if (game_cpy.board[k][7] == game_cpy.cur_player){
                        score = score-1;
                    }
                }
                else if (i==2){
                    if (game_cpy.board[k][0] == game_cpy.cur_player){
                        score = score-1;
                    }
                    if (game_cpy.board[7][k] == game_cpy.cur_player){
                        score = score-1;
                    }
                }
                else if (i==3){
                    if (game_cpy.board[7][k] == game_cpy.cur_player){
                        score = score-1;
                    }
                    if (game_cpy.board[k][7] == game_cpy.cur_player){
                        score = score-1;
                    }
                }
            }
            //score = score+10;
//            for (int k=0; k<3; k++){
//                int d_i = danger[i*3+k].x;
//                int d_j = danger[i*3+k].y;
//                //danger
//                if (game_cpy.board[d_i][d_j] == game_cpy.get_next_player(game_cpy.cur_player)){
//                    score = score-3; // -1 to -2
//                }
//            }
        }
//        int d_i = danger[i].y;
//        int d_j = danger[i].x;
//        //danger
//        if (game_cpy.board[d_i][d_j] == game_cpy.cur_player){
//            score = score-2; // -1 to -2
//        }

    }
    int top,left,right,bottom;
    int top_n, left_n, right_n, bottom_n;
    top_n = left_n = right_n = bottom_n = 0;
    top = left = right = bottom = 1;
    for (int k=0; k<8; k++){
        //top l
        if (game_cpy.board[0][k] != game_cpy.cur_player && game_cpy.board[0][k] != game_cpy.EMPTY){
            top = 0;
        }
        else {
            top_n++;
        }
        if (game_cpy.board[k][0] != game_cpy.cur_player && game_cpy.board[k][0] != game_cpy.EMPTY){
            left = 0;
        }
        else left_n++;
        if (game_cpy.board[k][7] != game_cpy.cur_player && game_cpy.board[k][7] != game_cpy.EMPTY){
            right = 0;
        }
        else right_n++;
        if (game_cpy.board[7][k] != game_cpy.cur_player && game_cpy.board[7][k] != game_cpy.EMPTY){
            bottom = 0;
        }
        else bottom_n++;
    }
    if (top){
        score += top_n;
    }
    if (left){
        score += left_n;
    }
    if (right){
        score += right_n;
    }
    if (bottom){
        score += bottom_n;
    }
    game_cpy.cur_player = game_cpy.get_next_player(game_cpy.cur_player);
    score -= game_cpy.get_valid_spots().size();
    //score -= game_cpy.disc_count[game_cpy.cur_player];
    game_cpy.cur_player = game_cpy.get_next_player(game_cpy.cur_player);
    //afterwards flip back for correct turn
    //game_cpy.cur_player = game_cpy.get_next_player(game_cpy.cur_player);
    //after put check ns for next player
    //std::vector<Point> ns = game_cpy.get_valid_spots();
    //return ns.size();
    return score;
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]); //read black O  //first player
    std::ofstream fout(argv[2]); //output white X  //second player
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

