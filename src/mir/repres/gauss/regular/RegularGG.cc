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


#include "mir/repres/gauss/regular/RegularGG.h"

#include <ostream>
#include <utility>

#include "mir/repres/gauss/GaussianIterator.h"
#include "mir/util/Exceptions.h"


namespace mir {
namespace repres {
namespace gauss {
namespace regular {


RegularGG::RegularGG(const param::MIRParametrisation& parametrisation) : Regular(parametrisation) {}


RegularGG::RegularGG(size_t N, const util::BoundingBox& bbox, double angularPrecision) :
    Regular(N, bbox, angularPrecision) {}


void RegularGG::print(std::ostream& out) const {
    out << "RegularGG["
           "N="
        << N_ << ",Ni=" << Ni_ << ",Nj=" << Nj_ << ",bbox=" << bbox_ << "]";
}


bool RegularGG::sameAs(const Representation& other) const {
    auto o = dynamic_cast<const RegularGG*>(&other);
    return (o != nullptr) && Regular::sameAs(other);
}


Iterator* RegularGG::iterator() const {
    std::vector<long> pl(N_ * 2, long(4 * N_));
    return new gauss::GaussianIterator(latitudes(), std::move(pl), bbox_, N_, Nj_, 0);
}


const Gridded* RegularGG::croppedRepresentation(const util::BoundingBox& bbox) const {
    return new RegularGG(N_, bbox, angularPrecision_);
}


std::string RegularGG::factory() const {
    return "regular_gg";
}


static RepresentationBuilder<RegularGG> reducedGG("regular_gg");


}  // namespace regular
}  // namespace gauss
}  // namespace repres
}  // namespace mir
