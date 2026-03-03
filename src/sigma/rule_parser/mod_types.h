#pragma once 
#include  <cstdint>

namespace sigma {
    enum class Modifier : int32_t {
        All = 0,
        StartsWith,
        EndsWith,
        Contains,
        Exists,
        Cased,
        Neq,
        WinDash,
        Re,//
        I, //
        M, //
        S, //
        Base64,
        Base64Offset,
        Utf16Le,
        Utf16Be,
        Utf16,
        Wide,
        Lt,
        Lte,
        Gt,
        Gte,
        Minute,  //
        Hour,    //
        Day,     //
        Week,    //
        Month,   //
        Year,    //
        Cidr
        //Expand,
        //Fieldref
    };

    // https://github.com/SigmaHQ/sigma-specification/blob/v2.1.0/specification/sigma-appendix-modifiers.md
    enum class ModType {
        Generic = 0,
        String,
        Regular,
        RegularSub,
        Encoding,
        Numeric,
        Time,
        Ip,
        Specific,
    };
}