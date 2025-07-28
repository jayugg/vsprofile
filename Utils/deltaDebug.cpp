//
// Created by Jacopo Uggeri on 27/07/2025.
//
#include <iostream>
#include <vector>
#include <functional>

// Interactive test
bool manualTest(const std::vector<std::string>& mods) {
    std::cout << "\nTesting (" << mods.size() << " mods):\n";
    for (auto &m : mods) std::cout << "  " << m << "\n";
    std::cout << "Crash? (y/n): ";
    char c; std::cin >> c;
    return (c=='y');
}

std::vector<std::string> withoutChunk(const std::vector<std::string>& mods,
                                      int i, int n) {
    int sz = mods.size();
    int chunkSize = (sz + n - 1) / n;  // ceil(sz/n)
    int start = i * chunkSize;
    int end   = std::min(start + chunkSize, sz);

    std::vector<std::string> out;
    for (int j = 0; j < sz; ++j) {
        if (j < start || j >= end) out.push_back(mods[j]);
    }
    return out;
}

std::vector<std::string> ddmin(const std::vector<std::string>& mods,
                               std::function<bool(const std::vector<std::string>&)> test) {
    int n = 2;
    std::vector<std::string> current = mods;

    while (current.size() >= 2) {
        bool reduced = false;
        for (int i = 0; i < n; ++i) {
            auto trial = withoutChunk(current, i, n);
            // If removing this chunk STILL crashes, we can drop it
            if (trial.size() > 0 && test(trial)) {
                current = std::move(trial);
                n = 2;
                reduced = true;
                break;
            }
        }
        if (!reduced) {
            if (n >= (int)current.size()) break;    // can’t split finer
            n = std::min((int)current.size(), n * 2);
        }
    }
    return current;
}

int run() {
    std::vector<std::string> allMods = {
            "modA.zip","modB.zip","modC.zip","modD.zip","modE.zip"
    };

    std::cout << "Full set crash? ";
    if (!manualTest(allMods)) {
        std::cout << "Nothing to debug.\n";
        return 0;
    }

    auto culprit = ddmin(allMods, manualTest);

    std::cout << "\n>>> Minimal failing set (" << culprit.size() << "):\n";
    for (auto &m: culprit) std::cout << "  " << m << "\n";
}
