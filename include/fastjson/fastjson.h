/*
 * Copyright (c) 2011, Run With Robots
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the fastjson library nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY RUN WITH ROBOTS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL MICHAEL ANDERSON BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FASTJSON_H
#define FASTJSON_H

#include <stdint.h>
#include <string>

#include <vector>

#include "fastjson/core.h"

//TODO: Remove this when we have better error handling.
#include <iostream>
#include <string.h>

namespace fastjson
{
  struct JsonElementCount
  {
    public:
      JsonElementCount() :
        arrays(0),
        dicts(0),
        strings(0),
        total_string_length(0)
    {
      context.reserve(5);
      context.push_back(Root);
      memset( counts, 0, sizeof(counts) );
    }


      void start_array()  { context.push_back( InArray ); }
      void start_dict()   { context.push_back( InDict );  }
      void start_string() {};
      void start_number() {};

      void on_true()  { counts[ context.back() ]++; };
      void on_false() { counts[ context.back() ]++; };
      void on_null()  { counts[ context.back() ]++; };

      void end_array() { ++arrays; context.pop_back(); counts[ context.back() ]++; }
      void end_dict() { ++dicts; context.pop_back(); counts[ context.back() ]++; }

      void end_string() { ++strings; counts[ context.back() ]++; } //Should this be changed to be the same as end_number?
      void string_add_ubyte( const unsigned char ) { total_string_length++; }
      void end_number( const unsigned char * start, const unsigned char * end )
      {
        ++strings;
        counts[ context.back() ]++;
        total_string_length += ( end-start);
      };

      uint32_t n_arrays()  const { return arrays; }
      uint32_t n_dicts()   const { return dicts; }
      uint32_t n_strings() const { return strings; }
      uint32_t n_array_elements() const { return counts[InArray]; }
      uint32_t n_dict_elements() const { return counts[InDict]/2; }
      uint32_t n_string_length() const { return total_string_length; }

      void on_error( int errcode, const std::string & err, const unsigned char * start_context, const unsigned char * locn, const unsigned char * end_context )
      {
          std::cerr<<"OMG an error ["<<errcode<<"] : "<<err<<std::endl;
          std::cerr<<"It seems to have happened here..."<<std::endl;
          const unsigned char * ep = (locn+10<end_context)?locn+10:end_context;
          const unsigned char * sp = (locn-10>start_context)?locn-10:start_context;
          std::cerr<<std::string(sp, ep)<<std::endl;
          while( sp < locn ) { std::cerr<<' '; ++sp; }
          std::cerr<<"^"<<std::endl;
      }

    protected:
      uint32_t arrays;
      uint32_t dicts;
      uint32_t strings;
      uint32_t total_string_length;

      enum Context { Root, InArray, InDict, NContexts };
      uint32_t counts[NContexts];
      std::vector<Context> context;

    public:
  };

  bool count_elements( const std::string & json_str, JsonElementCount * count );
  bool count_elements( const unsigned char * start, const unsigned char * end, JsonElementCount * count );

  class Document
  {
    public:
    Token root;
    unsigned char * string_store;
    ArrayEntry * array_store;
    DictEntry * dict_store;
  };

  bool parse_doc( const unsigned char * start, const unsigned char * end, Document * doc );
  bool parse_doc( const std::string & json_str, Document * doc );

}

#endif
