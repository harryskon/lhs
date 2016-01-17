// This program converts a list of node edges
// into a list of Ising coefficients with
// Hamiltonian H = sum_ij (N-e_ij) s_i s_j,
// where e_ij=1 if the edge exists between i and j,
// and e_ij=0 otherwise.

// Input file is a list of edges defined as
// two numbers (indices i j) per line.
// Indices start at 0 and continuously numbered.

// Empty lines are allowed as well as comments
// with lines started with #.
// Repetitions of edges are not allowed.
// Standalone node can be defined as edge to itself.

// Oleg Mazonka, 2016

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <cctype>

using std::string;

void usage()
{
    std::cout << "Usage: 'qxt input_file' or 'qxt -r'\n";
    std::cout << "\tinput_file is a list of edges, one per line\n";
    std::cout << "\t'-r' is to parse result from isakov\n";
}

// workaround: gcc 4.9.2 (!) under windows does not have std::to_string
namespace std {string my_string(int x) {std::ostringstream os; os << x; return os.str();}}


int main(int ac, char * av[])
try
{
    if ( ac < 2 )
    {
        usage();
        return 0;
    }

    string av1 = av[1];

    if ( av1 == "-r" )
    {
        int parse_result();
        return parse_result();
    }

    std::map< int, std::set<int> > edges;
    std::set< std::pair<int, int> > check;

    std::ifstream in(av[1]);
    if ( !in ) throw "Cannot open file " + av1;

    int line_counter = 1;
    for ( string line; std::getline(in, line); line_counter++ )
    {
        if ( line.empty() || line[0] == '#' ) continue;
        if ( line.size() == 1 && line[0] == '\r' ) continue; // dos/unix

        int x, y;
        std::istringstream is(line);
        is >> x >> y;

        if ( !is ) throw "Bad indices at line " + std::to_string(line_counter);

        // check that edge is not repeated
        if ( check.find(std::make_pair(x, y)) != check.end() )
            throw "Edge repetition at line " + std::to_string(line_counter);

        check.insert(std::make_pair(x, y));
        check.insert(std::make_pair(y, x));

        edges[x].insert(y);
        edges[y].insert(x);
    }

    // check that indices are continous
    int current_node_counter = -1;
    for ( const auto & node : edges )
    {
        int idx = node.first;

        if ( ++current_node_counter != idx )
        {
            if ( idx == 0 )
                throw string() + "Node 0 must be defined";
            else
                throw "Node " + std::to_string(idx)
                + " is defined, but not " + std::to_string(idx - 1);
        }
    }

    // number of nodes
    int N = edges.size();

    // check that number of nodes is even
    if ( N == 0 ) throw "Input file " + av1 + " does not define any edges";
    if ( N % 2 ) throw string("Number of nodes must be even");

    // output coefficients
    std::ofstream of((av1 + ".isakov").c_str());
    for ( const auto & node : edges )
    {
        int index = node.first;

        of << index << ' ' << index << " 0\n";
        for ( int i = index + 1; i < N; i++ )
        {
            bool e = node.second.find(i) != node.second.end();

            // output coefficient (N-e), i.e. N-1 if edge; N if not
            of << index << ' ' << i << ' ' << (e ? N - 1 : N) << '\n';
        }
    }
}
catch (string e)
{
    std::cerr << "Error: " << e << '\n'; return 3;
}
catch (std::exception & e)
{
    std::cerr << "Error: " << e.what() << '\n'; return 2;
}
catch (...)
{
    std::cerr << "Unknown error\n"; return 1;
}


int parse_result()
{
    // find the best solution
    int bestH = 0;
    std::set<string> solutions;
    while (true)
    {
        int H, N;
        string s, t;
        std::cin >> H >> N >> t;
        if ( !std::cin ) break;

        // check that split is equal and rebuild unique solution
        size_t cntr = 0;
        for ( size_t i = 0; i < t.size(); i++ )
        {
            if ( t[i] == t[0] )
            {
                cntr++;
                s += '0';
            }
            else
                s += '1';
        }

        if ( cntr * 2 != s.size() ) continue; // not equal groups

        if ( solutions.empty() )
        {
            solutions.insert(s);
            bestH = H;
        }

        if ( H > bestH ) continue;
        else if ( H == bestH ) solutions.insert(s);
        else
        {
            solutions.clear();
            bestH = H;
            solutions.insert(s);
        }
    }

    if ( solutions.empty() )
    {
        std::cout << "No solutions found\n";
        return 4;
    }

    // output unique solutions in sorted order
    for ( const auto & S : solutions )
    {
        std::cout << "Group A:";
        for ( size_t i = 0; i < S.size(); i++ )
            if ( S[i] == S[0] )
                std::cout << ' ' << i;

        std::cout << "\nGroup B:";
        for ( size_t i = 0; i < S.size(); i++ )
            if ( S[i] != S[0] )
                std::cout << ' ' << i;

        std::cout << "\n\n";
    }

    return 0;
}
