#include "vmrp.h"
#include <ctime>
#include <algorithm>

Replace::Replace() {
    int i;
    std::cout << "Please input page numbers :";
    std::cin >> PageNumber;
    ReferencePage = new int[PageNumber];
    EliminatePage = new int[PageNumber];

    std::cout << "Please input reference page string :";
    for (i = 0; i < PageNumber; i++)
        std::cin >> ReferencePage[i];

    std::cout << "Please input page frames :";
    std::cin >> FrameNumber;
    PageFrames = new int[FrameNumber];
}

Replace::~Replace() {
    delete [] ReferencePage;
    delete [] EliminatePage;
    delete [] PageFrames;
}

void Replace::InitSpace(const char *MethodName) {
    std::cout << std::endl << MethodName << std::endl;
    FaultNumber = 0;
    for (int i = 0; i < PageNumber; i++)
        EliminatePage[i] = -1;
    for (int i = 0; i < FrameNumber; i++)
        PageFrames[i] = -1;
}

void Replace::Report() {
    std::cout << std::endl << "Eliminate page:";
    for (int i = 0; EliminatePage[i] != -1; i++)
        std::cout << EliminatePage[i] << " ";
    std::cout << std::endl << "Number of page faults = " << FaultNumber << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Rate of page faults = " << 100.0f * FaultNumber / PageNumber << "%" << std::endl;
}

void Replace::Fifo() {
    InitSpace("FIFO");
    int pointer = 0;
    int eliminateIndex = 0;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            if (PageFrames[pointer] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[pointer];
            PageFrames[pointer] = next;
            pointer = (pointer + 1) % FrameNumber;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << " ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    Report();
}

void Replace::Lru() {
    InitSpace("LRU");
    int *lastUsed = new int[FrameNumber];
    int eliminateIndex = 0;
    for (int i = 0; i < FrameNumber; i++) lastUsed[i] = -1;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                lastUsed[i] = k;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            int replaceIdx = 0;
            for (int i = 1; i < FrameNumber; i++) {
                if (lastUsed[i] < lastUsed[replaceIdx])
                    replaceIdx = i;
            }
            if (PageFrames[replaceIdx] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[replaceIdx];
            PageFrames[replaceIdx] = next;
            lastUsed[replaceIdx] = k;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << " ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    delete[] lastUsed;
    Report();
}

void Replace::Clock() {
    InitSpace("CLOCK");
    int pointer = 0;
    bool *used = new bool[FrameNumber];
    int eliminateIndex = 0;
    for (int i = 0; i < FrameNumber; i++) used[i] = false;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                used[i] = true;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            while (used[pointer]) {
                used[pointer] = false;
                pointer = (pointer + 1) % FrameNumber;
            }
            if (PageFrames[pointer] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[pointer];
            PageFrames[pointer] = next;
            used[pointer] = true;
            pointer = (pointer + 1) % FrameNumber;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << "(" << (used[i]?"1":"0") << ") ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    delete[] used;
    Report();
}

void Replace::Eclock() {
    InitSpace("ECLOCK");
    int pointer = 0;
    bool *used = new bool[FrameNumber];
    bool *modified = new bool[FrameNumber];
    int eliminateIndex = 0;
    for (int i = 0; i < FrameNumber; i++) {
        used[i] = false;
        modified[i] = false;
    }

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                used[i] = true;
                modified[i] = true;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            while (true) {
                if (!used[pointer] && !modified[pointer]) break;
                if (used[pointer]) {
                    used[pointer] = false;
                } else {
                    modified[pointer] = false;
                }
                pointer = (pointer + 1) % FrameNumber;
            }
            if (PageFrames[pointer] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[pointer];
            PageFrames[pointer] = next;
            used[pointer] = true;
            modified[pointer] = (rand() % 2 == 0);
            pointer = (pointer + 1) % FrameNumber;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << "(" << (used[i]?"1":"0") << "," << (modified[i]?"1":"0") << ") ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    delete[] used;
    delete[] modified;
    Report();
}

void Replace::Lfu() {
    InitSpace("LFU");
    int *frequency = new int[FrameNumber];
    int eliminateIndex = 0;
    for (int i = 0; i < FrameNumber; i++) frequency[i] = 0;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                frequency[i]++;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            int replaceIdx = 0;
            for (int i = 1; i < FrameNumber; i++) {
                if (frequency[i] < frequency[replaceIdx])
                    replaceIdx = i;
            }
            if (PageFrames[replaceIdx] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[replaceIdx];
            PageFrames[replaceIdx] = next;
            frequency[replaceIdx] = 1;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << "(" << frequency[i] << ") ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    delete[] frequency;
    Report();
}

void Replace::Mfu() {
    InitSpace("MFU");
    int *frequency = new int[FrameNumber];
    int eliminateIndex = 0;
    for (int i = 0; i < FrameNumber; i++) frequency[i] = 0;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                frequency[i]++;
                break;
            }
        }

        if (!found) {
            FaultNumber++;
            int replaceIdx = 0;
            for (int i = 1; i < FrameNumber; i++) {
                if (frequency[i] > frequency[replaceIdx])
                    replaceIdx = i;
            }
            if (PageFrames[replaceIdx] != -1)
                EliminatePage[eliminateIndex++] = PageFrames[replaceIdx];
            PageFrames[replaceIdx] = next;
            frequency[replaceIdx] = 1;
        }
        
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1)
                std::cout << PageFrames[i] << "(" << frequency[i] << ") ";
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex-1] != -1)
            std::cout << "->" << EliminatePage[eliminateIndex-1];
        std::cout << std::endl;
    }
    delete[] frequency;
    Report();
}

void Replace::GenerateRandomRef(int length, int max_page) {
    std::srand(std::time(0));
    PageNumber = length;
    ReferencePage = new int[PageNumber];
    std::cout << "Generated reference string: ";
    for (int i = 0; i < PageNumber; i++) {
        ReferencePage[i] = std::rand() % max_page + 1;
        std::cout << ReferencePage[i] << " ";
    }
    std::cout << std::endl;
}

int main() {
    Replace *vmpr = new Replace();
    vmpr->Fifo();
    vmpr->Lru();
    vmpr->Clock();
    vmpr->Eclock();
    vmpr->Lfu();
    vmpr->Mfu();
    delete vmpr;
    return 0;
}
