#include <iostream>
#include <cstdlib>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>

using namespace std;
using namespace tbb;

inline uint64_t hashKey(uint64_t k) {
    // MurmurHash64A
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    uint64_t h = 0x8445d61a4e774912 ^(8 * m);
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h | (1ull << 63);
}

uint32_t get_size(uint32_t size) {
    int cnt = 0;
    int last = 0;
    for (int i = 0; i < 32; ++i)
        if ((size & (1 << i)) != 0)
            ++cnt, last = i;
    if (cnt > 1)
        return 1 << (last + 1);
    return 1 << last;
}


class ChainingLockingHT {
    uint32_t size;
public:
    // Chained tuple entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        Entry *next;
    };

    Entry **table;
    tbb::spin_mutex *locks;

    // Constructor
    ChainingLockingHT(uint64_t size) : size(get_size(size)) {
        locks = new tbb::spin_mutex[size];
        for (int i = 0; i < size; ++i)
            locks[i].internal_construct();
        table = new Entry *[size];
    }

    // Destructor
    ~ChainingLockingHT() {
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
        uint32_t idx = hashKey(key) % size;
        Entry *head = table[idx];
        uint64_t cnt = 0;
        while (head != NULL) {
            if (head->key == key)
                ++cnt;
            head = head->next;
        }
        return cnt;
    }

    inline void insert(Entry *entry) {
        uint32_t idx = hashKey(entry->key) % size;
        locks[idx].lock();
        entry->next = table[idx];
        swap(entry->next, table[idx]);
        locks[idx].unlock();
    }
};

class ChainingHT {
public:
    // Chained tuple entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        Entry *next;
    };
    uint64_t size;
    std::atomic<Entry *> *table;
    tbb::spin_mutex *locks;

    // Constructor
    ChainingHT(uint64_t size) : size(get_size(size)) {
        locks = new tbb::spin_mutex[size];
        for (int i = 0; i < size; ++i)
            locks[i].internal_construct();
        table = new std::atomic<Entry *>[size];
    }

    // Destructor
    ~ChainingHT() {
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
        uint32_t idx = hashKey(key) % size;
        Entry *head = table[idx];
        uint64_t cnt = 0;
        while (head != NULL) {
            if (head->key == key)
                ++cnt;
            head = head->next;
        }
        return cnt;
    }

    inline void insert(Entry *entry) {
        uint64_t idx = hashKey(entry->key) % size;
        do {
            entry->next = table[idx].load();
        } while (!table[idx].compare_exchange_weak(entry->next));
    }
};

class LinearProbingHT {
public:
    // Entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        std::atomic<bool> marker;
    };

    // Constructor
    LinearProbingHT(uint64_t size) {
    }

    // Destructor
    ~LinearProbingHT() {
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
    }

    inline void insert(uint64_t key, uint64_t value) {
    }
};

int main(int argc, char **argv) {
    uint64_t sizeR = atoi(argv[1]);
    uint64_t sizeS = atoi(argv[2]);
    unsigned threadCount = atoi(argv[3]);

    task_scheduler_init init(threadCount);

    // Init build-side relation R with random data
    uint64_t *R = static_cast<uint64_t *>(malloc(sizeR * sizeof(uint64_t)));
    parallel_for(blocked_range<size_t>(0, sizeR), [&](const blocked_range<size_t> &range) {
        unsigned int seed = range.begin();
        for (size_t i = range.begin(); i != range.end(); ++i)
            R[i] = rand_r(&seed) % sizeR;
    });

    // Init probe-side relation S with random data
    uint64_t *S = static_cast<uint64_t *>(malloc(sizeS * sizeof(uint64_t)));
    parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t> &range) {
        unsigned int seed = range.begin();
        for (size_t i = range.begin(); i != range.end(); ++i)
            S[i] = rand_r(&seed) % sizeR;
    });

    // STL
    {
        // Build hash table (single threaded)
        tick_count buildTS = tick_count::now();
        unordered_multimap<uint64_t, uint64_t> ht(sizeR);
        for (uint64_t i = 0; i < sizeR; i++)
            ht.emplace(R[i], 0);
        tick_count probeTS = tick_count::now();
        cout << "STL      build:" << (sizeR / 1e6) / (probeTS - buildTS).seconds() << "MT/s ";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const blocked_range<size_t> &range) {
            uint64_t localHitCounter = 0;
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto range = ht.equal_range(S[i]);
                for (unordered_multimap<uint64_t, uint64_t>::iterator it = range.first; it != range.second; ++it)
                    localHitCounter++;
            }
            hitCounter += localHitCounter;
        });
        tick_count stopTS = tick_count::now();
        cout << "probe: " << (sizeS / 1e6) / (stopTS - probeTS).seconds() << "MT/s "
             << "total: " << ((sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s "
             << "count: " << hitCounter << endl;
    }

    // Test you implementation here... (like the STL test above)

    return 0;
}
