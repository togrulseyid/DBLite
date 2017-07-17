#include <iostream>
#include <atomic>
#include <tbb/tbb.h>
#include <unordered_map>
#include <mutex>

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

bool used[10010000];
int primes[1001000];
int cnt_prime = 0;

int get_size(uint32_t size, bool prime) { // if prime bool, it returns a prime number greater than size, or the power of two
    if(!prime){
        int cnt = 0;
        int last = 0;
        for (int i = 0; i < 32; ++i)
            if ((size & (1 << i)) != 0)
                ++cnt, last = i;
        return (cnt > 1) ? (uint32_t)(1 << (last + 2)) : (uint32_t)(1 << last + 1);
    }
    int l = 1, r = cnt_prime;
    int cur = 1;
    while(l <= r){
        int mid = (l + r) >> 1;
        if(primes[mid] <= size)
            cur = mid, l = mid + 1;
        else
            r = mid - 1;
    }
    return primes[cur + 15];
}

class ChainingLocking {
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
    ChainingLocking(uint64_t size, bool prime = false) : size(get_size(size, prime)) {
        locks = new tbb::spin_mutex[this->size];
        for (int i = 0; i < this->size; ++i)
            locks[i].internal_construct();
        table = new Entry *[this->size];
        for (int i = 0; i < this->size; ++i)
            table[i] = NULL;
    }

    // Destructor
    ~ChainingLocking() {
        delete[] table;
        delete[] locks;
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

    inline void insert(Entry *insert_entry) {
        Entry *entry = new Entry();
        entry->key = insert_entry->key;
        entry->value = insert_entry->value;
        uint32_t idx = hashKey(insert_entry->key) % size;
        locks[idx].lock();
        entry->next = table[idx];
        swap(table[idx], entry);
        locks[idx].unlock();
    }
};

class ChainingLockFree {
public:
    // Chained tuple entry
    struct Entry {
        uint64_t key;
        uint64_t value;
        Entry *next;
    };
    uint64_t size;
    std::atomic<Entry *> *table;

    // Constructor
    ChainingLockFree(uint64_t size, bool prime = false) : size(get_size(size, prime)) {

        table = new std::atomic<Entry *>[this->size];
        for(int i = 0; i < this->size; ++i)
            table[i] = NULL;
    }

    // Destructor
    ~ChainingLockFree() {
        delete[] table;
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
        uint32_t idx = hashKey(key) % size;
        Entry *head = table[idx].load();
        uint64_t cnt = 0;
        while (head != NULL) {
            if (head->key == key)
                ++cnt;
            head = head->next;
        }
        return cnt;
    }

    inline void insert(Entry *insert_entry) {
        Entry *entry = new Entry();
        entry->key = insert_entry->key;
        entry->value = insert_entry->value;
        uint64_t idx = hashKey(entry->key) % size;
        do {
            entry->next = table[idx].load();
        } while (!table[idx].compare_exchange_weak(entry->next, entry));
    }
};

class LinearProbing {
public:
    // Entry
    struct Entry {
        uint64_t key;
        uint64_t value;
    };
    uint64_t size;
    uint64_t *keys;
    uint64_t *values;
    std::atomic<bool> *used;

    // Constructor
    LinearProbing(uint64_t size, bool prime = false) : size(get_size(size, prime)) {
        keys = new uint64_t[this->size];
        values = new uint64_t[this->size];
        used = new std::atomic<bool>[this->size];
        for(int i = 0; i < this->size; ++i)
            used[i] = false;
    }

    // Destructor
    ~LinearProbing() {
        delete[] values;
        delete[] keys;
        delete[] used;
    }

    // Returns the number of hits
    inline uint64_t lookup(uint64_t key) {
        int idx = hashKey(key) % size;
        int cnt = 0;
        while (used[idx].load()) {
            if (keys[idx] == key)
                ++cnt;
            idx = (idx + 1) % size;
        }
        return cnt;
    }

