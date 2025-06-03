#ifndef VMRP_H
#define VMRP_H

#include <iostream>
#include <iomanip>
#include <cstdlib>

class Replace {
public:
    Replace();
    ~Replace();
    void InitSpace(const char *MethodName);
    void Report();
    void Fifo();
    void Lru();
    void Clock();
    void Eclock();
    void Lfu();
    void Mfu();
    void GenerateRandomRef(int length, int max_page);

private:
    int *ReferencePage;
    int *EliminatePage;
    int *PageFrames;
    int PageNumber;
    int FrameNumber;
    int FaultNumber;
};

#endif
