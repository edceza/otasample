/*
    Copyright (c) 2014 Audioneex.com.
    Copyright (c) 2014 Alberto Gramaglia.
    All rights reserved.
	
	This source code is part of the Audioneex Software Development Kit and is
	subject to the terms and conditions stated in the accompanying license.
	Please refer to the Audioneex license document provided with the package
	at www.audioneex.com for more information.

*/

#ifndef TCDATASTORE_H
#define TCDATASTORE_H

#include <tcabinet/tchdb.h>

#include "KVDataStore.h"

class TCDataStore;

/// Defines a key-value database/collection in the data store.
/// This is represented by a file in the datastore directory.

class TCCollection
{
protected:

    TCDataStore*   m_Datastore;
    TCHDB*         m_DBHandle;
    std::string    m_DBName;
    std::string    m_DBURL;
    bool           m_IsOpen;

    /// Internal buffer for read/write operations
    std::vector<uint8_t>   m_Buffer;

public:

    TCCollection(TCDataStore *datastore);
    virtual ~TCCollection();

    /// Set the database file name
    void SetName(const std::string &filename) { m_DBName = filename; }

    /// Get the database file name
    std::string GetName() const { return m_DBName; }

    /// Set the database URL
    void SetURL(const std::string &url) { m_DBURL = url; }

    /// Get the database URL
    std::string GetURL() const { return m_DBURL; }

    /// Open the databse
    void Open(int mode = OPEN_READ);

    /// Close the database
    void Close();

    /// Drop the database (all contents cleared)
    void Drop();

    /// Query open status
    bool IsOpen() const { return m_IsOpen; }

    /// Get the number of records in the database
    std::uint64_t GetRecordsCount() const;

    /// Merge this collection to the given one
    virtual void Merge(TCCollection*) {}

};

// ----------------------------------------------------------------------------

/// The fingerprints index

class TCIndex : public TCCollection
{
    BlockCache          m_BlocksCache;

public:

    TCIndex(TCDataStore *dstore);
    ~TCIndex(){}

    /// Get the header for the specified index list
    Audioneex::PListHeader GetPListHeader(int list_id);

    /// Get the header for the spcified block in the specified list
    Audioneex::PListBlockHeader GetPListBlockHeader(int list_id, int block_id);

    /// Read the specified index list block data into 'buffer'. The 'headers'
    /// flag specifies whether to include the block headers in the read data.
    /// Return the number of read bytes.
    size_t ReadBlock(int list_id, int block_id, std::vector<uint8_t> &buffer, bool headers=true);

    /// Write the contents of the given block in the specified index list.
    /// A new block is created if the specified block does not exist.
    void WriteBlock(int list_id, int block_id, std::vector<uint8_t> &buffer, size_t data_size);

    /// Append a chunk to the specified block. If the block does not exist,
    /// a new one is created.
    void AppendChunk(int list_id,
                     Audioneex::PListHeader &lhdr,
                     Audioneex::PListBlockHeader &hdr,
                     uint8_t* chunk, size_t chunk_size,
                     bool new_block=false);

    /// Update the specified list header
    void UpdateListHeader(int list_id, Audioneex::PListHeader &lhdr);

    /// Merge this index with the given index.
    void Merge(TCCollection *plidx);

    /// Turn a raw block byte stream into a block structure.
    PListBlock RawBlockToBlock(uint8_t *block, size_t block_size, bool isFirst=false);

    /// Flush any remaining data in the block cache
    void FlushBlockCache();

    void ClearCache();

};

// ----------------------------------------------------------------------------

/// The fingerprints database

class TCFingerprints : public TCCollection
{
public:

    TCFingerprints(TCDataStore *dstore);
    ~TCFingerprints(){}

    /// Read the size of the specified fingerprint (in bytes)
    size_t ReadFingerprintSize(uint32_t FID);

    /// Read the specified fingerprint's data into the given buffer. If 'size'
    /// is non zero, then 'size' bytes are read starting at offset bo (in bytes)
    size_t ReadFingerprint(uint32_t FID, std::vector<uint8_t> &buffer, size_t size, uint32_t bo);

