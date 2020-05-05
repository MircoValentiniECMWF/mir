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


#include "TenMinutesMask.h"

#include <cstdio>
#include <memory>

#include "eckit/exception/Exceptions.h"
#include "eckit/io/StdFile.h"
#include "eckit/thread/AutoLock.h"
#include "eckit/thread/Mutex.h"
#include "eckit/utils/MD5.h"

#include "mir/config/LibMir.h"
#include "mir/repres/Iterator.h"
#include "mir/repres/Representation.h"


static eckit::Mutex local_mutex;
static std::vector<std::vector<bool> > ten_minutes_;


namespace mir {
namespace lsm {


/*
From EMOSLIB:
     The 10 minute land-sea mask file contains 1080 lines of 2160
     values with each value stored as one bit. (Each line of latitude
     is packed in 68*32 = 2176 bits). Each value consists
     of a 0 for sea or a 1 for land. The bits are assumed to be in
     the centre of a 10 minute square (e.g. the first value is
     assumed to be at 0  5' East and 89  55' North).

    ~mir-data/share/mir/masks/lsm.10min.mask is a copy of ~emos/tables/interpolation/lsm_32_lsm10m01
*/
TenMinutesMask::TenMinutesMask(const std::string& name, const eckit::PathName& path, const param::MIRParametrisation&,
                               const repres::Representation& representation, const std::string&) :
    name_(name),
    path_(path) {

    const size_t ROWS = 1080;
    const size_t COLS = 2160;

    if (ten_minutes_.empty()) {

        eckit::TraceTimer<LibMir> timer("Load 10 minutes LSM");
        eckit::AutoLock<eckit::Mutex> lock(local_mutex);
        eckit::Log::debug<LibMir>() << "TenMinutesMask loading " << path_ << std::endl;

        eckit::AutoStdFile file(path_);
        ten_minutes_.resize(ROWS);

        size_t bytes = (((COLS + 31) / 32) * 32) / 8;
        unsigned char c;

        for (size_t i = 0; i < ROWS; i++) {
            size_t k             = 0;
            std::vector<bool>& v = ten_minutes_[i] = std::vector<bool>(COLS);
            for (size_t j = 0; j < bytes; j++) {
                ASSERT(std::fread(&c, 1, 1, file) == 1);
                for (size_t b = 0; b < 8 && k < COLS; b++) {
                    v[k++] = ((c >> (7 - b)) & 0x1) != 0;
                }
            }
        }
    }

    eckit::TraceTimer<LibMir> timer("Extract point from 10 minutes LSM");


    // NOTE: this is not using 3D coordinate systems
    // mask_.reserve(grid.size());

    std::unique_ptr<repres::Iterator> iter(representation.iterator());
    while (iter->next()) {
        const auto& p = iter->pointUnrotated();
        Latitude lat  = p.lat();
        Longitude lon = p.lon().normalise(Longitude::GREENWICH);

        ASSERT(lat >= Latitude::SOUTH_POLE);
        ASSERT(lat <= Latitude::NORTH_POLE);

        auto row = int((Latitude::NORTH_POLE - lat).value() * (ROWS - 1) / Latitude::GLOBE.value());
        ASSERT(0 <= row && row < int(ROWS));

        auto col = int(lon.value() * COLS / Longitude::GLOBE.value());
        ASSERT(0 <= col && col < int(COLS));

        mask_.push_back(ten_minutes_[size_t(row)][size_t(col)]);
    }
}


TenMinutesMask::~TenMinutesMask() = default;


bool TenMinutesMask::active() const {
    return true;
}


bool TenMinutesMask::cacheable() const {
    return true;
}


void TenMinutesMask::hash(eckit::MD5& md5) const {
    Mask::hash(md5);
    md5.add(path_.asString());
}


void TenMinutesMask::print(std::ostream& out) const {
    out << "TenMinutesMask[path=" << path_ << "]";
}


const std::vector<bool>& TenMinutesMask::mask() const {
    return mask_;
}

std::string TenMinutesMask::cacheName() const {
    return name_;
}

}  // namespace lsm
}  // namespace mir
