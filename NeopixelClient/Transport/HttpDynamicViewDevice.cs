using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace DynamicView
{
    /// <summary>
    /// Base transport for device supporting DynamicView. Supports 
    /// GetRequestWorker() that performs HTTP Get requests and returns the JSON response
    /// as a string, and GetMetaDataAsync() that returns the JSON result as an array of ViewMetadata.
    /// 
    /// Declares abstract GetStatusAsync(). Provides UpdateStatus() for use by implementation to deliver
    /// received (device specific) status as an object. Runs timer to poll for status at specified interval.
    /// </summary>
    public abstract class HttpDynamicViewDevice : IDynamicViewDevice
    {
        private readonly HttpClient _client;
        private Timer _timer;

        public event StatusUpdated Updated;
        public String Address { get; set; }
        public object Peripheral { get => null; set { } }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="timeoutSec"></param>
        public HttpDynamicViewDevice(String uri, int timeoutSec = 10)
        {
            Address = uri;
            _client = new HttpClient();
            _client.Timeout = new TimeSpan(0, 0, timeoutSec);
        }

         /// <summary>
        /// Poll timer expired. Poll for status
        /// </summary>
        /// <param name="state"></param>
        void poll_timedout(object state)
        {
            (state as HttpDynamicViewDevice).GetStatusAsync();
        }

        public async Task<bool> ConnectAsync()
        {
            if (await GetRequestWorker("http://" + Address) != null)
            {
                _timer = new Timer(poll_timedout, this, 0, 3000);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Fire the status update event
        /// </summary>
        /// <param name="status"></param>
        protected void UpdateStatus(Object status)
        {
            Updated?.Invoke(this, new StatusUpdatedEventArgs(status));
        }

        /// <summary>
        /// Derived class must implement this to get device status
        /// </summary>
        public abstract void GetStatusAsync();

        /// <summary>
        /// Derived class must implement this to submit parameter changes
        /// </summary>
        public abstract void Submit(String param, String value);


        /// <summary>
        /// Get metadata from device, parse it, and return as array of ViewMetadata
        /// </summary>
        /// <returns></returns>
        public virtual async Task<ViewMetadata[]> GetMetaDataAsync()
        {
            ViewMetadata[] Metadata = new ViewMetadata[0];

            try
            {
                String content = await GetRequestWorker("http://" + Address + "/metadata");
                Metadata = JsonSerializer.Deserialize<ViewMetadata[]>(content);
            }
            catch (Exception ex)
            {
                Console.WriteLine(@"\tERROR {0}", ex.Message);
            }

            return Metadata;
        }

        /// <summary>
        /// Worker method. Make a HTTP GET request to 'url' and return response as a string
        /// </summary>
        /// <param name="url"></param>
        /// <returns></returns>
        protected async Task<String> GetRequestWorker(String url)
        {
            Uri uri = new Uri(string.Format(url, string.Empty));
            try
            {
                HttpResponseMessage response = await _client.GetAsync(uri);
                if (response.IsSuccessStatusCode)
                {
                    return await response.Content.ReadAsStringAsync();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(@"\tERROR {0}", ex.Message);
            }
            return null;
        }
    }
}
