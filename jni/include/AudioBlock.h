/*

Copyright (c) 2014, Audioneex.com.
Copyright (c) 2014, Alberto Gramaglia.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list
   of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this
   list of conditions and the following disclaimer in the documentation and/or other
   materials provided with the distribution.

3. The name of the author(s) may not be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.

*/


#ifndef AUDIOBLOCK_H
#define AUDIOBLOCK_H

#include <cstdint>
#include <cstring>
#include <cmath>
#include <cassert>
#include <vector>
#include <iostream>
#include <sstream>

typedef int8_t   S8bit;
typedef int16_t  S16bit;
typedef int32_t  S32bit;
typedef uint8_t  U8bit;
typedef uint16_t U16bit;
typedef uint32_t U32bit;
typedef float    Sfloat;


template <class T> class AudioBlock;


/// A no-frills class for manipulating audio buffers. It is intended to be
/// used in contexts where audio buffers are mostly fixed size and dynamic
/// growths are exceptional events.

template <class T>
class AudioBlock
{
 public:

    /// Construct a null audio block.
    AudioBlock();

    /// Constructor.
    /// @param nsamples Maximum size (capacity) of the audio block.
    /// @param sampleRate Sampling frequency of the audio.
    /// @param nchans Number of audio channels.
    /// @param initSize Set the initial size of the block. A negative value means
    /// the block has an initial size equal to the value of the nsamples parameter.
    /// The block's audio data is initialized to zero (samples).
    AudioBlock(size_t nsamples, float sampleRate, size_t nchans, int initSize=-1);

    /// Copy constructor.
    AudioBlock(const AudioBlock<T> &block);

    /// D-tor.
    ~AudioBlock();

    /// Assignment operator.
    AudioBlock<T>& operator=(const AudioBlock<T> block);

    /// Sample access.
    T& operator[](size_t i);

    /// Create an audio block with the given parameters.
    /// @param nsamples Maximum size (capacity) of the audio block.
    /// @param sampleRate Sampling frequency of the audio.
    /// @param nchans Number of audio channels.
    /// @param initSize Initial size of the block (default is nsamples)
    void Create(size_t nsamples, float sampleRate, size_t nchans, int initSize=-1);

    /// Return the maximum size (capacity) of the audio block in samples (all channels)
    size_t  Capacity() const;
    /// Size of the available data in the audio block (in samples, all channels)
    size_t  Size() const;
    /// Size of the available data in the audio block (in bytes)
    size_t  SizeInBytes() const;
    /// Sampling frequency of the audio block
    float   SampleRate() const;
    /// Number of audio channels
    size_t  Channels() const;
    /// Audio block's sample resolution
    size_t  BytesPerSample() const;
    /// Duration of the available data in the audio block (in seconds)
    float   Duration() const;
    /// Duration of the audio block (in seconds)
    float   MaxDuration();
    /// Check whether the audio block is null (no data)
    bool    IsNull() const;
    /// Get normalization factor
    float   NormFactor() const;

    int32_t ID() const;
    void    SetID(int32_t id);
    int64_t Timestamp() const;
    void    SetTimestamp(int64_t tstamp);
    void    SetChannels(int nchans);

    /// Resize the block. This method does not perform any reallocation, it merely
    /// sets the amount of data to be considered in the block (available data).
    /// The method also changes the duration of the block.
    /// @param newsize The new block size in samples. If this value exceeds the
    /// capacity then it is truncated.
    /// TODO: check whether 'newsize' is an integer multiple of the number of channels.
    void    Resize(size_t newsize);

    /// Pointer to audio data
    T*       Data();
    const T* Data() const;

    /// Set the block's audio data. The data in the given buffer is copied in the internal
    /// buffer. Note that an audio block doesn't resize its buffer when written to, so
    /// writing more data than its capacity will result in the data being truncated.
    /// @return the number of written samples.
    size_t  SetData(const T *data, size_t nsamples);

    /// This method returns a normalized version of this audio block in the range [-1,1].
    /// @param nblock An audio block of float samples in which the normalized version
    /// of this audio block is computed. It must have the same size of this block, and
    /// the same sample rate and number of channels.
    void Normalize(AudioBlock<Sfloat> &nblock);

    /// This method returns a normalized version of this audio block in the range [-1,1].
    ///
    /// @return A new audio block of float samples normalized in [-1,1]. If this block is
    /// already in float format, the returned block is a mere copy of it. The clients are
    /// responsible for deleting the returned blocks. NOTE: before deleting the returned
    /// block, the pointer should be checked against this block's pointer.
    AudioBlock<Sfloat>* Normalize();

    /// This method appends the (available) data of the given block to this block's available
    /// data. It does not reallocate this block's buffer if there is not enough space
    /// to append all the data, in which case a truncation occurs.
    AudioBlock<T>& Append(AudioBlock<T> &block);
    AudioBlock<T>& Append(const T *data, size_t nsamples);

    /// Get a sub-block
    void GetSubBlock(size_t start, size_t size, AudioBlock<T> &block);


 private:

    size_t  mCapacity;     ///< Maximum size (fixed) of the audio block.
    float   mDuration;     ///< Time lenght of the available data (seconds)
    size_t  mSize;         ///< Amount of data available in the block.
    float   mSampleRate;
    size_t  mChannels;
    T*      mData;

    int32_t  mID;          ///< Audio block identifier (whatever it means)
    uint64_t mTimestamp;   ///< Timestamp [ms] (i.e. the time at which the block was read in the stream)
    float    mNormFactor;  ///< Normalization factor used to normalize the audio in the range [-1,1].


    /// Compute the factor to be used for normalizing this block in the range [-1,1].
    /// The value depends on the current block's sample format and is computed when the
    /// block is created.
    void ComputeNormalizationFactor();
    void DoAppend(const T* data, size_t nsamples);
    void Swap(AudioBlock<T> &b1, AudioBlock<T> &b2);

};


// -----------------------------------------------------------------------------
//                              Implementation
// -----------------------------------------------------------------------------


template <class T>
AudioBlock<T>::AudioBlock() :
    mCapacity(0),
    mDuration(0),
    mSize(0),
    mSampleRate(0),
    mChannels(0),
    mData(nullptr),
    mID(0),
    mTimestamp(0),
    mNormFactor(1.f)
{
    ComputeNormalizationFactor();
}


template <class T>
AudioBlock<T>::AudioBlock(size_t nsamples, float sampleRate, size_t nchans, int initSize) :
    mCapacity(0),
    mDuration(0),
    mSize(0),
    mSampleRate(0),
    mChannels(0),
    mData(nullptr),
    mID(0),
    mTimestamp(0),
    mNormFactor(1.f)
{
    Create(nsamples,sampleRate,nchans,initSize);
    ComputeNormalizationFactor();
}


template <class T>
AudioBlock<T>::AudioBlock(const AudioBlock<T> &block) :
    mCapacity   (block.mCapacity),
    mDuration   (block.mDuration),
    mSize       (block.mSize),
    mSampleRate (block.mSampleRate),
    mChannels   (block.mChannels),
    mData       (block.mCapacity ? new T[block.mCapacity] : nullptr),
    mID         (0),
    mTimestamp  (block.mTimestamp),
    mNormFactor (block.mNormFactor)
{
    std::copy(block.mData, block.mData + mCapacity, mData);
}


template <class T>
AudioBlock<T>::~AudioBlock()
{
    delete[] mData;
    mData = nullptr;
}


template <class T>
inline void AudioBlock<T>::Create(size_t nsamples, float sampleRate, size_t nchans, int initSize)
{
    assert(nsamples > 0);
    assert(sampleRate > 0);
    assert(nchans > 0);
    assert(mData==nullptr);

    if(mData!=nullptr)
       return;

    // The number of samples should always be an integral multiple of the
    // number of channels.
    // NOTE: This can be avoided by requiring the number of 'frames' instead
    //       of the number of samples (nframes)
    size_t dn = nsamples % nchans;
    nsamples = dn ? nsamples - dn + nchans : nsamples;

    mCapacity = nsamples;
    mSampleRate = sampleRate;
    mChannels = nchans;

    mSize = (initSize < 0) ? nsamples : initSize;

    if(mSize > mCapacity)
       mSize = mCapacity;

    mDuration = mSize / (nchans * sampleRate);
    mData = new T[mCapacity];
    memset(mData, 0, mCapacity * sizeof(T));
}


template <class T>
inline void AudioBlock<T>::Swap(AudioBlock<T> &b1, AudioBlock<T> &b2)
{
    std::swap(b1.mCapacity, b2.mCapacity);
    std::swap(b1.mDuration, b2.mDuration);
    std::swap(b1.mSize, b2.mSize);
    std::swap(b1.mSampleRate, b2.mSampleRate);
    std::swap(b1.mChannels, b2.mChannels);
    std::swap(b1.mData, b2.mData);
    std::swap(b1.mTimestamp, b2.mTimestamp);
    std::swap(b1.mNormFactor, b2.mNormFactor);
}

template <class T>
inline AudioBlock<T>& AudioBlock<T>::operator=(AudioBlock<T> block)
{
    Swap(*this, block);
    return (*this);
}


template <class T>
inline T& AudioBlock<T>::operator[](size_t i)
{
    assert(0<=i && i<mSize);
    return mData[i];
}


template <class T>
inline size_t  AudioBlock<T>::Capacity() const       { return mCapacity; }

template <class T>
inline size_t  AudioBlock<T>::Size() const           { return mSize; }

template <class T>
inline size_t  AudioBlock<T>::SizeInBytes() const    { return mSize * sizeof(T); }

template <class T>
inline float   AudioBlock<T>::Duration() const       { return mDuration; }

template <class T>
inline float   AudioBlock<T>::MaxDuration()          { return IsNull() ? 0 : mCapacity/(mChannels*mSampleRate);}

template <class T>
inline float   AudioBlock<T>::SampleRate() const     { return mSampleRate; }

template <class T>
inline size_t  AudioBlock<T>::Channels() const       { return mChannels; }