    inline void insert(Entry R) {
        uint64_t idx = hashKey(R.key) % size;
        while (used[idx].exchange(true)) {
            idx = (idx + 1) % size;
        }
        keys[idx] = R.key;
        values[idx] = R.value;
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
        cout << "STL\t\t\tbuild:" << (sizeR / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

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
        cout << "probe: " << (sizeS / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << ((sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    // Test you implementation here... (like the STL test above)

    // ChainingLocking
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        ChainingLocking ht(sizeR);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            auto numElems = size_t(range.end() - range.begin());
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto entry = ChainingLocking::Entry{R[i], 0ull, NULL};
                ht.insert(&entry);
            }
        });

        const auto probeTS = tick_count::now();
        cout << "ChainingLocking\t\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    // ChainingLockFree
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        ChainingLockFree ht(sizeR);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto entry = ChainingLockFree::Entry{R[i], 0ull, NULL};
                ht.insert(&entry);
            }
        });

        const auto probeTS = tick_count::now();
        cout << "ChainingLockFree\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    // LinearProbing
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        LinearProbing ht(sizeR);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            for (auto i = range.begin(); i != range.end(); ++i) {
                auto entry = LinearProbing::Entry{R[i], 0ull};
                ht.insert(entry);
            }
        });

        const auto probeTS = tick_count::now();
        cout << "LinearProbing\t\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }
    // generating primes up to 10000000
    for(int i = 2; i <= 10000000; ++i){
        if(!used[i]) {
            for(int j = 2 * i; j <= 10000000; j += i)
                used[j] = true;
        }
    }

    for(int i = 2; i <= 10000000; ++i)
        if(!used[i])
            primes[++cnt_prime] = i;


    // the same with prime number of slots for hash table
    cout<<endl<<"The same with prime number of slots for hash table"<<endl<<endl;
    // ChainingLocking
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        ChainingLocking ht(sizeR, true);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto entry = ChainingLocking::Entry{R[i], 0ull, NULL};
                ht.insert(&entry);
            }
        });

        const auto probeTS = tick_count::now();
        cout << "ChainingLocking\t\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    // ChainingLockFree
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        ChainingLockFree ht(sizeR, true);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            for (size_t i = range.begin(); i != range.end(); ++i) {
                auto entry = ChainingLockFree::Entry{R[i], 0ull, NULL};
                ht.insert(&entry);
            }
        });

        const auto probeTS = tick_count::now();
        cout << "ChainingLockFree\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    // LinearProbing
    {
        // Building hash table in a parallel fashion
        const auto buildTS = tick_count::now();
        LinearProbing ht(sizeR, true);
        parallel_for(blocked_range<size_t>(0, sizeR), [&](const auto &range) {
            for (auto i = range.begin(); i != range.end(); ++i) {
                auto entry = LinearProbing::Entry{R[i], 0ull};
                ht.insert(entry);
            }
        });
        const auto probeTS = tick_count::now();
        cout << "LinearProbing\t\tbuild:" << (double(sizeR) / 1e6) / (probeTS - buildTS).seconds() << "MT/s\t";

        // Probe hash table and count number of hits
        std::atomic<uint64_t> hitCounter;
        hitCounter = 0;
        parallel_for(blocked_range<size_t>(0, sizeS), [&](const auto &range) {
            uint64_t localHitCounter = 0;
            for (auto i = range.begin(); i != range.end(); ++i) {
                localHitCounter += ht.lookup(S[i]);
            }
            hitCounter += localHitCounter;
        });
        const auto stopTS = tick_count::now();
        cout << "probe: " << (double(sizeS) / 1e6) / (stopTS - probeTS).seconds() << "MT/s\t"
             << "total: " << (double(sizeR + sizeS) / 1e6) / (stopTS - buildTS).seconds() << "MT/s\t"
             << "count: " << hitCounter << endl;
    }

    return 0;
}
