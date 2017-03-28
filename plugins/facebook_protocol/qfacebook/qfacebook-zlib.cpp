/*
 * %kadu copyright begin%
 * Copyright 2017 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "qfacebook-zlib.h"

#include <QByteArray>
#include <zlib.h>

constexpr const auto CHUNK_SIZE{32 * 1024};
constexpr const auto MAX_SIZE{1 << 20};

namespace
{
class ZLibDeflateStream
{
public:
    explicit ZLibDeflateStream()
    {
        m_stream.avail_in = 0;
        m_stream.avail_out = 0;
        m_stream.next_in = Z_NULL;
        m_stream.next_out = Z_NULL;
        m_stream.opaque = Z_NULL;
        m_stream.zalloc = Z_NULL;
        m_stream.zfree = Z_NULL;

        auto res = deflateInit2(&m_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
        if (Z_OK != res)
            throw QFacebookZLibException{"deflateInit2 failed"};
    }
    ZLibDeflateStream(const ZLibDeflateStream &) = delete;
    ZLibDeflateStream(ZLibDeflateStream &&) = delete;
    ZLibDeflateStream &operator=(const ZLibDeflateStream &) = delete;
    ZLibDeflateStream &operator=(ZLibDeflateStream &&) = delete;

    ~ZLibDeflateStream()
    {
        deflateEnd(&m_stream);
    }

    z_stream &operator()()
    {
        return m_stream;
    }

    QByteArray deflate(const QByteArray &chunk, bool lastChunk)
    {
        prepareDeflate(chunk);
        return doDeflate(lastChunk);
    }

private:
    z_stream m_stream;

    void prepareDeflate(const QByteArray &chunk)
    {
        m_stream.avail_in = chunk.size();
        m_stream.avail_out = 0;
        m_stream.next_in = reinterpret_cast<unsigned char *>(const_cast<char *>(chunk.data()));
    }

    QByteArray doDeflate(bool lastChunk)
    {
        auto result = QByteArray{};
        while (m_stream.avail_out == 0)
        {
            auto deflated = QByteArray{CHUNK_SIZE, 0};
            m_stream.next_out = reinterpret_cast<unsigned char *>(deflated.data());
            m_stream.avail_out = deflated.size();
            auto res = ::deflate(&m_stream, lastChunk ? Z_FINISH : Z_NO_FLUSH);

            if (res == Z_STREAM_ERROR)
                throw QFacebookZLibException{"deflate failed"};

            auto done = (deflated.size() - m_stream.avail_out);
            if (done > 0)
            {
                deflated.resize(done);
                result.append(deflated);
            }

            if (result.size() > MAX_SIZE)
                throw QFacebookZLibException{"MAX_SIZE exceeded"};
        }
        return result;
    }
};

class ZLibInflateStream
{
public:
    explicit ZLibInflateStream()
    {
        m_stream.avail_in = 0;
        m_stream.avail_out = 0;
        m_stream.next_in = Z_NULL;
        m_stream.next_out = Z_NULL;
        m_stream.opaque = Z_NULL;
        m_stream.zalloc = Z_NULL;
        m_stream.zfree = Z_NULL;

        if (Z_OK != inflateInit2(&m_stream, 0))
            throw QFacebookZLibException{"inflateInit2 failed"};
    }
    ZLibInflateStream(const ZLibInflateStream &) = delete;
    ZLibInflateStream(ZLibInflateStream &&) = delete;
    ZLibInflateStream &operator=(const ZLibInflateStream &) = delete;
    ZLibInflateStream &operator=(ZLibInflateStream &&) = delete;

    ~ZLibInflateStream()
    {
        inflateEnd(&m_stream);
    }

    z_stream &operator()()
    {
        return m_stream;
    }

    QByteArray inflate(const QByteArray &chunk, bool lastChunk)
    {
        prepareInflate(chunk);
        return doInflate(lastChunk);
    }

private:
    z_stream m_stream;

    void prepareInflate(const QByteArray &chunk)
    {
        m_stream.avail_in = chunk.size();
        m_stream.avail_out = 0;
        m_stream.next_in = reinterpret_cast<unsigned char *>(const_cast<char *>(chunk.data()));
    }

    QByteArray doInflate(bool lastChunk)
    {
        auto result = QByteArray{};
        while (m_stream.avail_out == 0)
        {
            auto deflated = QByteArray{CHUNK_SIZE, 0};
            m_stream.next_out = reinterpret_cast<unsigned char *>(deflated.data());
            m_stream.avail_out = deflated.size();
            auto res = ::inflate(&m_stream, lastChunk ? Z_FINISH : Z_NO_FLUSH);

            switch (res)
            {
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            case Z_NEED_DICT:
            case Z_STREAM_ERROR:
                throw QFacebookZLibException{"inflate failed"};
            default:
                break;
            }

            auto done = (deflated.size() - m_stream.avail_out);
            if (done > 0)
            {
                deflated.resize(done);
                result.append(deflated);
            }

            if (result.size() > MAX_SIZE)
                throw QFacebookZLibException{"MAX_SIZE exceeded"};
        }
        return result;
    }
};
}

QByteArray qfacebookDeflate(const QByteArray &data)
{
    auto result = QByteArray{};
    if (data.isEmpty())
        return result;

    ZLibDeflateStream deflateStream{};

    auto offset = 0;
    auto left = data.length();
    while (left > 0)
    {
        auto currentChunkSize = std::min(CHUNK_SIZE, left);
        auto chunk = data.mid(offset, currentChunkSize);
        offset += currentChunkSize;
        left -= currentChunkSize;
        result.append(deflateStream.deflate(chunk, left == 0));

        if (result.size() > MAX_SIZE)
            throw QFacebookZLibException{"MAX_SIZE exceeded"};
    }

    return result;
}

QByteArray qfacebookInflate(const QByteArray &data)
{
    auto result = QByteArray{};
    if (data.isEmpty())
        return result;

    ZLibInflateStream inflateStream{};

    auto offset = 0;
    auto left = data.length();
    while (left > 0)
    {
        auto currentChunkSize = std::min(CHUNK_SIZE, left);
        auto chunk = data.mid(offset, currentChunkSize);
        offset += currentChunkSize;
        left -= currentChunkSize;
        result.append(inflateStream.inflate(chunk, left == 0));

        if (result.size() > MAX_SIZE)
            throw QFacebookZLibException{"MAX_SIZE exceeded"};
    }

    return result;
}
