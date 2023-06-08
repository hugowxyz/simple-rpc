#ifndef SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H
#define SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H

#include "abstract_id_generator.h"

//! Chronologically sortable and globally unique in clusters
class chronological_id_generator : private abstract_id_generator {
public:
    std::string generate_id() override {

    }
};

#endif //SIMPLERPC_CHRONOLOGICAL_ID_GENERATOR_H
