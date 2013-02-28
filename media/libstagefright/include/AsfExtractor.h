/*
* Copyright (c) 2009-2011 Intel Corporation.  All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


#ifndef ASF_EXTRACTOR_H_
#define ASF_EXTRACTOR_H_

#include <media/stagefright/MediaExtractor.h>
#include <utils/Vector.h>
#include <media/stagefright/MediaBuffer.h>
#include <media/stagefright/MediaSource.h>


namespace android {

struct AMessage;
class DataSource;


class AsfExtractor : public MediaExtractor {
public:
    // Extractor assumes ownership of "source".
    AsfExtractor(const sp<DataSource> &source);
    virtual ~AsfExtractor();

    virtual size_t countTracks();
    virtual sp<MediaSource> getTrack(size_t index);
    virtual sp<MetaData> getTrackMetaData(size_t index, uint32_t flags);
    virtual sp<MetaData> getMetaData();

private:
    status_t read(
        int trackIndex,
        MediaBuffer **buffer,
        const MediaSource::ReadOptions *options = NULL);

    friend class ASFSource;

private:
    struct Track  {
        Track *next;
        sp<MetaData> meta;
        bool skipTrack;
        bool seekCompleted;
        bool encrypted;
        uint8_t streamNumber;

        // outgoing buffer queue (ready for decoding)
        Vector<MediaBuffer*> bufferQueue;

        // buffer currently being used to read payload data
        MediaBuffer *bufferActive;
        Mutex lock;
    };

    sp<DataSource> mDataSource;
    bool mInitialized;
    Track *mFirstTrack;
    Track *mLastTrack;

    Mutex mReadLock;
    sp<MetaData> mFileMetaData;
    class AsfStreamParser *mParser;

    int64_t mHeaderObjectSize;
    int64_t mDataObjectSize;

    int64_t mDataPacketBeginOffset;
    int64_t mDataPacketEndOffset;
    int64_t mDataPacketCurrentOffset;

    int64_t mDataPacketSize;
    uint8_t *mDataPacketData;

    enum {
        // 100 nano seconds to micro second
        SCALE_100_NANOSEC_TO_USEC = 10,
    };

    AsfExtractor(const AsfExtractor &);
    AsfExtractor &operator=(const AsfExtractor &);

    status_t initialize();
    void uninitialize();
    status_t setupTracks();
    inline Track* getTrackByTrackIndex(int index);
    inline Track* getTrackByStreamNumber(int stream);
    status_t seek_l(Track* track, const MediaSource::ReadOptions *options);
    status_t read_l(Track *track, MediaBuffer **buffer);
    status_t readPacket();
};


bool SniffAsf(
    const sp<DataSource> &source,
    String8 *mimeType,
    float *confidence,
    sp<AMessage> *);

}  // namespace android

#endif  // ASF_EXTRACTOR_H_

