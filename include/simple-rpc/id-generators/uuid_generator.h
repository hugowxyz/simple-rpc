#pragma once

#include "abstract_id_generator.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class uuid_generator : public abstract_id_generator {
private:
    boost::uuids::random_generator generator_;
public:
    std::string generate_id() override {
        return boost::uuids::to_string(generator_());
    }
};
