#include "cmd_options.h"
#include <boost/program_options/errors.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <print>
#include <string>
#include <string_view>

namespace CryptoGuard {

namespace po = boost::program_options;
using namespace std;

ProgramOptions::ProgramOptions() : desc_("Allowed options"), command_(COMMAND_TYPE::INVALID) {
    // clang-format off
    // I want this to look beautiful but clang won't let me
    desc_.add_options()
    ("help,h", "print this")
    ("command,c", po::value<string>(), "encrypt, decrypt or checksum")
    ("input,i", po::value<string>(), "input file")
    ("output,o", po::value<string>(),"output file")
    ("password,p", po::value<string>(), "password");
    // clang-format on
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::Parse(int argc, char *argv[]) {
    try {
        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc_), vm);
        po::notify(vm);

        if (vm.count("help")) {
            reset();
            std::ostringstream oss;
            oss << desc_;
            std::print("{} \n", oss.str());
            return;
        }

        readParams(vm);
        if (!validate()) {
            reset();
        }

    } catch (po::error &e) {
        reset();
        print("Error: {}\n", e.what());
    }
}

void ProgramOptions::reset() {
    command_ = ProgramOptions::COMMAND_TYPE::INVALID;
    inputFile_.clear();
    outputFile_.clear();
    password_.clear();
};

void ProgramOptions::readParams(const po::variables_map &vm) {
    if (vm.count("input")) {
        inputFile_ = vm["input"].as<string>();
    }
    if (vm.count("output")) {
        outputFile_ = vm["output"].as<string>();
    }
    if (vm.count("password")) {
        password_ = vm["password"].as<string>();
    }
    if (vm.count("command")) {
        std::string_view c = vm["command"].as<string>();
        if (commandMapping_.contains(c)) {
            command_ = commandMapping_.at(c);
        } else
            command_ = ProgramOptions::COMMAND_TYPE::INVALID;
    }
}

bool ProgramOptions::validate() {
    if (command_ == ProgramOptions::COMMAND_TYPE::INVALID) {
        print("Incorrect --command(-c) option. See --help\n");
        return false;
    }
    if (inputFile_.empty()) {
        print("Incorrect input file parameter. See --help\n");
        return false;
    }
    if (command_ == ProgramOptions::COMMAND_TYPE::DECRYPT || command_ == ProgramOptions::COMMAND_TYPE::ENCRYPT) {
        if (outputFile_.empty()) {
            print("Incorrect output file parameter. See --help\n");
            return false;
        }
        if (password_.empty()) {
            print("Incorrect password parameter. See --help\n");
            return false;
        }
    }
    return true;
}

}  // namespace CryptoGuard
