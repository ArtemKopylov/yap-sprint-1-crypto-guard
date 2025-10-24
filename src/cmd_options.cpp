#include "cmd_options.h"
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace CryptoGuard {

namespace po = boost::program_options;
using namespace std;

ProgramOptions::ProgramOptions() : desc_("Allowed options"), command_(COMMAND_TYPE::INVALID) {
    desc_.add_options()("help,h", "print this")("command,c", po::value<string>(), "encrypt, decrypt or checksum")(
        "input,i", po::value<string>(), "input file")("output,o", po::value<string>(),
                                                      "output file")("password,p", po::value<string>(), "password");
}

ProgramOptions::~ProgramOptions() = default;

void ProgramOptions::Parse(int argc, char *argv[]) {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc_), vm);
    po::notify(vm);
}

}  // namespace CryptoGuard
