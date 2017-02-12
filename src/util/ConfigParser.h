#pragma once
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

typedef std::vector<std::pair<std::string, float*>> fp_vars;

// Reads variables from a text config file
// Currently, interprets everything as a float
class ConfigParser {
    std::string filename;
    fp_vars * config_vars;

    void updateVal(std::string varname, float val);

public:
    ConfigParser(std::string, fp_vars*);
    bool updateFromFile();

    // Static helper function to create fp_vars init code
    static void genFPVars(std::string infile, std::string outfile, std::string varname);
};