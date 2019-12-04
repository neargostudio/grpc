#region Copyright notice and license
// Copyright 2015 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#endregion

#if GRPC_SUPPORT_WATCH
using System.Threading.Channels;
using System.Threading.Tasks;

using Grpc.Core;
using Grpc.Health.V1;

namespace Grpc.HealthCheck.Tests
{
    internal class TestResponseStreamWriter : IServerStreamWriter<HealthCheckResponse>
    {
        private Channel<HealthCheckResponse> _channel;

        public TestResponseStreamWriter(int maxCapacity = 1)
        {
            _channel = System.Threading.Channels.Channel.CreateBounded<HealthCheckResponse>(new BoundedChannelOptions(maxCapacity) {
                SingleReader = false,
                SingleWriter = true,
                FullMode = BoundedChannelFullMode.Wait
            });
        }

        public ChannelReader<HealthCheckResponse> WrittenMessagesReader => _channel.Reader;

        public WriteOptions WriteOptions { get; set; }

        public Task WriteAsync(HealthCheckResponse message)
        {
            return _channel.Writer.WriteAsync(message).AsTask();
        }

        public void Complete()
        {
            _channel.Writer.Complete();
        }
    }
}
#endif