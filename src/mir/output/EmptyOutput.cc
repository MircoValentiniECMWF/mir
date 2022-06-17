/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/output/EmptyOutput.h"

#include <ostream>

#include "mir/param/MIRParametrisation.h"


namespace mir {
namespace output {


EmptyOutput::EmptyOutput() = default;


EmptyOutput::EmptyOutput(const std::string& /*unused*/) {}


size_t EmptyOutput::save(const param::MIRParametrisation& /*unused*/, context::Context& /*unused*/) {
    return 0;
}


bool EmptyOutput::sameAs(const MIROutput& other) const {
    const auto* o = dynamic_cast<const EmptyOutput*>(&other);
    return (o != nullptr);
}


bool EmptyOutput::sameParametrisation(const param::MIRParametrisation& /*unused*/,
                                      const param::MIRParametrisation& /*unused*/) const {
    return false;
}


bool EmptyOutput::printParametrisation(std::ostream& out, const param::MIRParametrisation& param) const {
    bool ok = false;

    long accuracy = 0;
    if (param.userParametrisation().get("accuracy", accuracy)) {
        out << "accuracy=" << accuracy;
        ok = true;
    }

    std::string packing;
    if (param.userParametrisation().get("packing", packing)) {
        if (ok) {
            out << ",";
        }
        out << "packing=" << packing;
        ok = true;
    }

    long edition = 0;
    if (param.userParametrisation().get("edition", edition)) {
        if (ok) {
            out << ",";
        }
        out << "edition=" << edition;
        ok = true;
    }

    return ok;
}


void EmptyOutput::print(std::ostream& out) const {
    out << "EmptyOutput[]";
}


static const MIROutputBuilder<EmptyOutput> output("empty");


}  // namespace output
}  // namespace mir
