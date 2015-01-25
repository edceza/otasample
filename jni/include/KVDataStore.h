/*
    Copyright (c) 2014 Audioneex.com.
    Copyright (c) 2014 Alberto Gramaglia.
    All rights reserved.
	
	This source code is part of the Audioneex Software Development Kit and is
	subject to the terms and conditions stated in the accompanying license.
	Please refer to the Audioneex license document provided with the package
	at www.audioneex.com for more information.

	Author: Alberto Gramaglia
	
*/


#ifndef DATASTORETYPES_H
#define DATASTORETYPES_H

#include <cstdint>
#include <vector>
#include <memory>
#include <boost/unordered_map.hpp>

#include "audioneex.h"

typedef boost::unordered::unordered_map< int, std::vector<uint8_t> > block_map;
struct DBInfo_t;


/// This interface extends the functionality of the DataStore interface by
/// adding basic database operations and some other application-specific
/// functionality. In the examples we use key-value datastores, so we call
/// this interface accordingly.

class KVDataStore : public Audioneex::DataStore
{
public:

    typedef std::unique_ptr<KVDataStore> Ptr;

    enum eOperation{
        GET,
        BUILD,
        BUILD_MERGE
    };

    virtual ~KVDataStore(){}

    /// Open the datastore. This will open all database/collections
    /// used by the identification system.
    virtual void Open(eOperation op = GET,
                      bool use_fing_db=true,
                      bool use_meta_db=false,
                      bool use_info_db=false) = 0;

    /// Close the datastore. This will close all database/collections
    /// used by the identification engine
    virtual void Close() = 0;

    /// Set the URL where all database will be located.
    virtual void SetDatabaseURL(const std::string &url) = 0;

    /// Get the URL where all database are located
    virtual std::string GetDatabaseURL()  = 0;

    /// Chech whether the datastore is empty
    virtual bool Empty() = 0;

    /// Clear the datastore
    virtual void Clear() = 0;

    /// Query for open status
    virtual bool IsOpen() = 0;

    /// Get the number of fingerprints in the data store
    virtual size_t GetFingerprintsCount() = 0;

    /// Save a fingerprint in the datastore
    virtual void PutFingerprint(uint32_t FID, const uint8_t* data, size_t size) = 0;

    /// Write metadata associated to a fingerprint
    virtual void PutMetadata(uint32_t FID, const std::string& meta) = 0;

    /// Get metadata associated to a fingerprint
    virtual std::string GetMetadata(uint32_t FID) = 0;

    /// Save datastore info
    virtual void PutInfo(const DBInfo_t& info) = 0;

    /// Get datastore info
    virtual DBInfo_t GetInfo() = 0;

    /// Get operation mode
    virtual eOperation GetOpMode() = 0;

    /// Set operation mode
    virtual void SetOpMode(eOperation mode) = 0;

};


// ----------------------------------------------------------------------------


/// Data store info record
struct DBInfo_t{
    int MatchType;
};

/// Database open modes
enum{
    OPEN_READ,
    OPEN_WRITE,
    OPEN_READ_WRITE
};


/// Convenience structure to manipulate index list blocks
struct PListBlock
{
    Audioneex::PListHeader* ListHeader;
    Audioneex::PListBlockHeader* Header;
    uint8_t* Body;
    size_t BodySize;
};

struct BlockCache
{
    int list_id;       // List to which the blocks belong
    size_t accum;      // General-purpose accumulator
    block_map buffer;  // Blocks buffer

    BlockCache() : list_id(0), accum(0) {}
};

/// Convenience function to check for emptiness
inline bool IsNull(const PListBlock& hdr){
    return hdr.ListHeader==nullptr &&
           hdr.Header==nullptr &&
           hdr.Body==nullptr;
}


#endif

