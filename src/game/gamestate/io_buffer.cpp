#include <cassert>
#include "io_buffer.h"

void io_buffer::hook(buffer *buf) {
    if (this == nullptr)
        return;
    p_buf = buf;
}

bool io_buffer::validate() {
    if (this == nullptr)
        return false;
    if (p_buf == nullptr || !p_buf->is_valid(1))
        return false;
    if (access_type != CHUNK_ACCESS_REVOKED)
        return false;
    if (bind_callback == nullptr)
        return false;
    return true;
}

bool io_buffer::io_run(chunk_buffer_access_e flag) {
    if (!validate())
        return false;
    access_type = flag;
    if (!bind_callback(this))
        return false;
    access_type = CHUNK_ACCESS_REVOKED;
    return true;
}
bool io_buffer::read() {
    return io_run(CHUNK_ACCESS_READ);
}
bool io_buffer::write() {
    return io_run(CHUNK_ACCESS_WRITE);
}

io_buffer::io_buffer(bool (*bclb)(io_buffer *)) {
    bind_callback = bclb;
}
io_buffer::~io_buffer() {
    // this DOES NOT free up the internal buffer memory (for now)!!!
    // it WILL become a dangling pointer!
}