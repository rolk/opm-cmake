/*
  Copyright (c) 2018 Statoil ASA

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <opm/output/eclipse/DoubHEAD.hpp>

// Note: DynamicState.hpp and <map> are prerequisites of Tuning.hpp
#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/ScheduleEnums.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Schedule.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Tuning.hpp>
#include <opm/parser/eclipse/Units/Units.hpp>

#include <opm/output/eclipse/InteHEAD.hpp> // Opm::RestartIO::makeUTCTime()

#include <chrono>
#include <cmath>
#include <ctime>
#include <iterator>
#include <map>
#include <numeric>
#include <ratio>
#include <utility>
#include <vector>

enum Index : std::vector<double>::size_type {
    // 0..9
    SimTime =   0,
    TsInit  =   1,
    TsMaxz  =   2,
    TsMinz  =   3,
    TsMchp  =   4,
    TsFMax  =   5,
    TsFMin  =   6,
    TsFcnv  =   7,
    TrgTTE  =   8,
    TrgCNV  =   9,

    // 10..19
    TrgMBE  =  10,
    TrgLCV  =  11,
    dh_012  =  12,
    dh_013  =  13,
    dh_014  =  14,
    dh_015  =  15,
    XxxTTE  =  16,
    XxxCNV  =  17,
    XxxMBE  =  18,
    XxxLCV  =  19,

    // 20..29
    XxxWFL  =  20,
    dh_021  =  21,
    dh_022  =  22,
    dh_023  =  23,
    dh_024  =  24,
    dRsdt   =  25,
    dh_026  =  26,
    dh_027  =  27,
    dh_028  =  28,
    dh_029  =  29,

    // 30..39
    dh_030  =  30,
    dh_031  =  31,
    dh_032  =  32,
    dh_033  =  33,
    dh_034  =  34,
    dh_035  =  35,
    dh_036  =  36,
    dh_037  =  37,
    dh_038  =  38,
    dh_039  =  39,

    // 40..49
    dh_040  =  40,
    dh_041  =  41,
    dh_042  =  42,
    dh_043  =  43,
    dh_044  =  44,
    dh_045  =  45,
    dh_046  =  46,
    dh_047  =  47,
    dh_048  =  48,
    dh_049  =  49,

    // 50..59
    dh_050  =  50,
    dh_051  =  51,
    dh_052  =  52,
    dh_053  =  53,
    dh_054  =  54,
    dh_055  =  55,
    dh_056  =  56,
    dh_057  =  57,
    dh_058  =  58,
    dh_059  =  59,

    // 60..69
    dh_060  =  60,
    dh_061  =  61,
    dh_062  =  62,
    dh_063  =  63,
    dh_064  =  64,
    dh_065  =  65,
    dh_066  =  66,
    dh_067  =  67,
    dh_068  =  68,
    dh_069  =  69,

    // 70..79
    dh_070  =  70,
    dh_071  =  71,
    dh_072  =  72,
    dh_073  =  73,
    dh_074  =  74,
    dh_075  =  75,
    dh_076  =  76,
    dh_077  =  77,
    dh_078  =  78,
    dh_079  =  79,

    // 80..89
    dh_080  =  80,
    dh_081  =  81,
    TrgDPR  =  82,
    TfDIFF  =  83,
    DdpLim  =  84,
    DdsLim  =  85,
    dh_086  =  86,
    dh_087  =  87,
    dh_088  =  88,
    dh_089  =  89,

    // 90..99
    dh_090  =  90,
    dh_091  =  91,
    dh_092  =  92,
    dh_093  =  93,
    dh_094  =  94,
    dh_095  =  95,
    dh_096  =  96,
    dh_097  =  97,
    dh_098  =  98,
    ThrUPT  =  99,

    // 100..109
    XxxDPR  = 100,
    TrgFIP  = 101,
    TrgSFT  = 102,
    dh_103  = 103,
    dh_104  = 104,
    dh_105  = 105,
    dh_106  = 106,
    dh_107  = 107,
    dh_108  = 108,
    dh_109  = 109,

    // 110..119
    dh_110  = 110,
    dh_111  = 111,
    dh_112  = 112,
    dh_113  = 113,
    dh_114  = 114,
    dh_115  = 115,
    dh_116  = 116,
    dh_117  = 117,
    dh_118  = 118,
    dh_119  = 119,

    // 120..129
    dh_120  = 120,
    dh_121  = 121,
    dh_122  = 122,
    dh_123  = 123,
    dh_124  = 124,
    dh_125  = 125,
    dh_126  = 126,
    dh_127  = 127,
    dh_128  = 128,
    dh_129  = 129,

    // 130..139
    dh_130  = 130,
    dh_131  = 131,
    dh_132  = 132,
    dh_133  = 133,
    dh_134  = 134,
    dh_135  = 135,
    dh_136  = 136,
    dh_137  = 137,
    dh_138  = 138,
    dh_139  = 139,

    // 140..149
    dh_140  = 140,
    dh_141  = 141,
    dh_142  = 142,
    dh_143  = 143,
    dh_144  = 144,
    dh_145  = 145,
    dh_146  = 146,
    dh_147  = 147,
    dh_148  = 148,
    dh_149  = 149,

    // 150..159
    dh_150  = 150,
    dh_151  = 151,
    dh_152  = 152,
    dh_153  = 153,
    dh_154  = 154,
    dh_155  = 155,
    dh_156  = 156,
    dh_157  = 157,
    dh_158  = 158,
    dh_159  = 159,

    // 160..169
    Start   = 160,
    Time    = 161,
    dh_162  = 162,
    dh_163  = 163,
    dh_164  = 164,
    dh_165  = 165,
    dh_166  = 166,
    dh_167  = 167,
    dh_168  = 168,
    dh_169  = 169,

    // 170..179
    dh_170  = 170,
    dh_171  = 171,
    dh_172  = 172,
    dh_173  = 173,
    dh_174  = 174,
    dh_175  = 175,
    dh_176  = 176,
    dh_177  = 177,
    dh_178  = 178,
    dh_179  = 179,

    // 180..189
    dh_180  = 180,
    dh_181  = 181,
    dh_182  = 182,
    dh_183  = 183,
    dh_184  = 184,
    dh_185  = 185,
    dh_186  = 186,
    dh_187  = 187,
    dh_188  = 188,
    dh_189  = 189,

    // 190..199
    dh_190  = 190,
    dh_191  = 191,
    dh_192  = 192,
    dh_193  = 193,
    dh_194  = 194,
    dh_195  = 195,
    dh_196  = 196,
    dh_197  = 197,
    dh_198  = 198,
    dh_199  = 199,

    // 200..209
    dh_200  = 200,
    dh_201  = 201,
    dh_202  = 202,
    dh_203  = 203,
    dh_204  = 204,
    dh_205  = 205,
    dh_206  = 206,
    dh_207  = 207,
    dh_208  = 208,
    dh_209  = 209,

    // 210..219
    dh_210  = 210,
    dh_211  = 211,
    dh_212  = 212,
    dh_213  = 213,
    dh_214  = 214,
    dh_215  = 215,
    dh_216  = 216,
    dh_217  = 217,
    dh_218  = 218,
    dh_219  = 219,

    // 220..227
    dh_220  = 220,
    dh_221  = 221,
    dh_222  = 222,
    dh_223  = 223,
    dh_224  = 224,
    dh_225  = 225,
    dh_226  = 226,
    dh_227  = 227,
    dh_228  = 228,

// ---------------------------------------------------------------------
// ---------------------------------------------------------------------

    NUMBER_OF_ITEMS        // MUST be last element of enum.
};

namespace {
    /// Convert std::tm{} to Date-Number.
    ///
    /// ECL Restrictions:
    ///   - Calendar start: 1st January 0000
    ///   - Year length: 365.25 days
    ///   - No special leap year handling
    ///
    /// \param[in] year tm::tm_year of date (years since 1900).
    ///
    /// \param[in] yday tm::tm_yday of date (days since 1st January).
    ///
    /// \return Date-number corresponding to specified day-of-year in
    ///    specified year, subject to restrictions outlined above.
    double toDateNum(const int year, const int yday)
    {
        return std::floor(365.25 * (year + 1900))
            + (yday + 1); // Day of year [1 .. 365]
    }

    double toDateNum(const std::chrono::time_point<std::chrono::system_clock> tp)
    {
        const auto t0  =  std::chrono::system_clock::to_time_t(tp);
        const auto tm0 = *std::gmtime(&t0);

        // Set clock to 01:00:00+0000 on 2001-<month>-<day> to get
        // "accurate" day-of-year calculation (no leap year, no DST offset,
        // not previous day reported).
        auto tm1 = std::tm{};
        tm1.tm_year = 101;      // 2001
        tm1.tm_mon  = tm0.tm_mon;
        tm1.tm_mday = tm0.tm_mday;

        tm1.tm_hour  = 1;
        tm1.tm_min   = 0;
        tm1.tm_sec   = 0;
        tm1.tm_isdst = 0;

        const auto t1 = ::Opm::RestartIO::makeUTCTime(tm1);

        if (t1 != static_cast<std::time_t>(-1)) {
            tm1 = *std::gmtime(&t1); // Get new tm_yday.
            return toDateNum(tm0.tm_year, tm1.tm_yday);
        }

        // Failed to convert tm1 to time_t (unexpected).  Use initial value.
        return toDateNum(tm0.tm_year, tm0.tm_yday);
    }
}

// =====================================================================
// Public Interface (DoubHEAD member functions) Below Separator
// ---------------------------------------------------------------------

Opm::RestartIO::DoubHEAD::DoubHEAD()
    : data_(Index::NUMBER_OF_ITEMS, 0.0)
    //: data_(Index::NUMBER_OF_ITEMS, -1.0e20)
{
    // Numbers below have unknown usage, values have been determined by
    // experiments to be constant across a range of reference cases.
    this->data_[Index::dh_024] = 1.0e+20;
    this->data_[Index::dh_026] = 0.0;
    this->data_[Index::dh_027] = 0.0;
    this->data_[Index::dh_028] = 0.0;
    this->data_[Index::dh_050] = 0.01;
    this->data_[Index::dh_054] = 1.0e+20;
    this->data_[Index::dh_055] = 1.0e+20;
    this->data_[Index::dh_063] = 1.0e+20;
    this->data_[Index::dh_064] = 1.0e+20;
    this->data_[Index::dh_065] = 0.0;
    this->data_[Index::dh_066] = 0.0;
    this->data_[Index::dh_069] = -1.0;
    this->data_[Index::dh_080] = 1.0e+20;
    this->data_[Index::dh_081] = 1.0e+20;
    this->data_[Index::dh_091] = 0.0;
    this->data_[Index::dh_092] = 0.0;
    this->data_[Index::dh_093] = 0.0;
    this->data_[Index::dh_096] = 0.0;
    this->data_[Index::dh_105] = 1.0;
    this->data_[Index::dh_108] = 0.0;
    this->data_[Index::dh_109] = 0.0;
    this->data_[Index::dh_110] = 0.0;

    this->data_[Index::dh_123] = 365.0;
    this->data_[Index::dh_124] = 0.1;
    this->data_[Index::dh_125] = 0.15;
    this->data_[Index::dh_126] = 3.0;
    this->data_[Index::dh_127] = 0.3;
    this->data_[Index::dh_128] = 0.1;
    this->data_[Index::dh_129] = 0.1;

    this->data_[Index::dh_130] = 0.001;
    this->data_[Index::dh_131] = 1.0e-7;
    this->data_[Index::dh_132] = 1.0e-4;
    this->data_[Index::dh_133] = 10.0;
    this->data_[Index::dh_134] = 0.01;
    this->data_[Index::dh_135] = 1.0e-6;
    this->data_[Index::dh_136] = 0.001;
    this->data_[Index::dh_137] = 0.001;

    this->data_[Index::dh_140] = 1.0e-20;   // check this value
    this->data_[Index::dh_141] = 1.013;
    this->data_[Index::dh_142] = 0.0;
    this->data_[Index::dh_143] = 1.0;
    this->data_[Index::dh_145] = 0.3;
    this->data_[Index::dh_146] = 2.0;
    this->data_[Index::dh_147] = 0.0;
    this->data_[Index::dh_148] = 0.0;
    this->data_[Index::dh_149] = 0.0;

    this->data_[Index::dh_150] = 0.0;
    this->data_[Index::dh_151] = 0.0;
    this->data_[Index::dh_152] = 0.0;
    this->data_[Index::dh_153] = 0.0;
    this->data_[Index::dh_154] = 0.0;

    this->data_[Index::dh_162] = 1.0;
    this->data_[Index::dh_163] = 0.2;
    this->data_[Index::dh_164] = 0.4;
    this->data_[Index::dh_165] = 1.2;
    this->data_[Index::dh_166] = 0.3;
    this->data_[Index::dh_167] = 1.0;

    this->data_[Index::dh_170] = 0.4;
    this->data_[Index::dh_171] = 0.7;
    this->data_[Index::dh_172] = 2.0;
    this->data_[Index::dh_178] = 1.0;
    this->data_[Index::dh_179] = 1.0;

    this->data_[Index::dh_180] = 1.0;
    this->data_[Index::dh_181] = 0.0;
    this->data_[Index::dh_182] = 0.0;
    this->data_[Index::dh_183] = 1.0;
    this->data_[Index::dh_184] = 1.0e-4;
    this->data_[Index::dh_185] = 0.0;
    this->data_[Index::dh_186] = 0.0;
    this->data_[Index::dh_187] = 1.0e+20;
    this->data_[Index::dh_188] = 1.0e+20;
    this->data_[Index::dh_189] = 1.0e+20;

    this->data_[Index::dh_190] = 1.0e+20;
    this->data_[Index::dh_191] = 1.0e+20;
    this->data_[Index::dh_192] = 1.0e+20;
    this->data_[Index::dh_193] = 1.0e+20;
    this->data_[Index::dh_194] = 1.0e+20;
    this->data_[Index::dh_195] = 1.0e+20;
    this->data_[Index::dh_196] = 1.0e+20;
    this->data_[Index::dh_197] = 1.0e+20;
    this->data_[Index::dh_198] = 1.0e+20;
    this->data_[Index::dh_199] = 1.0;

    this->data_[Index::dh_200] = 0.0;
    this->data_[Index::dh_201] = 0.0;
    this->data_[Index::dh_202] = 0.0;
    this->data_[Index::dh_203] = 0.0;
    this->data_[Index::dh_204] = 0.0;
    this->data_[Index::dh_205] = 0.0;
    this->data_[Index::dh_206] = 0.0;
    this->data_[Index::dh_207] = 0.0;
    this->data_[Index::dh_208] = 0.0;
    this->data_[Index::dh_209] = 0.0;

    this->data_[Index::dh_210] = 0.0;
    this->data_[Index::dh_211] = 0.0;
    this->data_[Index::dh_214] = 1.0e-4;
    this->data_[Index::dh_215] = -2.0e+20;
    this->data_[Index::dh_217] = 0.0;
    this->data_[Index::dh_218] = 0.0;
    this->data_[Index::dh_219] = 0.0;

    this->data_[Index::dh_220] = 0.01;
    this->data_[Index::dh_221] = 1.0;
    this->data_[Index::dh_222] = 0.0;
    this->data_[Index::dh_223] = 1.0e+20;
    this->data_[Index::dh_225] = 0.0;
    this->data_[Index::dh_226] = 0.0;
    this->data_[Index::dh_227] = 0.0;

    this->data_[Index::TsInit]  = 1.0;
    this->data_[Index::TsMaxz]  = 365.0;
    this->data_[Index::TsMinz]  = 0.1;
    this->data_[Index::TsMchp]  = 0.15;
    this->data_[Index::TsFMax]  = 3.0;
    this->data_[Index::TsFMin]  = 0.3;
    this->data_[Index::TsFcnv]  = 0.1;
    this->data_[Index::TrgTTE]  = 0.1;
    this->data_[Index::TrgCNV]  = 0.001;
    this->data_[Index::TrgMBE]  = 1.0E-7;
    this->data_[Index::TrgLCV]  = 0.0001;

    this->data_[Index::XxxTTE] = 10.0;
    this->data_[Index::XxxCNV] = 0.01;
    this->data_[Index::XxxMBE] = 1.0e-6;
    this->data_[Index::XxxLCV] = 0.001;
    this->data_[Index::XxxWFL] = 0.001;

    this->data_[Index::dRsdt]  = 1.0e+20; // "Infinity"

    this->data_[Index::TrgDPR] = 1.0e+6;
    this->data_[Index::TfDIFF] = 1.25;
    this->data_[Index::DdpLim] = 1.0e+6;
    this->data_[Index::DdsLim] = 1.0e+6;


    this->data_[Index::ThrUPT] = 1.0e+20;
    this->data_[Index::XxxDPR] = 1.0e+20;
    this->data_[Index::TrgFIP] = 0.025;
    this->data_[Index::TrgSFT] = 1.0e+20;
}

Opm::RestartIO::DoubHEAD&
Opm::RestartIO::DoubHEAD::tuningParameters(const Tuning&     tuning,
                                           const std::size_t lookup_step,
                                           const double      cnvT)
{
    // Record 1
    this->data_[Index::TsInit] = tuning.getTSINIT(lookup_step) / cnvT;
    this->data_[Index::TsMaxz] = tuning.getTSMAXZ(lookup_step) / cnvT;
    this->data_[Index::TsMinz] = tuning.getTSMINZ(lookup_step) / cnvT;
    this->data_[Index::TsMchp] = tuning.getTSMCHP(lookup_step) / cnvT;
    this->data_[Index::TsFMax] = tuning.getTSFMAX(lookup_step);
    this->data_[Index::TsFMin] = tuning.getTSFMIN(lookup_step);
    this->data_[Index::TsFcnv] = tuning.getTSFCNV(lookup_step);
    this->data_[Index::ThrUPT] = tuning.getTHRUPT(lookup_step);
    this->data_[Index::TfDIFF] = tuning.getTFDIFF(lookup_step);

    // Record 2
    this->data_[Index::TrgTTE] = tuning.getTRGTTE(lookup_step);
    this->data_[Index::TrgCNV] = tuning.getTRGCNV(lookup_step);
    this->data_[Index::TrgMBE] = tuning.getTRGMBE(lookup_step);
    this->data_[Index::TrgLCV] = tuning.getTRGLCV(lookup_step);
    this->data_[Index::XxxTTE] = tuning.getXXXTTE(lookup_step);
    this->data_[Index::XxxCNV] = tuning.getXXXCNV(lookup_step);
    this->data_[Index::XxxMBE] = tuning.getXXXMBE(lookup_step);
    this->data_[Index::XxxLCV] = tuning.getXXXLCV(lookup_step);
    this->data_[Index::XxxWFL] = tuning.getXXXWFL(lookup_step);
    this->data_[Index::TrgFIP] = tuning.getTRGFIP(lookup_step);
    this->data_[Index::TrgSFT] = tuning.getTRGSFT(lookup_step);

    // Record 3
    this->data_[Index::TrgDPR] = tuning.getTRGDPR(lookup_step);
    this->data_[Index::XxxDPR] = tuning.getXXXDPR(lookup_step);
    this->data_[Index::DdpLim] = tuning.getDDPLIM(lookup_step);
    this->data_[Index::DdsLim] = tuning.getDDSLIM(lookup_step);

    return *this;
}

Opm::RestartIO::DoubHEAD&
Opm::RestartIO::DoubHEAD::timeStamp(const TimeStamp& ts)
{
    using day = std::chrono::duration<double,
        std::ratio_multiply<std::chrono::hours::period, std::ratio<24>>
    >;

    // Elapsed time in days
    this->data_[Index::SimTime] = day{ ts.elapsed }.count();

    // Start time in date-numbers
    this->data_[Index::Start] = toDateNum(ts.start);

    // Simulation time-stamp in date-numbers
    this->data_[Index::Time] = this->data_[Index::Start]
        + this->data_[Index::SimTime];

    return *this;
}

Opm::RestartIO::DoubHEAD&
Opm::RestartIO::DoubHEAD::drsdt(const Schedule&   sched,
                                const std::size_t lookup_step)
{
    const auto& vappar =
        sched.getOilVaporizationProperties(lookup_step);

    this->data_[dRsdt] =
        (vappar.getType() == Opm::OilVaporizationEnum::DRSDT)
        ? vappar.getMaxDRSDT()
        : 1.0e+20;

    return *this;
}