template <class T>
inline size_t  AudioBlock<T>::BytesPerSample() const { return sizeof(T); }

template <class T>
inline bool    AudioBlock<T>::IsNull() const         { return (mData == nullptr); }

template <class T>
inline int32_t AudioBlock<T>::ID() const             { return mID; }

template <class T>
inline int64_t AudioBlock<T>::Timestamp() const      { return mTimestamp; }

template <class T>
inline float   AudioBlock<T>::NormFactor() const     { return mNormFactor; }

template <class T>
inline void    AudioBlock<T>::SetID(int32_t id)      { mID = id; }

template <class T>
inline void    AudioBlock<T>::SetTimestamp(int64_t tstamp) { mTimestamp = tstamp; }

template <class T>
inline void    AudioBlock<T>::SetChannels(int nchans) { assert(nchans>0); mChannels = nchans; }


template <class T>
inline void    AudioBlock<T>::Resize(size_t newsize)
{
    assert(!IsNull());
    assert(newsize >= 0);

    if(newsize > mCapacity)
       newsize = mCapacity;

    mSize = newsize;
    mDuration = mSize / (mChannels * mSampleRate);
}


template <class T>
inline T*  AudioBlock<T>::Data()  { return mData; }

template <class T>
inline const T*  AudioBlock<T>::Data() const  { return mData; }

template <class T>
inline size_t  AudioBlock<T>::SetData(const T* data, size_t nsamples)
{
    assert(!IsNull());

    if(nsamples != mSize)
       Resize(nsamples);

    memcpy(mData, data, mSize * sizeof(T));

    return mSize;
}


template <class T>
inline void AudioBlock<T>::Normalize(AudioBlock<Sfloat> &nblock)
{
    assert(!IsNull() && !nblock.IsNull());

    // Resize the output block if sizes mismatch
    if(nblock.Size() != this->mSize)
       nblock.Resize( this->mSize );

    for(size_t i=0; i<mSize; i++)
        nblock[i] = mData[i] / mNormFactor;
}


template <class T>
inline AudioBlock<Sfloat>* AudioBlock<T>::Normalize()
{
    assert(!IsNull());

    AudioBlock<Sfloat> *nblock = new AudioBlock<Sfloat>(mSize, mSampleRate, mChannels);

    for(size_t i=0; i<mSize; i++)
        nblock->Data()[i] = mData[i] / mNormFactor;

    return nblock;
}


template <>
inline AudioBlock<Sfloat>* AudioBlock<Sfloat>::Normalize()
{
    assert(!IsNull());

    AudioBlock<Sfloat> *nblock = new AudioBlock<Sfloat>(mSize, mSampleRate, mChannels);

    if(nblock!=nullptr)
       nblock->SetData(mData, mSize);

    return nblock;
}


template <class T>
inline void AudioBlock<T>::ComputeNormalizationFactor()
{
    mNormFactor = pow(2.0f, static_cast<int>(sizeof(T)*8-1));
}


template <>
inline void AudioBlock<Sfloat>::ComputeNormalizationFactor(){}


template <class T>
inline AudioBlock<T>& AudioBlock<T>::Append(AudioBlock<T> &block)
{
    assert(!IsNull() && !block.IsNull());
    assert(block.Channels() == mChannels);
    assert(block.SampleRate() == mSampleRate);

    // nothing to append? just return
    if(block.IsNull() || block.Size() == 0)
       return *this;

    DoAppend(block.Data(), block.Size());
    return *this;
}


template <class T>
inline AudioBlock<T>& AudioBlock<T>::Append(const T* data, size_t nsamples)
{
    // nothing to append? just return
    if(data == nullptr || nsamples == 0)
       return *this;

    DoAppend(data, nsamples);
    return *this;
}


template <class T>
inline void AudioBlock<T>::DoAppend(const T *data, size_t nsamples)
{
    // compute remaining space in the buffer
    size_t available = mCapacity - mSize;

    if(available > 0)
    {
        // compute copyable data
       size_t copyable = (available >= nsamples) ? nsamples : available;

       // appended data should always be an integer multiple of the # of channels.
       assert(copyable % mChannels == 0);

       memcpy(reinterpret_cast<char*>(mData) + (mSize * sizeof(T)),
              data,
              copyable * sizeof(T));

       // resize block
       Resize(mSize + copyable);
    }
}


template <class T>
inline void AudioBlock<T>::GetSubBlock(size_t start, size_t size, AudioBlock<T> &block)
{
//    assert(!IsNull() && !block.IsNull());
//    assert(0<=start && start<mSize);
//    assert(size>=0);

    if( (IsNull() || block.IsNull()) ||
        !(0<=start && start<mSize) ||
        !(size>=0))
    {
        block.Resize(0);
        return;
    }

    size_t sbsize = std::min<size_t>(size, mSize - start);

    memcpy(reinterpret_cast<char*>(block.Data()),
           reinterpret_cast<char*>(mData) + (start * sizeof(T)),
           sbsize * sizeof(T));

    block.Resize(sbsize);
}


#endif // AUDIOBLOCK_H
