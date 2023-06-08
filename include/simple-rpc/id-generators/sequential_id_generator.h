#ifndef SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H
#define SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H

#include "abstract_id_generator.h"

class sequential_id_generator : public abstract_id_generator {
private:
    uint64_t counter;
public:
    sequential_id_generator(uint64_t start=0) : counter(start) {}

    std::string generate_id() override {
        return std::to_string(counter++);
    }
};

#endif //SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H
