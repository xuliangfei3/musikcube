//////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2007-2017 musikcube team
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may
//      be used to endorse or promote products derived from this software
//      without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <core/library/query/local/LocalQueryBase.h>
#include <core/library/query/local/util/CategoryQueryUtil.h>
#include <core/library/query/local/util/SdkWrappers.h>
#include <core/db/Statement.h>
#include <core/db/Connection.h>
#include <core/sdk/IValueList.h>
#include <core/support/Common.h>
#include <memory>

namespace musik { namespace core { namespace db { namespace local {

    class CategoryListQuery : public musik::core::db::LocalQueryBase {
        public:
            using Result = SdkValueList::Shared;

            CategoryListQuery(
                const std::string& trackField,
                const std::string& filter = "");

            CategoryListQuery(
                const std::string& trackField,
                const category::Predicate predicate,
                const std::string& filter = "");

            CategoryListQuery(
                const std::string& trackField,
                const category::PredicateList predicate,
                const std::string& filter = "");

            virtual ~CategoryListQuery();

            std::string Name() { return "CategoryListQuery"; }

            virtual Result GetResult();
            virtual int GetIndexOf(int64_t id);

            musik::core::sdk::IValueList* GetSdkResult();

        protected:
            virtual bool OnRun(musik::core::db::Connection &db);

        private:
            enum OutputType { Regular, Extended, Playlist };

            void QueryPlaylist(musik::core::db::Connection &db);
            void QueryRegular(musik::core::db::Connection &db);
            void QueryExtended(musik::core::db::Connection &db);
            void ProcessResult(musik::core::db::Statement &stmt);

            std::string trackField;
            std::string filter;
            OutputType outputType;
            category::PredicateList regular, extended;
            Result result;
    };

} } } }
