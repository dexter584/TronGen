#ifndef HPP_HELP
#define HPP_HELP

#include <string>

const std::string g_strHelp = R"(
Usage: ./profanity [OPTIONS]

  Help:
    --help              Show help information

  Modes with arguments:
    --matching          Matching input, file or single address.

  Matching configuration:
    --prefix-count      Minimum number of prefix matches, default 0
    --suffix-count      Minimum number of suffix matches, default 6
    --quit-count        Exit the program when the generated number is greater than, default 0

  Device control:
    --skip              Skip device given by index

  Output control:
    --output            The file to output the results to

Examples:

  ./profanity --matching matching.txt
  ./profanity --matching matching.txt --skip 1
  ./profanity --matching matching.txt --prefix-count 1 --suffix-count 8
  ./profanity --matching matching.txt --prefix-count 1 --suffix-count 10 --quit-count 1
  ./profanity --matching matching.txt --output result.txt
  ./profanity --matching TUqEg3dzVEJNQSVW2HY98z5X8SBdhmao8D --prefix-count 2 --suffix-count 4 --quit-count 1

About:

  Profanity is a vanity address generator for tron: https://tron.network/
  The software is modified based on ethereum profanity.
  Author: telegram -> @jackslowfak

Fbi Warning:

  Before using a generated vanigity address, always verify that it matches the printed private key.
  And always multi-sign the address to ensure account security.
)";

#endif /* HPP_HELP */
