using System.Text;
using System.Text.Json;
using triaxis.BluetoothLE;

namespace DynamicView
{
    /// <summary>
    /// Specialization of HttpDynamicViewTransport to implement status as NeopixelStatus
    /// </summary>
    public class NeopixelHttpDevice : HttpDynamicViewDevice
    {
        public NeopixelStatus Status { get; private set; } = new NeopixelStatus();
        public string DeviceName { get { return "http connection"; } }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="timeoutSec"></param>
        public NeopixelHttpDevice(String uri, int timeoutSec = 10)
            : base(uri, timeoutSec)
        {
        }

        /// <summary>
        /// Submit 'param' change to 'value' and get / update status
        /// </summary>
        /// <param name="param"></param>
        /// <param name="value"></param>
        public override async void Submit(String param, String value)
        {
            await StatusWorkerAsync("http://" + Address + "/status?" + param + "=" + value);
        }

        /// <summary>
        /// Get / update status
        /// </summary>
        public override async void GetStatusAsync()
        {
            await StatusWorkerAsync("http://" + Address + "/status");
        }

        /// <summary>
        /// Status worker. Make get request to 'url' and parse response as JSON encoded NeopixelStatus
        /// </summary>
        /// <param name="url"></param>
        /// <returns></returns>
        private async Task<NeopixelStatus> StatusWorkerAsync(String url)
        {
            try
            {
                String content = await GetRequestWorker(url);
                Status = JsonSerializer.Deserialize<NeopixelStatus>(content);
                UpdateStatus(Status);
            }
            catch (Exception ex)
            {
                Console.WriteLine(@"\tERROR {0}", ex.Message);
            }

            return Status;
        }
    }
}
