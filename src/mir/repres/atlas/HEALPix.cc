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


#include "mir/repres/atlas/HEALPix.h"

#include <ostream>

#include "mir/param/MIRParametrisation.h"
#include "mir/util/Exceptions.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace atlas {


static const RepresentationBuilder<HEALPix> unstructured_grid("healpix");


HEALPix::HEALPix(const param::MIRParametrisation& param) : N_(0) {
    param.get("N", N_);
    ASSERT(N_ > 0);

    param.get("orderingConvention", orderingConvention_);
    ASSERT(!orderingConvention_.empty());
}


const ::atlas::Grid& HEALPix::atlasGridRef() const {
    return grid_ ? grid_ : (grid_ = {name()});
}


std::string HEALPix::name() const {
    return "H" + std::to_string(N_);
}


bool HEALPix::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const HEALPix*>(&other);
    return (o != nullptr) && N_ == o->N_ && orderingConvention_ == o->orderingConvention_;
}


void HEALPix::makeName(std::ostream& out) const {
    out << name();
}


void HEALPix::fill(grib_info& info) const {
    NOTIMP;
}


void HEALPix::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "structured";
    }
}


::atlas::Grid HEALPix::atlasGrid() const {
    return atlasGridRef();
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[name=" << name() << "]";
}


}  // namespace atlas
}  // namespace repres
}  // namespace mir
