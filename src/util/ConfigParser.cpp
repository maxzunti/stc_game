#include "ConfigParser.h"
#include <string>

using namespace std;

ConfigParser::ConfigParser(string fname, fp_vars * vars) {
    filename = fname;
    config_vars = vars;
}

bool ConfigParser::updateFromFile() {
    ifstream config_file(filename);
    string line;
    if (config_file.is_open()) {
        cout << "\n\n**** Reading '" << filename << ": ****" << endl;
        while (getline(config_file, line)) {
            // cout << line << endl;

            int pos = line.find("=");
            string varname = line.substr(0, pos);

            if (line.length() == pos) {
                std::cout << "EQ AT END";
                continue;
            }

            try {
                float val = stof(line.substr(pos + 1));
                updateVal(varname, val);
            } catch (invalid_argument) {
               // cout << "Failed to convert " << line.substr(pos + 1) << "to float." << std::endl;
            }
        }
        config_file.close();
        return true;
    }
    else {
        return false;
    }
}


void ConfigParser::updateVal(std::string varname, float val) {
    for (const auto &p : *config_vars) {
        if (p.first == varname) {
            *p.second = val;
            cout << "Set " << varname << " = " << val << endl;
        }
    }
}

void ConfigParser::genFPVars(std::string infile, std::string outfile, std::string vname) {
    ifstream config_file(infile);
    string out_str = "fp_vars " + vname + ";\n";

    string line;
    if (config_file.is_open()) {
        cout << "Reading '" << infile << ":" << endl;
        while (getline(config_file, line)) {

            int pos = line.find("=");
            if (pos != line.npos) {
                string varname = line.substr(0, pos);
                out_str += vname + ".push_back(std::make_pair(std::string(\"" + varname + "\"), &" + varname + "));\n";
            }
        }
        config_file.close();

        ofstream ofile(outfile);
        if (ofile) {
            ofile << out_str;
        }
        ofile.close();
    }
}
