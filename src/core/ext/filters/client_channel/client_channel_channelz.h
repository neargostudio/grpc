/*
 *
 * Copyright 2018 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_CLIENT_CHANNEL_CHANNELZ_H
#define GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_CLIENT_CHANNEL_CHANNELZ_H

#include <grpc/support/port_platform.h>

#include "src/core/lib/channel/channel_args.h"
#include "src/core/lib/channel/channel_stack.h"
#include "src/core/lib/channel/channel_trace.h"
#include "src/core/lib/channel/channelz.h"

namespace grpc_core {

class Subchannel;

namespace channelz {

class SubchannelNode : public BaseNode {
 public:
  SubchannelNode(const char* target_address, size_t channel_tracer_max_nodes);
  ~SubchannelNode() override;

  void MarkSubchannelDestroyed() {
    GPR_ASSERT(!subchannel_destroyed_);
    subchannel_destroyed_ = true;
  }

  // Used when the subchannel's transport connectivity state changes.
  void UpdateConnectivityState(grpc_connectivity_state state);

  // Used when the subchannel's child socket uuid changes. This should be set
  // when the subchannel's transport is created and set to 0 when the subchannel
  // unrefs the transport. A uuid of 0 indicates that the child socket is no
  // longer associated with this subchannel.
  void SetChildSocketUuid(intptr_t uuid);

  grpc_json* RenderJson() override;

  // proxy methods to composed classes.
  void AddTraceEvent(ChannelTrace::Severity severity, const grpc_slice& data) {
    trace_.AddTraceEvent(severity, data);
  }
  void AddTraceEventWithReference(ChannelTrace::Severity severity,
                                  const grpc_slice& data,
                                  RefCountedPtr<BaseNode> referenced_channel) {
    trace_.AddTraceEventWithReference(severity, data,
                                      std::move(referenced_channel));
  }
  void RecordCallStarted() { call_counter_.RecordCallStarted(); }
  void RecordCallFailed() { call_counter_.RecordCallFailed(); }
  void RecordCallSucceeded() { call_counter_.RecordCallSucceeded(); }

 private:
  void PopulateConnectivityState(grpc_json* json);

  bool subchannel_destroyed_;
  Atomic<grpc_connectivity_state> connectivity_state_{GRPC_CHANNEL_IDLE};
  Atomic<intptr_t> child_socket_uuid_{0};
  UniquePtr<char> target_;
  CallCountingHelper call_counter_;
  ChannelTrace trace_;
};

}  // namespace channelz
}  // namespace grpc_core

#endif /* GRPC_CORE_EXT_FILTERS_CLIENT_CHANNEL_CLIENT_CHANNEL_CHANNELZ_H */
