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


#include "mir/repres/other/HEALPix.h"

#include <ostream>

// #include "mir/param/MIRParametrisation.h"
#include "mir/repres/Iterator.h"
#include "mir/util/Exceptions.h"
#include "mir/util/Log.h"
#include "mir/util/MeshGeneratorParameters.h"


namespace mir {
namespace repres {
namespace other {


HEALPix::HEALPix(const param::MIRParametrisation&) {
    NOTIMP;
}


const atlas::Grid& HEALPix::atlasGridRef() const {
    return grid_ ? grid_ : (grid_ = atlas::Grid({/*spec_*/}));
}


bool HEALPix::sameAs(const Representation& other) const {
    const auto* o = dynamic_cast<const HEALPix*>(&other);
    return (o != nullptr);  // FIXME && spec_.getString("uid") == o->spec_.getString("uid");
}


void HEALPix::validate(const MIRValuesVector& values) const {
    size_t count = numberOfPoints();

    Log::debug() << "HEALPix::validate checked " << Log::Pretty(values.size(), {"value"}) << ", iterator counts "
                 << Log::Pretty(count) << "." << std::endl;

    ASSERT_VALUES_SIZE_EQ_ITERATOR_COUNT("HEALPix", values.size(), count);
}


size_t HEALPix::numberOfPoints() const {
    return static_cast<size_t>(atlasGridRef().size());
}


void HEALPix::makeName(std::ostream& out) const {
    NOTIMP;
#if 0
    const auto* sep = "";
    for (const auto& key : grib_keys) {
        out << sep << spec_.getString(key.first);
        sep = "_";
    }
#endif
}


void HEALPix::fill(grib_info& info) const {
    NOTIMP;
}


void HEALPix::fill(util::MeshGeneratorParameters& params) const {
    if (params.meshGenerator_.empty()) {
        params.meshGenerator_ = "HEALPix";
    }
}


Iterator* HEALPix::iterator() const {
    class HEALPixIterator : public Iterator {
        ::atlas::Grid grid_;  // Note: needs the object because IterateLonLat uses a Grid reference
        ::atlas::Grid::IterateLonLat lonlat_;

        decltype(lonlat_)::iterator it_;
        decltype(lonlat_)::iterator::value_type p_;

        const size_t total_;
        size_t count_;
        bool first_;

        void print(std::ostream& out) const override {
            out << "HEALPixIterator[";
            Iterator::print(out);
            out << ",count=" << count_ << ",total=" << total_ << "]";
        }

        bool next(Latitude& _lat, Longitude& _lon) override {
            if (it_.next(p_)) {
                point_[0] = p_.lat();
                point_[1] = p_.lon();
                _lat      = p_.lat();
                _lon      = p_.lon();

                if (first_) {
                    first_ = false;
                }
                else {
                    count_++;
                }

                return true;
            }

            ASSERT(count_ == total_);
            return false;
        }

        size_t index() const override { return count_; }

    public:
        HEALPixIterator(::atlas::Grid grid) :
            grid_(grid),
            lonlat_(grid.lonlat()),
            it_(lonlat_.begin()),
            total_(size_t(grid.size())),
            count_(0),
            first_(true) {}

        ~HEALPixIterator() override = default;

        HEALPixIterator(const HEALPixIterator&) = delete;
        HEALPixIterator(HEALPixIterator&&)      = delete;
        HEALPixIterator& operator=(const HEALPixIterator&) = delete;
        HEALPixIterator& operator=(HEALPixIterator&&) = delete;
    };
    return new HEALPixIterator(atlasGridRef());
}


atlas::Grid HEALPix::atlasGrid() const {
    return atlasGridRef();
}


void HEALPix::print(std::ostream& out) const {
    out << "HEALPix[spec=" << 0. /*spec_*/ << "]";
}


}  // namespace other
}  // namespace repres
}  // namespace mir