    /// Write the given fingerprint into the database
    void   WriteFingerprint(uint32_t FID, const uint8_t *data, size_t size);
};

// ----------------------------------------------------------------------------

/// Metadata database

class TCMetadata : public TCCollection
{
public:

    TCMetadata(TCDataStore *dstore);
    ~TCMetadata(){}

    /// Read metadata for fingerprint FID
    std::string Read(uint32_t FID);

    /// Write metadata for fingerprint FID
    void   Write(uint32_t FID, const std::string& meta);
};

// ----------------------------------------------------------------------------

/// Datastore info database

class TCInfo : public TCCollection
{
public:

    TCInfo(TCDataStore *dstore);
    ~TCInfo(){}

    DBInfo_t Read();
    void Write(const DBInfo_t &info);
};

// ----------------------------------------------------------------------------

/// Implements a data store connection. In our context a connection is
/// a communication channel (and related resources) to all the databases
/// used by the audio identification engine, namely the index database and
/// the fingerprints database. Here we also use an additional "delta index"
/// database for the build-merge strategy.

class TCDataStore : public KVDataStore
{
    std::string               m_DBURL;          ///< URL to all database
    TCIndex                   m_MainIndex;      ///< The index database
    TCIndex                   m_DeltaIndex;     ///< The delta index database
    TCFingerprints            m_QFingerprints;  ///< The fingerprints database
    TCMetadata                m_Metadata;       ///< The metadata database
    TCInfo                    m_Info;           ///< Datastore info

    bool                      m_IsOpen;

    /// Buffer used to cache all data accessed by the ID instance
    /// using this connection.
    std::vector<uint8_t>   m_ReadBuffer;

public:

    explicit TCDataStore(const std::string &url = std::string());
    ~TCDataStore(){}

    void Open(eOperation op = GET,
              bool use_fing_db=true,
              bool use_meta_db=false,
              bool use_info_db=false);

    void Close();

    void SetDatabaseURL(const std::string &url) { m_DBURL = url; }

    std::string GetDatabaseURL()  { return m_DBURL; }

    bool Empty();

    void Clear();

    bool IsOpen() { return m_IsOpen; }

    eOperation GetOpMode() { return m_Op; }

    void SetOpMode(eOperation mode);

    void PutFingerprint(uint32_t FID, const uint8_t* data, size_t size){
        m_QFingerprints.WriteFingerprint(FID, data, size);
    }

    void PutMetadata(uint32_t FID, const std::string& meta){
        m_Metadata.Write(FID, meta);
    }

    std::string GetMetadata(uint32_t FID) { return m_Metadata.Read(FID); }

    DBInfo_t GetInfo() { return m_Info.Read(); }

    void PutInfo(const DBInfo_t& info) { m_Info.Write(info); }

    // API Interface

    const uint8_t* GetPListBlock(int list_id, int block, size_t& data_size, bool headers=true);
    size_t GetFingerprintSize(uint32_t FID);
    const uint8_t* GetFingerprint(uint32_t FID, size_t &read, size_t nbytes = 0, uint32_t bo = 0);
    size_t GetFingerprintsCount();
    void OnIndexerStart();
    void OnIndexerEnd();
    void OnIndexerFlushStart();
    void OnIndexerFlushEnd();
    Audioneex::PListHeader OnIndexerListHeader(int list_id);
    Audioneex::PListBlockHeader OnIndexerBlockHeader(int list_id, int block);

    void OnIndexerChunk(int list_id,
                        Audioneex::PListHeader &lhdr,
                        Audioneex::PListBlockHeader &hdr,
                        uint8_t* data, size_t data_size);

    void OnIndexerNewBlock (int list_id,
                            Audioneex::PListHeader &lhdr,
                            Audioneex::PListBlockHeader &hdr,
                            uint8_t* data, size_t data_size);

    void OnIndexerFingerprint(uint32_t FID, uint8_t* data, size_t size);

private:

    eOperation m_Op;
    int        m_Run;
};


#endif
