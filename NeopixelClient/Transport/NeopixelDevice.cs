using System.Text;
using System.Text.Json;
using triaxis.BluetoothLE;

namespace DynamicView
{
#pragma warning disable IDE1006 // Naming Styles
    /// <summary>
    /// Status values for Neopixel Device
    /// </summary>
    public class NeopixelStatus
    {
        public int textalign { get; set; } = 0;
        public int bmpalign { get; set; } = 0;
        public int txtclr { get; set; } = 0;
        public string text { get; set; } = "";
        public int bmp { get; set; } = 0;
        public int[] outputs { get; set; } = new int[0];
        public int[] leds { get; set; } = new int[0];
        public int[] ledcolora { get; set; } = new int[0];
        public int[] ledcolorb { get; set; } = new int[0];
    }
#pragma warning restore IDE1006 // Naming Styles

    public class NeopixelBleDevice : BleDevice, IDynamicViewDevice
    {
        const String BLE_SERVER_NAME = "Neopixel BLE";
        public const String SERVICE_UUID = "991e4749-bdb3-4e79-96d8-9a1b20cd51ab";
        const String METADATA_1_CHARACTERISTIC_UUID = "9da99ba6-6a55-4bfd-aee8-401fdcd69bf5";
        const String METADATA_2_CHARACTERISTIC_UUID = "76cd10b8-c3ea-4c16-a589-771506833f1c";
        const String STATUS_CHARACTERISTIC_UUID = "c0126900-f29f-4baf-8d8d-9e313d881339";

        ICharacteristic chStatus;
        ICharacteristic chMetadata1;
        ICharacteristic chMetadata2;

        object IDynamicViewDevice.Peripheral { get => base.Peripheral; set { } }

        public event StatusUpdated Updated;

        public NeopixelBleDevice(IAdvertisement advertisement) : base(advertisement)
        {
        }

        public async Task<bool> ConnectAsync()
        {
            try
            {
                IPeripheralConnection periph = await Peripheral.ConnectAsync(1000);

                chMetadata1 = await periph.FindServiceCharacteristicAsync(
                    new ServiceCharacteristicUuid(new ServiceUuid(new Uuid(SERVICE_UUID)),
                    new CharacteristicUuid(new Uuid(METADATA_1_CHARACTERISTIC_UUID))));
                chMetadata2 = await periph.FindServiceCharacteristicAsync(
                    new ServiceCharacteristicUuid(new ServiceUuid(new Uuid(SERVICE_UUID)),
                    new CharacteristicUuid(new Uuid(METADATA_2_CHARACTERISTIC_UUID))));
                chStatus = await periph.FindServiceCharacteristicAsync(
                    new ServiceCharacteristicUuid(new ServiceUuid(new Uuid(SERVICE_UUID)),
                    new CharacteristicUuid(new Uuid(STATUS_CHARACTERISTIC_UUID))));
                //IObservable<byte[]> chStatusNotify = chStatus.Notifications();
                //chStatusNotify.Subscribe()
                return true;
            }
            catch (Exception ex)
            {
                Console.WriteLine("Connect failed: " + ex.Message);
                return false;
            }
        }

        /// <summary>
        /// Submit 'param' change to 'value' and get / update status
        /// </summary>
        /// <param name="param"></param>
        /// <param name="value"></param>
        public async void Submit(String param, String value)
        {
            await chStatus.WriteAsync(System.Text.Encoding.Default.GetBytes(
                "{ \"" + param + "\":\"" + value + "\" }"));

            GetStatusAsync();
        }

        /// <summary>
        /// Get / update status
        /// </summary>
        public async void GetStatusAsync()
        {
            string status = System.Text.Encoding.Default.GetString(await chStatus.ReadAsync());
            NeopixelStatus statusJson = JsonSerializer.Deserialize<NeopixelStatus>(status);
            UpdateStatus(statusJson);
        }

        public async Task<ViewMetadata[]> GetMetaDataAsync()
        {
            string meta = System.Text.Encoding.Default.GetString(await chMetadata1.ReadAsync());
            meta += System.Text.Encoding.Default.GetString(await chMetadata2.ReadAsync());
            return JsonSerializer.Deserialize<ViewMetadata[]>(meta);
        }
        /// <summary>
        /// Fire the status update event
        /// </summary>
        /// <param name="status"></param>
        protected void UpdateStatus(Object status)
        {
            Updated?.Invoke(this, new StatusUpdatedEventArgs(status));
        }

    }

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
