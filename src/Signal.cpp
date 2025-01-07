#include "Signal.hpp"
#include <iostream>
#include <csignal>

Signal::Signal()
{
    signals();  
}

void Signal::signalHandler(int signal) {
    std::cerr << "signal: " << signal << std::endl;
    exit(signal);
}

void Signal::signals()
{
    signal(SIGSEGV, signalHandler); // Seg  == 11
    signal(SIGINT, signalHandler); // Ctrl+C == 2
    signal(SIGTERM, signalHandler); // Termin ==15
} 