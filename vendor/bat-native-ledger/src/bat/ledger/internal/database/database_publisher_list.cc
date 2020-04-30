/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/database/database_publisher_list.h"

#include <tuple>
#include <utility>

#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "bat/ledger/internal/database/database_util.h"
#include "bat/ledger/internal/publisher/prefix_util.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "bat/ledger/internal/state/state_keys.h"

using std::placeholders::_1;
using braveledger_publisher::PrefixIterator;

namespace {

const char kTableName[] = "publisher_list";

constexpr size_t kHashPrefixSize = 4;
constexpr size_t kMaxInsertRecords = 100'000;

void DropAndCreateTableV28(ledger::DBTransaction* transaction) {
  DCHECK(transaction);

  if (!braveledger_database::DropTable(transaction, kTableName)) {
    NOTREACHED();
  }

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::EXECUTE;
  command->command = base::StringPrintf(
      "CREATE TABLE %s "
      "(hash_prefix BLOB PRIMARY KEY NOT NULL)",
      kTableName);

  transaction->commands.push_back(std::move(command));
}

void AddDropAndCreateTableCommand(ledger::DBTransaction* transaction) {
  DropAndCreateTableV28(transaction);
}

std::tuple<PrefixIterator, std::string, size_t> GetPrefixInsertList(
    PrefixIterator begin,
    PrefixIterator end) {
  DCHECK(begin != end);
  size_t count = 0;
  std::string values;
  PrefixIterator iter = begin;
  for (iter = begin; iter != end && count++ < kMaxInsertRecords; ++iter) {
    auto prefix = *iter;
    DCHECK(prefix.size() >= kHashPrefixSize);
    values.append(iter == begin ? "(x'" : "'),(x'");
    values.append(base::HexEncode(prefix.data(), kHashPrefixSize));
  }
  values.append("')");
  return {iter, std::move(values), count};
}

}  // namespace

namespace braveledger_database {

DatabasePublisherList::DatabasePublisherList(bat_ledger::LedgerImpl* ledger)
    : DatabaseTable(ledger) {}

DatabasePublisherList::~DatabasePublisherList() = default;

bool DatabasePublisherList::Migrate(
    ledger::DBTransaction* transaction,
    int target) {
  switch (target) {
    case 28:
      MigrateToV28(transaction);
      return true;
    default:
      return true;
  }
}

void DatabasePublisherList::MigrateToV28(
    ledger::DBTransaction* transaction) {
  DropAndCreateTableV28(transaction);
  ledger_->ClearState(ledger::kStateServerPublisherListStamp);
}

void DatabasePublisherList::Search(
    const std::string& publisher_key,
    ledger::SearchPublisherListCallback callback) {
  std::string hex = braveledger_publisher::GetHashPrefixInHex(
      publisher_key,
      kHashPrefixSize);

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::READ;
  command->command = base::StringPrintf(
      "SELECT COUNT(*) as count FROM %s WHERE hash_prefix = x'%s'",
      kTableName,
      hex.c_str());

  command->record_bindings = {
    ledger::DBCommand::RecordBindingType::INT_TYPE
  };

  auto transaction = ledger::DBTransaction::New();
  transaction->commands.push_back(std::move(command));

  ledger_->RunDBTransaction(std::move(transaction), [callback](
      ledger::DBCommandResponsePtr response) {
    if (response && response->result) {
      for (auto& record : response->result->get_records()) {
        int count = GetIntColumn(record.get(), 0);
        callback(count > 0);
        return;
      }
    }
    BLOG(0, "Unexpected database result while searching "
        "publisher prefix list.");
    callback(false);
  });
}

void DatabasePublisherList::ResetPrefixes(
    std::unique_ptr<braveledger_publisher::PublisherListReader> reader,
    ledger::ResultCallback callback) {
  if (reader_) {
    BLOG(0, "Publisher prefix list batch insert in progress");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }
  if (reader->size() == 0) {
    BLOG(0, "Cannot reset with an empty publisher prefix list");
    callback(ledger::Result::LEDGER_ERROR);
    return;
  }
  reader_ = std::move(reader);
  InsertNext(reader_->begin(), callback);
}

void DatabasePublisherList::InsertNext(
    PrefixIterator begin,
    ledger::ResultCallback callback) {
  DCHECK(reader_);
  DCHECK(begin != reader_->end());

  auto transaction = ledger::DBTransaction::New();

  if (begin == reader_->begin()) {
    BLOG(1, "Clearing publisher prefixes table");
    AddDropAndCreateTableCommand(transaction.get());
  }

  auto insert_tuple = GetPrefixInsertList(begin, reader_->end());

  BLOG(1, "Inserting " << std::get<size_t>(insert_tuple)
      << " records into publisher prefix table");

  auto command = ledger::DBCommand::New();
  command->type = ledger::DBCommand::Type::RUN;
  command->command = base::StringPrintf(
      "INSERT OR REPLACE INTO %s (hash_prefix) VALUES %s",
      kTableName,
      std::get<std::string>(insert_tuple).data());

  transaction->commands.push_back(std::move(command));

  auto iter = std::get<PrefixIterator>(insert_tuple);

  ledger_->RunDBTransaction(std::move(transaction), [this, iter, callback](
      ledger::DBCommandResponsePtr response) {
    if (!response ||
        response->status != ledger::DBCommandResponse::Status::RESPONSE_OK) {
      reader_ = nullptr;
      callback(ledger::Result::LEDGER_ERROR);
      return;
    }

    if (iter == reader_->end()) {
      reader_ = nullptr;
      callback(ledger::Result::LEDGER_OK);
      return;
    }

    InsertNext(iter, callback);
  });
}

}  // namespace braveledger_database
