/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2019 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup depsgraph
 */

#pragma once

#include "intern/depsgraph_type.h"
#include "intern/eval/deg_eval_runtime_backup_sequence.h"

struct Scene;

namespace blender {
namespace deg {

struct Depsgraph;

/* Backup of sequencer strips runtime data. */
class SequencerBackup {
 public:
  SequencerBackup(const Depsgraph *depsgraph);

  void init_from_scene(Scene *scene);
  void restore_to_scene(Scene *scene);

  const Depsgraph *depsgraph;

  Map<Sequence *, SequenceBackup> sequences_backup;
};

}  // namespace deg
}  // namespace blender
