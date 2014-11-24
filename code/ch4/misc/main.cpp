/*
    Towers of Hanoi
*/

#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include "CImg.h"
#include <ctime>
#include <random>
#include <string>
#include <stdexcept>
#include <chrono>
#include <thread>


template<typename T>
class Stack {
private:
    struct Node {
        T item;
        Node* next;
    };
    Node* head;
    size_t num_nodes;
public:
    Stack(): head(nullptr), num_nodes(0) {}
    Stack(Stack& other): Stack() {
        //copy constructor
        for (Node* n = other.head; n != nullptr; n = n->next) {
            push(n->item);
        }
        //items now in reverse order, so correct that
        Node* i = nullptr;
        Node* j = head;
        while ( j != nullptr ) {
            Node* k = j->next;
            j->next = i;
            i = j;
            j = k;
        }
        head = i;
    }
    void push(T item) {
        Node* n = new Node();
        n->item = item;
        n->next = head;
        head = n;
        num_nodes++;
    }
    size_t size() {
        return num_nodes;
    }
    T pop() {
        if (is_empty()) {
            throw std::underflow_error("can't pop: stack is empty");
        }
        Node* n = head;
        T item = n->item;
        head = n->next;
        delete n;
        num_nodes--;
        return item;
    }
    bool is_empty() {
        return head == nullptr;
    }
};

typedef unsigned int disc_id;
static std::string tmp_img_pathname = "/tmp/image";
static unsigned int op_id = 0;
static Stack<disc_id> pegs[3];
static cimg_library::CImgDisplay main_disp;

/*
static std::map<unsigned int, std::string> disc_move_operation_postscript;

struct worker {
    unsigned int operation_id;
    unsigned int worker_tag;
};

void foobar() {
    std::list<std::future<worker>> workers;
    std::queue<worker> payload_queue;
    uint num_threads = 5;

    for (long j = minLength; j < maxLength; j++) {
        worker_payload payload;
        payload.length = j;
        payload.ts_ptr = &TS;
        payload.size = SIZE;
        payload_queue.push(payload);
    }

    for (uint worker_id = 0; worker_id != num_threads; ++worker_id) {
        if (!payload_queue.empty()) {
            std::cout << "adding worker" << std::endl;
            //add a worker
            worker_payload payload = payload_queue.front();
            payload_queue.pop();
            workers.push_back(std::async(std::launch::async, &Program::FindMotif, this, payload));
        }
    }
    while (!workers.empty())
    {
        for (auto worker_iterator = workers.begin(); worker_iterator != workers.end(); ++worker_iterator) {
            if (worker_iterator->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                worker_result result = worker_iterator->get();
                long length = result.length;
                findmotif_results[length] = result.list;
                std::cout << "worker finished:" << result.dummy << std::endl;
                worker_iterator = workers.erase(worker_iterator);
                if (!payload_queue.empty()) {
                    //add a worker
                    worker_payload payload = payload_queue.front();
                    payload_queue.pop();
                    workers.emplace(worker_iterator, std::async(std::launch::async, &Program::FindMotif, this, payload));
                }
            }
        }
    }
}
*/

void compile_image() {
    std::string cmd1 = std::string("convert ") + tmp_img_pathname + std::to_string(op_id) + std::string(".ps") + std::string(" ") + tmp_img_pathname + std::to_string(op_id) + std::string(".jpg");
    std::string cmd2 = std::string("rm -f ") + tmp_img_pathname + std::to_string(op_id) + std::string(".ps");
    system(cmd1.c_str());
    system(cmd2.c_str());
    op_id++;
}

std::string rand_color() {
    int val = rand() % 11;
    if (val == 10) return "1";
    return std::string("0.") + std::to_string(val);
}

std::string get_disc_color(int disc_id) {
    srand(disc_id + 1);
    return rand_color() + std::string(" ") + rand_color() + std::string(" ") + rand_color();
}

unsigned int disc_width(disc_id disc) {
    return 10 * (disc + 1);
}

