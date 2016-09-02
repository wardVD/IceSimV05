/**
 * Copyright (c) 2011, 2012
 * Claudio Kopper <claudio.kopper@icecube.wisc.edu>
 * and the IceCube Collaboration <http://www.icecube.wisc.edu>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * $Id: I3CLSimScalarFieldConstant.cxx 139192 2015-11-04 22:02:04Z claudio.kopper $
 *
 * @file I3CLSimScalarFieldConstant.cxx
 * @version $Revision: 139192 $
 * @date $Date: 2015-11-04 16:02:04 -0600 (Wed, 04 Nov 2015) $
 * @author Claudio Kopper
 */

#include <icetray/serialization.h>
#include <clsim/function/I3CLSimScalarFieldConstant.h>

#include <typeinfo>
#include <cmath>
#include <math.h>
#include <sstream>
#include <stdexcept>

#include "clsim/I3CLSimHelperToFloatString.h"
using namespace I3CLSimHelper;

I3CLSimScalarFieldConstant::
I3CLSimScalarFieldConstant(double value)
:
value_(value)
{
}

I3CLSimScalarFieldConstant::I3CLSimScalarFieldConstant() {;}

I3CLSimScalarFieldConstant::~I3CLSimScalarFieldConstant() 
{;}

bool I3CLSimScalarFieldConstant::HasNativeImplementation() const 
{
    return true;
}

double I3CLSimScalarFieldConstant::GetValue(double x, double y, double z) const
{
    return value_;
}

std::string I3CLSimScalarFieldConstant::GetOpenCLFunction(const std::string &functionName) const
{
    // the OpenCL interface takes a float4, but ignores the fourth component
    // (well.. since this is just a constant, the input is ignored altogether..)

    std::string funcHint =
    std::string("\n#define ") + functionName + "_IS_CONSTANT " + ToFloatString(value_) + "\n";

    std::string funcDef = 
    std::string("inline float ") + functionName + std::string("(float4 vec)\n");
    
    std::string funcBody = std::string() + 
    "{\n"
    "    return " + ToFloatString(value_) + ";\n"
    "}\n"
    ;
    
    return funcDef + ";\n\n" + funcHint + funcDef + funcBody;
}

bool I3CLSimScalarFieldConstant::CompareTo(const I3CLSimScalarField &other) const
{
    try
    {
        const I3CLSimScalarFieldConstant &other_ = dynamic_cast<const I3CLSimScalarFieldConstant &>(other);
        if ((other_.value_ != value_))
            return false;
        
        return true;
    }
    catch (const std::bad_cast& e)
    {
        // not of the same type, treat it as non-equal
        return false;
    }
    
}



template <class Archive>
void I3CLSimScalarFieldConstant::serialize(Archive &ar, unsigned version)
{
    if (version>i3clsimscalarfieldconstant_version_)
        log_fatal("Attempting to read version %u from file but running version %u of I3CLSimScalarFieldConstant class.",version,i3clsimscalarfieldconstant_version_);

    ar & make_nvp("I3CLSimScalarField", base_object<I3CLSimScalarField>(*this));
    ar & make_nvp("value", value_);
}     


I3_SERIALIZABLE(I3CLSimScalarFieldConstant);
