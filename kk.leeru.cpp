#include <iostream>
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
//add class
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
//add
int state(OthelloBoard st);
int minimax(OthelloBoard st, int depth, bool maximizingPlayer, int alpha, int beta);

int player;
const int SIZE = 8;
std::array<std::array<int, SIZE>, SIZE> board;
std::vector<Point> next_valid_spots;

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
    //int n_valid_spots = next_valid_spots.size();
    //srand(time(NULL));
    // Keep updating the output until getting killed.
//    while (true) {
//        // Choose random spot. (Not random uniform here)
//        int index = (rand() % n_valid_spots);
//        Point p = next_valid_spots[index];
//        // Remember to flush the output to ensure the last action is written to file.
//        fout << p.x << " " << p.y << std::endl;
//        fout.flush();
//    }

    int max_ns = -1000000;
    int beta = 1000000;
    Point p;
    for (auto c: next_valid_spots){
        int value = 0;
        OthelloBoard g_cpy;
        g_cpy.cur_player = player;
        g_cpy.board = board;
        g_cpy.put_disc(c);
        value = minimax(g_cpy, 3, false, max_ns, beta);
        if (max_ns<=value){
            max_ns = value;
            p = c;
            fout << p.x << " " << p.y << std::endl;
        }
    }
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}
int minimax(OthelloBoard st, int depth, bool maximizingPlayer, int alpha, int beta)
{
    if (depth == 0){
        return state(st);
    }
    if (maximizingPlayer == true){
        int value = -1000000;
        for (auto c: st.get_valid_spots()){
            OthelloBoard g_cpy = st;
            g_cpy.put_disc(c);
            value = max(value, minimax(g_cpy, depth-1, false, alpha, beta));
            alpha = max(alpha, value);
//            if (alpha >= beta){
//                break;
//            }
        }
        //return alpha;
        return value;
    }
    else if (maximizingPlayer == false){
        int value = 1000000;
        for (auto c: st.get_valid_spots()){
            OthelloBoard g_cpy = st;
            g_cpy.put_disc(c);
            value = min(value, minimax(g_cpy, depth-1, true, alpha, beta));
            beta = min(beta, value);
//            if (beta <= alpha){
//                break;
//            }
        }
        //return beta;
        return value;
    }
    return state(st);
}
int state(OthelloBoard st)
{
    int h[8][8] =
    {{20,-3,11,8,8,11,-3,20},
     {-3,-7,-4,-1,-1,-4,-7,-3},
     {11,-4,2,2, 2,2,-4,11},
     {8,-1,2,-3,-3,2,-1,8},
     {8,-1,2,-3,-3,2,-1,8},
     {11,-4,2,2,2,2,-4,11},
     {-3,-7,-4,-1,-1,-4,-7,-3},
     {20,-3, 11,8,8,11,-3,20}};
    int score = 0;
    st.disc_count[st.BLACK] = st.disc_count[st.WHITE] = 0;
    st.disc_count[st.EMPTY] = 64;
    st.cur_player = player;
    //oppo board
    OthelloBoard oppo = st;
    oppo.cur_player = st.cur_player;
    oppo.cur_player = oppo.get_next_player(oppo.cur_player);
    oppo.board = st.board;

    for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
            if (st.board[i][j] == st.cur_player){
                st.disc_count[st.cur_player]++;
                st.disc_count[st.EMPTY]--;
                score += h[i][j];
            }
            else if (st.board[i][j] == oppo.cur_player){
                st.disc_count[oppo.cur_player]++;
                st.disc_count[st.EMPTY]--;
                score -= h[i][j];
            }
        }
    }
    return score;
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}

