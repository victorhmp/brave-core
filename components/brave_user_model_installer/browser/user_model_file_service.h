/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_USER_MODEL_FILE_SERVICE_H_
#define BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_USER_MODEL_FILE_SERVICE_H_

#include <memory>
#include <string>
#include <map>

#include "base/files/file_path.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "brave/components/brave_component_updater/browser/brave_component.h"

using brave_component_updater::BraveComponent;

namespace brave_user_model_installer {

struct UserModelFileInfo {
  std::string model_id;
  uint16_t version;
  base::FilePath path;
};

class UserModelFileService : public BraveComponent {
 public:
  class Observer : public base::CheckedObserver {
   public:
    virtual void OnUserModelFilesUpdated(
        const std::string& model_id,
        const base::FilePath& model_path) = 0;

   protected:
    ~Observer() override = default;
  };

  explicit UserModelFileService(Delegate* delegate);
  ~UserModelFileService() override;

  UserModelFileService(const UserModelFileService&) = delete;
  UserModelFileService& operator=(const UserModelFileService&) =
      delete;

  void AddObserver(
      Observer* observer);
  void RemoveObserver(
      Observer* observer);
  void NotifyObservers(
      const std::string& model_id,
      const base::FilePath& model_path);
  base::Optional<base::FilePath> GetPath(
      const std::string& model_id);

 private:
  void OnComponentReady(
      const std::string& component_id,
      const base::FilePath& install_dir,
      const std::string& manifest) override;
  void OnGetManifest(
      const base::FilePath& install_dir,
      const std::string& manifest_json);

  std::map<std::string, UserModelFileInfo> user_model_files_;
  base::ObserverList<Observer> observers_;
  base::WeakPtrFactory<UserModelFileService> weak_factory_{this};
};

}  // namespace brave_user_model_installer

#endif  // BRAVE_COMPONENTS_BRAVE_USER_MODEL_INSTALLER_BROWSER_USER_MODEL_FILE_SERVICE_H_
