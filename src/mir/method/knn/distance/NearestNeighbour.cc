/*
 * (C) Copyright 1996- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */


#include "mir/method/knn/distance/NearestNeighbour.h"

#include "eckit/exception/Exceptions.h"
#include "eckit/utils/MD5.h"


namespace mir {
namespace method {
namespace knn {
namespace distance {


NearestNeighbour::NearestNeighbour(const param::MIRParametrisation&) {
}


void NearestNeighbour::operator()(
        const Point3&,
        const neighbours_t& neighbours,
        std::vector<double>& weights ) const {

    ASSERT(!neighbours.empty());
    weights.assign(1, 1.);
}


bool NearestNeighbour::sameAs(const DistanceWeighting& other) const {
    auto o = dynamic_cast<const NearestNeighbour*>(&other);
    return o;
}


void NearestNeighbour::print(std::ostream& out) const {
    out << "NearestNeighbour[]";
}


void NearestNeighbour::hash(eckit::MD5& h) const {
    std::ostringstream s;
    s << *this;
    h.add(s.str());
}


static DistanceWeightingBuilder<NearestNeighbour> __distance("nearest-neighbour");


}  // namespace distance
}  // namespace knn
}  // namespace method
}  // namespace mir

