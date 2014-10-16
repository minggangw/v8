// Copyright 2009-2010 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_PROFILER_HEAP_PROFILER_H_
#define V8_PROFILER_HEAP_PROFILER_H_

#include "src/base/smart-pointers.h"
#include "src/isolate.h"
#include "src/list.h"

namespace v8 {
namespace internal {

// Forward declarations.
class AllocationTracker;
class HeapObjectsMap;
class HeapSnapshot;
class HeapEventXDK;
class XDKAllocationTracker;
class SamplingHeapProfiler;
class StringsStorage;

class HeapProfiler {
 public:
  explicit HeapProfiler(Heap* heap);
  ~HeapProfiler();

  size_t GetMemorySizeUsedByProfiler();

  HeapSnapshot* TakeSnapshot(
      v8::ActivityControl* control,
      v8::HeapProfiler::ObjectNameResolver* resolver);

  bool StartSamplingHeapProfiler(uint64_t sample_interval, int stack_depth,
                                 v8::HeapProfiler::SamplingFlags);
  void StopSamplingHeapProfiler();
  bool is_sampling_allocations() { return !sampling_heap_profiler_.is_empty(); }
  AllocationProfile* GetAllocationProfile();

  void StartHeapObjectsTracking(bool track_allocations);
  void StopHeapObjectsTracking();
  AllocationTracker* allocation_tracker() const {
    return allocation_tracker_.get();
  }
  HeapObjectsMap* heap_object_map() const { return ids_.get(); }
  StringsStorage* names() const { return names_.get(); }

  SnapshotObjectId PushHeapObjectsStats(OutputStream* stream,
                                        int64_t* timestamp_us);
  void PushHeapObjectsXDKStats(OutputStream* stream);
  void StartHeapObjectsTrackingXDK(int stackDepth, bool retentions,
      bool strict_collection = false);
  v8::internal::HeapEventXDK* StopHeapObjectsTrackingXDK();

  int GetSnapshotsCount();
  HeapSnapshot* GetSnapshot(int index);
  SnapshotObjectId GetSnapshotObjectId(Handle<Object> obj);
  void DeleteAllSnapshots();
  void RemoveSnapshot(HeapSnapshot* snapshot);

  void ObjectMoveEvent(Address from, Address to, int size);

  void AllocationEvent(Address addr, int size);

  void UpdateObjectSizeEvent(Address addr, int size);

  void DefineWrapperClass(
      uint16_t class_id, v8::HeapProfiler::WrapperInfoCallback callback);

  v8::RetainedObjectInfo* ExecuteWrapperClassCallback(uint16_t class_id,
                                                      Object** wrapper);
  void SetRetainedObjectInfo(UniqueId id, RetainedObjectInfo* info);

  bool is_tracking_object_moves() const { return is_tracking_object_moves_; }
  bool is_tracking_allocations() const {
    return (!allocation_tracker_.is_empty() ||
        !allocation_tracker_xdk_.is_empty());
  }

  Handle<HeapObject> FindHeapObjectById(SnapshotObjectId id);
  void ClearHeapObjectMap();

  Isolate* isolate() const { return heap()->isolate(); }

 private:
  Heap* heap() const;

  // Mapping from HeapObject addresses to objects' uids.
  base::SmartPointer<HeapObjectsMap> ids_;
  List<HeapSnapshot*> snapshots_;
  base::SmartPointer<StringsStorage> names_;
  List<v8::HeapProfiler::WrapperInfoCallback> wrapper_callbacks_;
  base::SmartPointer<AllocationTracker> allocation_tracker_;
  base::SmartPointer<XDKAllocationTracker> allocation_tracker_xdk_;
  bool is_tracking_object_moves_;
  base::Mutex profiler_mutex_;
  base::SmartPointer<SamplingHeapProfiler> sampling_heap_profiler_;
};

}  // namespace internal
}  // namespace v8

#endif  // V8_PROFILER_HEAP_PROFILER_H_
