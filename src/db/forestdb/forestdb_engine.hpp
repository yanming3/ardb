/*
 *Copyright (c) 2013-2016, yinqiwen <yinqiwen@gmail.com>
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of Redis nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 *BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FORESTDB_ENGINE_HPP_
#define FORESTDB_ENGINE_HPP_

#include "libforestdb/forestdb.h"
#include "db/engine.hpp"
#include "channel/all_includes.hpp"
#include "util/config_helper.hpp"
#include "thread/thread_local.hpp"
#include "thread/thread_mutex_lock.hpp"
#include "thread/spin_rwlock.hpp"
#include "thread/event_condition.hpp"
#include "util/concurrent_queue.hpp"
#include <stack>

namespace ardb
{
    class ForestDBEngine;
    class ForestDBIterator: public Iterator
    {
        private:
            ForestDBEngine *m_engine;
            fdb_iterator* m_iter;
            fdb_doc* m_raw;
            Data m_ns;
            KeyObject m_key;
            ValueObject m_value;
            bool m_valid;

            void DoJump(const KeyObject& next);

            bool Valid();
            void Next();
            void Prev();
            void Jump(const KeyObject& next);
            void JumpToFirst();
            void JumpToLast();
            KeyObject& Key(bool clone_str);
            ValueObject& Value(bool clone_str);
            Slice RawKey();
            Slice RawValue();

            void SetIterator(fdb_iterator *cursor)
            {
                m_iter = cursor;
            }
            void ClearState();
            void CheckBound();
            friend class ForestDBEngine;
        public:
            ForestDBIterator(ForestDBEngine * e, const Data& ns) :
                    m_engine(e), m_iter(NULL), m_raw(NULL), m_ns(ns), m_valid(true)
            {
            }
            void MarkValid(bool valid)
            {
                m_valid = valid;
            }
            ~ForestDBIterator();
    };
    class ForestDBLocalContext;
    class ForestDBEngine: public Engine
    {
        private:
//            fdb_file_handle* m_meta_db;
//            fdb_kvs_handle* m_meta_kv;
//            DataSet m_nss;
//            SpinRWLock m_lock;
            fdb_kvs_handle* GetKVStore(Context& ctx, const Data& name, bool create_if_noexist);
//            void AddNamespace(const Data& ns);
//            int ListNameSpaces(DataArray& nss);
            friend class ForestDBLocalContext;
        public:
            ForestDBEngine();
            ~ForestDBEngine();
            int Init(const std::string& dir, const Properties& props);
            int Put(Context& ctx, const KeyObject& key, const ValueObject& value);
            int PutRaw(Context& ctx, const Data& ns, const Slice& key, const Slice& value);
            int Get(Context& ctx, const KeyObject& key, ValueObject& value);
            int MultiGet(Context& ctx, const KeyObjectArray& keys, ValueObjectArray& values, ErrCodeArray& errs);
            int Del(Context& ctx, const KeyObject& key);
            int Merge(Context& ctx, const KeyObject& key, uint16_t op, const DataArray& args);bool Exists(Context& ctx, const KeyObject& key);
            int BeginTransaction();
            int CommitTransaction();
            int DiscardTransaction();
            int Compact(Context& ctx, const KeyObject& start, const KeyObject& end);
            int ListNameSpaces(Context& ctx, DataArray& nss);
            int DropNameSpace(Context& ctx, const Data& ns);
            void Stats(Context& ctx, std::string& str);
            int64_t EstimateKeysNum(Context& ctx, const Data& ns);
            Iterator* Find(Context& ctx, const KeyObject& key);
            const FeatureSet GetFeatureSet()
            {
                FeatureSet features;
                features.support_compactilter = 1;
                features.support_namespace = 1;
                return features;
            }

    };
}

#endif /* LMDB_ENGINE_HPP_ */