void write_postscript() {
    std::ofstream out(tmp_img_pathname + std::to_string(op_id) + std::string(".ps"));
    out << ""
        "/disc_outline_color { 0 0 0 1 setcmykcolor } def \n"
        "/disc_fill_color {  } def \n"
        "/disc_height { 7 } def \n"
        ""
        "/calc_peg_offset { \n"
        "  200 %peg spacing \n"
        "  mul %peg x-offset \n"
        "} def \n"
        ""
        "/peg { \n"
        "  calc_peg_offset  \n"
        "  dup 0 rmoveto %move to bottom of peg \n"
        "  currentpoint %save spot \n"
        "  0 200 rlineto %draw peg \n"
        "  moveto %return to bottom of peg \n"
        "  neg %get the negative of the x-offset \n"
        "  0 rmoveto %return to where we were before drawing the peg \n"
        "} def \n"
        ""
        "/disc { \n"
        "  goto_start \n"
        "  0 2 rmoveto %nudge upwards off base \n"
        "  3 disc_height add %account for disc outline when calculating height \n"
        "  mul %vertical disc position \n"
        "  exch %flip that result with the next parameter \n"
        "  calc_peg_offset  %horizontal disc position \n"
        "  exch rmoveto %move to position on peg where disc sits \n"
        "  currentpoint %save drawing start point \n"
        "  newpath %start drawing the disc (erases saved position) \n"
        "  moveto %return to that saved position \n"
        "  disc_height %disc height \n"
        "  exch %this puts width at top of stack, followed by the height \n"
        "  dup 2 div %half disc width \n"
        "  neg \n"
        "  0 rmoveto %draw left-most side of disc \n"
        "  dup 0 rlineto %draw bottom of peg \n"
        "  exch %flip width and height \n"
        "  dup \n"
        "  0 exch rlineto \n"
        "  exch \n"
        "  dup neg \n"
        "  0 rlineto \n"
        "  pop %remove width from stack \n"
        "  neg \n"
        "  0 exch rlineto  \n"
        "  %add an outline \n"
        "  closepath \n"
        "  gsave \n"
        "    3 setlinewidth \n"
        "    disc_outline_color stroke \n"
        "  grestore \n"
        "  setrgbcolor fill \n"
        "} def \n"
        ""
        "/goto_start { \n"
        "  100 400 moveto \n"
        "} def \n"
        ""
        "newpath \n"
        "%set up the base \n"
        "goto_start \n"
        "100 neg 0 rmoveto \n"
        "600 0 rlineto %draw base \n"
        "goto_start %move back to starting point \n"
        "0 peg %draw source peg \n"
        "1 peg %draw temp peg \n"
        "2 peg %draw dest peg \n"
        "stroke \n";
    for (int peg_id = 0; peg_id != 3; ++peg_id) {
        Stack<disc_id> peg(pegs[peg_id]); //make a copy
        disc_id disc;
        int num_discs = peg.size();
        for (int i = 1; ! peg.is_empty(); ++i) {
            int disc_position = num_discs - i;
            disc = peg.pop();
            out << get_disc_color(disc) << " ";
            out << disc_width(disc) << " ";
            out << peg_id << " ";
            out << disc_position << " ";
            out << "disc \n";
        }
    }
    out.close();
}

void wait_for_input() {
    //draw image while waiting on key response
    while ( ! main_disp.is_closed()) {
        main_disp.wait();
        if (main_disp.released_key() == cimg_library::cimg::keySPACE) {
            break;
        }
    }
}

void show_image(unsigned int img_op_id) {
    std::string path = tmp_img_pathname + std::to_string(img_op_id) + std::string(".jpg");
    cimg_library::CImg<unsigned char> img(path.c_str());
    main_disp.display(img);
    //wait_for_input();
}

void move_disk(unsigned int src_peg_index, unsigned int dst_peg_index) {
    disc_id disc = pegs[src_peg_index].pop();
    pegs[dst_peg_index].push(disc);
    write_postscript();
    compile_image();
    //show_image();
}

void move_stack(unsigned int src_peg_index, unsigned int dst_peg_index, unsigned int length) {
    if (length == 1) {
        move_disk(src_peg_index, dst_peg_index);
    } else {
        unsigned int tmp_peg_index = 3 - src_peg_index - dst_peg_index;
        move_stack(src_peg_index, tmp_peg_index, length - 1);
        move_disk(src_peg_index, dst_peg_index);
        move_stack(tmp_peg_index, dst_peg_index, length - 1);
    }
}

int main(int argc, const char* argv[])
{
    if (std::string(argv[1]) == "compile") {
        int num_discs = 10;
        for (int i = num_discs - 1; i >= 0; --i) {
            pegs[0].push(i);
        }
        write_postscript();
        compile_image();
        //show_image();
        move_stack(0, 2, num_discs);
    } else if (std::string(argv[1]) == "display") {
        unsigned int num_ops = atoi(argv[2]);
        main_disp = cimg_library::CImgDisplay(500, 500, "Towers of Hanoi");
        main_disp.move(0, 0);
        for (unsigned int i = 0; i < num_ops; ++i) {
            show_image(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        std::cerr << "invalid input" << std::endl;
    }
    //wait_for_input();
    return 0;
}
