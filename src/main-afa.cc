#include <string>
#include <iostream>
#include <fstream>
#include <mata/inter-aut.hh>
#include <mata/mintermization.hh>
#include <mata/afa.hh>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "ERROR: Program expects at least input file as argument, try '--help' for help" << std::endl;
        return -1;
    }

    bool use_forward = true;
    bool use_new = true;

    bool print_help = false;

    std::string input_name;

    for (unsigned i = 1; i < argc; ++i) {
        std::string arg = std::string(argv[i]);
        if (arg == "--help" || arg == "-h") {
            print_help = true;
        } else if (arg == "--backward") {
            use_forward = false;
        } else if (arg == "--old") {
            use_new = false;
        } else {
            input_name = arg;
        }
    }

    if (print_help) {
        std::cout << "Usage: afa-emptiness-checker [--old] [--backward] input.emp\n"
                     "  --old          compute emptiness using the old algorithm instead of the new one\n"
                     "  --backward     compute emptiness by the backward algorithm instead of the forward one\n" << std::endl;
        return 0;
    }

    if (input_name == "") {
        std::cout << "ERROR: no input file given, try '--help' for help" << std::endl;
        return -1;
    }

    std::ifstream input(input_name);
    if (!input.is_open()) {
        std::cerr << "ERROR: could not open file " << input_name << std::endl;
        return -1;
    }

    std::cerr << "Starting mintermization" << std::endl;
    auto start = std::chrono::steady_clock::now();
    Mata::Mintermization mintermization;
    auto mintermized_input = mintermization.mintermize(Mata::IntermediateAut::parse_from_mf(Mata::Parser::parse_mf(input))[0]);
    Mata::Nfa::OnTheFlyAlphabet alphabet;
    Mata::Afa::Afa result = Mata::Afa::construct(mintermized_input, &alphabet);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cerr << "Time of mintermization: " << elapsed_seconds.count() << std::endl;

    bool is_empty;
    if (use_forward) {
        if (use_new) {
            is_empty = Mata::Afa::antichain_concrete_forward_emptiness_test_new(result);
        } else {
            is_empty = Mata::Afa::antichain_concrete_forward_emptiness_test_old(result);
        }
    } else {
        if (use_new) {
            is_empty = Mata::Afa::antichain_concrete_backward_emptiness_test_new(result);
        } else {
            is_empty = Mata::Afa::antichain_concrete_backward_emptiness_test_old(result);
        }
    }

    if (is_empty) {
        std::cout << "EMPTY" << std::endl;
    } else {
        std::cout << "NOT EMPTY" << std::endl;
    }
}