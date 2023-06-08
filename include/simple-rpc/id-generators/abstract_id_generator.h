#ifndef SIMPLERPC_ABSTRACT_ID_GENERATOR_H
#define SIMPLERPC_ABSTRACT_ID_GENERATOR_H

#include <string>

class abstract_id_generator {
public:
    virtual std::string generate_id() = 0;
};

#endif //SIMPLERPC_ABSTRACT_ID_GENERATOR_H
