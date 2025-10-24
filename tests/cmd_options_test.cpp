#include "cmd_options.h"
#include <gtest/gtest.h>

namespace CryptoGuard::Test {

// Helper
struct Args {
    std::vector<std::string> storage;  // holds the strings
    std::vector<char *> argv_;         // pointers into storage
    int argc = 0;

    explicit Args(const std::string &cmdline) {
        std::istringstream iss(cmdline);
        std::string arg;
        storage.push_back("program");  // argv[0] usually the program name
        while (iss >> arg)
            storage.push_back(arg);

        for (auto &s : storage)
            argv_.push_back(s.data());
        argc = static_cast<int>(argv_.size());
    }

    char **argv() { return argv_.data(); }
};

void assert_po_parse_result(const ProgramOptions &po, const ProgramOptions::COMMAND_TYPE &command,
                            const std::string &in, const std::string &out, const std::string &pass) {
    ASSERT_EQ(po.GetCommand(), command);
    ASSERT_EQ(po.GetInputFile(), in);
    ASSERT_EQ(po.GetOutputFile(), out);
    ASSERT_EQ(po.GetPassword(), pass);
}

void assert_po_parse_fail(const ProgramOptions &po) {
    assert_po_parse_result(po, ProgramOptions::COMMAND_TYPE::INVALID, "", "", "");
}

TEST(ProgramOptions, SimpleEncryptTest) {
    ProgramOptions po;
    Args args("-i input.txt -o output.txt -p 1234 -c encrypt");
    ASSERT_NO_THROW(po.Parse(args.argc, args.argv()));
    assert_po_parse_result(po, ProgramOptions::COMMAND_TYPE::ENCRYPT, "input.txt", "output.txt", "1234");
}

TEST(ProgramOptions, SimpleDecryptTest) {
    ProgramOptions po;
    Args args("-i in.txt -o out.txt -p 12345 -c decrypt");
    ASSERT_NO_THROW(po.Parse(args.argc, args.argv()));
    assert_po_parse_result(po, ProgramOptions::COMMAND_TYPE::DECRYPT, "in.txt", "out.txt", "12345");
}

TEST(ProgramOptions, SimpleChecksumTest) {
    ProgramOptions po;
    Args args("-i somefile.txt -c checksum");
    ASSERT_NO_THROW(po.Parse(args.argc, args.argv()));
    assert_po_parse_result(po, ProgramOptions::COMMAND_TYPE::CHECKSUM, "somefile.txt", "", "");
}

TEST(ProgramOptions, SimpleWrongOptionTest) {
    ProgramOptions po;
    Args args("We shall cease means of production!");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(args.argc, args.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Incorrect --command(-c) option"));
}

TEST(ProgramOptions, SimpleHelpTest) {
    ProgramOptions po;
    Args args("--help");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(args.argc, args.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Allowed options"));

    Args sargs("-h");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(sargs.argc, sargs.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Allowed options"));
}

TEST(ProgramOptions, InvalidCombinationTest) {
    ProgramOptions po;
    // No input file
    Args noInputArgs("--output out.txt --password 1234 --command encrypt");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(noInputArgs.argc, noInputArgs.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Incorrect input file parameter"));

    // No output file
    Args noOutArgs("--input in.txt --password 1234 --command encrypt");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(noOutArgs.argc, noOutArgs.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Incorrect output file parameter"));

    // No password
    Args noPassArgs("--input in.txt --output out.txt --command encrypt");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(noPassArgs.argc, noPassArgs.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Incorrect password parameter"));

    // No command
    Args noCArgs("--input in.txt --output out.txt --password 1234");
    testing::internal::CaptureStdout();
    ASSERT_NO_THROW(po.Parse(noCArgs.argc, noCArgs.argv()));
    assert_po_parse_fail(po);
    ASSERT_TRUE(testing::internal::GetCapturedStdout().contains("Incorrect --command(-c) option"));
}
}  // namespace CryptoGuard::Test