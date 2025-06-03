#include "vmrp.h"
#include <ctime>
#include <algorithm>
#include <iomanip>

Replace::Replace() {
    std::cout << "=== Page Replacement Algorithm Simulator (Auto Mode) ===" << std::endl;
    
    // 自动生成参数
    PageNumber = 12;                          // 默认12个页面引用
    int max_page = 5;                         // 页面号范围1~5
    FrameNumber = 4;                          // 默认4个内存帧

    // 随机生成引用串
    ReferencePage = new int[PageNumber];
    EliminatePage = new int[PageNumber];
    GenerateRandomRef(PageNumber, max_page);

    // 分配内存帧
    PageFrames = new int[FrameNumber];
}

Replace::~Replace() {
    delete[] ReferencePage;
    delete[] EliminatePage;
    delete[] PageFrames;
}

void Replace::GenerateRandomRef(int length, int max_page) {
    std::srand(std::time(0));
    std::cout << "Generated reference string: ";
    for (int i = 0; i < length; i++) {
        ReferencePage[i] = std::rand() % max_page + 1;
        std::cout << ReferencePage[i] << " ";
    }
    std::cout << "\nMemory frames: " << FrameNumber << std::endl;
}

void Replace::InitSpace(const char *MethodName) {
    std::cout << "\n===== " << MethodName << " =====" << std::endl;
    FaultNumber = 0;
    std::fill_n(EliminatePage, PageNumber, -1);
    std::fill_n(PageFrames, FrameNumber, -1);
}

void Replace::Report() {
    std::cout << "-> Page faults: " << FaultNumber 
              << " (" << std::fixed << std::setprecision(1) 
              << 100.0 * FaultNumber / PageNumber << "%)" << std::endl;
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
    int *frequency = new int[FrameNumber](); // 频率计数器初始化为0
    int eliminateIndex = 0;

    // 调试输出确认参数
    std::cout << "[DEBUG] FrameNumber=" << FrameNumber << ", PageNumber=" << PageNumber << std::endl;

    for (int k = 0; k < PageNumber; k++) {
        int next = ReferencePage[k];
        bool found = false;

        // 1. 检查页面是否已在内存
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] == next) {
                found = true;
                frequency[i]++;
                break;
            }
        }

        // 2. 处理缺页
        if (!found) {
            FaultNumber++;
            int replaceIdx = 0;
            bool hasFreeFrame = false;

            // 2.1 优先使用空闲帧
            for (int i = 0; i < FrameNumber; i++) {
                if (PageFrames[i] == -1) {
                    replaceIdx = i;
                    hasFreeFrame = true;
                    break;
                }
            }

            // 2.2 若无空闲帧，选择频率最高的页替换
            if (!hasFreeFrame) {
                for (int i = 1; i < FrameNumber; i++) {
                    if (frequency[i] > frequency[replaceIdx]) {
                        replaceIdx = i;
                    }
                }
                EliminatePage[eliminateIndex++] = PageFrames[replaceIdx];
            }

            // 2.3 装入新页面并重置频率
            PageFrames[replaceIdx] = next;
            frequency[replaceIdx] = 1;
        }

        // 3. 输出当前状态
        std::cout << "Step " << k + 1 << ": ";
        for (int i = 0; i < FrameNumber; i++) {
            if (PageFrames[i] != -1) {
                std::cout << PageFrames[i] << "(" << frequency[i] << ") ";
            }
        }
        if (!found && eliminateIndex > 0 && EliminatePage[eliminateIndex - 1] != -1) {
            std::cout << "-> " << EliminatePage[eliminateIndex - 1];
        }
        std::cout << std::endl;
    }

    delete[] frequency;
    Report();
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
