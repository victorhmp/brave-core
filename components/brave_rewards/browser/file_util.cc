/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_rewards/browser/file_util.h"

#include <memory>

#include "base/logging.h"

namespace brave_rewards {

namespace {

const int64_t kChunkSize = 1024;

int64_t SeekNumLines(
    base::File* file,
    const uint64_t num_lines) {
  DCHECK(file);

  if (num_lines == 0) {
    return 0;
  }

  int64_t length = file->GetLength();
  if (length == -1) {
    return -1;
  }

  if (length == 0) {
    return 0;
  }

  uint64_t line_count = 0;

  char chunk[kChunkSize];
  int64_t chunk_size = kChunkSize;
  int64_t last_chunk_size = 0;

  if (file->Seek(base::File::FROM_END, 0) == -1) {
    return -1;
  }

  do {
    if (chunk_size > length) {
      chunk_size = length;
    }

    const int64_t seek_offset = chunk_size + last_chunk_size;
    if (file->Seek(base::File::FROM_CURRENT, -seek_offset) == -1) {
      return -1;
    }

    if (file->ReadAtCurrentPos(chunk, chunk_size) == -1) {
      return -1;
    }

    for (int i = chunk_size - 1; i >= 0; i--) {
      if (chunk[i] == '\n') {
        line_count++;
        if (line_count == num_lines + 1) {
          return length;
        }
      }

      length--;
    }

    last_chunk_size = chunk_size;
  } while (length > 0);

  return length;
}

bool TruncateFileFromEnd(
    base::File* file,
    const int64_t offset) {
  DCHECK(file);

  if (offset < 0) {
    return false;
  }

  if (offset == 0) {
    return true;
  }

  if (file->Seek(base::File::FROM_BEGIN, offset) == -1) {
    return false;
  }

  const int64_t size = file->GetLength() - offset;
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);

  if (file->ReadAtCurrentPos(buffer.get(), size) == -1) {
    return false;
  }

  if (file->Seek(base::File::FROM_BEGIN, 0) == -1) {
    return false;
  }

  if (file->WriteAtCurrentPos(buffer.get(), size) == -1) {
    return false;
  }

  if (!file->SetLength(size)) {
    return false;
  }

  return true;
}

}  // namespace

bool TailFile(
    base::File* file,
    const uint64_t num_lines) {
  DCHECK(file);

  const int64_t offset = SeekNumLines(file, num_lines);
  if (offset == -1) {
    return false;
  }

  return TruncateFileFromEnd(file, offset);
}

std::string GetLastFileError(
    base::File* file) {
  DCHECK(file);

  const base::File::Error error = file->GetLastFileError();
  return file->ErrorToString(error);
}

}  // namespace brave_rewards
