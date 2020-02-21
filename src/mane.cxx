#define PRODUCTION

#include "executable.cxx"


auto parse_args(int argc, char** argv) {
  if (argc < 2) {
    cerr << "not enough args" << endl;
    exit(0);
  }

  map<string, function<void(int, char**)>> functions;

  functions["show"] = [] (int n, char** args) {
    string_view sig{"void show(path p)"}; 

    if (n < 1) {
      cerr << "not enough args for function (" << sig << ")" << endl;
    }

    show(path{args[0]});

  };

  functions["change-entry"] = [](int n, char** args) {

    vector<char> v;
    const auto read_file = [&v](auto f) {
      f.seek(0);
      v.resize(f.size());
      f.gets(v); 
    };

    string_view sig {"void text_end_insert(auto exec, vector<char>& bs)"};

    if (n < 2) {
      cerr << "not enough args for function (" << sig << ")" << endl;
    }

    Executable ex { path{args[0]} };
    File opcodes { path{args[1]} };
    read_file(opcodes);

    
    cout << "ok. executing function..." << endl;

    return change_entry(ex, v);        

  };


  if (auto f = functions[argv[1]]; f) {
    return tuple {f, argc - 2, &argv[2]} ;
  }
  else {
    cerr << "unknown function" << endl;
    exit(0);
  }
}


int main(int argc, char** argv) {
  cout << "HI. YOU'RE 'BOUT TO PERFORM SOME ACTIONS ON A FILE" << endl;
  auto [action, n, args] = parse_args(argc, argv);
  action(n, args);
}
