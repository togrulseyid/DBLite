//
// Created by mahammad on 5/21/17.
//

#ifndef DBLITE_TID_H
#define DBLITE_TID_H

#include <cstdint>
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <string>
#include <utility>

struct TID {
    uint64_t page_id;
    uint32_t slot_id;

    bool operator==(const TID &tid) const { return page_id == tid.page_id && slot_id == tid.slot_id; }

    bool operator<(const TID &tid) const {
        return page_id < tid.page_id || (page_id == tid.page_id && slot_id < tid.slot_id);
    }
};

namespace std {
    template<>
    struct hash<TID> {
        size_t operator()(TID const &v) const {
            return (hash<uint64_t>()(v.page_id) ^ (hash<uint16_t>()(v.slot_id)));
        }
    };
}

#endif //DBLITE_TID_H
