#pragma once

class Signal
{
    public:
        Signal();
        void signals();
        static void signalHandler(int signal);
}; 